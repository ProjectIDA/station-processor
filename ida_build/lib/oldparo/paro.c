#pragma ident "$Id: paro.c,v 1.1 2006/03/30 22:38:46 dechavez Exp $"
/*======================================================================
 *
 *  Routines to interact with Paroscientific Digiquartz devices.
 *
 *  These routines use the deprecated old ttyio library.  New code
 *  should use the routines in libparo and link with libtty for
 *  maximum portability.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "util.h"
#include "oldttyio.h"
#include "oldparo.h"

/* Command equivalents of flags in paro.h */

static char *paro_cmdmap[] = {
    (char *) NULL,
    "*0100P1", "*0100P2", "*0100P3", "*0100P4", "*0100P5", "*0100P6",
    "*0100P7", "*0100Q1", "*0100Q2", "*0100Q3", "*0100Q4", "*0100Q5",
    "*0100Q6", "*0100DB", "*0100DS", "*9900BL", "*0100EW", "*0100PR",
    "*0100TR", "*0100UN", "*0100UF", "*0100MD", "*0100VR", "*0100MC",
    "*0100CS", "*0100CT", "*0100CX", "*0100SN", "*0100PA", "*0100PM",
    "*0100TC", "*0100C1", "*0100C2", "*0100C3", "*0100D1", "*0100D2",
    "*0100T1", "*0100T2", "*0100T3", "*0100T4", "*0100T5", "*0100U0",
    "*0100Y1", "*0100Y2", "*0100Y3"
};

/* String equivalent of flags in paro.h */

static char *paro_keymap[] = {
    (char *) NULL,
    "P1", "P2", "P3", "P4", "P5", "P6", "P7", "Q1", "Q2", "Q3", "Q4",
    "Q5", "Q6", "DB", "DS", "BL", "EW", "PR", "TR", "UN", "UF", "MD",
    "VR", "MC", "CS", "CT", "CX", "SN", "PA", "PM", "TC", "C1", "C2",
    "C3", "D1", "D2", "T1", "T2", "T3", "T4", "T5", "U0", "Y1", "Y2",
    "Y3"
};

/* Convert from key to string equivalent */

char *paro_itoa(int key)
{
    if (key < PARO_MINKEY || key > PARO_MAXKEY) return (char *) NULL;
    return paro_keymap[key];
}

/* Convert from string to key */

int paro_atoi(char *string)
{
int i;

    for (i = 1; i <= PARO_MAXKEY; i++) {
        if (strcasecmp(string, paro_keymap[i]) == 0) return i;
    }
    return -1;
}

/* Write to device with trailing CRLF */

int paro_write(PARO *pp, char *string)
{
int len;
static char crlf[2] = {0x0d, 0x0a};

    if (pp == (PARO *) NULL) {
        errno = EINVAL;
        return -1;
    }

    len = strlen(string);
    if (write(pp->fd, string, len) != len) return -1;
    if (write(pp->fd, crlf, 2) != 2) return -1;

    return 0;
}

/* Send one command */

char *paro_command(PARO *pp, int key)
{
char *command;

    if (pp == (PARO *) NULL) {
        errno = EINVAL;
        return (char *) NULL;
    }

    if (key < PARO_MINKEY || key > PARO_MAXKEY) {
        errno = EINVAL;
        return (char *) NULL;
    }

    command = paro_cmdmap[key];
    return paro_write(pp, command) == 0 ? command : (char *) NULL;
}

/* "reset" the device */

int paro_reset(PARO *pp)
{
    if (pp == (PARO *) NULL) {
        errno = EINVAL;
        return -1;
    }

    if (paro_command(pp, PARO_VR) == (char *) NULL) return -1;
    while(util_read(pp->fd, pp->buf, 1, pp->to));

    return 0;
}

/* Set a value */

