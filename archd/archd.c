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

#define ASYNC_TIMEOUT_SECONDS       5
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
static struct s_mapstatus *mapstatus=NULL;
static archd_context_t *g_archd=NULL;
//static int iSeedRecordSize;

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
    int   iClient;
    int   iBuf;
    int   i,j,k;

    struct timeval now;
    int   year, doy, hour, min, sec;
    int   touched;

    char   *initMsg;

    pcomm_result_t pcomm_result;

    if ((archd = malloc(sizeof(archd_context_t))) == NULL) {
        fprintf(stderr, "%s: could not allocate memory for archd context\n", WHOAMI);
        exit(1);
    }

    if ((archd->pcomm = calloc(sizeof(pcomm_context_t), 1)) == NULL) {
        fprintf(stderr, "%s: could not allocate memory for pcomm context\n", WHOAMI);
        exit(1);
    }

    g_archd = archd;
    archd->tz_info.tz_minuteswest = 0;
    archd->tz_info.tz_dsttime = 0;
    archd->timeout.tv_sec = ASYNC_TIMEOUT_SECONDS;
    archd->timeout.tv_usec = ASYNC_TIMEOUT_MICROSECONDS;
    archd->status_map = NULL;
    archd->write_index = 0;
    archd->log_message = NULL;
    archd->server_port = -1;
    archd->server_socket = -1;
    archd->client_count = 0;
    archd->debug = 0;
    archd->record_size = 512;
    prioqueue_init(&archd->record_queue);
    archd->records_received = 0LL;
    archd->records_archived = 0LL;
    archd->log_record_count = 0LL;

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
    LoopRecordSize(&archd->record_size);
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
    if (!archd->debug) {
        daemonize();
    }
    signal(SIGTERM, sigterm_archd); // Gracefuly shutdown

    // Report the station for which the default IDA diskloop will be opened
    if (archd->debug) {
        fprintf (stderr, "%s archive name %s\n", WHOAMI, archd->ida_name);
    } else {
        syslog (LOG_INFO, "%s archive name %s", WHOAMI, archd->ida_name);
    }

    // Ignore SIGPIPE when a connection is closed
    signal (SIGPIPE, SIG_IGN);

    // Open the dispstatus shared memory region
    retmsg = MapStatusMem((void **)&mapstatus);
    archd->status_map = mapstatus;
    if (retmsg != NULL)
    {
        if (archd->debug)
            fprintf (stderr, "Error loading dispstatus shared memory: %s\n",
                    retmsg);
        else
            syslog (LOG_ERR, "Error loading dispstatus shared memory: %s",
                    retmsg);
        exit(1);
    }

    // IDA initialization
    initMsg = idaInit(station, WHOAMI);

    if (initMsg != NULL) {
        if (archd->debug) {
            fprintf(stderr, "%s:%d> idaInit(): %s\n", WHOAMI, __LINE__, strerror(errno));
        } else {
            syslog(LOG_ERR, "%s:%d> idaInit(): %s", WHOAMI, __LINE__, strerror(errno));
        }
        exit(1);
    }

    // Start the server listening for clients in the background
    LogServerPort(&archd->server_port);
    if (pcomm_result = pcomm_init(archd->pcomm))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:%d> pcomm_init(): %s\n",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:%d> pcomm_init(): %s",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        }
    }
    else if (pcomm_result = pcomm_set_external_context(archd->pcomm, archd))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:%d> pcomm_set_external_context(): %s\n",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:%d> pcomm_set_external_context(): %s",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        }
    }
    else if (!init_server(archd)) { // if successful, sets archd->server_socket
        if (archd->debug) {
            fprintf(stderr, "%s:%d> init_server(): failed to start server\n", WHOAMI, __LINE__);
        } else {
            syslog(LOG_ERR, "%s:%d> init_server(): failed to start server", WHOAMI, __LINE__);
        }
    }
    else if (pcomm_result = pcomm_monitor_read_fd(archd->pcomm,
                                                   archd->server_socket,
                                                   callback_connect_request))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:%d> pcomm_monitor_read_fd(): %s\n",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:%d> pcomm_monitor_read_fd(): %s",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        }
    }
    else if (pcomm_result = pcomm_set_timeout(archd->pcomm, &archd->timeout)) {
        if (archd->debug) {
            fprintf(stderr, "%s:%d> pcomm_set_timeout(): %s\n",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:%d> pcomm_set_timeout(): %s",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        }
    } 
    else if (pcomm_result = pcomm_set_timeout_callback(archd->pcomm, callback_async_timeout)) {
        if (archd->debug) {
            fprintf(stderr, "%s:%d> pcomm_set_timeout_callback(): %s\n",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:%d> pcomm_set_timeout_callback(): %s",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        }
    } 
    // archive all available records before receiving more
    else if (pcomm_result = pcomm_set_prepare_callback(archd->pcomm, callback_archive)) {
        if (archd->debug) {
            fprintf(stderr, "%s:%d> pcomm_set_select_callback(): %s\n",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:%d> pcomm_set_select_callback(): %s",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        }
    } 
    else {
        pcomm_set_debug(archd->pcomm, archd->debug);

        if (archd->debug) {
            fprintf(stdout, "\n");
            fprintf(stdout, "archd -> pcomm = 0x%0lx\n", archd->pcomm);
            fprintf(stdout, "archd -> timeout\n");
            fprintf(stdout, "           -> tv_sec = %0d\n", archd->timeout.tv_sec);
            fprintf(stdout, "           -> tv_usec = %0d\n", archd->timeout.tv_usec);
            fprintf(stdout, "archd -> status_map = 0x%0lx\n", archd->status_map);
            fprintf(stdout, "archd -> write_index = %0d\n", archd->write_index);
            fprintf(stdout, "archd -> log_message = 0x%0lx\n", archd->log_message);
            fprintf(stdout, "archd -> server_port = %0d\n", archd->server_port);
            fprintf(stdout, "archd -> server_socket = %0d\n", archd->server_socket);
            fprintf(stdout, "archd -> client_count = %0lu\n", archd->client_count);
            fprintf(stdout, "archd -> record_queue = 0x%0lx\n", archd->record_queue);
            fprintf(stdout, "archd -> debug = %0d\n", archd->debug);
            fprintf(stdout, "archd -> record_size = %0d\n", archd->record_size);
            fprintf(stdout, "archd -> ida_name = '%s'\n", archd->ida_name);
            fprintf(stdout, "\n");
        }

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
    char  *error_message;
    struct sockaddr_in listen_addr; /* Listen address setup */

    if ((error_message = LoopRecordSize(&archd->record_size)) != NULL)
    {
        fprintf(stderr, "%s\n", error_message);
        return 0;
    }

    if (archd->debug) {
        printf("init_server(): port=%d recLen=%d\n", archd->server_port, archd->record_size);
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
        else if (archd->debug) {
            printf("init_server(): bind succeeded for port %d\n", archd->server_port);
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
    data_record_t *record;
    int control_id;

    int log_delta;
    struct timeval now;

    char  station[8];
    char  loc[4];
    char  chan[4];
    char  network[4];
    int   year, doy, hour, min, sec;

    //char  queuebuf[MAX_RECORD_SIZE];
    char  tempbuf[MAX_RECORD_SIZE];
    char *result_message;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);

    if (archd->debug) {
        fprintf(stdout, "%s: callback_archive()\n", WHOAMI);
        fprintf(stdout, "%s:   start - %lli records buffered\n", WHOAMI, archd->records_received - archd->records_archived);
        fprintf(stdout, "%s:   start - %lli log records processed\n", WHOAMI, archd->log_record_count);
    }

    while (record = (data_record_t *)prioqueue_pop_high(&archd->record_queue))
 // QUEUE ITERATOR LOOP
    {
        archd->records_archived++;
        // Immediately archive non LOG seed record
        if (strncmp("LOG", record->data + 15, 3) != 0)
        {
            if ((result_message = ArchiveSeed((char *)record->data)) != NULL)
            {
                fprintf(stderr, "%s: %s\n", WHOAMI, result_message);
            }
            // free up record resources
            free(record->data);
            free(record);
            continue;
        } // Not a LOG record
        archd->log_record_count++;

        // See if there is a message waiting to be sent out
        if (archd->log_message != NULL)
        {
            // Try to combine the two records to save space
            if (CombineSeed(archd->log_message->data, record->data, archd->record_size))
            {
                // Free up the record's resources
                free(record->data);
                free(record);
                continue;
            }
            else
            {
                // Send queued message since we can't combine the two
                SeedRecordMsg(tempbuf, archd->log_message->data,
                              station, network, chan, loc,
                              &year, &doy, &hour, &min, &sec);
                if ((result_message = ArchiveSeed(archd->log_message->data)) != NULL)
                {
                    fprintf(stderr, "%s: %s\n", WHOAMI, result_message);
                }
                archd->log_message = NULL;
            } // combine failed
        } // we have a waiting message to possibly combine with

        archd->log_message = record;

        // See if oldest message is more than BUFFER_TIME_SEC old
        // Or if this is not a log message
        gettimeofday(&now, &archd->tz_info);
        log_delta = now.tv_sec - archd->log_message->receive_time.tv_sec;
        if (archd->log_message != NULL &&
            (log_delta < 0 || log_delta >= BUFFER_TIME_SEC))
        {
            SeedRecordMsg(tempbuf, archd->log_message->data,
                          station, network, chan, loc,
                          &year, &doy, &hour, &min, &sec);
            if ((result_message = ArchiveSeed(archd->log_message->data)) != NULL)
            {
                fprintf(stderr, "%s: %s\n", WHOAMI, result_message);
            }
            archd->log_message = NULL;
        } // timeout elapsed

 // QUEUE ITERATOR LOOP
    }

    if (archd->debug) {
        fprintf(stdout, "%s:     end - %lli records buffered\n", WHOAMI, archd->records_received - archd->records_archived);
        fprintf(stdout, "%s:     end - %lli log records procssed\n", WHOAMI, archd->log_record_count);
    }
}

/* May eventually do something special for timeouts.
 * For now, just call the archive function.
 */
void callback_async_timeout (pcomm_context_t *pcomm)
{
    struct timeval now;
    struct timezone tz;
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;
    gettimeofday(&now, &tz);

    //fprintf(stderr, "%s: pcomm timed out (%d.%d)", WHOAMI, now.tv_sec, now.tv_usec);
    callback_archive(pcomm);
}

/* Handles new connection requests, adding new file descriptors to pcomm
 * for new clients.
 */
void callback_connect_request (pcomm_context_t *pcomm, int fd)
{
    struct sockaddr_in address;
    archd_context_t *archd;
    client_context_t *client = NULL;
    int client_sock;

    pcomm_result_t pcomm_result;
    unsigned int addr_len = sizeof(struct sockaddr_in);

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);

    if (archd->debug) {
        fprintf(stderr, "callback_connect_request()\n");
    }

    if (archd->client_count >= MAX_CLIENTS) {
        fprintf(stderr, "Maximum number of client reached, rejecting\n");
        return;
    }

    if ((client = (client_context_t *)malloc(sizeof(client_context_t))) == NULL) {
        if (archd->debug) {
            fprintf(stderr, "%s: failed to allocate memory for client context\n", WHOAMI);
        } else {
            syslog(LOG_ERR, "%s: failed to allocate memory for client context", WHOAMI);
        }
        return;
    }
    gettimeofday(&client->connect_time, &archd->tz_info);
    client->received = 0;
    client->confirmed = 0;
    prioqueue_init(&client->reply_queue);

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
            syslog(LOG_ERR,"accept failed (%d,%d)", errno, client_sock);
        }
        return;
    }

    if (getpeername(client_sock, (struct sockaddr *)(&address), &addr_len) < 0)
    {
        if (archd->debug) {
            fprintf(stderr,"BAD ERROR encountered - error %d - %s\n", errno, strerror(errno));
        } else {
            syslog(LOG_ERR,"BAD ERROR encountered - error %d - %s", errno, strerror(errno));
        }
        close(client_sock);
        return;
    }

    if (pcomm_result = pcomm_monitor_read_fd(archd->pcomm,
                                             client_sock,
                                             callback_client_can_recv))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:%d> pcomm_monitor_read_fd(): %s\n",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:%d> pcomm_monitor_read_fd(): %s",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        }
        return;
    }
    if (pcomm_result = pcomm_set_external_fd_context(archd->pcomm,
                                                     PCOMM_STREAM_READ,
                                                     client_sock,
                                                     client))
    {
        if (archd->debug) {
            fprintf(stderr, "%s:%d> pcomm_set_external_fd_context(): %s\n",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        } else {
            syslog(LOG_ERR, "%s:%d> pcomm_set_external_fd_context(): %s",
                    WHOAMI, __LINE__, pcomm_strresult(pcomm_result));
        }
        return;
    }
    archd->client_count++;

    if (archd->debug) {
        printf("New client at socket %d (%d of %d available)\n",
               client_sock, archd->client_count, MAX_CLIENTS);
    } else {
        syslog(LOG_INFO, "New client at socket %d (%d of %d available)",
               client_sock, archd->client_count, MAX_CLIENTS);
    }
}

/* Stubb for handling server disconnects.
 * This is not being used since detecting closes has not yet been worked out.
 */
void callback_server_closed (pcomm_context_t *pcomm, int fd)
{
    archd_context_t *archd = NULL;
    client_context_t *client = NULL;
    pcomm_result_t pcomm_result = 0;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
    client = (client_context_t *)pcomm_get_external_fd_context(pcomm, PCOMM_STREAM_READ, fd);
    if (archd->debug) {
        fprintf(stderr, "%s:callback_server_closed(): server socket closed\n", WHOAMI); 
    }
    else {
        syslog(LOG_ERR, "%s:callback_server_closed(): server socket closed", WHOAMI); 
    }

    pcomm_result = pcomm_stop(pcomm, 1/*immediately*/);

    // TODO: attempt to re-connect instead of just halting
}

/* check how many bytes are buffered on a socket */
long bytes_on_socket(int socket)
{
    size_t bytes_available = 0;
    if ( ioctl(socket, FIONREAD, (char *)&bytes_available) < 0 ) {
        return -1;
    }
    return((long)bytes_available);
}

/* allocates a new reply message structure containing the passed message */
reply_message_t *make_reply(uint8_t *message, size_t length)
{
    reply_message_t *reply = NULL;

    if ((reply = (reply_message_t *)malloc(sizeof(reply_message_t))) != NULL) {
        if ((reply->message = (uint8_t *)malloc(length)) == NULL) {
            free(reply);
            reply = NULL;
        }
        else {
            memcpy(reply->message, message, length);
        }
    }

    return reply;
}

/* Handles new data from the client, adding to the archive
 * queue once a complete record is available on the socket.
 */
void callback_client_can_recv (pcomm_context_t *pcomm, int fd)
{
    archd_context_t *archd = NULL;
    client_context_t *client = NULL;
    reply_message_t *reply = NULL;
    data_record_t *record = NULL;

    int result = 0;
    pcomm_result_t pcomm_result = 0;
    ssize_t bytes_received = 0;
    size_t bytes_waiting = 0;

    int   control_id;
    char  control[128];
    char  response[128];
    char *p_buf;
    char *p_msg;
    char c;
    int k;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
    client = (client_context_t *)pcomm_get_external_fd_context(pcomm, PCOMM_STREAM_READ, fd);
    bytes_waiting = bytes_on_socket(fd);

    if (archd->debug) {
        fprintf(stderr, "%s: %d bytes waiting for client %d\n", WHOAMI, bytes_waiting, fd);
    }
    while (bytes_waiting >= archd->record_size)
    {
        if ((record = (data_record_t *)malloc(sizeof(data_record_t))) == NULL) {
            if (archd->debug) {
                fprintf(stderr, "%s:malloc(): could not allocate memory for record\n", WHOAMI);
            } else {
                syslog(LOG_ERR, "%s:malloc(): could not allocate memory for record", WHOAMI);
            }
            // Wait for the memory to be freed by the process
            break;
        }
        if ((record->data = (uint8_t *)malloc(archd->record_size)) == NULL) {
            if (archd->debug) {
                fprintf(stderr, "%s:malloc(): could not allocate memory for record buffer\n", WHOAMI);
            } else {
                syslog(LOG_ERR, "%s:malloc(): could not allocate memory for record buffer", WHOAMI);
            }
            // Wait for the memory to be freed by the process
            break;
        }
        bytes_received = recv(fd, record->data, archd->record_size, MSG_DONTWAIT);

        if (bytes_received > 0) {
            bytes_waiting -= bytes_received;

            // Update the Watchdog for the status display
            // Save the current time, let others know where updated time is stored
            // TODO: Keep an eye on this. We may need to fix this logic if the 
            //       watchdog doesn't work as I expect it should.
            archd->status_map->archWatchdog[0] = ST_GetCurrentTime2();
            archd->status_map->ixWriteArch = 0;
        }

        if (bytes_received < archd->record_size) {
            // Sanity check to ensure we were not promised more than 
            // the socket had to offer
            if (archd->debug) {
                fprintf(stderr, "%s: ioctl() reported more than available. Exiting now!\n", WHOAMI);
            } else {
                syslog(LOG_ERR, "%s: ioctl() reported more than available. Exiting now!", WHOAMI);
            }
            pcomm_stop(pcomm, 1/*immediately*/);
            return;
        }

        // Handle channel control commands
        if (strncmp("CHANNELCONTROL-", record->data, 15) == 0)
        {
            p_buf = record->data;
            p_msg = control;
            for (k=0; k < 127; k++, p_buf++, p_msg++) {
                c = *p_buf;
                if (c == 0) {
                    break;
                }
                else if (k > 60) {
                    c = '.';
                }
                else if (!isprint(c)) {
                    c = '*';
                }
                *p_msg = c;
            }
            *p_msg = 0;

            result = ChannelControl((char *)record->data, &control_id);
        }

        switch (result)
        {
         // Channel Control Command SUCCEEDED
            case RESULT_CHAN_CMD_OKAY:
                if (archd->debug) {
                    fprintf(stderr, "%s: Channel control applied: '%s'\n", WHOAMI, control);
                } else {
                    syslog(LOG_ERR, "%s: Channel control applied: '%s'", WHOAMI, control);
                }
                sprintf(response, "CHANNELCONTROL-%d-OKAY.", control_id);
                reply = make_reply(response, strlen(response)+1);
                break;

         // Channel Control Command FAILED
            case RESULT_CHAN_CMD_FAIL:
                if (archd->debug) {
                    fprintf(stderr, "%s: Failed to apply channel control: '%s'\n",
                            WHOAMI, control);
                } else {
                    syslog(LOG_ERR, "%s: Failed to apply channel control: '%s'",
                           WHOAMI, control);
                }
                sprintf(response, "CHANNELCONTROL-%d-FAIL.", control_id);
                reply = make_reply(response, strlen(response)+1);
                break;

         // Channel Control Command INVALID
            case RESULT_CHAN_CMD_INVALID:
                if (archd->debug) {
                    fprintf(stderr, "%s: Invalid channel control message: '%s'\n",
                            WHOAMI, control);
                } else {
                    syslog(LOG_ERR, "%s: Invalid channel control message: '%s'",
                           WHOAMI, control);
                }
                sprintf(response, "CHANNELCONTROL-INVALID.");
                reply = make_reply(response, strlen(response)+1);
                break;

         // Pass this record on to the archive mechanism
            default:
                // Least significant digit of the sequence number
                reply = make_reply(record->data + 5, 1);
                gettimeofday(&record->receive_time, &archd->tz_info);
                client->received++;
                prioqueue_add(&archd->record_queue, record, archd->record_size);

                archd->records_received++;
        } 

        if (reply == NULL)
        {
            if (archd->debug) {
                fprintf(stderr, "%s:make_reply(): could not create reply message\n", WHOAMI);
            } else {
                syslog(LOG_ERR, "%s:make_reply(): could not create reply message", WHOAMI);
            }
            // Hand control back to the process incase this is a memory issue.
            // Space will be freed up once the records in the queue have been
            // processed.
            break;
        }
        prioqueue_add(&client->reply_queue, reply, 0/*priority*/);

        // Prevent accidental overwrite/re-evaluation of old ata
        record = NULL;
        reply = NULL;
    }

    if (prioqueue_peek_high(&client->reply_queue)) {
        if (pcomm_result = pcomm_monitor_write_fd(pcomm, fd,
                                                  callback_client_can_send))
        {
            if (archd->debug) {
                fprintf(stderr, "%s:pcomm_monitor_write_fd(): %s\n",
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
    reply_message_t *reply = NULL;
    ssize_t bytes_sent = 0;

    archd = (archd_context_t *)pcomm_get_external_context(pcomm);
    client = (client_context_t *)pcomm_get_external_fd_context(pcomm, PCOMM_STREAM_READ, fd);

    if (archd->debug) {
        fprintf(stdout, "%s: replying to client\n", WHOAMI);
    }

    while (reply = (reply_message_t *)prioqueue_peek_high(&client->reply_queue))
    {
        bytes_sent = send(fd, reply->message, reply->length, MSG_DONTWAIT);
        if (bytes_sent < reply->length ) {
            if (bytes_sent < 0) {
                switch (errno) {
                    case EOPNOTSUPP:
                        if (archd->debug) {
                            fprintf(stderr, "%s:%d> bad socket option\n",
                                    WHOAMI, __LINE__);
                        } else {
                            syslog(LOG_ERR, "%s:%d> bad socket option",
                                    WHOAMI, __LINE__);
                        }
                        exit(1);
                    case EBADF:
                    case ECONNRESET:
                    case EDESTADDRREQ:
                    case EFAULT:
                    case EMSGSIZE:
                    case ENOTCONN:
                    case ENOTSOCK:
                    case EPIPE:
                        if (archd->debug) {
                            fprintf(stderr, "%s:%d> connection closed by client %d\n",
                                    WHOAMI, fd);
                        } else {
                            syslog(LOG_ERR, "%s:%d> connection closed by client %d",
                                    WHOAMI, fd);
                        }
                        callback_client_closed(pcomm, fd);
                        return;
                    case EAGAIN: // may be the same as EWOULDBLOCK
                    case EINTR:
                    case ENOBUFS:
                    case ENOMEM:
                    default:
                        break;
                }
            }
            // break out of the loop
            break;
        } else {
            // Remove this reply if it was sent succesfully
            prioqueue_pop_high(&client->reply_queue);
            client->confirmed++;
        }
    }

    if (!prioqueue_peek_high(&client->reply_queue)) {
        // Don't monitor this descriptor if there are not replies waiting
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
        syslog(LOG_ERR, "%s: client closed socket %d", WHOAMI, fd);
    }

    client = (client_context_t *)pcomm_get_external_fd_context(pcomm, PCOMM_STREAM_READ, fd);

    pcomm_remove_read_fd(pcomm, fd);
    pcomm_remove_write_fd(pcomm, fd);
    close(fd);

    if (client) {
        free(client);
    }

    archd->client_count--;
}

