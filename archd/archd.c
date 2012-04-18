/*
File:       archd
Copyright:  (C) 2012 by Albuquerque Seismological Laboratory
Purpose:    Server to merge tcpip seed records from multiple processes
Then it archives the data into IDA and new ASL disk loops

Algorithm:  
Opens a file descriptors for the necessary communcations, and uses callbacks
for all I/O operations. All events are driven by the pcomm asynchronous
I/O library. 

File Descriptors:
- message server
(Accepts connection requests and creates client fds)
- client fds
(Wait for messages, and add them to the processing buffer when they
arrive. Initializes creation of the diskloop fd for this channel.)
(When a new LOG message arrives, combine it with queued LOG message if 
possible. If combine would be too large, send queued message, make new
message the new queued message.)
Once queued LOG message reaches BUFFER_TIME_SEC in age, or non LOG record:
Archive message, empty queue
- diskloop fds
(Selects on the file fds when data is available, and writes
new data to the diskloop.)
- isid fd
(Selects on the isi fd when data is available, and sends new
message to the IDA diskloop.)

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2012-04-12 JDE - Creation
 ******************************************************************************/
#define WHOAMI "archd"
const char *VersionIdentString = "Release 2.0";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/time.h>
#include "libpcomm/pcomm.h"
#include "include/map.h"
#include "include/idaapi.h"
#include "include/diskloop.h"
#include "include/shmstatus.h"
#include "include/archd.h"
#include "include/netreq.h"
#include "include/dcc_time_proto2.h"

#define ASYNC_TIMEOUT_SECONDS       1 
#define ASYNC_TIMEOUT_MICROSECONDS  0
#define BIND_RETRY_INTERVAL         15

void daemonize();

//////////////////////////////////////////////////////////////////////////////

int init_server (archd_context_t *archd);
void callback_archive (pcomm_context_t *pcomm);
void callback_async_timeout (pcomm_context_t *pcomm);
void callback_connect_request (pcomm_context_t *pcomm, int fd);
void callback_server_closed (pcomm_context_t *pcomm, int fd);
void callback_client_can_recv (pcomm_context_t *pcomm, int fd);
void callback_client_can_send (pcomm_context_t *pcomm, int fd);
void callback_client_closed (pcomm_context_t *pcomm, int fd);

// Make the server_pid global so it can be killed when program is shut down
// This is required by daemonize()
int   server_pid=0;

int   g_bDebug=0;
//char  g_sIDAname[6];
//static struct s_mapshm *mapshm=NULL;
//static struct s_mapstatus *mapstatus=NULL;
static archd_context_t *g_archd=NULL;
static int iSeedRecordSize;

//////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{
    fprintf(stderr,
            "Usage:\n");
    fprintf(stderr,
            "  archd <configfile> [debug]\n");
    fprintf(stderr,
            "    Starts log seed message server as a daemon\n");
    fprintf(stderr,
            "      <configfile>\n");
    fprintf(stderr,
            "    Usually /etc/q330/DLG1/diskloop.config\n");
    fprintf(stderr,
            "      [debug]\n");
    fprintf(stderr,
            "    Optionaly start server in debug mode\n");
    fprintf(stderr,
            "Version %s  %s\n", VersionIdentString, __DATE__);
} // ShowUsage()

//////////////////////////////////////////////////////////////////////////////

static void sigterm_archd()
{
    // Set flat requesting a graceful program exit
    if ((g_archd != NULL) && (g_archd->pcomm != NULL)) {
        // Tell pcomm to finish processing all data
        pcomm_stop(g_archd->pcomm, 0/*not immediately*/);
    }
    else {
        // If our context is not complete, force a program exit
        exit(0);
    }
} // sigterm_archd()

