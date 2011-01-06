#pragma ident "$Id: calib.c,v 1.3 2003/06/11 20:55:52 dechavez Exp $"
/*======================================================================
 *
 * Interactively generate a DAS calibration request.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "isp_console.h"

#define QUIT "q"

#define MAXLINLEN 16
static char buffer[MAXLINLEN];
static char *result;
static int val;

static int SelectValue(
    char *what,
    int def,
    int flag,
    int minval,
    int maxval,
    int quit
) {
int val;

    while (1) {
        if (flag) printf("\n");
        printf("Enter %s (%s to quit) ", what, QUIT);
        printf("[%d]", def);
        printf(": ");
        fflush(stdout);
        result = fgets(buffer, MAXLINLEN, stdin);
        if (result == (char *) NULL) {
            perror("fgets");
            exit(1);
        }
        result[strlen(result)-1] = 0;
        if (result[0] == 0) {
            return def;
        } else if (util_isinteger(result)) {
            val = atoi(result);
            if (val >= minval && val <= maxval) return val;
        } else if (strcasecmp(QUIT, result) == 0) {
            return quit;
        }
        printf("`%s' is not a valid response... try again\n", result);
        flag = 0;
    }
}

static int SelectPreset(int def, int quit)
{
    printf("\n");
    printf("Select one of the following:\n");
    printf("       Function    Amplitude   Period    Duration\n");
#ifdef USER_DEFINED_OK
    printf("0 =      USER         USER      USER       USER\n");
#endif /* USER_DEFINED_OK */
    printf("1 =  Random Binary   5.00V     20 sec    12 hour\n");
    printf("2 =  Random Binary   1.25V     20 sec    12 hour\n");
    printf("3 =  Random Binary   0.31V     20 sec    12 hour\n");
    printf("4 =  Triangle Wave   1.25V      1 sec     1 hour\n");
    printf("5 =  Triangle Wave   1.25V     10 sec     1 hour\n");
    printf("\n");
#ifdef USER_DEFINED_OK
    return SelectValue("code", def, 0, 0, 5, quit);
#else
    return SelectValue("code", def, 0, 1, 5, quit);
#endif /* USER_DEFINED_OK */
}

static int SelectFnc(int def, int quit)
{
    printf("\n");
    printf("Select a function:\n");
    printf("%d = impulse   ",          DASCAL_IMPULSE);
    printf("%d = square   ",           DASCAL_SQUARE);
    printf("%d = sine   ",             DASCAL_SINE);
    printf("%d = triangle   ",         DASCAL_TRIANGLE);
    printf("%d = random telegraph   ", DASCAL_RANDOM);
    printf("\n");
    return SelectValue("code", def, 0, DASCAL_FNCMIN, DASCAL_FNCMAX, quit);
}

static int SelectAmp(int def, int quit)
{
    printf("\n");
    printf("Select amplitude:\n");
    printf("%d = 5 V   ",    DASCAL_AMP50000);
    printf("%d = 1.25 V   ", DASCAL_AMP12500);
    printf("%d = 0.31 V   ", DASCAL_AMP03100);
    printf("%d = 78 mV   ",  DASCAL_AMP00780);
    printf("%d = 20 mV   ",  DASCAL_AMP00200);
    printf("%d = 4.9 mV   ", DASCAL_AMP00049);
    printf("%d = 1.2 mV",    DASCAL_AMP00012);
    printf("\n");
    return SelectValue("code", def, 0, DASCAL_AMPMIN, DASCAL_AMPMAX, quit);
}

static long SelectStart(int def, int quit)
{
struct tm *tm;
time_t now;
int yr, da, hr, mn, sc, result;

    printf("\n");
    printf("Select calibration start time:\n");
    printf("0 = immediately\n");
    printf("1 = user selected\n");
    printf("\n");
    result = SelectValue("code", def, 0, 0, 1, quit);
    if (result == 0) return 0;
    if (result == quit) return quit;

    now = time(NULL) + 900;
    printf("The earliest start time you will be able to enter is %s\n",
        util_lttostr(now, 0)
    );

    tm  = gmtime(&now);
    yr  = tm->tm_year + 1900;
    hr  = tm->tm_hour;

    yr = SelectValue("start year", yr, 1, yr, yr + 1, quit);
    if (yr == quit) return quit;
    da = 1 + ((yr == tm->tm_year + 1900) ? tm->tm_yday : 0);
    da = SelectValue("       day", da, 0, da, daysize(yr), quit);
    if (da == quit) return quit;
    if (yr == tm->tm_year + 1900 && da == tm->tm_yday + 1) {
        hr = tm->tm_hour;
    } else {
        hr = 0;
    }
    hr = SelectValue("      hour",  hr, 0, hr, 23, quit);
    if (hr == quit) return quit;
    if (
        yr == tm->tm_year + 1900 &&
        da == tm->tm_yday + 1 &&
        hr == tm->tm_hour
    ) {
        mn = tm->tm_min;
    } else {
        mn = 0;
    }
    mn = SelectValue("    minute", mn, 0, mn, 59, quit);
    if (mn == quit) return quit;
    if (
        yr == tm->tm_year + 1900 &&
        da == tm->tm_yday + 1 &&
        hr == tm->tm_hour &&
        mn == tm->tm_min
    ) {
        sc = tm->tm_sec;
    } else {
        sc = 0;
    }
    sc = SelectValue("    second", sc, 0, sc, 59, quit);
    if (sc == quit) return quit;
    return (long) util_ydhmsmtod(yr, da, hr, mn, sc, 0);
}

