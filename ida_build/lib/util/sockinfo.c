#pragma ident "$Id: sockinfo.c,v 1.2 2001/05/07 22:40:13 dec Exp $"
/*======================================================================
 *
 *  Log socket parameters.
 *
 *====================================================================*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include "util.h"

struct opt_map {
    int option;
    char *name;
};

#ifdef BSDOS
#define NUMOPT 15
struct opt_map map[NUMOPT] = {
    {SO_DEBUG,     "SO_DEBUG    "},
    {SO_REUSEADDR, "SO_REUSEADDR"},
    {SO_REUSEPORT, "SO_REUSEPORT"},
    {SO_KEEPALIVE, "SO_KEEPALIVE"},
    {SO_DONTROUTE, "SO_DONTROUTE"},
    {SO_BROADCAST, "SO_BROADCAST"},
    {SO_OOBINLINE, "SO_OOBINLINE"},
    {SO_SNDBUF,    "SO_SNDBUF   "},
    {SO_RCVBUF,    "SO_RCVBUF   "},
    {SO_SNDLOWAT,  "SO_SNDLOWAT "},
    {SO_RCVLOWAT,  "SO_RCVLOWAT "},
    {SO_SNDTIMEO,  "SO_SNDTIMEO "},
    {SO_RCVTIMEO,  "SO_RCVTIMEO "},
    {SO_TYPE,      "SO_TYPE     "},
    {SO_ERROR,     "SO_ERROR    "}
};
#else
#define NUMOPT 10
struct opt_map map[NUMOPT] = {
    {SO_DEBUG,     "SO_DEBUG    "},
    {SO_REUSEADDR, "SO_REUSEADDR"},
    {SO_KEEPALIVE, "SO_KEEPALIVE"},
    {SO_DONTROUTE, "SO_DONTROUTE"},
    {SO_BROADCAST, "SO_BROADCAST"},
    {SO_OOBINLINE, "SO_OOBINLINE"},
    {SO_SNDBUF,    "SO_SNDBUF   "},
    {SO_RCVBUF,    "SO_RCVBUF   "},
    {SO_TYPE,      "SO_TYPE     "},
    {SO_ERROR,     "SO_ERROR    "}
};
#endif

void util_sockinfo(int sd, int loglevel)
{
int i, level = SOL_SOCKET;
char *name;
int option, optval, optlen;
#ifdef SO_LINGER
struct linger linger;
#endif

    optlen = sizeof(optval);
    for (i = 0; i < NUMOPT; i++) {
        option = map[i].option;
        name   = map[i].name;
        if (getsockopt(sd, level, option, (char *) &optval, &optlen) != 0) {
            util_log(1, "getsockopt: %s: %s (ignored)",
                map[i].name, syserrmsg(errno)
            );
        } else {
            util_log(loglevel, "%s = %d", name, optval);
        }
    }

#ifdef SO_LINGER
    optlen = sizeof(linger);
    if (getsockopt(sd, level, SO_LINGER, (char *) &linger, &optlen) != 0) {
            util_log(1, "getsockopt: %s: %s (ignored)",
                "SO_LINGER", syserrmsg(errno)
            );
    } else {
        util_log(loglevel, "SO_LINGER   = %d/%d", 
            linger.l_onoff, linger.l_linger
        );
    }
#endif
}

/* Revision History
 *
 * $Log: sockinfo.c,v $
 * Revision 1.2  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