int ChannelControl(char *command, int *msgId)
{
    int i, setResult = 0;
    size_t base = 14;
    char station[6];
    char loc[3];
    char chan[4];
    char idString[9];
    char c;
    char recordStartString[128];
    char *ptr = command + 14;

    for (i=0; i < 127; i++) {
        c = command[i];
        if (c == 0) {
            break;
        }
        if (!isprint(c)) {
            c = '*';
        }
        recordStartString[i] = c;
    }
    recordStartString[i] = 0;
    if (g_bDebug)
        fprintf(stderr, "%s: CHANNELCONTROL Command: %s\n", WHOAMI, recordStartString);

    if (*ptr != '-') {
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL Parse: invalid separator '%c' after base\n", WHOAMI, *ptr);
        return -1;
    }
    ptr++;

    // Parse message ID
    for (i=0; i < 8; i++, ptr++) {
        c = *ptr;
        if (c == '-') {
            if (i == 0) {
                if (g_bDebug)
                    fprintf(stderr, "%s: CHANNELCONTROL Parse: no message ID\n", WHOAMI);
                return -1;
            }
            break;
        }
        else if (isdigit(c)) {
            idString[i] = c;
        } 
        else {
            if (g_bDebug)
                fprintf(stderr, "%s: CHANNELCONTROL Parse: invalid message ID character\n", WHOAMI);
            return -1;
        }
    }
    idString[i] = 0;
    *msgId = atoi(idString);

    if (*ptr != '-') {
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL Parse: invalid separator '%c' after message ID\n", WHOAMI, *ptr);
        return -1;
    }
    ptr++;

    // Copy station name
    for (i=0; i < 5; i++, ptr++) {
        c = *ptr;
        if (c == '-') {
            break;
        }
        else if (isalnum(c)) {
            station[i] = c;
        }
        else {
            if (g_bDebug)
                fprintf(stderr, "%s: CHANNELCONTROL Parse: invalid station name character '%c'\n", WHOAMI, c);
            return -1;
        }
    }
    station[i] = 0;

    if (*ptr != '-') {
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL Parse: invalid separator '%c' after station\n", WHOAMI, *ptr);
        return -1;
    }
    ptr++;

    // Copy location code
    for (i=0; i < 2; i++, ptr++) {
        c = *ptr;
        if (c == '-') {
            break;
        }
        else if (isalnum(c)) {
            loc[i] = c;
        }
        else {
            if (g_bDebug)
                fprintf(stderr, "%s: CHANNELCONTROL Parse: invalid location code character '%c'\n", WHOAMI, c);
            return -1;
        }
    }
    loc[i] = 0;

    if (*ptr != '-') {
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL Parse: invalid separator '%c' after location\n", WHOAMI, *ptr);
        return -1;
    }
    ptr++;

    // Copy channel name
    for (i=0; i < 3; i++, ptr++) {
        c = *ptr;
        if (c == '-') {
            break;
        }
        else if (isalnum(c)) {
            chan[i] = c;
        }
        else {
            if (g_bDebug)
                fprintf(stderr, "%s: CHANNELCONTROL Parse: invalid channel name character '%c'\n", WHOAMI, c);
            return -1;
        }
    }
    chan[i] = 0;

    if (*ptr != '-')  {
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL Parse: invalid separator '%c' after channel\n", WHOAMI, *ptr);
        return -1;
    }
    ptr++;

    if (strncmp("ARCHIVE-DEFAULT", ptr, 15) == 0) {
        setResult = DefaultChannelToArchive(station, chan, loc);
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL: Archive [DEFAULT] result=%d\n", WHOAMI, setResult);
    }
    else if (strncmp("ARCHIVE-OFF", ptr, 11) == 0) {
        setResult = SetChannelToArchive(station, chan, loc, 0) == 0;
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL: Archive [OFF] result=%d\n", WHOAMI, setResult);
    }
    else if (strncmp("ARCHIVE-ON", ptr, 10) == 0) {
        setResult = SetChannelToArchive(station, chan, loc, 1) == 1;
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL: Archive [ON] result=%d\n", WHOAMI, setResult);
    }
    else if (strncmp("IDA-DEFAULT", ptr, 11) == 0) {
        setResult = DefaultChannelToIDA(station, chan, loc);
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL: IDA [DEFAULT] result=%d\n", WHOAMI, setResult);
    }
    else if (strncmp("IDA-OFF", ptr, 7) == 0) {
        setResult = SetChannelToIDA(station, chan, loc, 0) == 0;
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL: IDA [OFF] result=%d\n", WHOAMI, setResult);
    }
    else if (strncmp("IDA-ON", ptr, 6) == 0) {
        setResult = SetChannelToIDA(station, chan, loc, 1) == 1;
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL: IDA [ON] result=%d\n", WHOAMI, setResult);
    }
    else {
        if (g_bDebug)
            fprintf(stderr, "%s: CHANNELCONTROL Parse: unrecognized command\n", WHOAMI);
        return -1;
    }

    return (setResult > 0);
}

