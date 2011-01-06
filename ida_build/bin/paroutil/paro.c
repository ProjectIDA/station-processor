#pragma ident "$Id: paro.c,v 1.5 2009/03/24 21:07:18 dechavez Exp $"
/*======================================================================
 *
 *  Paroscientific Digiquartz pressure device configuration and
 *  test utility.
 *
 *====================================================================*/
#include "paro.h"

extern char *VersionIdentString;

#ifdef FREEBSD
#define DEF_PORT "/dev/cuad1"
#endif
#ifdef SOLARIS
#define DEF_PORT "/dev/term/a"
#endif
#ifdef LINUX
#define DEF_PORT "/dev/ttyS0"
#endif
#ifndef DEF_PORT
#define DEF_PORT "/dev/baro"
#endif
#define DEF_BAUD 9600

PARO *pp = (PARO *) NULL;
int raw =  0;
BOOL interrupted = TRUE;

void help(char *name)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [device baud=speed]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "defaults: ");
    fprintf(stderr, "%s baud=%d ", DEF_PORT, DEF_BAUD);
    exit(1);
}

void catcher(int sig)
{

    signal(sig, catcher);

    interrupted = TRUE;
    printf(" interrupted\n");

    paroReset(pp);

    if (sig == SIGTERM || sig == SIGQUIT) exit(0);
}

void get_cont()
{
char byte, *reply;

    interrupted = FALSE;
    if (raw) {
        do {
            if (ttyioRead(pp->tty, &byte, 1) == 1) {
                if (byte == 0x2a) printf("recv: "); fflush(stdout);
                printf("%c", byte);
            } else {
                if (!interrupted) printf("timed out\n");
                return;
            }
        } while (!interrupted);
    } else {
        do {
            printf("recv: "); fflush(stdout);
            if ((reply = paroRead(pp)) == (char *) NULL) {
                if (!interrupted) {
                    printf("%s\n", syserrmsg(errno));
                }
            } else {
                printf("%s\n", reply);
            }
        } while (!interrupted);
    }
}

void get_once()
{
char byte, *reply;
BOOL done;

    done = interrupted = FALSE;
    if (raw) {
        printf("recv: "); fflush(stdout);
        while (!done && !interrupted) {
            if (ttyioRead(pp->tty, &byte, 1) == 1) {
                printf("%c", byte);
                if (byte == 0x0a) done = 1;
                } else {
                if (!interrupted) printf("timed out\n");
                return;
            }
        }
    } else {
        printf("recv: "); fflush(stdout);
        if ((reply = paroRead(pp)) == (char *) NULL) {
            if (interrupted) {
                printf("interrupted\n");
            } else {
                printf("%s\n", syserrmsg(errno));
            }
        } else {
            printf("%s\n", reply);
        }
    }
}

char *sendr(int key)
{
    paroCommand(pp, key);
    return paroRead(pp);
}

void pass_thru(char *command, int flag)
{
static char crlf[2] = {0x0d, 0x0a};

    paroWrite(pp, command);
    if (flag > 0) {
        get_cont();
    } else if (flag == 0) {
        get_once();
    }
}

void doit(int key, int flag)
{
    printf("send: %s\n", paroCommand(pp, key));
    if (flag > 0) {
        get_cont();
    } else if (flag == 0) {
        get_once();
    }
}

void read32()
{
INT32 result;

    interrupted = FALSE;
    paroGetcnf(pp);
    paroCommand(pp, PARO_P4);
    do {
        if (paroReadINT32(pp, &result)) {
            printf("%ld\n", result);
            fflush(stdout);
        } else {
            if (!interrupted) perror("paroReadINT32");
            return;
        }
    } while (!interrupted);
}

