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
#include "libtree/libtree.h"
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
void callback_async_timeout (pcomm_context_t *pcomm);
void callback_connect_request (pcomm_context_t *pcomm, int fd);
void callback_server_closed (pcomm_context_t *pcomm, int fd);
void callback_client_data (pcomm_context_t *pcomm, int fd, uint8_t *data, size_t length);
void callback_client_closed (pcomm_context_t *pcomm, int fd);

// Make the server_pid global so it can be killed when program is shut down
int   server_pid=0;
int   g_bDebug=0;
char  g_sIDAname[6];
static struct s_mapshm *mapshm=NULL;
static struct s_mapstatus *mapstatus=NULL;
static archd_context_t *archd=NULL;
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
    if ((archd != NULL) && (archd->pcomm != NULL)) {
        // Tell pcomm to finish processing all data
        pcomm_stop(archd->pcomm, 0/*not immediately*/);
    }
    else {
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
    char  station[8];
    char  loc[4];
    char  chan[4];
    char  network[4];
    char  *retmsg;
    int   msgId;
    int   result;
    int   iSeedRecordSize;
    int   iPort;
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

    archd->timeout.tv_sec = (long)ASYNC_TIMEOUT_SECONDS;
    archd->timeout.tv_sec = (long)ASYNC_TIMEOUT_MICROSECONDS;
    archd->server_port = -1;
    archd->server_socket = -1;
    archd->client_count = 0;
    archd->debug = 0;

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
    LoopRecordSize(&iSeedRecordSize);
    LogSNCL(station, network, chan, loc);
    strncpy(g_sIDAname, station, 5);
    g_sIDAname[5] = 0;

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

    // Run this program as a daemon unless requested otherwise by user
    if (!g_bDebug)
    {
        daemonize();
    }
    signal(SIGTERM, sigterm_archd); // Gracefuly shutdown

    // Let user know what station we are archiving for
    if (g_bDebug)
        fprintf (stderr, "%s archive name %s\n", WHOAMI, g_sIDAname);
    else
        syslog (LOG_INFO, "%s archive name %s", WHOAMI, g_sIDAname);

    // Ignore SIGPIPE when a connection is closed
    signal (SIGPIPE, SIG_IGN);

    // IDA initialization
    initMsg = idaInit(station, WHOAMI);

    if (initMsg != NULL) {
        if (g_bDebug) {
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
    archd->server_port = iPort;
    archd->debug = g_bDebug;
    if (pcomm_result = pcomm_init(archd->pcomm))
    {
        if (g_bDebug) {
            fprintf(stderr, "%s:pcomm_init(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_init(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    }
    else if (pcomm_result = pcomm_set_external_context(archd->pcomm, archd))
    {
        if (g_bDebug) {
            fprintf(stderr, "%s:pcomm_set_external_context(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_set_external_context(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    }
    else if (!init_server(archd)) {
        if (g_bDebug) {
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
        if (g_bDebug) {
            fprintf(stderr, "%s:pcomm_monitor_read_fd(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_monitor_read_fd(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    }
    else if (pcomm_result = pcomm_set_timeout(archd->pcomm, &archd->timeout)) {
        if (g_bDebug) {
            fprintf(stderr, "%s:pcomm_set_timeout(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_set_timeout(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
    } 
    else if (pcomm_result = pcomm_set_timeout_callback(archd->pcomm, callback_async_timeout)) {
        if (g_bDebug) {
            fprintf(stderr, "%s:pcomm_set_timeout_callback(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_set_timeout_callback(): %s",
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
    }

} // main()

/* Creates the server socket, binds, and starts listening for new clients.
 * XXX: We need to allocate a buffer for each new connection associated with
 *      its file descriptor
 */
int init_server(archd_context_t *archd) {
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

/* TODO: populate this function with tasks that should be performed after 
 *       a comm timeout.
 */
void callback_async_timeout(pcomm_context_t *pcomm) {
    archd_context_t *archd;
    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
}

/* Handles new connection requests, adding new file descriptors to pcomm
 * for new clients.
 * XXX: We need to allocate a buffer for each new connection associated with
 *      its file descriptor
 */
void callback_connect_request(pcomm_context_t *pcomm, int fd) {
    struct sockaddr_in address;
    archd_context_t *archd;
    pcomm_result_t pcomm_result;
    int client_sock;
    unsigned int addr_len = sizeof(struct sockaddr_in);
    int client;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);

    if (archd->client_count >= MAX_CLIENTS) {
        fprintf(stderr, "Maximum number of client reached, rejecting\n");
        return;
    }

    // Accept a new connection request
    if (archd->debug)
    {
        printf("Accepting connect request (port %d)\n", archd->server_port);
    }

    client_sock = accept(archd->server_socket, (struct sockaddr *)(&address), &addr_len);
    if (client_sock<0)
    {
        fprintf(stderr,"accept failed (%d,%d)\n", errno, client_sock);
        return;
    }

    if (getpeername(client_sock, (struct sockaddr *)(&address), &addr_len) < 0)
    {
        fprintf(stderr,"BAD ERROR encountered - error %d - %s\n", errno, strerror(errno));
        close(client_sock);
        return;
    }

    // FOR EACH FD ADDITION
    //pcomm_add_write_fd(archd->pcomm, <fd>, <data>, <length>, <io_callback>, <close_callback>);
    //pcomm_add_(read|error)_fd(archd->pcomm, <fd>, <io_callback>, <close_callback>);

    if (pcomm_result = pcomm_add_read_fd(archd->pcomm,
                                         client_sock,
                                         callback_client_data,
                                         callback_client_closed))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:pcomm_add_read_fd(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:pcomm_add_read_fd(): %s",
                    WHOAMI, pcomm_strresult(pcomm_result));
        }
        return;
    }
    archd->client_count++;

    if (archd->debug) {
        printf("New client at socket %d (%d of %d available)\n",
               client_sock, archd->client_count, MAX_CLIENTS);
    }
}

/* Stubb for handling server disconnects.
 */
void callback_server_closed (pcomm_context_t *pcomm, int fd) {
    // TODO: attempt to re-connect
    pcomm_stop(pcomm, 1/*immediately*/);
}

/* Handles new data from the client, adding it to the queue, and adding to the archive
 * queue once a complete record has been assembled.
 * TODO: We need a way to buffer data for each connection to perform the above
 * TODO: Create an AVL tree based priority queue
 */
void callback_client_data (pcomm_context_t *pcomm, int fd, uint8_t *data, size_t length) {
    archd_context_t *archd;
    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
    // TODO: add to the priority queue
    ;
}

/* Handles the closing of a client connection, and perform the necessary 
 * clean up.
 * XXX: We need to flush, and remove the buffers for a connection when it is
 *      removed
 */
void callback_client_closed (pcomm_context_t *pcomm, int fd) {
    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
    ;
}


/*
void old_logic () {

    // ========================================
    //            OLD LOGIC BELOW
    // ========================================

    mapshm->iPort = iPort;
    mapshm->bDebug = g_bDebug;
    if (pthread_create(&server_tid, NULL, StartServer, (void *)mapshm))
    {
        if (g_bDebug)
            fprintf(stderr, "%s:main pthread_create StartServer: %s\n",
                    WHOAMI, strerror(errno));
        else
            syslog(LOG_ERR, "%s:main pthread_create StartServer: %s",
                    WHOAMI, strerror(errno));
        exit(1);
    }
    pthread_detach(server_tid);
    mapshm->listen_tid = server_tid;

    // Infinite loop waiting for buffers to get filled up
    iClient = 0;
    touched = 0;
    while (1)
    {
        // Handle case where quit flag has been set
        if (mapshm->bQuit)
        {
            if (mapshm->listen_tid)
            {
                if (mapshm->bDebug)
                    fprintf(stderr, "Quit flag detected, init shutdown %s\n", WHOAMI);

                pthread_kill(mapshm->listen_tid, 15);
                touched = 1;
                mapshm->listen_tid = 0;
            }
            for (i=0; i<MAX_CLIENTS; i++)
            {
                if (mapshm->client_tid[i] > 0)
                {
                    touched = 1;
                    pthread_kill(mapshm->client_tid[i], 15);
                    mapshm->client_tid[i] = 0;
                }
            }
        } // quit flag was set

        // Notice if we don't get any data this loop
        if (!touched)
        {
            if (mapshm->bQuit)
            {
                if (g_bDebug)
                    fprintf(stderr, "Exiting %s via quit flag\n", WHOAMI);
                else
                    syslog(LOG_INFO, "Exiting %s", WHOAMI);
                exit(EXIT_SUCCESS);
            }
            usleep(10000);
        }
        touched = 0;

        // Update watchdog timer so dispstatus knows we are alive
        // First determine which ixWriteArch index to use
        if (mapstatus->ixWriteArch != 0 && mapstatus->ixReadArch != 0)
            iWriteIndexArch = 0;
        if (mapstatus->ixWriteArch != 1 && mapstatus->ixReadArch != 1)
            iWriteIndexArch = 1;
        else
            iWriteIndexArch = 2;
        // Save the current time, let others know where updated time is stored
        mapstatus->archWatchdog[iWriteIndexArch] = ST_GetCurrentTime2();
        mapstatus->ixWriteArch = iWriteIndexArch;

        // Check for new messages
        for (i=0; i < MAX_CLIENTS; i++)
        {
            iClient = (iClient+1) % MAX_CLIENTS;
            iBuf = mapshm->read_index[iClient];
            if (mapshm->read_index[iClient] != mapshm->write_index[iClient])
            {
                touched = 1;

                // Handle channel control commands
                if (strncmp("CHANNELCONTROL-", &mapshm->buffer[iClient][iBuf][0], 15) == 0)
                {
                    bufPtr = &mapshm->buffer[iClient][iBuf][0];
                    msgPtr = tempMsg;
                    for (k=0; k < 63; k++, bufPtr++, msgPtr++) {
                        c = *bufPtr;
                        if (c == 0) {
                            break;
                        }
                        else if (k > 60) {
                            c = '.';
                        }
                        else if (!isprint(c)) {
                            c = '*';
                        }
                        *msgPtr = c;
                    }
                    *msgPtr = 0;

                    result = ChannelControl((char *)&mapshm->buffer[iClient][iBuf], &msgId);
                    if (result < 1) {
                        if (result == 0) {
                            if (g_bDebug)
                                fprintf(stderr, "%s: Failed to apply channel control: '%s'\n", WHOAMI, tempMsg);
                            else
                                syslog(LOG_ERR, "%s: Failed to apply channel control: '%s'", WHOAMI, tempMsg);
                            mapshm->result[iClient][0] = msgId;
                            mapshm->result[iClient][1] = RESULT_CHAN_CMD_FAIL; 
                        } else {
                            if (g_bDebug)
                                fprintf(stderr, "%s: Invalid channel control message: '%s'\n", WHOAMI, tempMsg);
                            else
                                syslog(LOG_ERR, "%s: Invalid channel control message: '%s'", WHOAMI, tempMsg);
                            mapshm->result[iClient][0] = -1;
                            mapshm->result[iClient][1] = RESULT_CHAN_CMD_INVALID; 
                        }
                    }
                    else {
                        if (g_bDebug)
                            fprintf(stderr, "%s: Channel control applied: '%s'\n", WHOAMI, tempMsg);
                        else
                            syslog(LOG_ERR, "%s: Channel control applied: '%s'", WHOAMI, tempMsg);
                        mapshm->result[iClient][0] = msgId;
                        mapshm->result[iClient][1] = RESULT_CHAN_CMD_OKAY; 
                    }
                    mapshm->write_index[iClient] = iBuf;
                    continue;
                } // Channel control command

                // Immediately archive non LOG seed record
                if (strncmp("LOG", &mapshm->buffer[iClient][iBuf][15], 3) != 0)
                {
                    if ((retmsg=ArchiveSeed((char *)&mapshm->buffer[iClient][iBuf]) )
                            != NULL)
                    {
                        fprintf(stderr, "%s: %s\n", WHOAMI, retmsg);
                    }
                    // Indicate that we have processed message
                    mapshm->write_index[iClient] = iBuf;
                    continue;
                } // Not a LOG record

                // See if there is a message waiting to be sent out
                if (queuemsg != NULL)
                {
                    // Try to combine the two records to save space
                    if (CombineSeed(queuemsg, mapshm->buffer[iClient][iBuf],
                                iSeedRecordSize))
                    {
                        // Indicate that we have processed message and that's all
                        mapshm->write_index[iClient] = iBuf;
                        continue;
                    }
                    else
                    {
                        // Send queued message since we can't combine the two
                        SeedRecordMsg(tempbuf, queuebuf, station, network, chan, loc,
                                &year, &doy, &hour, &min, &sec);
                        if ((retmsg=ArchiveSeed(queuemsg)) != NULL)
                        {
                            fprintf(stderr, "archd: %s\n", retmsg);
                        }
                        queuemsg = NULL;
                    } // combine failed
                } // we have a waiting message to possibly combine with

                // Copy new message to free up shared memory buffer
                memcpy(queuebuf, mapshm->buffer[iClient][iBuf], iSeedRecordSize);
                queuemsg = queuebuf;
                queuetimetag = mapshm->timetag[iClient];

                // Indicate that we have processed message
                mapshm->write_index[iClient] = iBuf;
            } // new message from this client
        } // check each possible client

        // See if oldest message is more than BUFFER_TIME_SEC old
        // Or if this is not a log message
        iDeltaTime = time(NULL) - queuetimetag;
        if (queuemsg != NULL &&
                (iDeltaTime < 0 || iDeltaTime >= BUFFER_TIME_SEC))
        {
            SeedRecordMsg(tempbuf, queuebuf, station, network, chan, loc,
                    &year, &doy, &hour, &min, &sec);
            if ((retmsg=ArchiveSeed(queuemsg)) != NULL)
            {
                fprintf(stderr, "archd: %s\n", retmsg);
            }
            queuemsg = NULL;
        } // timeout elapsed

    } // loop forever

}
*/