char *ArchiveSeed(char *record)
{
    int  i, to = 0;
    seed_header *seedrec;
    char station[6];
    char channel[4];
    char location[4];
    char *retmsg1;
    char *retmsg2;
    char *toMessage;

    seedrec = (seed_header *)record;

    memcpy(station, seedrec->station_id_call_letters, 5);
    for (i=0; i < 5 && station[i] != ' '; i++)
        ; // stop at first blank or 5th char
    station[i] = 0;

    memcpy(channel, seedrec->channel_id, 3);
    channel[3] = 0;

    memcpy(location, seedrec->location_id, 2);
    location[2] = 0;

    // We write to archive first to give archive max chance of being complete
    retmsg2=NULL;
    if (!CheckNoArchive(station, channel, location))
    {
        retmsg2 = WriteChan(station, channel, location, record);
        to |= 1;
    }

    // Make sure channel isn't listed with a NoIDA keyword in diskloop.config
    retmsg1=NULL;
    if (!CheckNoIDA(station, channel, location))
    {
        retmsg1 = idaWriteChan(station, channel, location, record, station);
        to |= 2;
    }

    switch (to) {
        case 0:
            toMessage = "Neither";
            break;
        case 1:
            toMessage = "Archive Only";
            break;
        case 2:
            toMessage = "IDA Diskloop Only";
            break;
        case 3:
            toMessage = "Both";
            break;
    }

    if (g_bDebug)
    {
        fprintf(stderr, "DEBUG archd wrote record %6.6s %5.5s-%2.2s/%3.3s to %s\n",
                &record[0], &record[8], &record[13], &record[15], toMessage);
    }
    if (retmsg1 != NULL)
        return retmsg1;
    else
        return retmsg2;
} // ArchiveSeed()