static void PrintCal(struct isp_dascal *cal)
{

    printf("\n");
    printf("Current calibration settings:\n");

    if (cal->preset) {
        printf("Preset configuration #%d\n", cal->preset);
    } else {

        printf("Function:  ");
        switch (cal->fnc) {
          case DASCAL_IMPULSE:
            printf("impulse");
            break;
          case DASCAL_SQUARE:
            printf("square wave");
            break;
          case DASCAL_SINE:
            printf("sine wave");
            break;
          case DASCAL_TRIANGLE:
            printf("triangle wave");
            break;
          case DASCAL_RANDOM:
            printf("random binary");
            break;
          default:
            printf("UNKNOWN");
        }
        printf("\n");

        printf("Amplitude: ");
        switch (cal->amp) {
          case DASCAL_AMP50000:
            printf("5 V");
            break;
          case DASCAL_AMP12500:
            printf("1.25 V");
            break;
          case DASCAL_AMP03100:
            printf("0.31 V");
            break;
          case DASCAL_AMP00780:
            printf("78 mV");
            break;
          case DASCAL_AMP00200:
            printf("4.9 mV");
            break;
          case DASCAL_AMP00049:
            printf("1.2 mV");
            break;
          case DASCAL_AMP00012:
            break;
          default:
            printf("UNKNOWN");
        }
        printf("\n");

        if (cal->fnc == DASCAL_SINE || cal->fnc == DASCAL_TRIANGLE) {
            printf("Period:    %d secs\n", cal->wid / 200);
        } else if (cal->fnc == DASCAL_RANDOM) {
            printf("Min per:   %d secs\n", cal->wid / 200);
        } else {
            printf("Interval:  %d secs\n", cal->ntr / 200);
            printf("Width:     %d secs\n", cal->wid / 200);
        }

        printf("Duration:  %s\n", util_lttostr((long)cal->dur, 8));
    }

    printf("Start:     ");
    if (cal->on <= 0) {
        printf("immediately");
    } else {
        printf("%s", util_lttostr(cal->on, 0));
    }
    printf("\n");
    printf("\n");
    fflush(stdout);
}

int DasCalibrate(struct isp_dascal *cal)
{
int da, hr, mn, sc, quit = -1, ok = 0;

    do {

    /* Offer pre-set configuration */

        cal->preset = SelectPreset(2, quit);
        if (cal->preset == quit) {
            return quit;
        } else if (!cal->preset) {

    /* User specified configuration requested */

            cal->fnc = DASCAL_RANDOM;
            cal->amp = DASCAL_AMP12500;
            cal->wid = 4000;
            cal->dur = 43200;
            cal->on  = 0;
            cal->ntr = 0;

            cal->fnc = SelectFnc(cal->fnc, quit);
            if (cal->fnc == quit) return quit;

            cal->amp = SelectAmp(cal->amp, quit);
            if (cal->amp == quit) return quit;

            if (cal->fnc == DASCAL_SINE || cal->fnc == DASCAL_TRIANGLE) {
                cal->wid = SelectValue(
                    "period (sec)", cal->wid/200, 1, 1, 86400, quit
                );
                if (cal->wid == quit) return quit;
                cal->wid *= 200;
                cal->ntr = 0;
            } else if (cal->fnc == DASCAL_RANDOM) {
                cal->wid = SelectValue(
                    "min. period (sec)", cal->wid/200, 1, 1, 86500, quit
                );
                if (cal->wid == quit) return quit;
                cal->wid *= 200;
                cal->ntr = 0;
            } else {
                do {
                    cal->wid = SelectValue(
                        "width (sec)", cal->wid/200, 1, 1, 86400, quit
                    );
                    if (cal->wid == quit) return quit;
                    cal->wid *= 200;
                    cal->ntr = SelectValue(
                        "interval (sec)", 2*cal->wid/200, 1, 1, 86400, quit
                    );
                    if (cal->ntr == quit) return quit;
                    cal->ntr *= 200;
                    if (cal->wid >= cal->ntr) {
                        printf("Illegal combo: width > interval!\n");
                    }
                } while (cal->wid >= cal->ntr);
            }
            hr = cal->dur / 3600;
            mn = (cal->dur - (hr * 3600)) / 60;
            sc = cal->dur - (hr * 3600) - (mn * 60);
            do {
                hr = SelectValue("duration hours", hr, 1, 0, 23, quit);
                if (hr == quit) return quit;
                mn = SelectValue("          mins", mn, 0, 0, 59, quit);
                if (mn == quit) return quit;
                sc = SelectValue("          secs", sc, 0, 0, 59, quit);
                if (sc == quit) return quit;
                cal->dur = (hr * 3600) + (mn * 60) + sc;
                if (cal->dur < 1) printf("Illegal duration, try again.\n");
            } while (cal->dur < 1);
        }
        cal->on = SelectStart(1, quit);
        if (cal->on == quit) return quit;
        PrintCal(cal);
    } while (!utilQuery("Accept? "));

    return ok;
}

#ifdef DEBUG_TEST

void main(int argc, char **argv)
{
struct isp_dascal cal;

    calibrate(&cal);
    PrintCal(&cal);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: calib.c,v $
 * Revision 1.3  2003/06/11 20:55:52  dechavez
 * use new form of utilQuery and utilPause
 *
 * Revision 1.2  2000/10/19 22:24:52  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:05  dec
 * import existing IDA/NRTS sources
 *
 */
