#pragma ident "$Id: paro.c,v 1.11 2009/05/14 16:29:33 dechavez Exp $"
/*======================================================================
 *
 *  Routines to interact with Paroscientific Digiquartz devices.
 *
 *====================================================================*/
#include "paro.h"

/* Command equivalents of flags in paro.h */

static char *paroCmdmap[] = {
    (char *) NULL,
    "*0100P1", "*0100P2", "*0100P3", "*0100P4", "*0100P5", "*0100P6",
    "*0100P7", "*0100Q1", "*0100Q2", "*0100Q3", "*0100Q4", "*0100Q5",
    "*0100Q6", "*0100DB", "*0100DS", "*9900BL", "*0100EW", "*0100PR",
    "*0100TR", "*0100UN", "*0100UF", "*0100MD", "*0100VR", "*0100MC",
    "*0100CS", "*0100CT", "*0100CX", "*0100SN", "*0100PA", "*0100PM",
    "*0100TC", "*0100C1", "*0100C2", "*0100C3", "*0100D1", "*0100D2",
    "*0100T1", "*0100T2", "*0100T3", "*0100T4", "*0100T5", "*0100U0",
    "*0100Y1", "*0100Y2", "*0100Y3", "*0100XM", "*0100IA", "*0100XT",
    "*0100XN", "*0100PI", "*0100TI", "*9900BR"
};

/* String equivalent of flags in paro.h */

static char *paroKeymap[] = {
    (char *) NULL,
    "P1", "P2", "P3", "P4", "P5", "P6", "P7", "Q1", "Q2", "Q3", "Q4",
    "Q5", "Q6", "DB", "DS", "BL", "EW", "PR", "TR", "UN", "UF", "MD",
    "VR", "MC", "CS", "CT", "CX", "SN", "PA", "PM", "TC", "C1", "C2",
    "C3", "D1", "D2", "T1", "T2", "T3", "T4", "T5", "U0", "Y1", "Y2",
    "Y3", "XM", "IA", "XT", "XN", "PI", "TI", "BR"
};

/* Convert from key to string equivalent */

char *paroItoa(int key)
{
    if (key < PARO_MINKEY || key > PARO_MAXKEY) return (char *) NULL;
    return paroKeymap[key];
}

/* Convert from string to key */

int paroAtoi(char *string)
{
int i;

    for (i = 1; i <= PARO_MAXKEY; i++) {
        if (strcasecmp(string, paroKeymap[i]) == 0) return i;
    }
    return -1;
}

/* Write to device with trailing CRLF */

