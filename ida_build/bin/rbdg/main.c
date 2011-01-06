#pragma ident "$Id: main.c,v 1.2 2008/03/21 17:22:15 dechavez Exp $"
/*======================================================================
 *
 *  Reboot a digi via the telnet port
 *
 *====================================================================*/
#include "platform.h"

extern char *VersionIdentString;

#define DEFAULT_USER   "root"
#define DEFAULT_PASSWD "dbps"
#define DEFAULT_PORT   23

#define CR 0x0d
#define LF 0x0a

static void help(char *myname)
{
    fprintf(stderr, "usage: %s digi [ passwd | user passwd ]\n", myname);
    exit(0);
}

static int ConnectToDevice(char *digi, int port)
{
unsigned long addr;
int sd;
struct hostent *hp;
char *host;
struct sockaddr_in peer_addr;
static char *fid = "ConnectToDevice";

    if ((hp = gethostbyname(digi)) == NULL) {
        fprintf(stderr, "can't resolve address for `%s'\n", digi);
        exit(1);
    }

    memcpy(&peer_addr.sin_addr, hp->h_addr, hp->h_length);
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port   = htons(port);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(3);
    }

    if (connect(sd, (struct sockaddr *)&peer_addr,sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "connect: ");
        perror(digi);
        close(sd);
        exit(4);
    }

    return sd;
}

static void SendString(int sd, char *string)
{
int len;
char *ptr;
#define BUFLEN 1024
static char buf[BUFLEN+2];

    if (strlen(string) > BUFLEN) {
        fprintf(stderr, "String '%s' is too long!\n", string);
        exit(1);
    }

    ptr = buf; memcpy(ptr, string, strlen(string)); ptr += strlen(string);
    *ptr = CR; ++ptr;
    *ptr = LF; ++ptr;
    len = (int) (ptr - buf);
    
    if (send(sd, buf, len, 0) != len) {
        fprintf(stderr, "ERROR: ");
        perror("send");
        exit(1);
    }
}

int main (int argc, char **argv)
{
int i, sd;
char *digi = NULL;
char *user = DEFAULT_USER;
char *passwd = DEFAULT_PASSWD;
int port     = DEFAULT_PORT;
static char *reboot = "boot action=reset";

    if (argc == 2) {
        digi = argv[1];
    } else if (argc == 3) {
        digi = argv[1];
        passwd = argv[2];
    } else if (argc == 4) {
        digi = argv[1];
        user = argv[2];
        passwd = argv[3];
    } else {
        help(argv[0]);
    }

    if (digi == NULL) help(argv[0]);

    if ((sd = ConnectToDevice(digi, port)) < 0) {
        fprintf(stderr, "ERROR: can't connect to %s, port %d\n", digi, port);
        exit(1);
    }

    SendString(sd, user);   sleep(1);
    SendString(sd, passwd); sleep(1);
    SendString(sd, reboot); sleep(1);
    shutdown(sd, 2);
    close(sd);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2008/03/21 17:22:15  dechavez
 * cleaned up help
 *
 * Revision 1.1  2008/03/21 16:51:52  dechavez
 * initial release
 *
 */