void print_param(PARO_PARAM *param)
{
char *unstring[] = {
    "user defined (see UF)",
    "psi",
    "mbar or hPa",
    "bar",
    "kPa",
    "MPa",
    "in Hg",
    "mm Hg or torr",
    "m H20"
};

    printf("Identification parameters\n");
    printf("S/N = %s    Firmware rev = %s\n", param->sn, param->vr);
    printf("\n");
    printf("Configuration parameters\n");
    printf("PR = %6d   TR = %6d   UN = %d ",
        param->pr, param->tr, param->un
    );
    if (param->un == 0) {
        printf("(user defined, UF = %9.7g)\n", param->uf);
    } else {
        printf("(%s)\n", unstring[param->un]);
    }
    if (param->revision >= PARO_MIN_PI_REVISION) printf("PI = %6d   TI = %6d\n", param->pi, param->ti);
    printf("\n");
    printf("Calibration parameters\n");
    printf("PA = %9.7g   PM = %9.7g  TC = %9.7g\n",
        param->pa, param->pm, param->tc
    );
    printf("C1 = %9.7g   C2 = %9.7g  C3 = %9.7g\n",
        param->c1, param->c2, param->c3
    );
    printf("D1 = %9.7g   D2 = %9.7g\n",
        param->d1, param->d2
    );
    printf("T1 = %9.7g   T2 = %9.7g  T3 = %9.7g  T4 = %9.7g  T5 = %9.7g\n",
        param->t1, param->t2, param->t3, param->t4, param->t5
    );
    printf("U0 = %9.7g\n",
        param->u0
    );
    printf("Y1 = %9.7g   Y2 = %9.7g  Y3 = %9.7g\n",
        param->y1, param->y2, param->y3
    );
    printf("\n");
    printf("Nano-resolution parameters\n");
    printf("XM = %d ", param->xm);
    switch (param->xm) {
      case 0: printf("(Standard Resolution)  XN = %d\n", param->xn); break;
      case 1: printf("(Nano-resolution, FIR Filter Mode)  IA = %d   XN = %d\n", param->ia, param->xn); break;
      case 2: printf("(Nano-resolution, FIR Filter Mode)  XT = %d   XN = %d\n", param->xt, param->xn); break;
      default: printf("*** UNRECOGNIZED MODE ***\n");
    }
}

void list_commands()
{
    printf("P1/Q1 Sample and send one pressure/temperature sensor period.\n");
    printf("P2/Q2 Continuously sample and send pressure/temperature period.\n");
    printf("P3/Q3 Sample and send one pressure/temperature.\n");
    printf("P4/Q4 Continuously sample and send pressure/temperature.\n");
    printf("P5/Q5 Sample and hold one pressure/temperature (see DB).\n");
    printf("P6/Q6 Sample and hold one pressure/temperature period (see DB).\n");
    printf("P7    Burst and sample pressure: read temperature once, then\n");
    printf("      continuously send pressure compensated using that temperature.\n");
    printf("DB    Dump buffer (sends value stored by previous P5, Q5, P6, or Q6.\n");
    printf("\n");
    printf("CS    Check stack of microprocessor.\n");
    printf("CT    Check counter timebase.\n");
    printf("CX    Check crystal of microprocessor clock.\n");
    printf("\n");
    printf("EW    Enable EEPROM write for subsequent command.\n");
    printf("PR/TR Read pressure/temperature resolution.\n");
    printf("PI/TI Read pressure/temperature integration time (%.2lf or greater).\n", PARO_MIN_PI_REVISION);
    printf("UN    Read choice of pressure units:\n");
    printf("      0 = user defined    3 = bar    6 = in. Hg\n");
    printf("      1 = psi             4 = kPa    7 = Torr or mm Hg\n");
    printf("      2 = mbar or hPa     5 = MPa    8 = m H20\n");
    printf("UF    Read user defined units conversion factor.\n");
    printf("MD    Read power-up mode\n");
    printf("            Display     Continuous RS-232\n");
    printf("      0 =>    off               off\n");
    printf("      1 =>     on               off\n");
    printf("      2 =>    off                on\n");
    printf("      3 =>     on                on\n");
    printf("\n");
    printf("SN    Read transmitter serial number.\n");
    printf("PA    Read pressure adder.\n");
    printf("PM    Read pressure multiplier.\n");
    printf("TC    Read timebase correction factor.\n");
    printf("Cx    Read Cx pressure coefficient (x = 1, 2, 3).\n");
    printf("Dx    Read Dx pressure coefficient (x = 1, 2, 3).\n");
    printf("Tx    Read Tx pressure coefficient (x = 1, 2, 3).\n");
    printf("U0    Read U0 pressure coefficient.\n");
    printf("Yx    Read Yx pressure coefficient (x = 1, 2, 3).\n");
    printf("XM    Read nano-resolution mode (%.2lf or greater)\n", PARO_MIN_XM_REVISION);
    printf("IA    Read cutoff frequency parameter (%.2lf or greater)\n", PARO_MIN_XM_REVISION);
    printf("XT    Read temperature smothing (%.2lf or greater)\n", PARO_MIN_XM_REVISION);
    printf("XN    Read numeric format (%.2lf or greater)\n", PARO_MIN_XM_REVISION);
    printf("\n");
    printf("set   Set a value (eg, set un 1).\n");
    printf("isp   Set default parameters for use with an ISP.\n");
    printf("*...  Pass thru input directly to device (eg, *0100VR)\n");
    printf("raw   Print raw device replies.\n");
    printf("strip Strip preamble from device replies.\n");
    printf("cnf   Query device for current configuration.\n");
    printf("quit  Quit program.\n");
    
} 

