#pragma ident "$Id: signal.c,v 1.3 2003/11/13 19:36:00 dechavez Exp $"
#include "util.h"

char *util_sigtoa(int signal)
{
static char string[] = "signal number such and such";
#ifdef SIGHUP
static char *sighup = "SIGHUP";
#endif
#ifdef SIGINT
static char *sigint = "SIGINT";
#endif
#ifdef SIGQUIT
static char *sigquit = "SIGQUIT";
#endif
#ifdef SIGILL
static char *sigill = "SIGILL";
#endif
#ifdef SIGTRAP
static char *sigtrap = "SIGTRAP";
#endif
#ifdef SIGIOT
static char *sigiot = "SIGIOT";
#endif
#ifdef SIGABRT
static char *sigabrt = "SIGABRT";
#endif
#ifdef SIGEMT
static char *sigemt = "SIGEMT";
#endif
#ifdef SIGFPE
static char *sigfpe = "SIGFPE";
#endif
#ifdef SIGKILL
static char *sigkill = "SIGKILL";
#endif
#ifdef SIGBUS
static char *sigbus = "SIGBUS";
#endif
#ifdef SIGSEGV
static char *sigsegv = "SIGSEGV";
#endif
#ifdef SIGSYS
static char *sigsys = "SIGSYS";
#endif
#ifdef SIGPIPE
static char *sigpipe = "SIGPIPE";
#endif
#ifdef SIGALRM
static char *sigalrm = "SIGALRM";
#endif
#ifdef SIGTERM
static char *sigterm = "SIGTERM";
#endif
#ifdef SIGURG
static char *sigurg = "SIGURG";
#endif
#ifdef SIGSTOP
static char *sigstop = "SIGSTOP";
#endif
#ifdef SIGTSTP
static char *sigtstp = "SIGTSTP";
#endif
#ifdef SIGCONT
static char *sigcont = "SIGCONT";
#endif
#ifdef SIGCHLD
static char *sigchld = "SIGCHLD";
#endif
#ifdef SIGCLD
static char *sigcld = "SIGCLD";
#endif
#ifdef SIGTTIN
static char *sigttin = "SIGTTIN";
#endif
#ifdef SIGTTOU
static char *sigttou = "SIGTTOU";
#endif
#ifdef SIGIO
static char *sigio = "SIGIO";
#endif
#ifdef SIGPOLL
static char *sigpoll = "SIGPOLL";
#endif
#ifdef SIGXCPU
static char *sigxcpu = "SIGXCPU";
#endif
#ifdef SIGXFSZ
static char *sigxfsz = "SIGXFSZ";
#endif
#ifdef SIGVTALRM
static char *sigvtalrm = "SIGVTALRM";
#endif
#ifdef SIGPROF
static char *sigprof = "SIGPROF";
#endif
#ifdef SIGWINCH
static char *sigwinch = "SIGWINCH";
#endif
#ifdef SIGLOST
static char *siglost = "SIGLOST";
#endif
#ifdef SIGUSR1
static char *sigusr1 = "SIGUSR1";
#endif
#ifdef SIGUSR2
static char *sigusr2 = "SIGUSR2";
#endif

#ifdef SIGHUP
    if (signal == SIGHUP) return sighup;
#endif
#ifdef SIGINT
    if (signal == SIGINT) return sigint;
#endif
#ifdef SIGQUIT
    if (signal == SIGQUIT) return sigquit;
#endif
#ifdef SIGILL
    if (signal == SIGILL) return sigill;
#endif
#ifdef SIGTRAP
    if (signal == SIGTRAP) return sigtrap;
#endif
#ifdef SIGIOT
    if (signal == SIGIOT) return sigiot;
#endif
#ifdef SIGABRT
    if (signal == SIGABRT) return sigabrt;
#endif
#ifdef SIGEMT
    if (signal == SIGEMT) return sigemt;
#endif
#ifdef SIGFPE
    if (signal == SIGFPE) return sigfpe;
#endif
#ifdef SIGKILL
    if (signal == SIGKILL) return sigkill;
#endif
#ifdef SIGBUS
    if (signal == SIGBUS) return sigbus;
#endif
#ifdef SIGSEGV
    if (signal == SIGSEGV) return sigsegv;
#endif
#ifdef SIGSYS
    if (signal == SIGSYS) return sigsys;
#endif
#ifdef SIGPIPE
    if (signal == SIGPIPE) return sigpipe;
#endif
#ifdef SIGALRM
    if (signal == SIGALRM) return sigalrm;
#endif
#ifdef SIGTERM
    if (signal == SIGTERM) return sigterm;
#endif
#ifdef SIGURG
    if (signal == SIGURG) return sigurg;
#endif
#ifdef SIGSTOP
    if (signal == SIGSTOP) return sigstop;
#endif
#ifdef SIGTSTP
    if (signal == SIGTSTP) return sigtstp;
#endif
#ifdef SIGCONT
    if (signal == SIGCONT) return sigcont;
#endif
#ifdef SIGCHLD
    if (signal == SIGCHLD) return sigchld;
#endif
#ifdef SIGCLD
    if (signal == SIGCLD) return sigcld;
#endif
#ifdef SIGTTIN
    if (signal == SIGTTIN) return sigttin;
#endif
#ifdef SIGTTOU
    if (signal == SIGTTOU) return sigttou;
#endif
#ifdef SIGIO
    if (signal == SIGIO) return sigio;
#endif
#ifdef SIGPOLL
    if (signal == SIGPOLL) return sigpoll;
#endif
#ifdef SIGXCPU
    if (signal == SIGXCPU) return sigxcpu;
#endif
#ifdef SIGXFSZ
    if (signal == SIGXFSZ) return sigxfsz;
#endif
#ifdef SIGVTALRM
    if (signal == SIGVTALRM) return sigvtalrm;
#endif
#ifdef SIGPROF
    if (signal == SIGPROF) return sigprof;
#endif
#ifdef SIGWINCH
    if (signal == SIGWINCH) return sigwinch;
#endif
#ifdef SIGLOST
    if (signal == SIGLOST) return siglost;
#endif
#ifdef SIGUSR1
    if (signal == SIGUSR1) return sigusr1;
#endif
#ifdef SIGUSR2
    if (signal == SIGUSR2) return sigusr2;
#endif
    sprintf(string, "signal %d", signal);
    return string;
}

/* Revision History
 *
 * $Log: signal.c,v $
 * Revision 1.3  2003/11/13 19:36:00  dechavez
 * removed util_signal and made util_sigtoa() almost reentrant
 *
 * Revision 1.2  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
