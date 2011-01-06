/* @(#)timefunc.c	2.4 8/6/96 */
/*======================================================================
 *
 *  Misc. time related functions.
 *
 *----------------------------------------------------------------------
 *
 *  attodt:
 *  Convert a string of the form "yy:ddd-hh:mm:ss:msc" to double time.
 *  Can truncate fields from the right.
 *
 *----------------------------------------------------------------------
 *
 *  dttostr:
 *  Given a double time value and format code, make a string of one of
 *  the following formats:
 *
 *  Format code   Output string
 *       0        yy:ddd-hh:mm:ss.msc
 *       1        Mon dd, year hh:mm:ss:msc
 *       2        yy:ddd-hh:mm:ss.msc, where input time is an interval
 *       3        yydddhhmmssmsc
 *       4        yyyyddd
 *       5        Day Mon dd, year
 *       6        yyyymmddhhmmss
 *       7        yyyy mm dd hh mm ss
 *      10        yyyy:ddd-hh:mm:ss.msc
 *
 *  No newline is appended.
 *
 *  Returns a pointer to static space, therefore a call of the form:
 *
 *  printf("%s %s\n", dttostr(dtime1, form1), dttostr(dtime2, form1));
 *
 *  will result in two duplicate time strings, even if dtime1 != dtime2
 *  or form1 != form2.  In such a case you should make distinct calls to
 *  printf.  For example:
 *
 *  printf("%s ",   dttostr(dtime1, form1));
 *  printf(" %s\n", dttostr(dtime2, form1));
 *
 *----------------------------------------------------------------------
 *
 *  lttostr:
 *  Given a long time value and format code, make a string of one of
 *  the following formats:
 *
 *  Format code   Output string
 *       0        yy:ddd-hh:mm:ss
 *       1        Mon dd, year hh:mm:ss
 *       2        yy:ddd-hh:mm:ss, where input time is an interval
 *       3        yydddhhmmss
 *       4        yyyyddd
 *       5        Day Mon dd, year
 *       6        yyyymmddhhmmss
 *       7        yyyy mm dd hh mm ss
 *
 *  No newline is appended.
 *
 *  Returns a pointer to static space, therefore a call of the form:
 *
 *  printf("%s %s\n", lttostr(ltime1, form1), lttostr(ltime2, form1));
 *
 *  will result in two duplicate time strings, even if ltime1 != ltime2
 *  or form1 != form2.  In such a case you should make distinct calls to
 *  printf.  For example:
 *
 *  printf("%s ",   lttostr(ltime1, form1));
 *  printf(" %s\n", lttostr(ltime2, form1));
 *
 *----------------------------------------------------------------------
 *
 *  tsplit:
 *  Split a double time to yr, ddd, hh, mm, ss, msc.
 *
 *----------------------------------------------------------------------
 *
 *  ydhmsmtod:
 *  Given year, day, hour, minutes, seconds, and milliseconds, return
 *  a double containing the seconds since 00:00:00.000 Jan 1, 1970.
 *
 *  Only works for times after Jan 1, 1970!
 *
 *----------------------------------------------------------------------
 *
 *  jdtomd:
 *  Given year and day of year, determine month and day of month.
 *
 *----------------------------------------------------------------------
 *
 *  ymdtojd:
 *  Given year, month, and day determine day of year.
 *
 *----------------------------------------------------------------------
 *
 *  today:
 *  Returns today's date in YYYYDDD form.
 *
 *====================================================================*/
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
//#include "util.h"
#include "platform.h"

#ifndef daysize
#define daysize(i) (365 + leap_year(i))
#endif


#ifndef leap_year
#define leap_year(i) ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
#endif

static char copy[1024];