//////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
    archd_context_t *archd;

    char  station[8];
    char  loc[4];
    char  chan[4];
    char  network[4];
    char  *retmsg;
    int   msgId;
    int   result;
    int   iSeedRecordSize; // TODO: switch to archd->record_size
    int   iClient;
    int   iBuf;
    int   i,j,k;
    int   iDeltaTime;
    int   year, doy, hour, min, sec;
    int   touched;
    int   iWriteIndexArch;

    //pthread_t server_tid;
    char   *initMsg;
    char   queuebuf[8192];
    char   tempbuf[8192];
    char   *queuemsg=NULL; 
    char   tempMsg[64];
    char   *bufPtr, *msgPtr, c;
    time_t  queuetimetag=0;

    pcomm_result_t pcomm_result;

    if ((archd = malloc(sizeof(archd_context_t))) == NULL) {
        fprintf(stderr, "Could not allocate memory for archive daemon (%s) context\n", WHOAMI, retmsg);
    }

    if ((archd->pcomm = malloc(sizeof(pcomm_context_t))) == NULL) {
        fprintf(stderr, "Could not allocate memory for pcomm context\n", WHOAMI, retmsg);
    }

    g_archd = archd;
    archd->timeout.tv_sec = (long)ASYNC_TIMEOUT_SECONDS;
    archd->timeout.tv_sec = (long)ASYNC_TIMEOUT_MICROSECONDS;
    archd->server_port = -1;
    archd->server_socket = -1;
    archd->client_count = 0;
    archd->debug = 0;
    archd->record_size = 512;

    prioqueue_init(&archd->record_queue);

    // Check for right number of arguments
    if (argc != 2 && argc != 3)
    {
        ShowUsage();
        exit(1);
    }

    // Parse the configuration file
    if ((retmsg=ParseDiskLoopConfig(argv[1])) != NULL)
    {
        fprintf(stderr, "%s: %s\n", WHOAMI, retmsg);
        //syslog(LOG_ERR, "%s: %s", WHOAMI, retmsg);
        exit(1);
    }
    // Retrieve the record size from the config file
    LoopRecordSize(&iSeedRecordSize);
    archd->record_size = iSeedRecordSize;
    // Retrieve the initial station name for the IDA diskloop
    LogSNCL(station, network, chan, loc);
    strncpy(archd->ida_name, station, 5);
    archd->ida_name[5] = 0;

    // Handle command line debug request 
    g_bDebug = 0;
    if (argc == 3)
    {
        if (strcmp(argv[2], "debug") != 0)
        {
            printf("Unknown keyword '%s'\n", argv[2]);
            ShowUsage();
            exit(1);
        }
        g_bDebug = 1;
    } // command line arguments request running server in debug mode
    archd->debug = g_bDebug;

    // Run this program as a daemon unless requested otherwise by user
    if (!archd->debug)
    {
        daemonize();
    }
    signal(SIGTERM, sigterm_archd); // Gracefuly shutdown

    // Let user know what station we are archiving for
    if (archd->debug)
        fprintf (stderr, "%s archive name %s\n", WHOAMI, archd->ida_name);
    else
        syslog (LOG_INFO, "%s archive name %s", WHOAMI, archd->ida_name);

    // Ignore SIGPIPE when a connection is closed
    signal (SIGPIPE, SIG_IGN);

    // IDA initialization
    initMsg = idaInit(station, WHOAMI);

    if (initMsg != NULL) {
        if (archd->debug) {
            fprintf(stderr, "%s:idaInit(): %s\n",
                    WHOAMI, strerror(errno));
        }
        else {
            syslog(LOG_ERR, "%s:idaInit(): %s",
                    WHOAMI, strerror(errno));
        }
    }

    // Start the server listening for clients in the background
    LogServerPort(&archd->server_port);
    if (pcomm_result = pcomm_init(archd->pcomm))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:pcomm_init(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_init(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    }
    else if (pcomm_result = pcomm_set_external_context(archd->pcomm, archd))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:pcomm_set_external_context(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_set_external_context(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    }
    else if (!init_server(archd)) { // if successful, sets archd->server_socket
        if (archd->debug) {
            fprintf(stderr, "%s:init_server(): failed to start server", WHOAMI);
        } else {
            syslog(LOG_ERR, "%s:init_server(): failed to start server", WHOAMI);
        }
    }
    else if (pcomm_result =  pcomm_monitor_read_fd(archd->pcomm,
                                                   archd->server_socket,
                                                   callback_connect_request,
                                                   callback_server_closed))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:pcomm_monitor_read_fd(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_monitor_read_fd(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    }
    else if (pcomm_result = pcomm_set_timeout(archd->pcomm, &archd->timeout)) {
        if (archd->debug) {
            fprintf(stderr, "%s:pcomm_set_timeout(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_set_timeout(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    } 
    else if (pcomm_result = pcomm_set_timeout_callback(archd->pcomm, callback_async_timeout)) {
        if (archd->debug) {
            fprintf(stderr, "%s:pcomm_set_timeout_callback(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_set_timeout_callback(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    } 
    // archive all available records before receiving more
    else if (pcomm_result = pcomm_set_prepare_callback(archd->pcomm, callback_archive)) {
        if (archd->debug) {
            fprintf(stderr, "%s:pcomm_set_select_callback(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_set_select_callback(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    } 
    else {
        // RUN pcomm main loop
        pcomm_main(archd->pcomm);
    }

    // CLEAN up upon completion
    if (archd) {
        if (archd->pcomm) {
            pcomm_destroy(archd->pcomm);
            free(archd->pcomm);
        }
        free(archd);
        g_archd = archd = NULL;
    }

} // main()

/* Creates the server socket, binds, and starts listening for new clients. */
int init_server (archd_context_t *archd)
{
    //archd->server_socket = bind(archd->server_port);
    int   i;
    char  *errstr;
    struct sockaddr_in listen_addr; /* Listen address setup */

    if ((errstr=LoopRecordSize(&iSeedRecordSize)) != NULL)
    {
        fprintf(stderr, "%s\n", errstr);
        return 0;
    }

    if (archd->debug) {
        printf("init_server(): port=%d recLen=%d\n",
                archd->server_port, iSeedRecordSize);
    }

    /* Create the socket we will listen on error */
    archd->server_socket = socket(AF_INET, SOCK_STREAM, 0); /* TCP */
    if (archd->server_socket<0)
    {
        fprintf(stderr,"init_server: Cannot open socket (err %d,%d)\n",
                errno,archd->server_socket);
        return 0;
    }

    /* Set up structure and bind to port number */
    // Only local programs can connect via loopback address
    listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(archd->server_port);

    // Loop forever waiting for bind to succeed
    while (1)
    {

        if (bind( archd->server_socket,
                  (struct sockaddr *) &listen_addr, 
                  sizeof(listen_addr) ) < 0)
        {
            if (errno==EADDRINUSE)
            {
                if (archd->debug)
                    fprintf(stderr,"Port %d in use? Trying again in %d sec\n",
                            archd->server_port, (int)BIND_RETRY_INTERVAL);
                else
                    syslog(LOG_INFO,"Port %d in use? Trying again in %d sec",
                            archd->server_port, (int)BIND_RETRY_INTERVAL);
                sleep(BIND_RETRY_INTERVAL);
                continue;
            }
            fprintf(stderr,"Cannot bind (err %d - %s)\n",errno,strerror(errno));
            return 0;
        }
        break;
    } // infinite loop

    // Set up listener
    if (listen(archd->server_socket, MAX_CLIENTS) < 0) {
        fprintf(stderr,"dnetport: cannot set listen depth to %d (%d)\n",
                MAX_CLIENTS, errno);
        return 0;
    }
    return 1;
}

/* performs archive operations for all buffered records */
void callback_archive (pcomm_context_t *pcomm)
{
    archd_context_t *archd;
    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
}

/* May eventually do something special for timeouts.
 * For now, just call the archive function.
 */
void callback_async_timeout (pcomm_context_t *pcomm)
{
    callback_archive(pcomm);
}

/* Handles new connection requests, adding new file descriptors to pcomm
 * for new clients.
 */
void callback_connect_request (pcomm_context_t *pcomm, int fd)
{
    struct sockaddr_in address;
    archd_context_t *archd;
    pcomm_result_t pcomm_result;
    int client_sock;
    unsigned int addr_len = sizeof(struct sockaddr_in);
    client_context_t *client = NULL;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);

    if (archd->client_count >= MAX_CLIENTS) {
        fprintf(stderr, "Maximum number of client reached, rejecting\n");
        return;
    }

    if ((client = (client_context_t *)malloc(sizeof(client_context_t))) == NULL) {
        if (archd->debug) {
            fprintf(stderr, "%s: failed to allocate memory for client context", WHOAMI);
        } else {
            syslog(LOG_ERR, "%s: failed to allocate memory for client context", WHOAMI);
        }
        return;
    }
    client->received = 0;
    client->confirmed = 0;
    client->reply_length = 0;

    // Accept a new connection request
    if (archd->debug)
    {
        printf("Accepting connect request (port %d)\n", archd->server_port);
    }

    client_sock = accept(archd->server_socket, (struct sockaddr *)(&address), &addr_len);
    if (client_sock < 0)
    {
        if (archd->debug) {
            fprintf(stderr,"accept failed (%d,%d)\n", errno, client_sock);
        } else {
            syslog(LOG_ERR,"accept failed (%d,%d)\n", errno, client_sock);
        }
        return;
    }

    if (getpeername(client_sock, (struct sockaddr *)(&address), &addr_len) < 0)
    {
        if (archd->debug) {
            fprintf(stderr,"BAD ERROR encountered - error %d - %s\n", errno, strerror(errno));
        } else {
            syslog(LOG_ERR,"BAD ERROR encountered - error %d - %s\n", errno, strerror(errno));
        }
        close(client_sock);
        return;
    }

    if (pcomm_result = pcomm_monitor_read_fd(archd->pcomm,
                                             client_sock,
                                             callback_client_can_recv,
                                             callback_client_closed))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:pcomm_monitor_read_fd(): %s\n",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_monitor_read_fd(): %s\n",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
        return;
    }
    if (pcomm_result = pcomm_set_external_fd_context(archd->pcomm,
                                                     client_sock,
                                                     PCOMM_STREAM_READ,
                                                     client))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:pcomm_monitor_read_fd(): %s\n",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_monitor_read_fd(): %s\n",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
        return;
    }
    archd->client_count++;

    if (archd->debug) {
        printf("New client at socket %d (%d of %d available)\n",
               client_sock, archd->client_count, MAX_CLIENTS);
    } else {
        syslog(LOG_INFO, "New client at socket %d (%d of %d available)\n",
               client_sock, archd->client_count, MAX_CLIENTS);
    }
}

/* Stubb for handling server disconnects.
 */
void callback_server_closed (pcomm_context_t *pcomm, int fd)
{
    archd_context_t *archd = NULL;
    client_context_t *client = NULL;
    pcomm_result_t pcomm_result = 0;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
    client = (client_context_t *)pcomm_get_external_fd_context(pcomm, PCOMM_STREAM_READ, fd);
    if (archd->debug) {
        fprintf(stderr, "%s:callback_server_closed(): server socket closed", WHOAMI); 
    }
    else {
        syslog(LOG_ERR, "%s:callback_server_closed(): server socket closed", WHOAMI); 
    }

    pcomm_result = pcomm_stop(pcomm, 1/*immediately*/);

    // TODO: attempt to re-connect instead of just halting
}

/* check how many bytes are buffered on a socket */
long bytes_on_socket(int socket) {
    size_t bytes_available = 0;
    if ( ioctl(socket, FIONREAD, (char *)&bytes_available) < 0 ) {
        return -1;
    }
    return((long)bytes_available);
}

/* Handles new data from the client, adding it to the queue, and adding to the archive
 * queue once a complete record has been assembled.
 */
void callback_client_can_recv (pcomm_context_t *pcomm, int fd)
{
    archd_context_t *archd = NULL;
    client_context_t *client = NULL;
    pcomm_result_t pcomm_result = 0;
    ssize_t bytes_received = 0;
    size_t bytes_waiting = 0;
    uint8_t *tempBuffer = NULL;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
    client = (client_context_t *)pcomm_get_external_fd_context(pcomm, PCOMM_STREAM_READ, fd);
    bytes_waiting = bytes_on_socket(fd);

    while ((bytes_waiting >= archd->record_size) &&
           ((REPLY_BUFFER_SIZE - client->reply_length) >= REPLY_MESSAGE_SIZE)) 
    {
        if ((tempBuffer = (uint8_t *)malloc(archd->record_size)) == NULL) {
            if (archd->debug) {
                fprintf(stderr, "%s:malloc(): could not allocate memory for record", WHOAMI);
            } else {
                syslog(LOG_ERR, "%s:malloc(): could not allocate memory for record", WHOAMI);
            }
            // Wait for the memory to be freed by the process
            break;
        }
        bytes_received = recv(fd, tempBuffer, archd->record_size, MSG_DONTWAIT);
        if (bytes_received < archd->record_size) {
            if (archd->debug) {
                fprintf(stderr, "%s: ioctl() reported more than available. Exiting now!", WHOAMI);
            } else {
                syslog(LOG_ERR, "%s: ioctl() reported more than available. Exiting now!", WHOAMI);
            }
            pcomm_stop(pcomm, 1/*immediately*/);
            return;
        }

        prioqueue_add(&archd->record_queue, tempBuffer, archd->record_size);

        memcpy(client->reply_buffer + client->reply_length,
               tempBuffer + 5, REPLY_MESSAGE_SIZE);
        client->received++;
        client->reply_length += REPLY_MESSAGE_SIZE;

        tempBuffer = NULL;
        bytes_waiting -= bytes_received;
    }

    if (client->reply_length > 0) {
        if (pcomm_result = pcomm_monitor_write_fd(pcomm, fd,
                                                  callback_client_can_send,
                                                  callback_client_closed))
        {
            if (archd->debug) {
                fprintf(stderr, "%s:pcomm_monitor_write_fd(): %s",
                        WHOAMI, pcomm_strresult(pcomm_result));
            } else {
                syslog(LOG_ERR, "%s:pcomm_monitor_write_fd(): %s",
                        WHOAMI, pcomm_strresult(pcomm_result));
            }
            return;
        }
    }
}

/* Handles sending of confirmations to the client. */
void callback_client_can_send (pcomm_context_t *pcomm, int fd)
{
    archd_context_t *archd = NULL;
    client_context_t *client = NULL;
    ssize_t bytes_sent = 0;
    size_t total_sent = 0;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
    client = (client_context_t *)pcomm_get_external_fd_context(pcomm, PCOMM_STREAM_READ, fd);

    while (client->reply_length > REPLY_MESSAGE_SIZE) {
        bytes_sent = send(fd, client->reply_buffer, REPLY_MESSAGE_SIZE, MSG_DONTWAIT);
        if (bytes_sent < REPLY_MESSAGE_SIZE ) {
            break;
        } else {
            client->confirmed++;
            total_sent += bytes_sent;
        }
    }

    client->reply_length -= total_sent;
    if (client->reply_length > 0) {
        // Update the contents of the clients reply buffer, but only if
        // there is still data in the buffer, otherwise we let it be
        // automatically recycled.
        memmove(client->reply_buffer, client->reply_buffer + bytes_sent, client->reply_length);
    }
    else {
        // Don't monitor this descriptor if there are not writes waiting
        pcomm_remove_write_fd(pcomm, fd);
    }
}

/* Frees up the client context resource. */
void callback_client_closed (pcomm_context_t *pcomm, int fd)
{
    archd_context_t *archd = NULL;
    client_context_t *client = NULL;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);

    if (archd->debug) {
        fprintf(stderr, "%s: client closed socket %d\n", WHOAMI, fd);
    } else {
        syslog(LOG_ERR, "%s: client closed socket %d\n", WHOAMI, fd);
    }

    client = (client_context_t *)pcomm_get_external_fd_context(pcomm, PCOMM_STREAM_READ, fd);

    if (client) {
        free(client);
    }

    pcomm_remove_read_fd(pcomm, fd);
    pcomm_remove_write_fd(pcomm, fd);
}

