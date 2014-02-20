#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

#define CAL_CONS 1720982L	/* Nov, 1, 1BC */
	
extern UBYTE _dmsize[12];
extern BOOL _tleap(short int year);

/* This algorithm is only accurate from 1-Mar-1900 to 28-Feb-2100 */

_SUB LONG _julday(LONG year,LONG mon,LONG day)
{

	LONG julian,yearprime,monprime,dayprime;

	yearprime=year;
	monprime=mon+1;
	dayprime=day;

	if (mon==1||mon==2) {
		yearprime=year-1;
		monprime=mon+13;
	}

	julian=dayprime+CAL_CONS;
	julian+=(36525L*yearprime)/100L;
	julian+=(306001L*monprime)/10000L;

	return(julian);

}
_SUB LONG ST_Julian(LONG year, LONG mon, LONG day)
{

	return(_julday(year,mon,day));
}

/* This algorithm is only accurate from 1-Jan-1901 to 28-Feb-2100 */

_SUB VOID ST_CnvJulToCal(LONG injul,WORD *outyr,WORD *outmon,
	WORD *outday,WORD *outjday)
{

	int y1,m1,d,m,y,j,t;
	float a,b,c,jdu;

	jdu = injul - CAL_CONS;
	y1 = ((jdu-122.1)/365.25);
	a = (int) (365.25*((float) y1));
	m1 = (jdu-a)/30.6001;
	b = (int) (30.6001*((float) m1));
	d = jdu-a-b;
	if (m1==14||m1==15) m=m1-13;
	else m=m1-1;
	if (m>2) y=y1;
	else y=y1+1;

	if (_tleap(y)) _dmsize[1]=29;
	else _dmsize[1]=28;
	
	j = d;
	for (t=0; t<(m-1); t++) j+=_dmsize[t];

	*outyr = y;
	*outmon = m;
	*outday = d;
	*outjday = j;

	return;	

}

_SUB STDTIME ST_CnvJulToSTD(JULIAN injul)
{

	STDTIME rettim = {0,0,0,0,0,0};

	WORD outyr,outmon,outday,outjday;

	ST_CnvJulToCal(injul,&outyr,&outmon,&outday,&outjday);

	rettim.year = outyr;
	rettim.day = outjday;

	return(rettim);
}

#ifdef MAINTEST

UBYTE _dmsize[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

BOOL _tleap(year)		/* Gregorian leap rules */
WORD year;
{

	if (year%400==0) return(TRUE);	/* Yes on 400s */
	if (year%100==0) return(FALSE);	/* No on 100s */
	if (year%4==0) return(TRUE);	/* Yes on 4s */
	return(FALSE);			/* Otherwise no */

}

main()
{

	LONG yr,mon,day;
	LONG jday,oday;
	WORD ry,rm,rd,rj,dm;

	yr = 1700;
	mon = 1;
	day = 1;
	jday = 0;

	printf("Month/Day test\n");

	FOREVER {

		if (yr>2300) break;
		if (mon==1&&day==1) {
			if (_tleap(yr)) _dmsize[1]=29;
			else _dmsize[1]=28;
		}

		jday = _julday(yr,mon,day);

		if (jday==0) oday==jday-1;

		if (oday!=jday-1) {
			printf("Calendar unsync to %d-%d-%d\n",yr,mon,day);
		}

		oday = jday;

		ST_CnvJulToCal(jday,&ry,&rm,&rd,&rj);

		if (ry!=yr||rd!=day||rm!=mon) {
			printf("Calendar reconv to %d-%d-%d\n",yr,mon,day);
			printf("   returned date   %d-%d-%d\n",ry,rm,rd);
		}

		day++;
		if (day>_dmsize[mon-1]) {
			day=1;
			mon++;
		}
		if (mon>12) {
			mon=1;
			yr++;
			/* if (yr%20==0) printf("Begin year %d\n",yr); */
		}

	}
	
	yr = 1700;
	day = 1;
	jday = 0;

	printf("Julian day of year test\n");

	FOREVER {

		if (yr>2300) break;
		if (mon==1&&day==1) {
			if (_tleap(yr)) dm=366;
			else dm=365;
		}

		jday = _julday(yr,1,day);

		if (jday==0) oday==jday-1;

		if (oday!=jday-1) {
			printf("Calendar unsync to %d-%d-%d\n",yr,mon,day);
		}

		oday = jday;

		ST_CnvJulToCal(jday,&ry,&rm,&rd,&rj);

		if (ry!=yr||rj!=day) {
			printf("Calendar reconv to %d-%d\n",yr,day);
			printf("   returned date   %d-%d\n",ry,rj);
		}

		day++;
		if (day>dm) {
			yr++;
			day = 1;
		}

	}
	

}

#endif
