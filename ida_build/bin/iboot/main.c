#pragma ident "$Id: main.c,v 1.4 2008/08/20 20:48:13 dechavez Exp $"
/*======================================================================
 *
 *  manage an iBoot device
 *
 *====================================================================*/
#include "iboot.h"

extern char *VersionIdentString;

static char DecodeCommand(char *string)
{
int i;
typedef struct {
    char data;
    char *string;
} COMMAND;
static COMMAND command[] = {
    { IBOOT_ON, "on" },
    { IBOOT_OFF, "off" },
    { IBOOT_CYCLE, "cycle" },
    { IBOOT_QUERY, "query" },
    {0, NULL}
};
    for (i = 0; command[i].data != 0; i++) {
        if (strcasecmp(string, command[i].string) == 0) return command[i].data;
    }

    return -1;
}

static void help(char *myname)
{
static char *VerboseHelp = 
"The arguments in [ square brackets ] are optional:\n"
"\n"
"server=string - name/address of the device\n"
"     port=int - port number\n"
"passwd=string - password\n"
"\n"
"You must specify ONE of the arguments in { curly brackets }.\n"
"\n"
"     on - turn power on\n"
"    off - turn power off\n"
"  cycle - cycle power (cycle time is configured in the device)\n"
"  query - query device";

    fprintf(stderr, "%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr,"usage: %s ", myname);
    fprintf(stderr, "[ server=string port=int passwd=string ] { on off cycle query }\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s\n", VerboseHelp);
    fprintf(stderr, "\n");
    fprintf(stderr, "default: server=%s port=%d passwd=%s\n", IBOOT_DEFAULT_SERVER, IBOOT_DEFAULT_PORT, IBOOT_DEFAULT_PASSWD);
    exit(1);

}

int main (int argc, char **argv)
{
int i;
char *reply;
char *server = IBOOT_DEFAULT_SERVER;
char *passwd = IBOOT_DEFAULT_PASSWD;
int port     = IBOOT_DEFAULT_PORT;
char action  = 0;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "server=", strlen("server=")) == 0) {
            server = argv[i] + strlen("server=");
        } else if (strncmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
        } else if (strncmp(argv[i], "passwd=", strlen("passwd=")) == 0) {
            passwd = argv[i] + strlen("passwd=");
        } else if (strcmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (action == 0) {
            action = DecodeCommand(argv[i]);
        } else {
           fprintf(stderr, "%s: unrecognized argument: '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (action <= 0) help(argv[0]);

    if ((reply = ibootCommand(server, port, passwd, action)) == NULL) {
        fprintf(stderr, "ERROR: can't send command '%c' to %s, port %d\n", action, server, port);
        exit(1);
    }

    if (strcmp(reply, IBOOT_ON_STRING) == 0) {
        printf("Power is ON\n");
    } else if (strcmp(reply, IBOOT_OFF_STRING) == 0) {
        printf("Power is OFF\n");
    } else if (strcmp(reply, IBOOT_CYCLE_STRING) == 0) {
        printf("Cycle Command Received\n");
    } else if (strcmp(reply, IBOOT_BUSY_STRING) == 0) {
        printf("iBoot is being controlled on another socket\n");
    } else {
        printf("unexpected response string '%s'\n", reply);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.4  2008/08/20 20:48:13  dechavez
 * moved all I/O into new iboot library and reworked to use ibootCommand() function
 *
 * Revision 1.3  2007/01/05 00:47:54  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.2  2004/08/25 20:22:17  dechavez
 * removed external dependencies, fixed typos in help message
 *
 * Revision 1.1  2004/08/25 18:54:23  dechavez
 * initial release
 *
 */
