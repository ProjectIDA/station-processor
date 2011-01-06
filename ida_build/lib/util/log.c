#pragma ident "$Id: log.c,v 1.18 2009/03/17 18:17:42 dechavez Exp $"
/*======================================================================
 *
 *  MT-safe (Solaris threads) version of the message logging utilities.
 *
 *  It is assumed that the util_logopen() function is called by the
 *  main thread, prior to the possiblity for any other threads to call
 *  util_log().
 *
 *====================================================================*/
#include "util.h"

#define MAX_TIMESTRING_LEN 64
#define MAX_TFMTSTRING_LEN 64
#define HOSTNAMLEN 128
#define LOGBUFLEN  ((size_t) 512)
#define DEFTIMEFMT "%Y:%j-%H:%M:%S"

static char Tfmt[MAX_TFMTSTRING_LEN];
static int Min_level, Max_level, Log_level;
static FILE *Fp = NULL;
static int Fd = -1;
static char Prefix[LOGBUFLEN];
static char Hostname[HOSTNAMLEN];
static char Message[LOGBUFLEN];
static char buf1[LOGBUFLEN], buf2[LOGBUFLEN];
static char *Crnt_msg, *Prev_msg;
static int Pid;
static int Count = 0;

static LOGIO *lp = NULL;

static MUTEX mutex = MUTEX_INITIALIZER;

static void InternalSignalHandler(sig)
int sig;
{

    util_log(Min_level, "%s", util_sigtoa(sig));
#ifndef WIN32
    if (sig == SIGUSR1) {
        util_log(0, "log level %d", ++Log_level);
        signal(SIGUSR1, InternalSignalHandler);
        return;
    } else if (sig == SIGUSR2) {
        util_log(0, "log level %d", Log_level = Min_level);
        signal(SIGUSR2, InternalSignalHandler);
        return;
    } else {
        util_log(Min_level, "signal ignored");
        return;
    }
#else
/* Win32 code will be here */
#endif
}

static int CurrentLogLevel()
{
int retval;

    MUTEX_LOCK(&mutex);
        retval = Log_level;
    MUTEX_UNLOCK(&mutex);

    return retval;
}

void util_incrloglevel()
{
int  old;

    old = CurrentLogLevel();
    MUTEX_LOCK(&mutex);
        if (Log_level < Max_level) ++Log_level;
    MUTEX_UNLOCK(&mutex);

    if (old != Log_level) {
        util_log(Min_level, "increment to log level %d", Log_level);
    } else {
        util_log(Min_level, "log level %d (max)", old);
    }
}

void util_rsetloglevel()
{
    MUTEX_LOCK(&mutex);
        Log_level = Min_level;
    MUTEX_UNLOCK(&mutex);
    util_log(0, "log level %d", Min_level);
}

#ifdef HAVE_SYSLOG
static int extract_facility(char *name)
{
char copy[MAXPATHLEN+1];
int i;
#ifdef SOLARIS
int default_facility = LOG_USER;
#else
int default_facility = LOG_LOCAL6;
#endif

    for (i = 0; i < strlen(name); i++) {
        if (name[i] == ':') {
            if (strcasecmp(&name[i+1], "local0") == 0) return LOG_LOCAL0;
            if (strcasecmp(&name[i+1], "local1") == 0) return LOG_LOCAL1;
            if (strcasecmp(&name[i+1], "local2") == 0) return LOG_LOCAL2;
            if (strcasecmp(&name[i+1], "local3") == 0) return LOG_LOCAL3;
            if (strcasecmp(&name[i+1], "local4") == 0) return LOG_LOCAL4;
            if (strcasecmp(&name[i+1], "local5") == 0) return LOG_LOCAL5;
            if (strcasecmp(&name[i+1], "local6") == 0) return LOG_LOCAL6;
            if (strcasecmp(&name[i+1], "local7") == 0) return LOG_LOCAL7;
            if (strcasecmp(&name[i+1], "user") == 0) return LOG_USER;
        }
    }

    return default_facility;
}
#endif

