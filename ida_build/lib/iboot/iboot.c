#pragma ident "$Id: iboot.c,v 1.1 2008/08/20 20:46:21 dechavez Exp $"
/*======================================================================
 *
 *  send a command to an iboot
 *
 *====================================================================*/
#include "iboot.h"

#define ESC 0x1b
#define CR  0x0d

#define IBOOT_MAX_PASSWD_LEN 60
#define IBOOT_TEMP_STRING_BUFLEN (1 + IBOOT_MAX_PASSWD_LEN + 1 + 1)

static int ConnectToDevice(char *server, int port)
{
unsigned long addr;
int sd;
struct hostent *hp;
char *host;
struct sockaddr_in peer_addr;
static char *fid = "ConnectToDevice";

    if ((hp = gethostbyname(server)) == NULL) {
        if ((hp = gethostbyaddr((char *)&addr,4,AF_INET)) == NULL) {
            return -1;
        } else {
            return -2;
        }
    }

    memcpy(&peer_addr.sin_addr, hp->h_addr, hp->h_length);
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port   = htons(port);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -3;

    if (connect(sd, (struct sockaddr *)&peer_addr,sizeof(struct sockaddr_in)) != 0) {
        close(sd);
        return -4;
    }

    return sd;
}

static BOOL SendCommand(int sd, char *passwd, char action)
{
int len;
char *ptr;
static char buf[IBOOT_TEMP_STRING_BUFLEN];

    if (strlen(passwd) > IBOOT_MAX_PASSWD_LEN) {
        errno = EINVAL;
        return FALSE;
    }

    ptr = buf;
    *ptr = ESC; ++ptr;
    memcpy(ptr, passwd, strlen(passwd)); ptr += strlen(passwd);
    *ptr = ESC; ++ptr;
    *ptr = action; ++ptr;
    *ptr = CR; ++ptr;

    len = (int) (ptr - buf);
    
    if (send(sd, buf, len, 0) != len) return FALSE;

    return TRUE;
}

static char *ReadReply(int sd)
{
int len;
static char *on    = IBOOT_ON_STRING;
static char *off   = IBOOT_OFF_STRING;
static char *cycle = IBOOT_CYCLE_STRING;
static char *busy  = IBOOT_BUSY_STRING;
char buf[IBOOT_TEMP_STRING_BUFLEN];

    len = recv(sd, buf, IBOOT_TEMP_STRING_BUFLEN-1, 0);

    buf[len] = 0;

    if (strcmp(buf, on) == 0)    return on;
    if (strcmp(buf, off) == 0)   return off;
    if (strcmp(buf, cycle) == 0) return cycle;
    if (strcmp(buf, busy) == 0)  return busy;

    return NULL;
    
}

char *ibootCommand(char *server, int port, char *passwd, char action)
{
int sd;
char *reply;

    if ((sd = ConnectToDevice(server, port)) < 0) return NULL;

    SendCommand(sd, passwd, action);
    reply = ReadReply(sd);

    shutdown(sd, 2);
    close(sd);

    return reply;
}

/* Revision History
 *
 * $Log: iboot.c,v $
 * Revision 1.1  2008/08/20 20:46:21  dechavez
 * initial release
 *
 */