int paro_set(PARO *pp, int key, char *value)
{
static int valid[] = {
 PARO_PR, PARO_TR, PARO_UN, PARO_UF, PARO_MD, PARO_SN, PARO_PA,
 PARO_PM, PARO_TC, PARO_C1, PARO_C2, PARO_C3, PARO_D1, PARO_D2,
 PARO_T1, PARO_T2, PARO_T3, PARO_T4, PARO_T5, PARO_U0, PARO_Y1,
 PARO_Y2, PARO_Y3, -1
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

    if (paro_command(pp, PARO_EW) == (char *) NULL) return -2;
    sprintf(pp->buf, "%s=%s", paro_cmdmap[key], value);
    paro_write(pp, pp->buf);
    while(util_read(pp->fd, pp->buf, 1, pp->to));

    return 0;
}
    
/* Read a reply, stripping off the preamble */

char *paro_read(PARO *pp)
{
int i, synced, drop, maxi;
static char *null = (char *) NULL;

    if (pp == (PARO *) NULL || pp->fd < 0 || pp->buf == (char *) NULL) {
        errno = EINVAL;
        return null;
    }

    i      = 1;
    maxi   = PARO_BUFLEN - 2;
    drop   = 0;
    synced = 0;
    while (1) {
        if (util_read(pp->fd, pp->buf + 0, 1, pp->to) != 1) return null;
        if (!synced) {
            if (pp->buf[0] == '*') {
                synced = 1;
                drop   = 4;
            }
        } else {
            if (drop > 0) {
                --drop;
            } else if (
                pp->buf[0] == 0x0d || pp->buf[0] == 0x0a || i == maxi
            ) {
                pp->buf[i] = 0;
                return pp->buf + 1;
            } else {
                pp->buf[i++] = pp->buf[0];
            }
        }
    }
}

/* Query the device for a specific parameter */

char *paro_rawval(PARO *pp, int key, int *type)
{
int try;
char *reply, *check;

    if (pp == (PARO *) NULL || type == (int *) NULL) {
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

      default:
        errno = EINVAL;
        return (char *) NULL;
    }

    try = 0;
    do {
        if (paro_command(pp, key) == (char *) NULL) return (char *) NULL;
        if ((reply = paro_read(pp)) != (char *) NULL) {
            reply = strstr(reply, check);
        }
    } while (reply == (char *) NULL && try++ < PARO_MAXTRY);

    return reply;
}

int paro_getval(PARO *pp, int key, void *ptr)
{
int type, ival;
double dval;
char *reply, *token;

    if (pp == (PARO *) NULL || ptr == (void *) NULL) {
        errno = EINVAL;
        return -1;
    }

    reply = paro_rawval(pp, key, &type);
    if (reply == (char *) NULL) return -1;

    if ((token = strstr(reply, "=")) == (char *) NULL) {
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

int paro_getcnf(PARO *pp, struct paro_param *param)
{

    if (pp == (PARO *) NULL || param == (struct paro_param *) NULL) {
        errno = EINVAL;
        return -1;
    }

    if (paro_getval(pp, PARO_SN,  param->sn) != 0) return -2;
    if (paro_getval(pp, PARO_VR,  param->vr) != 0) return -2;
    if (paro_getval(pp, PARO_UN, &param->un) != 0) return -2;
    if (paro_getval(pp, PARO_UF, &param->uf) != 0) return -2;
    if (paro_getval(pp, PARO_PR, &param->pr) != 0) return -2;
    if (paro_getval(pp, PARO_TR, &param->tr) != 0) return -2;
    if (paro_getval(pp, PARO_PA, &param->pa) != 0) return -2;
    if (paro_getval(pp, PARO_PM, &param->pm) != 0) return -2;
    if (paro_getval(pp, PARO_TC, &param->tc) != 0) return -2;
    if (paro_getval(pp, PARO_C1, &param->c1) != 0) return -2;
    if (paro_getval(pp, PARO_C2, &param->c2) != 0) return -2;
    if (paro_getval(pp, PARO_C3, &param->c3) != 0) return -2;
    if (paro_getval(pp, PARO_D1, &param->d1) != 0) return -2;
    if (paro_getval(pp, PARO_D2, &param->d2) != 0) return -2;
    if (paro_getval(pp, PARO_T1, &param->t1) != 0) return -2;
    if (paro_getval(pp, PARO_T2, &param->t2) != 0) return -2;
    if (paro_getval(pp, PARO_T3, &param->t3) != 0) return -2;
    if (paro_getval(pp, PARO_T4, &param->t4) != 0) return -2;
    if (paro_getval(pp, PARO_T5, &param->t5) != 0) return -2;
    if (paro_getval(pp, PARO_U0, &param->u0) != 0) return -2;
    if (paro_getval(pp, PARO_Y1, &param->y1) != 0) return -2;
    if (paro_getval(pp, PARO_Y2, &param->y2) != 0) return -2;
    if (paro_getval(pp, PARO_Y3, &param->y3) != 0) return -2;

    return 0;
}

/* Memory test */

int paro_test(PARO *pp)
{
int type;
char *reply;

    reply = paro_rawval(pp, PARO_MC, &type);
    if (reply == (char *) NULL) return -1;
    return (strcasecmp("MC=Y", reply));
}

/* Connect to the device */

PARO *paro_open(char *port, int speed, struct paro_param *param)
{
PARO *pp, *null = (PARO *) NULL;;

    if ((pp = (PARO *) malloc(sizeof(PARO))) == (PARO *) NULL) return null;

    pp->to = PARO_TO;
    if (pp->to < 2) pp->to = 2;

/* Connect to the device */

    pp->fd = ttyio_init(
        port,
        (TTYIO_READ & TTYIO_WRITE),
        speed,
        speed,
        TTYIO_PNONE,
        TTYIO_FNONE,
        (struct termios *) NULL,
        (struct termios *) NULL,
        1
    );
    if (pp->fd < 0) {
        free(pp);
        return null;
    }
    util_noblock(pp->fd);

    paro_reset(pp);

/* Get the current configuration */

    if (paro_getcnf(pp, param) != 0) {
        free(pp);
        return null;
    }

    return pp;
}

/* Close the device */

void paro_close(PARO *pp)
{
    if (pp != (PARO *) NULL) close(pp->fd);
    free(pp);
}

/* Revision History
 *
 * $Log: paro.c,v $
 * Revision 1.1  2006/03/30 22:38:46  dechavez
 * formally libparo
 *
 */