int util_logopen(
    char *file,
    int min_level,
    int max_level,
    int log_level,
    char *tfmt, 
    char *fmt, 
    ...
){
va_list ap;
char *ptr;
int facility;

    MUTEX_LOCK(&mutex);

    strlcpy(Tfmt, DEFTIMEFMT, MAX_TFMTSTRING_LEN);

    if (min_level>max_level || log_level<min_level || log_level>max_level) {
        errno = EINVAL;
        MUTEX_UNLOCK(&mutex);
        return -1;
    }

    Min_level = min_level;
    Max_level = max_level;
    Log_level = log_level;
    gethostname(Hostname, HOSTNAMLEN);
    Pid = getpid();

/*  Open log file  */

    if (file == NULL) {
        Fp = stderr;
#ifdef HAVE_SYSLOG
    } else if (strncasecmp(file, "syslog", strlen("syslog")) == 0) {
        facility = extract_facility(file);
        openlog(NULL, LOG_CONS, facility);
#endif
    } else if (Fp != NULL) {
        errno = EALREADY;
        MUTEX_UNLOCK(&mutex);
        return -1;
    } else if (strcmp(file, "-") == 0) {
        Fp = stdout;
    } else if ((Fp = fopen(file, "a+")) == NULL) {
        MUTEX_UNLOCK(&mutex);
        return -1;
    }
    Fd = (Fp == NULL) ? 0 : fileno(Fp);

    if (tfmt != NULL && (strlen(tfmt) < MAX_TFMTSTRING_LEN)) strlcpy(Tfmt, tfmt, MAX_TFMTSTRING_LEN);

/*  Set up signal handler for modifying log levels  */

#ifndef _WIN32
/* It does not work on Win32. signals retuns a mistake for SIGUSR1. In this case  
 Prev_msg & Crnt_msg remain uninitialized. After that  any call 
 of util_log crashes the program. Anyway error check of util_log 
 open required.
*/
    if (signal(SIGUSR1, InternalSignalHandler) == SIG_ERR) {
        MUTEX_UNLOCK(&mutex);
        return -1;
    }
    if (signal(SIGUSR2, InternalSignalHandler) == SIG_ERR) {
        MUTEX_UNLOCK(&mutex);
        return -1;
    }
#endif

/*  Initialize the message buffers  */

    memset((void *) buf1, 0, LOGBUFLEN); Crnt_msg = buf1;
    memset((void *) buf2, 0, LOGBUFLEN); Prev_msg = buf2;

/*  Create the message prefix  */

    memset((void *) Prefix, 0, LOGBUFLEN);
    if (fmt != (char *) NULL) {
        if (Fd == 0 && tfmt != NULL) {  /* using syslogd */
            sprintf(Prefix, "%s ", tfmt); /* overloaded... syscode */
            ptr = Prefix + strlen(Prefix);
        } else {
            ptr = Prefix;
        }
        va_start(ap, fmt);
        vsnprintf(ptr, LOGBUFLEN-strlen(ptr), fmt, ap);
        va_end(ap);
    } else {
        Prefix[0] = (char) 0;
    }

    MUTEX_UNLOCK(&mutex);
    return 0;

}

void util_logclose(
    void
){

    MUTEX_LOCK(&mutex);
        if (Fp != NULL && Fp != stdout && Fp != stderr) fclose(Fp);
        Fp = NULL;
    MUTEX_UNLOCK(&mutex);

    return;
}

void utilHookOldLog(LOGIO *ptr)
{
    memset((void *) buf1, 0, LOGBUFLEN); Crnt_msg = buf1;
    memset((void *) buf2, 0, LOGBUFLEN); Prev_msg = buf2;
    lp = ptr;
}