static char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static char *month[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char *day[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static char *bad_call = "Bad dttostr input!";

#define MIN_CODE 0
#define MAX_CODE 15

static char *bad_str[MAX_CODE-MIN_CODE+1] = {
  "                   ",
  "                         ",
  "                   ",
  "              ",
  "       ",
  "            "
};

static char out_str[] = "xxxxxxxxxxxxxxxxxxxxxxxxx";


#define SPM (       60L)
#define SPH (SPM *  60L)
#define SPD (SPH *  24L)
#define SPY (SPD * 365L)



double ydhmsmtod(int yr, int da, int hr, int mn, int sc, int ms);
double util_ydhmsmtod(int yr, int da, int hr, int mn, int sc, int ms);



/***********************************************************************/

double attodt(char *string)
{
int i, yr;
int da = 1;
int hr = 0;
int mn = 0;
int sc = 0;
int ms = 0;

    strcpy(copy, string);

/*  Check for special case (PRESENT)  */

    if (strcasecmp(copy, "PRESENT") == 0) return (double) 2147483647;

/*  Parse string  */

    for (i = 0; i < (int)strlen(copy); i++) if (!isdigit(copy[i])) copy[i] = ' ';

    yr = hr = mn = sc = ms = 0; da = 1;
    if (strlen(copy) == strlen("yy:ddd-hh:mm:ss:msc")) {
        sscanf(copy, "%2d %3d %2d %2d %2d %3d", &yr, &da, &hr, &mn, &sc, &ms);
        yr += 1900;
    } else if (strlen(copy) == strlen("yy:ddd-hh:mm:ss")) {
        sscanf(copy, "%2d %3d %2d %2d %2d", &yr, &da, &hr, &mn, &sc);
        yr += 1900;
    } else if (strlen(copy) == strlen("yy:ddd-hh:mm")) {
        sscanf(copy, "%2d %3d %2d %2d", &yr, &da, &hr, &mn);
        yr += 1900;
    } else if (strlen(copy) == strlen("yy:ddd-hh")) {
        sscanf(copy, "%2d %3d %2d", &yr, &da, &hr);
        yr += 1900;
    } else if (strlen(copy) == strlen("yy:ddd")) {
        sscanf(copy, "%2d %3d", &yr, &da);
        yr += 1900;
    } else if (strlen(copy) == strlen("yy")) {
        sscanf(copy, "%2d", &yr);
        yr += 1900;
    }else if (strlen(copy) == strlen("yyyy:ddd-hh:mm:ss:msc")) {
        sscanf(copy, "%4d %3d %2d %2d %2d %3d", &yr, &da, &hr, &mn, &sc, &ms);
    } else if (strlen(copy) == strlen("yyyy:ddd-hh:mm:ss")) {
        sscanf(copy, "%4d %3d %2d %2d %2d", &yr, &da, &hr, &mn, &sc);
    } else if (strlen(copy) == strlen("yyyy:ddd-hh:mm")) {
        sscanf(copy, "%4d %3d %2d %2d", &yr, &da, &hr, &mn);
    } else if (strlen(copy) == strlen("yyyy:ddd-hh")) {
        sscanf(copy, "%4d %3d %2d", &yr, &da, &hr);
    } else if (strlen(copy) == strlen("yyyy:ddd")) {
        sscanf(copy, "%4d %3d", &yr, &da);
    } else if (strlen(copy) == strlen("yyyy")) {
        sscanf(copy, "%4d", &yr);
    } else {
//        fprintf(stderr, "attodt error: illegal string '%s'\n", string);
//        exit(1);
		return -1;
    }

    if (
        yr < 70 || (yr >  99 && yr < 1970) ||
        da <  1 || da > 366 ||
        hr <  0 || hr >  23 ||
        mn <  0 || mn >  59 ||
        sc <  0 || sc >  59 ||
        ms <  0 || ms > 999
    ) {
//        fprintf(stderr, "attodt error: illegal value in string '%s'\n", string);
//        fprintf(stderr, "copy = `%s'\n", copy);
//        fprintf(stderr, "yr = %d\n", yr);
//        fprintf(stderr, "da = %d\n", da);
//        fprintf(stderr, "hr = %d\n", hr);
//        fprintf(stderr, "mn = %d\n", mn);
//        fprintf(stderr, "sc = %d\n", sc);
//        fprintf(stderr, "ms = %d\n", ms);
//        exit(1);
		return -1;
    }

    return ydhmsmtod(yr, da, hr, mn, sc, ms);

}

/***********************************************************************/

char *dttostr(double dtime,int code)
{
struct tm *tm;
long  ltime;
float ffrac;
int   ifrac, yr, da, hr, mn, sc, ms;

    if (code < MIN_CODE || code > MAX_CODE) return bad_call;

    ltime = (long) dtime;
    ffrac = (float) ((dtime - (double) ltime) * 1000.0);
    ifrac = (int) ffrac;
    if (ffrac - (float) ifrac >= 0.5) ifrac++;

    if (code == 2) {
        yr = ltime / SPY; ltime -= yr * SPY;
        da = ltime / SPD; ltime -= da * SPD;
        hr = ltime / SPH; ltime -= hr * SPH;
        mn = ltime / SPM; ltime -= mn * SPM;
        sc = ltime;
        ms = ifrac;
        sprintf(out_str,"%2.2d:%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
                        yr, da, hr, mn, sc, ms);
        return out_str;
    }

    if ((tm = gmtime(&ltime)) == NULL) return bad_str[code];

    switch (code) {
        case 0:
            sprintf(out_str,"%2.2d:%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
                   tm->tm_year, ++tm->tm_yday, tm->tm_hour,
                   tm->tm_min, tm->tm_sec, ifrac);
            break;
        case 10:
            sprintf(out_str,"%4.4d:%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
                   tm->tm_year + 1900, ++tm->tm_yday, tm->tm_hour,
                   tm->tm_min, tm->tm_sec, ifrac);
            break;
        case 1:
            sprintf(out_str, "%s %2.2d, %4.4d %2.2d:%2.2d:%2.2d.%3.3d",
              month[tm->tm_mon], tm->tm_mday,tm->tm_year + 1900,
              tm->tm_hour, tm->tm_min, tm->tm_sec, ifrac);
            break;
        case 3:
            sprintf(out_str,"%2.2d%3.3d%2.2d%2.2d%2.2d%3.3d",
                   tm->tm_year, ++tm->tm_yday, tm->tm_hour,
                   tm->tm_min, tm->tm_sec, ifrac);
            break;
        case 4:
            sprintf(out_str,"%4.4d%3.3d",
                   tm->tm_year + 1900, ++tm->tm_yday);
            break;
        case 5:
            sprintf(out_str, "%s %2.2d/%2.2d/%2.2d",
              day[tm->tm_wday], ++tm->tm_mon, tm->tm_mday, tm->tm_year);
            break;
        case 6:
            sprintf(out_str,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",
                   tm->tm_year + 1900, ++tm->tm_mon, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);
            break;
        case 7:
            sprintf(out_str,"%4.4d %2.2d %2.2d %2.2d %2.2d %2.2d",
                   tm->tm_year + 1900, ++tm->tm_mon, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);
            break;
		case 14:
            sprintf(out_str,"%02d %s %04d (%03d)", tm->tm_mday, month[tm->tm_mon], tm->tm_year + 1900, ++tm->tm_yday);
            break;
		case 15:
            sprintf(out_str,"%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
            break;

        default: 
            strcpy(out_str, bad_call);
            break;
    }

    return out_str;
}

/***********************************************************************/

char *lttostr(long ltime, int code)
{
char *buffer;

    buffer = dttostr((double) ltime, code);
    if (code >= 4 && code <= 7) return buffer;

    if (code != 3) {
        buffer[strlen(buffer)-strlen(".msc")] = 0;
    } else {
        buffer[strlen(buffer)-strlen("msc")] = 0;
    }

    return buffer;
}

/***********************************************************************/

void tsplit(double dtime,int *yr, int *da, int *hr, int *mn, int *sc, int *ms)
{
long  ltime;
int   imsc;
float fmsc;
struct tm *tiempo;

    ltime = (long) dtime;
    tiempo = gmtime(&ltime);
    fmsc = (float) ((dtime - (double) ltime) * 1000.0);
    imsc = (int) fmsc;
    if (fmsc - (float) imsc >= 0.5) imsc++;

    *yr = tiempo->tm_year,
    *da = ++tiempo->tm_yday,
    *hr = tiempo->tm_hour,
    *mn = tiempo->tm_min,
    *sc = tiempo->tm_sec,
    *ms = imsc;

}

/***********************************************************************/

double ydhmsmtod(int yr, int da, int hr, int mn, int sc, int ms)
{
int i, days_in_year_part;
long   secs;

    days_in_year_part = 0;
    for (i = 1970; i < yr; i++) days_in_year_part += daysize(i);
    secs = (long) days_in_year_part * SPD;

    secs += (long)(da-1)*SPD + (long)hr*SPH + (long)mn*SPM + (long)sc;

    return (double) secs + ((double) (ms)/1000.0);

}

/***********************************************************************/

int jdtomd(int year, int day, int *m_no, int *d_no)
{
int i, leap;

    leap = leap_year(year);
    
    for (i = 1; day > daytab[leap][i]; i++) day -= daytab[leap][i];

    *m_no = i;
    *d_no = day;
	return 1;
}

/***********************************************************************/

int ymdtojd(int year, int mo, int da)
{
int jd, m, leap;

    leap = leap_year(year);
    for (jd = 0, m = 1; m < mo; m++) jd += daytab[leap][m];
    jd += da;

    return jd;
}

/***********************************************************************/

long today()
{
time_t now;
struct tm *current;

    now = time(NULL);
    current = localtime(&now);
    current->tm_year += 1900;
    ++current->tm_yday;

    return (1000 * current->tm_year) + current->tm_yday;

}

/* util_xxx form */

static char _util_daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static char *_util_month[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char *_util_day[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static char *_util_bad_call = "Bad util_dttostr input!";

#define _UTIL_MIN_CODE 0
#define _UTIL_MAX_CODE 7

static char *_util_bad_str[MAX_CODE-MIN_CODE+1] = {
  "                     ",
  "                         ",
  "                     ",
  "                ",
  "       ",
  "              "
};

static char _util_out_str[] = "xxxxxxxxxxxxxxxxxxxxxxxxx";


#define SPM (       60L)
#define SPH (SPM *  60L)
#define SPD (SPH *  24L)
#define SPY (SPD * 365L)

/***********************************************************************/

double util_attodt(char *string)
{
int i, yr, da, hr, mn, sc, ms;

    strcpy(copy, string);

/*  Check for special case ("present")  */

    if (strcasecmp(copy, "present") == 0) return (double) 2147483647;

/*  Parse string  */

    for (i = 0; i < (int)strlen(copy); i++) if (!isdigit(copy[i])) copy[i] = ' ';

    yr = hr = mn = sc = ms = 0; da = 1;
    if (strlen(copy) == strlen("yyyy:ddd-hh:mm:ss:msc")) {
        sscanf(copy, "%4d %3d %2d %2d %2d %3d", &yr, &da, &hr, &mn, &sc, &ms);
    } else if (strlen(copy) == strlen("yyyy:ddd-hh:mm:ss")) {
        sscanf(copy, "%4d %3d %2d %2d %2d", &yr, &da, &hr, &mn, &sc);
    } else if (strlen(copy) == strlen("yyyy:ddd-hh:mm")) {
        sscanf(copy, "%4d %3d %2d %2d", &yr, &da, &hr, &mn);
    } else if (strlen(copy) == strlen("yyyy:ddd-hh")) {
        sscanf(copy, "%4d %3d %2d", &yr, &da, &hr);
    } else if (strlen(copy) == strlen("yyyy:ddd")) {
        sscanf(copy, "%4d %3d", &yr, &da);
    } else if (strlen(copy) == strlen("yyyy")) {
        sscanf(copy, "%4d", &yr);
    } else {
//        fprintf(stderr, "util_attodt error: illegal string '%s'\n", string);
//        exit(1);
		return -1;
    }

    if (
        yr < 1970 ||
        da <    1 || da > 366 ||
        hr <    0 || hr >  23 ||
        mn <    0 || mn >  59 ||
        sc <    0 || sc >  59 ||
        ms <    0 || ms > 999
    ) {
//        fprintf(stderr, "util_attodt error: illegal value in string '%s'\n",
//            string
//        );
//        exit(1);
		return -1;
    }

    return util_ydhmsmtod(yr, da, hr, mn, sc, ms);

}

/***********************************************************************/

char *util_dttostr(double dtime, int code)
{
struct tm *tm;
long  ltime;
float ffrac;
int   ifrac, yr, da, hr, mn, sc, ms;

    if (code < _UTIL_MIN_CODE || code > _UTIL_MAX_CODE) return _util_bad_call;

    ltime = (long) dtime;
    ffrac = (float) ((dtime - (double) ltime) * 1000.0);
    ifrac = (int) ffrac;
    if (ffrac - (float) ifrac >= 0.5) ifrac++;

    if (code == 2) {
        yr = ltime / SPY; ltime -= yr * SPY;
        da = ltime / SPD; ltime -= da * SPD;
        hr = ltime / SPH; ltime -= hr * SPH;
        mn = ltime / SPM; ltime -= mn * SPM;
        sc = ltime;
        ms = ifrac;
        sprintf(_util_out_str,"%2.2d:%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
                        yr, da, hr, mn, sc, ms);
        return _util_out_str;
    }

    if ((tm = gmtime(&ltime)) == NULL) return _util_bad_str[code];
    tm->tm_year += 1900;
    tm->tm_yday += 1;
    tm->tm_mon  += 1;

    switch (code) {
        case 0:
            sprintf(_util_out_str,"%4.4d:%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
                   tm->tm_year, tm->tm_yday, tm->tm_hour,
                   tm->tm_min, tm->tm_sec, ifrac);
            break;
        case 1:
            sprintf(_util_out_str, "%s %2.2d, %4.4d %2.2d:%2.2d:%2.2d.%3.3d",
              _util_month[tm->tm_mon], tm->tm_mday,tm->tm_year + 1900,
              tm->tm_hour, tm->tm_min, tm->tm_sec, ifrac);
            break;
        case 3:
            sprintf(_util_out_str,"%4.4d%3.3d%2.2d%2.2d%2.2d%3.3d",
                   tm->tm_year, tm->tm_yday, tm->tm_hour,
                   tm->tm_min, tm->tm_sec, ifrac);
            break;
        case 4:
            sprintf(_util_out_str,"%4.4d%3.3d",
                   tm->tm_year, tm->tm_yday);
            break;
        case 5:
            sprintf(_util_out_str, "%s %2.2d/%2.2d/%4.4d",
              _util_day[tm->tm_wday], tm->tm_mon, tm->tm_mday, tm->tm_year);
            break;
        case 6:
            sprintf(_util_out_str,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",
                   tm->tm_year, tm->tm_mon, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);
            break;
        case 7:
            sprintf(_util_out_str,"%4.4d %2.2d %2.2d %2.2d %2.2d %2.2d",
                   tm->tm_year, tm->tm_mon, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec);
            break;
        default: 
            strcpy(_util_out_str, _util_bad_call);
            break;
    }

    return _util_out_str;
}

/***********************************************************************/

char *util_lttostr(long ltime, int code)
	{
	char *buffer;

    buffer = util_dttostr((double) ltime, code);
    if (code >= 4 && code <= 7) return buffer;

    if (code != 3) {
        buffer[strlen(buffer)-strlen(".msc")] = 0;
    } else {
        buffer[strlen(buffer)-strlen("msc")] = 0;
    }

    return buffer;
}

/***********************************************************************/

void util_tsplit(double dtime, int *yr, int *da, int *hr, int *mn, int *sc, int *ms)
//double dtime;
//int *yr, *da, *hr, *mn, *sc, *ms;
{
long  ltime;
int   imsc;
double dmsc;
struct tm *tiempo;

    ltime = (long) dtime;
    dmsc = ((dtime - (double) ltime)) * (double) 1000.0;
    imsc = (int) dmsc;
    if (dmsc - (double) imsc >= (double) 0.5) imsc++;
    if (imsc == 1000) {
        ++ltime;
        imsc = 0;
    }

    tiempo = gmtime(&ltime);
    *yr = 1900 + tiempo->tm_year;
    *da = ++tiempo->tm_yday;
    *hr = tiempo->tm_hour;
    *mn = tiempo->tm_min;
    *sc = tiempo->tm_sec;
    *ms = imsc;

}

/***********************************************************************/

double util_ydhmsmtod(int yr, int da, int hr, int mn, int sc, int ms)
{
int i, days_in_year_part;
long   secs;

    days_in_year_part = 0;
    for (i = 1970; i < yr; i++) days_in_year_part += daysize(i);
    secs = (long) days_in_year_part * SPD;

    secs += (long)(da-1)*SPD + (long)hr*SPH + (long)mn*SPM + (long)sc;

    return (double) secs + ((double) (ms)/1000.0);

}

/***********************************************************************/

int util_jdtomd(int year, int day, int *m_no, int *d_no)
	{
	int i, leap;

    leap = leap_year(year);
    
    for (i = 1; day > _util_daytab[leap][i]; i++) day -= _util_daytab[leap][i];

    *m_no = i;
    *d_no = day;
	return 1;
}

/***********************************************************************/

int util_ymdtojd(int year, int mo, int da)
	{
	int jd, m, leap;

    leap = leap_year(year);
    for (jd = 0, m = 1; m < mo; m++) jd += _util_daytab[leap][m];
    jd += da;

    return jd;
}

/***********************************************************************/

long util_today()
{
time_t now;
struct tm *current;

    now = time(NULL);
    current = localtime(&now);
    current->tm_year += 1900;
    ++current->tm_yday;

    return (1000 * current->tm_year) + current->tm_yday;

}