#define MAXTOKEN 3

void set(char *command)
{
static char *token[MAXTOKEN];
int ntoken;

    ntoken = util_parse(command, token, " =", MAXTOKEN, 0);
    if (ntoken != 3) {
        printf("usage: set param value (eg, set un 1)\n");
        return;
    }

    if (paroSet(pp, paroAtoi(token[1]), token[2]) != 0) {
        printf("%s is not a modifiable parameter.\n", token[1]);
    }
}

void isp_defaults()
{
    printf("Loading ISP defaults... "); fflush(stdout);
    paroSet(pp, PARO_PR, "119");
    paroSet(pp, PARO_TR, "952");
    paroSet(pp, PARO_UN, "0");
    paroSet(pp, PARO_UF, "68947.57");
    paroSet(pp, PARO_PA, "0");
    paroSet(pp, PARO_PM, "1");
    printf("done\n");
}

main(int argc, char **argv)
{
int i;
speed_t baud = DEF_BAUD;
char *port   = DEF_PORT;
#define MAXCMDLEN 255
char command[MAXCMDLEN+1];
BOOL debug = FALSE;

    printf("paro - %s\n", VersionIdentString);

/*  Scan command line for default overrides  */

    for (i = 1; i < argc; i++) {

        if (strncmp(argv[i], "baud=", strlen("baud=")) == 0) {
            baud = (speed_t) atol(argv[i]+strlen("baud="));
        } else if (strcmp(argv[i], "-v") == 0) {
            debug = TRUE;
        } else {
            port = argv[i];
        }
    }

/*  Connect to the device  */

    printf("Connecting to device at `%s', speed %d\n", port, baud);
    if ((pp = paroOpen(port, baud, NULL, debug)) == (PARO *) NULL) {
        perror(port);
        exit(1);
    }
    usleep(250000);

    print_param(&pp->param);
    printf("\n");

/*  Set up exit handlers  */

    signal(SIGINT,  catcher);
    signal(SIGTERM, catcher);
    signal(SIGQUIT, catcher);

/* Process commands */

    printf("Enter `help' or `?' for help, `q' to quit.\n");
    while (1) {

        printf("paro> "); fflush(stdout);
        command[0] = 0;
        fgets(command, MAXCMDLEN, stdin); command[strlen(command)-1] = 0;

        if (strcasecmp(command, "q") == 0) {
            exit(0);
        } else if (strcasecmp(command, "quit") == 0) {
            exit(0);
        } else if (strcasecmp(command, "raw") == 0) {
            printf("raw device replies enabled\n");
            raw = 1;
        } else if (strcasecmp(command, "strip") == 0) {
            printf("stripped device replies enabled\n");
            raw = 0;
        } else if (strcasecmp(command, "cnf") == 0) {
            if (paroGetcnf(pp) != 0) {
                perror("paroGetcnf");
            } else {
                print_param(&pp->param);
            }
        } else if (strncasecmp(command, "set", strlen("set")) == 0) {
            set(command);
        } else if (strcasecmp(command, "isp") == 0) {
            isp_defaults();

        } else if (strcasecmp(command, "help") == 0) {
            list_commands();
        } else if (strcasecmp(command, "?") == 0) {
            list_commands();

        } else if (command[0] == '*') {
            pass_thru(command, 0);

        } else if (strcasecmp(command, "go") == 0) {
           read32();

        } else if (strcasecmp(command, "VR") == 0) {
            doit(PARO_VR, 0);

        } else if (strcasecmp(command, "P1") == 0) {
            doit(PARO_P1, 0);
        } else if (strcasecmp(command, "P2") == 0) {
            doit(PARO_P2, 1);
        } else if (strcasecmp(command, "P3") == 0) {
            doit(PARO_P3, 0);
        } else if (strcasecmp(command, "P4") == 0) {
            doit(PARO_P4, 1);
        } else if (strcasecmp(command, "P5") == 0) {
            doit(PARO_P5, -1);
        } else if (strcasecmp(command, "P6") == 0) {
            doit(PARO_P6, -1);
        } else if (strcasecmp(command, "P7") == 0) {
            doit(PARO_P7, 1);

        } else if (strcasecmp(command, "Q1") == 0) {
            doit(PARO_Q1, 0);
        } else if (strcasecmp(command, "Q2") == 0) {
            doit(PARO_Q2, 1);
        } else if (strcasecmp(command, "Q3") == 0) {
            doit(PARO_Q3, 0);
        } else if (strcasecmp(command, "Q4") == 0) {
            doit(PARO_Q4, 1);
        } else if (strcasecmp(command, "Q5") == 0) {
            doit(PARO_Q5, -1);
        } else if (strcasecmp(command, "Q6") == 0) {
            doit(PARO_Q6, -1);

        } else if (strcasecmp(command, "DB") == 0) {
            doit(PARO_DB, 0);
        } else if (strcasecmp(command, "DS") == 0) {
            doit(PARO_DS, 0);

        } else if (strcasecmp(command, "BL") == 0) {
            doit(PARO_BL, 0);
        } else if (strcasecmp(command, "EW") == 0) {
            doit(PARO_EW, -1);
        } else if (strcasecmp(command, "PR") == 0) {
            doit(PARO_PR, 0);
        } else if (strcasecmp(command, "TR") == 0) {
            doit(PARO_TR, 0);
        } else if (strcasecmp(command, "UN") == 0) {
            doit(PARO_UN, 0);
        } else if (strcasecmp(command, "UF") == 0) {
            doit(PARO_UF, 0);
        } else if (strcasecmp(command, "MD") == 0) {
            doit(PARO_MD, 0);

        } else if (strcasecmp(command, "CS") == 0) {
            doit(PARO_CS, 0);
        } else if (strcasecmp(command, "CT") == 0) {
            doit(PARO_CT, -1);
        } else if (strcasecmp(command, "CX") == 0) {
            doit(PARO_CX, -1);

        } else if (strcasecmp(command, "SN") == 0) {
            doit(PARO_SN, 0);
        } else if (strcasecmp(command, "PA") == 0) {
            doit(PARO_PA, 0);
        } else if (strcasecmp(command, "PM") == 0) {
            doit(PARO_PM, 0);
        } else if (strcasecmp(command, "TC") == 0) {
            doit(PARO_TC, 0);
        } else if (strcasecmp(command, "C1") == 0) {
            doit(PARO_C1, 0);
        } else if (strcasecmp(command, "C2") == 0) {
            doit(PARO_C2, 0);
        } else if (strcasecmp(command, "C3") == 0) {
            doit(PARO_C3, 0);
        } else if (strcasecmp(command, "D1") == 0) {
            doit(PARO_D1, 0);
        } else if (strcasecmp(command, "D2") == 0) {
            doit(PARO_D2, 0);
        } else if (strcasecmp(command, "T1") == 0) {
            doit(PARO_T1, 0);
        } else if (strcasecmp(command, "T2") == 0) {
            doit(PARO_T2, 0);
        } else if (strcasecmp(command, "T3") == 0) {
            doit(PARO_T3, 0);
        } else if (strcasecmp(command, "T4") == 0) {
            doit(PARO_T4, 0);
        } else if (strcasecmp(command, "T5") == 0) {
            doit(PARO_T5, 0);
        } else if (strcasecmp(command, "U0") == 0) {
            doit(PARO_U0, 0);
        } else if (strcasecmp(command, "Y1") == 0) {
            doit(PARO_Y1, 0);
        } else if (strcasecmp(command, "Y2") == 0) {
            doit(PARO_Y2, 0);
        } else if (strcasecmp(command, "Y3") == 0) {
            doit(PARO_Y3, 0);
        } else if (strcasecmp(command, "XM") == 0) {
            doit(PARO_XM, 0);
        } else if (strcasecmp(command, "IA") == 0) {
            doit(PARO_IA, 0);
        } else if (strcasecmp(command, "XT") == 0) {
            doit(PARO_XT, 0);
        } else if (strcasecmp(command, "XN") == 0) {
            doit(PARO_XN, 0);
        } else if (strcasecmp(command, "PI") == 0) {
            doit(PARO_PI, 0);
        } else if (strcasecmp(command, "TI") == 0) {
            doit(PARO_TI, 0);
        } else if (strcasecmp(command, "") == 0) {
            ;
        } else {
            printf("Unknown command: \"%s\"\n", command);
        }
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: paro.c,v $
 * Revision 1.5  2009/03/24 21:07:18  dechavez
 * 2.0.0
 *
 * Revision 1.4  2006/11/13 19:37:47  dechavez
 * added -v option
 *
 * Revision 1.3  2006/03/30 22:41:18  dechavez
 * use new libparo calls, removed MC command
 *
 * Revision 1.2  2005/05/27 00:18:14  dechavez
 * using oldttyio
 *
 * Revision 1.1.1.1  2000/02/08 20:20:17  dec
 * import existing IDA/NRTS sources
 *
 */
