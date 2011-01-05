#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>

/* Change this to whatever your daemon is called */
#define DAEMON_NAME "falcon"

/* Change this to the user under which to run -- only if run as root */
#define RUN_AS_USER "maint"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static char *lockfile = "/var/tmp/lock." DAEMON_NAME;

static void sigterm_handler()
{
  syslog(LOG_INFO, "Exiting daemon " DAEMON_NAME);
  unlink (lockfile);
  closelog();
  exit(EXIT_SUCCESS);
} // sigterm_handler()

static void child_handler(int signum)
{
    switch(signum) {
    case SIGALRM: exit(EXIT_FAILURE); break;
    case SIGUSR1: exit(EXIT_SUCCESS); break;
    case SIGCHLD: exit(EXIT_FAILURE); break;
    }
} // child_handler()

// exit if there is already a daemon running with this name
static int lock_daemon()
{
    int lfp=-1;
    int lpid = 0;
    int iTry = 0;
    char pidstr[80];
    int readcnt;

    while (lfp < 0 && iTry++ < 2)
    {
        // Create the lock file
        if ( lockfile && lockfile[0] )
        {
            lfp = open(lockfile,O_RDWR|O_CREAT|O_EXCL|O_TRUNC,0660);
            if ( lfp < 0 )
            {
                // Failed to create a lock file, so see if the creator
                // is still a valid process
                lfp = open(lockfile,O_RDONLY);
                if (lfp < 0)
                {
                    //Failed to open the lock file for reading
                    syslog(LOG_ERR, "Unable to read existing lock file %s\n",
                         lockfile);
                    return 0;
                }

                // Existing lockfile found, see if creator is still around
                memset(pidstr, 0, sizeof(pidstr));
                readcnt = read(lfp, pidstr, sizeof(pidstr)-1);
                close(lfp);
                lfp = -1;
                lpid = atoi(pidstr);
 
                sprintf(pidstr, "/proc/%d/cmdline", lpid);
                lfp = open(pidstr,O_RDONLY);
                if (lfp < 0)
                {
                  // process is not valid, we can delete the lock file
                  unlink(lockfile);
                  syslog(LOG_INFO, "Ignoring defunct lockfile %s\n", lockfile);
                  continue;
                }
                else
                {
                  // another valid daemon of this name exists
                  syslog(LOG_INFO, "%s daemon pid %d already running\n",
                         DAEMON_NAME, lpid);
                  close(lfp);
                  return 0;
                }
            } //  Unable to create a new lock file
            else
            {
              // We created this file, write our pid to it
              sprintf(pidstr, "%d\n", getpid()); 
              write(lfp, pidstr, strlen(pidstr));
            } // we created a new lock file
        } // lockfile name is defined
        else
        {
            return 1;
        }
    } // While we still want to try and create a lock file

    close(lfp);
    return 1;
} // lock_daemon()

void daemonize()
{
    pid_t pid, sid, parent;

    openlog(DAEMON_NAME, LOG_PID, LOG_LOCAL5);
    syslog(LOG_INFO, "Starting daemon");

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Drop user if there is one, and we were run as root */
    if ( getuid() == 0 || geteuid() == 0 ) {
        struct passwd *pw = getpwnam(RUN_AS_USER);
        if ( pw ) {
            syslog( LOG_NOTICE, "setting user to " RUN_AS_USER );
            setuid( pw->pw_uid );
        }
    }

    /* Trap signals that we expect to recieve */
    signal(SIGCHLD,child_handler);
    signal(SIGUSR1,child_handler);
    signal(SIGALRM,child_handler);

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        syslog( LOG_ERR, "unable to fork daemon, code=%d (%s)",
                errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {

        /* Wait for confirmation from the child via SIGTERM or SIGCHLD, or
           for two seconds to elapse (SIGALRM).  pause() should not return. */
        alarm(2);
        pause();

        exit(EXIT_FAILURE);
    }

    // only one daemon of this type allowed at once
    if (!lock_daemon())
    {
        syslog(LOG_ERR, "Another %s daemon is already running, aborting!\n",
               DAEMON_NAME);
        exit(EXIT_FAILURE);
    }

    /* At this point we are executing as the child process */
    parent = getppid();

    /* Cancel certain signals */
    signal(SIGCHLD,SIG_DFL); /* A child process dies */
    signal(SIGTSTP,SIG_IGN); /* Various TTY signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP, SIG_IGN); /* Ignore hangup signal */
    signal(SIGTERM,sigterm_handler); /* Die on SIGTERM */
//    signal(SIGTERM,SIG_DFL); /* Die on SIGTERM */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        syslog( LOG_ERR, "unable to create a new session, code %d (%s)",
                errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/etc/q330")) < 0) {
        syslog( LOG_ERR, "unable to change directory to %s, code %d (%s)",
                "/etc/q330", errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/var/tmp/" DAEMON_NAME, "w", stderr);
    chmod( "/var/tmp/" DAEMON_NAME, 0660);

    /* Tell the parent process that we are A-okay */
    kill( parent, SIGUSR1 );
} // daemonize()