void util_log(
    int level, 
    char *fmt, 
    ...
){
va_list ap;
char *ptr;
time_t ltime;
#ifndef WIN32
struct tm tm;
#endif
char preamble[LOGBUFLEN];
char timstr[MAX_TIMESTRING_LEN];

    MUTEX_LOCK(&mutex);

/* If we've got a logio hook, use it */

    if (lp != NULL) {
        memset((void *) Crnt_msg, 0, LOGBUFLEN);
        ptr = Crnt_msg;
        va_start(ap, fmt);
        vsnprintf(ptr, LOGBUFLEN-strlen(ptr), fmt, ap);
        va_end(ap);
        logioMsg(lp, (level > 1) ? LOG_DEBUG : LOG_INFO, Crnt_msg);
        MUTEX_UNLOCK(&mutex);
        return;
    }

/* If we haven't been initialized then quit */

    if (Fd != 0 && Fp == NULL) {
        MUTEX_UNLOCK(&mutex);
        return;
    }

/*  Don't print anything which is above the desired log level  */

    if (level > Log_level) {
        MUTEX_UNLOCK(&mutex);
        return;
    }

/*  Build the message string  */

    memset((void *) Crnt_msg, 0, LOGBUFLEN);
    if (Fd == 0) { /* using syslogd */
        sprintf(Crnt_msg, "%s[%d]: ", Prefix, Pid);
        ptr = Crnt_msg + strlen(Crnt_msg);
    } else {
        ptr = Crnt_msg;
    }

    va_start(ap, fmt);
    vsnprintf(ptr, LOGBUFLEN-strlen(ptr), fmt, ap);
    va_end(ap);

/*  If using syslogd then we are done  */

    if (Fd == 0) {
#ifdef WIN32
       /* no syslog in MS Windows */
#else
        syslog(LOG_INFO, Crnt_msg);
#endif
        MUTEX_UNLOCK(&mutex);
        return;
    }

    if (Crnt_msg[strlen(Crnt_msg)] != '\n') Crnt_msg[strlen(Crnt_msg)] = '\n';

/*  Don't print duplicate messages  */

    if (strcmp(Crnt_msg, Prev_msg) == 0) {
        ++Count;
        MUTEX_UNLOCK(&mutex);
        return;
    }

/*  Build the message preamble  */

    ltime = time(NULL);
    if (Fp != stderr && Fp != stdout) {
        util_wlockw(Fd, 0, SEEK_SET, 0);
        fseek(Fp, 0, SEEK_END);
    }
#ifdef WIN32
    if (strftime(timstr, MAX_TIMESTRING_LEN, Tfmt, localtime(&ltime))) {
#else
    if (strftime(timstr, MAX_TIMESTRING_LEN, Tfmt, localtime_r(&ltime, &tm))) {
#endif
        sprintf(preamble, "%s ", timstr);
    } else {
        sprintf(preamble, "CAN'T DETERMINE TIME STRING! ");
    }

    sprintf(preamble + strlen(preamble), "%s ",  Hostname);
    sprintf(preamble + strlen(preamble), "%s",   Prefix);
    sprintf(preamble + strlen(preamble), "[%d]", Pid);

/*  Print the message  */

    if (Count > 1) {
        fprintf(Fp, "%s ", preamble);
        fprintf(Fp, "previous message repeated %d times\n", Count);
        Count = 0;
    }

    fprintf(Fp, "%s %s", preamble, Crnt_msg);

    fflush(Fp);
    if (Fp != stderr && Fp != stdout) util_unlock(Fd, 0, SEEK_SET, 0);

/*  Save this message for comparison with the next one  */

    ptr      = Prev_msg;
    Prev_msg = Crnt_msg;
    Crnt_msg = ptr;

    MUTEX_UNLOCK(&mutex);
    return;
}

#ifdef DEBUG_TEST

main(argc, argv)
int argc;
char *argv[];
{
int i;

    if (argc != 2) {
        fprintf(stderr, "usage: %s logfilename\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "syslogd") == 0) {
        util_logopen(argv[1], 1, 9, 1, "SYSCODE", argv[0]);
    } else {
        util_logopen(argv[1], 1, 9, 1, NULL, argv[0]);
    }

    for (i = 0; i < 1024; i++) util_log(1, "this is a duplicate message");

    while (1) {
        util_log(4, "log entry %d", i++);
        sleep(1);
    }
}
#endif

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.18  2009/03/17 18:17:42  dechavez
 * allow specifying log facility with syslog option
 *
 * Revision 1.17  2007/01/31 22:13:19  dechavez
 * fixed long standing bug handling "syslogd" specifier
 *
 * Revision 1.16  2007/01/07 17:40:46  dechavez
 * strlcpy() instead of strcpy(), vsnprintf() instead of vsprintf()
 *
 * Revision 1.15  2005/07/06 15:35:37  dechavez
 * added hook to permit use of logio routines
 *
 * Revision 1.14  2005/05/25 22:58:52  dechavez
 * cleared up some Unix/Windows conflicts
 *
 * Revision 1.13  2005/05/25 22:41:46  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.12  2004/07/26 23:22:59  dechavez
 * win32 support mods
 *
 * Revision 1.11  2004/06/30 21:11:43  dechavez
 * remove "obnoxious "logging suspended" message in util_logclose
 *
 * Revision 1.10  2004/06/24 17:08:28  dechavez
 * WIN32 port (aap)
 *
 * Revision 1.9  2004/06/04 22:49:46  dechavez
 * various AAP windows portability modifications
 *
 * Revision 1.8  2003/12/04 23:29:07  dechavez
 * removed tabs
 *
 * Revision 1.7  2003/11/13 19:41:36  dechavez
 * removed Sigfunc casts
 *
 * Revision 1.6  2002/02/22 23:46:02  dec
 * fixed error in log level increment message
 *
 * Revision 1.5  2001/10/24 23:28:28  dec
 * use MUTEX macros
 *
 * Revision 1.4  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.3  2000/10/06 19:57:07  dec
 * print log level upon USR1 increments
 *
 * Revision 1.2  2000/06/24 23:44:30  dec
 * linux and bsdos to log use LOCAL6 facility
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
