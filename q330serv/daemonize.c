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

#include <globals.h>

/* Change this to whatever your daemon is called */
#define DAEMON_NAME "q330serv"

/* Change this to the user under which to run -- only if run as root */
#define RUN_AS_USER "maint"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static void sigterm_handler()
{
  int i;
  enum tliberr err ;
  enum tlibstate libstate;

  lib_change_state (context, LIBSTATE_IDLE, LIBERR_CLOSED) ;
  for (i=0; i < 200 && 
       lib_get_state(context, &err, &opstat) != LIBSTATE_IDLE; i++)
  {
    // Sleep 1/10th of a second
    usleep(100000);
  } // give state machine some time to idle
  lib_change_state (context, LIBSTATE_TERM, LIBERR_CLOSED) ;
  sleep(2);

  syslog(LOG_INFO, "Exiting daemon");
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

void daemonize()
{
    FILE *pid_file;
    pid_t pid, sid, parent;
    int lfp = -1;
    char *pid_filename;

    char *lockfile;

    lockfile = "/var/lock/subsys/" DAEMON_NAME;

    openlog(DAEMON_NAME, LOG_PID, LOG_LOCAL5);
    syslog(LOG_INFO, "Starting daemon");

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Create the lock file as the current user */
/*
    if ( lockfile && lockfile[0] ) {
        lfp = open(lockfile,O_RDWR|O_CREAT,0660);
        if ( lfp < 0 ) {
            fprintf(stderr,"Unable to create lock file %s\n", lockfile);
            syslog( LOG_ERR, "unable to create lock file %s, code=%d (%s)",
                    lockfile, errno, strerror(errno) );
            exit(EXIT_FAILURE);
        }
    }
*/

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

    /* At this point we are executing as the child process */
    parent = getppid();

    /* Cancel certain signals */
    signal(SIGCHLD,SIG_DFL); /* A child process dies */
    signal(SIGTSTP,SIG_IGN); /* Various TTY signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP, SIG_IGN); /* Ignore hangup signal */
    signal(SIGTERM,sigterm_handler); /* Die on SIGTERM */

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

/*
    // write our pid to /var/run/DAEMON_NAME.pid 
    pid_filename = "/var/run/" DAEMON_NAME ".pid";
    pid_file = fopen(pid_filename, "w");
    if (pid_file != NULL)
    {
      fprintf(pid_file, "%d\n", getpid());
    } else {
      syslog (LOG_ERR, "Unable to create pid file %s %d %s", pid_filename,
                errno, strerror(errno));
    }
*/

    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/var/tmp/" DAEMON_NAME, "w", stderr);
    chmod( "/var/tmp/" DAEMON_NAME, 0660);

    /* Tell the parent process that we are A-okay */
    kill( parent, SIGUSR1 );
} // daemonize()