BOOL paroWrite(PARO *pp, char *string)
{
INT32 len, result;
static char crlf[2] = {0x0d, 0x0a};

    if (pp == (PARO *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (pp->debug) {
        printf("send: '%s'", string);
        fflush(stdout);
    }

    len = strlen(string);
    if ((result = ttyioWrite(pp->tty, string, len) != len)) {
        if (pp->debug) perror(" failed: ");
        return FALSE;
    }
    if ((result = ttyioWrite(pp->tty, crlf, 2) != 2)) {
        if (pp->debug) perror(" failed: ");
        return FALSE;
    }

    if (pp->debug) printf("\n");

    return TRUE;
}

/* Send one command */

char *paroCommand(PARO *pp, int key)
{
int try;
BOOL ok;
char *command;
static char *fid = "paroCommand";

    if (pp == (PARO *) NULL) {
        logioMsg(pp->lp, LOG_ERR, "%s: NULL input", fid);
        errno = EINVAL;
        return (char *) NULL;
    }

    if (key < PARO_MINKEY || key > PARO_MAXKEY) {
        logioMsg(pp->lp, LOG_ERR, "%s: bad key value (%d)", fid, key);
        errno = EINVAL;
        return (char *) NULL;
    }

    command = paroCmdmap[key];
    try = 0;
    do {
        if (try > 0) utilDelayMsec(250);
        ok = paroWrite(pp, command);
    } while (!ok && ++try < PARO_MAXTRY);

    return ok ? command : NULL;
}

/* "reset" the device */

int paroReset(PARO *pp)
{
    if (pp == (PARO *) NULL) {
        errno = EINVAL;
        return -1;
    }

    if (paroCommand(pp, PARO_VR) == (char *) NULL) return -1;
    while(ttyioRead(pp->tty, pp->buf, 1));

    return 0;
}

/* Set a value */

int paroSet(PARO *pp, int key, char *value)
{
static int valid[] = {
 PARO_PR, PARO_TR, PARO_UN, PARO_UF, PARO_MD, PARO_SN, PARO_PA,
 PARO_PM, PARO_TC, PARO_C1, PARO_C2, PARO_C3, PARO_D1, PARO_D2,
 PARO_T1, PARO_T2, PARO_T3, PARO_T4, PARO_T5, PARO_U0, PARO_Y1,
 PARO_Y2, PARO_Y3, PARO_XM, PARO_IA, PARO_XT, PARO_XN, PARO_PI,
 PARO_TI, PARO_BL, PARO_BR, -1
};
int i, ok;

    if (pp == (PARO *) NULL) {
        errno = EINVAL;
        return -1;
    }

    ok = 0;
    for (i = 0; ok == 0 && valid[i] != -1; i++) ok = (key == valid[i]);

    if (!ok) {
        errno = EINVAL;
        return -1;
    }

    if (paroCommand(pp, PARO_EW) == (char *) NULL) return -2;
    sprintf(pp->buf, "%s=%s", paroCmdmap[key], value);
    paroWrite(pp, pp->buf);
    while(ttyioRead(pp->tty, pp->buf, 1));

    return 0;
}
    
/* Read a reply, stripping off the preamble */

char *paroRead(PARO *pp)
{
INT32 result;
int i, synced, drop, maxi;
static char *fid = "paroRead";

    if (pp == (PARO *) NULL || pp->tty < 0 || pp->buf == (char *) NULL) {
        logioMsg(pp->lp, LOG_ERR, "%s: NULL input", fid);
        errno = EINVAL;
        return NULL;
    }

    i      = 1;
    maxi   = PARO_BUFLEN - 2;
    drop   = 0;
    synced = 0;
    while (1) {
        if ((result = ttyioRead(pp->tty, pp->buf, 1)) != 1) {
            logioMsg(pp->lp, LOG_DEBUG, "%s: ttyioRead returns %d, not %d", fid, result, 1);
            return NULL;
        } else if (!synced) {
            if (pp->buf[0] == '*') {
                synced = 1;
                drop   = 4;
            }
        } else {
            if (drop > 0) {
                --drop;
            } else if (pp->buf[0] == 0x0d || pp->buf[0] == 0x0a || i == maxi) {
                pp->buf[i] = 0;
                if (pp->debug) printf("recv: '%s'\n", pp->buf + 1);
                return pp->buf + 1;
            } else {
                pp->buf[i++] = pp->buf[0];
            }
        }
    }
}

/* Read a pressure value, convert to INT32 */

BOOL paroReadINT32(PARO *pp, INT32 *result)
{
char *string;
double value;
static char *fid = "paroRead";

    if (pp == NULL || result == NULL) {
        logioMsg(pp->lp, LOG_ERR, "%s: NULL input", fid);
        errno = EINVAL;
        return FALSE;
    }
    if ((string = paroRead(pp)) == NULL) return FALSE;

    value = atof(string);
    if (pp->param.revision < PARO_MIN_XM_REVISION || pp->param.xm == 0) {
        *result = (INT32) value;
    } else {
        *result = (INT32) ((value - PARO_NORMAL_VALUE) * PARO_NORMAL_FACTOR);
    }

    return TRUE;
}

/* Query the device for a specific parameter */

char *paroRawval(PARO *pp, int key, int *type)
{
int try;
char *reply, *check;
static char *fid = "paroRawval";

    if (pp == (PARO *) NULL || type == (int *) NULL) {
        logioMsg(pp->lp, LOG_ERR, "%s: NULL input", fid);
        errno = EINVAL;
        return (char *) NULL;
    }

    switch (key) {
      case PARO_SN:
        *type = PARO_STRING;
        check = "SN";
        break;

      case PARO_VR:
        *type = PARO_STRING;
        check = "VR";
        break;

      case PARO_MC:
        *type = PARO_STRING;
        check = "MC";
        break;

      case PARO_UN:
        *type = PARO_INT;
        check = "UN";
        break;

      case PARO_UF:
        *type = PARO_DOUBLE;
        check = "UF";
        break;

      case PARO_PR:
        *type = PARO_INT;
        check = "PR";
        break;

      case PARO_TR:
        *type = PARO_INT;
        check = "TR";
        break;

      case PARO_PA:
        *type = PARO_DOUBLE;
        check = "PA";
        break;

      case PARO_PM:
        *type = PARO_DOUBLE;
        check = "PM";
        break;

      case PARO_TC:
        *type = PARO_DOUBLE;
        check = "TC";
        break;

      case PARO_C1:
        *type = PARO_DOUBLE;
        check = "C1";
        break;

      case PARO_C2:
        *type = PARO_DOUBLE;
        check = "C2";
        break;

      case PARO_C3:
        *type = PARO_DOUBLE;
        check = "C3";
        break;

      case PARO_D1:
        *type = PARO_DOUBLE;
        check = "D1";
        break;

      case PARO_D2:
        *type = PARO_DOUBLE;
        check = "D2";
        break;

      case PARO_T1:
        *type = PARO_DOUBLE;
        check = "T1";
        break;

      case PARO_T2:
        *type = PARO_DOUBLE;
        check = "T2";
        break;

      case PARO_T3:
        *type = PARO_DOUBLE;
        check = "T3";
        break;

      case PARO_T4:
        *type = PARO_DOUBLE;
        check = "T4";
        break;

      case PARO_T5:
        *type = PARO_DOUBLE;
        check = "T5";
        break;

      case PARO_U0:
        *type = PARO_DOUBLE;
        check = "U0";
        break;

      case PARO_Y1:
        *type = PARO_DOUBLE;
        check = "Y1";
        break;

      case PARO_Y2:
        *type = PARO_DOUBLE;
        check = "Y2";
        break;

      case PARO_Y3:
        *type = PARO_DOUBLE;
        check = "Y3";
        break;

      case PARO_XM:
        *type = PARO_INT;
        check = "XM";
        break;

      case PARO_IA:
        *type = PARO_INT;
        check = "IA";
        break;

      case PARO_XT:
        *type = PARO_INT;
        check = "XT";
        break;

      case PARO_XN:
        *type = PARO_INT;
        check = "XN";
        break;

      case PARO_PI:
        *type = PARO_INT;
        check = "PI";
        break;

      case PARO_TI:
        *type = PARO_INT;
        check = "TI";
        break;

      default:
        errno = EINVAL;
        return (char *) NULL;
    }

    try = 0;
    do {
        if (paroCommand(pp, key) == (char *) NULL) {
            logioMsg(pp->lp, LOG_ERR, "%s: paroCommand failed: errno = %d", fid, errno);
            return (char *) NULL;
        }
        if ((reply = paroRead(pp)) != (char *) NULL) {
            reply = strstr(reply, check);
        }
    } while (reply == (char *) NULL && try++ < PARO_MAXTRY);

    if (reply == NULL) logioMsg(pp->lp, LOG_DEBUG, "%s: return NULL reply", fid);
    return reply;
}

int paroGetval(PARO *pp, int key, void *ptr)
{
int type, ival;
double dval;
char *reply, *token;
static char *fid = "paroGetval";

    if (pp == (PARO *) NULL || ptr == (void *) NULL) {
        logioMsg(pp->lp, LOG_ERR, "%s: NULL input", fid);
        errno = EINVAL;
        return -1;
    }

    if ((reply = paroRawval(pp, key, &type)) == NULL) {
        logioMsg(pp->lp, LOG_DEBUG, "%s: paroRawval failed", fid);
        return -1;
    }

    if ((token = strstr(reply, "=")) == (char *) NULL) {
        logioMsg(pp->lp, LOG_DEBUG, "%s: strstr failed", fid);
        errno = EINVAL;
        return -2;
    }
    ++token;

    if (type == PARO_INT) {
        ival = atoi(token);
        memcpy(ptr, &ival, sizeof(ival));
    } else if (type == PARO_DOUBLE) {
        dval = atof(token);
        memcpy(ptr, &dval, sizeof(dval));
    } else {
        if (strlen(token) > PARO_MAXCHAR) token[PARO_MAXCHAR] = 0;
        while (token[0] == ' ') ++token;
        memcpy(ptr, token, strlen(token)+1);
    }

    return 0;
}

/* Get the current configuration */

int paroGetcnf(PARO *pp)
{
static char *fid = "paroGetcnf";

    if (pp == (PARO *) NULL) {
        errno = EINVAL;
        logioMsg(pp->lp, LOG_ERR, "%s: NULL input", fid);
        return -1;
    }

    if (paroGetval(pp, PARO_SN,  pp->param.sn) != 0) return -2;
    if (paroGetval(pp, PARO_VR,  pp->param.vr) != 0) return -2;
    pp->param.revision = atof(&pp->param.vr[1]);
    if (paroGetval(pp, PARO_UN, &pp->param.un) != 0) return -2;
    if (paroGetval(pp, PARO_UF, &pp->param.uf) != 0) return -2;
    if (paroGetval(pp, PARO_PR, &pp->param.pr) != 0) return -2;
    if (paroGetval(pp, PARO_TR, &pp->param.tr) != 0) return -2;
    if (paroGetval(pp, PARO_PA, &pp->param.pa) != 0) return -2;
    if (paroGetval(pp, PARO_PM, &pp->param.pm) != 0) return -2;
    if (paroGetval(pp, PARO_TC, &pp->param.tc) != 0) return -2;
    if (paroGetval(pp, PARO_C1, &pp->param.c1) != 0) return -2;
    if (paroGetval(pp, PARO_C2, &pp->param.c2) != 0) return -2;
    if (paroGetval(pp, PARO_C3, &pp->param.c3) != 0) return -2;
    if (paroGetval(pp, PARO_D1, &pp->param.d1) != 0) return -2;
    if (paroGetval(pp, PARO_D2, &pp->param.d2) != 0) return -2;
    if (paroGetval(pp, PARO_T1, &pp->param.t1) != 0) return -2;
    if (paroGetval(pp, PARO_T2, &pp->param.t2) != 0) return -2;
    if (paroGetval(pp, PARO_T3, &pp->param.t3) != 0) return -2;
    if (paroGetval(pp, PARO_T4, &pp->param.t4) != 0) return -2;
    if (paroGetval(pp, PARO_T5, &pp->param.t5) != 0) return -2;
    if (paroGetval(pp, PARO_U0, &pp->param.u0) != 0) return -2;
    if (paroGetval(pp, PARO_Y1, &pp->param.y1) != 0) return -2;
    if (paroGetval(pp, PARO_Y2, &pp->param.y2) != 0) return -2;
    if (paroGetval(pp, PARO_Y3, &pp->param.y3) != 0) return -2;
    if (pp->param.revision >= PARO_MIN_PI_REVISION) {
        if (paroGetval(pp, PARO_PI, &pp->param.pi) != 0) return -2;
        if (paroGetval(pp, PARO_TI, &pp->param.ti) != 0) return -2;
    }
    if (pp->param.revision >= PARO_MIN_XM_REVISION) {
        if (paroGetval(pp, PARO_XM, &pp->param.xm) != 0) return -2;
        if (paroGetval(pp, PARO_IA, &pp->param.ia) != 0) return -2;
        if (paroGetval(pp, PARO_XT, &pp->param.xt) != 0) return -2;
        if (paroGetval(pp, PARO_XN, &pp->param.xn) != 0) return -2;
    }

    return 0;
}

BOOL paroHasNano(PARO_PARAM *param)
{

}

/* Memory test */

int paroTest(PARO *pp)
{
int type;
char *reply;

    reply = paroRawval(pp, PARO_MC, &type);
    if (reply == (char *) NULL) return -1;
    return (strcasecmp("MC=Y", reply));
}

/* Connect to the device */

PARO *paroOpen(char *port, int speed, LOGIO *lp, BOOL debug)
{
int save;
PARO *pp;
static char *fid = "paroOpen";

    if ((pp = (PARO *) malloc(sizeof(PARO))) == (PARO *) NULL) return NULL;
    pp->lp = lp;
    pp->debug = debug;

/* Connect to the device */

    pp->tty = ttyioOpen(
        port,              /* device name */
        TRUE,              /* lock */
        speed,             /* input speed */
        speed,             /* output speed */
        TTYIO_PARITY_NONE, /* parity */
        TTYIO_FLOW_NONE,   /* flow control */
        2,                 /* stop bits */
        PARO_TO * 1000,    /* I/O timeout */
        0,                 /* pipe option */
        lp                 /* log handle */
    );

    if (pp->tty == NULL) {
        logioMsg(pp->lp, LOG_ERR, "%s: ttyioOpen: %s: %s", fid, port, strerror(errno));
        free(pp);
        return NULL;
    }
    if (pp->debug) printf("connected to %s, speed %d\n", port, speed);

    if ((paroReset(pp)) != 0) {
        logioMsg(pp->lp, LOG_DEBUG, "%s: paroReset: %s", fid, strerror(errno));
        ttyioClose(pp->tty);
        free(pp);
        return NULL;
    }

/* Get the current configuration */

    if (paroGetcnf(pp) != 0) {
        save = errno;
        logioMsg(pp->lp, LOG_DEBUG, "%s: paroGetcnf: %s", fid, strerror(errno));
        ttyioClose(pp->tty);
        free(pp);
        errno = save;
        return NULL;
    }

    return pp;
}

/* Close the device */

void paroClose(PARO *pp)
{
    if (pp != (PARO *) NULL) ttyioClose(pp->tty);
    free(pp);
}

/* This next section is to allow connecting to the device and setting the
 * configuration to a required state with a single call to paroInit()
 */

/* Set one integer value */

static void SetIntParoValue(PARO *pp, int key, int value)
{
char tmp[32];

    sprintf(tmp, "%d", value);
    paroSet(pp, key, tmp);
}

/* Convert a double to string for safe comparisons */

typedef struct {
    char buf[32];
    char *string;
} TEXT_THING;

static void BuildDoubleString(double value, TEXT_THING *text)
{
    sprintf(text->buf, "%9.7g", value);
    text->string = text->buf;
    while (*text->string == ' ') ++text->string;
}

/* Compare constants */

BOOL paroVerify(PARO *pp, PARO_PARAM *want)
{
struct {
    TEXT_THING have;
    TEXT_THING want;
} tmpstr;

    if (pp->param.pr != want->pr) return FALSE;
    if (pp->param.un != want->un) return FALSE;

/* Do string compares for PA, PM and UF, since they are not integers */

    BuildDoubleString(pp->param.pa, &tmpstr.have);
    BuildDoubleString(want->pa, &tmpstr.want);
    if (strcmp(tmpstr.have.string, tmpstr.want.string) != 0) return FALSE;

    BuildDoubleString(pp->param.pm, &tmpstr.have);
    BuildDoubleString(want->pm, &tmpstr.want);
    if (strcmp(tmpstr.have.string, tmpstr.want.string) != 0) return FALSE;

    BuildDoubleString(pp->param.uf, &tmpstr.have);
    BuildDoubleString(want->uf, &tmpstr.want);
    if (strcmp(tmpstr.have.string, tmpstr.want.string) != 0) return FALSE;

    return TRUE;
}

/* Ensure contants match */

static BOOL SetConfig(PARO *pp, PARO_PARAM *want)
{
struct {
    TEXT_THING have;
    TEXT_THING want;
} tmpstr;

    if (pp->param.revision >= PARO_MIN_PI_REVISION) {
        if (pp->param.pi != want->pi) {
            SetIntParoValue(pp, PARO_PI, want->pi);
            logioMsg(pp->lp, LOG_INFO, "set barometer PI to %d (was %d)\n", want->pi, pp->param.pi);
        }
    } else {
        if (pp->param.pr != want->pr) {
            SetIntParoValue(pp, PARO_PR, want->pr);
            logioMsg(pp->lp, LOG_INFO, "set barometer PR to %d (was %d)\n", want->pr, pp->param.pr);
        }
    }

    if (pp->param.un != want->un) {
        SetIntParoValue(pp, PARO_UN, want->un);
        logioMsg(pp->lp, LOG_INFO, "set barometer UN to %d (was %d)\n", want->un, pp->param.un);
    }

/* PA, PM, and UF are handled as strings since they are non-integer quantities */

    BuildDoubleString(want->pa, &tmpstr.want);
    BuildDoubleString(pp->param.pa, &tmpstr.have);
    if (strcmp(tmpstr.want.string, tmpstr.have.string) != 0) {
        paroSet(pp, PARO_PA, tmpstr.want.string);
        logioMsg(pp->lp, LOG_INFO, "set barometer PA to %s (was %s)\n", tmpstr.want.string, tmpstr.have.string);
    }

    BuildDoubleString(want->pm, &tmpstr.want);
    BuildDoubleString(pp->param.pm, &tmpstr.have);
    if (strcmp(tmpstr.want.string, tmpstr.have.string) != 0) {
        paroSet(pp, PARO_PM, tmpstr.want.string);
        logioMsg(pp->lp, LOG_INFO, "set barometer PM to %s (was %s)\n", tmpstr.want.string, tmpstr.have.string);
    }

    BuildDoubleString(want->uf, &tmpstr.want);
    BuildDoubleString(pp->param.uf, &tmpstr.have);
    if (strcmp(tmpstr.want.string, tmpstr.have.string) != 0) {
        paroSet(pp, PARO_UF, tmpstr.want.string);
        logioMsg(pp->lp, LOG_INFO, "set barometer UF to %s (was %s)\n", tmpstr.want.string, tmpstr.have.string);
    }

/* Read back config */

    if (paroGetcnf(pp) != 0) {
        logioMsg(pp->lp, LOG_WARN, "paroGetcnf: %s\n", strerror(errno));
        logioMsg(pp->lp, LOG_WARN, "unable to read barometer constants after update!\n");
        return FALSE;
    }

    return paroVerify(pp, want);
}

/* Connect to the device and set configuration */

BOOL paroInit(PARO *pp, PARO_PARAM *want)
{
static char *fid = "paroInit";

    if (pp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Get the current configuration */

    if (paroReset(pp) != 0) {
        if (errno != ETIMEDOUT) logioMsg(pp->lp, LOG_ERR, "%s: paroReset failed\n", fid);
        return FALSE;
    }

    if (paroGetcnf(pp) != 0) {
        if (errno != ETIMEDOUT) logioMsg(pp->lp, LOG_ERR, "%s: paroGetcnf failed\n", fid);
        return FALSE;
    }

/* Verify constants constants */

    if (!SetConfig(pp, want)) {
        logioMsg(pp->lp, LOG_ERR, "%s: Unable to set/verify barometer constants!\n", fid);
        return FALSE;
    }

/* Note device identifiers */

    sprintf(want->sn, pp->param.sn);
    sprintf(want->vr, pp->param.vr);
    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * Revision 1.11  2009/05/14 16:29:33  dechavez
 * added support for PARO_BR (baud rate)
 *
 * Revision 1.10  2009/03/24 20:57:03  dechavez
 * removed param from paroOpen, added debug.  Removed verbose from param field
 * and print now based on handle debug flag
 *
 * Revision 1.9  2009/03/20 21:09:51  dechavez
 *  added XM, IA, XT, XN, PI, and TI support, added device revision awarness
 * (not all units support these new commands).  Currently support for PI and TI
 * is abritrarily tagged at starting with R5.10 when in fact it is earlier (but
 * vendor can't say when!).
 *
 * Revision 1.8  2006/10/17 19:16:46  dechavez
 * added printf's when verbose flag is set, make sure that errno is preserved when paroOpen fails
 *
 * Revision 1.7  2006/06/26 22:08:20  dechavez
 * treat PA and PM as the floats they are instead of ints
 *
 * Revision 1.6  2006/05/17 23:23:45  dechavez
 * added copyright notice
 *
 * Revision 1.5  2006/04/03 21:42:01  dechavez
 * turned down logging volume, cleaned up memory/device leaks in paroOpen()
 *
 * Revision 1.4  2006/03/30 22:40:23  dechavez
 * initial release, based on what is now liboldparo but using new ttyio library calls
 *
 */
