#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wfdiscio.h"

void ScanWfdLine(struct wfdisc *pwfdisc, char *line)
	{
	sscanf(&line[0], "%s", pwfdisc->sta);
	sscanf(&line[7], "%s",pwfdisc->chan);
	sscanf(&line[16], "%lf",&pwfdisc->time);
	sscanf(&line[34], "%ld",&pwfdisc->wfid);
	sscanf(&line[43], "%ld",&pwfdisc->chanid);
	sscanf(&line[52], "%ld",&pwfdisc->jdate);
	sscanf(&line[61], "%lf",&pwfdisc->endtime);
	sscanf(&line[79], "%ld",&pwfdisc->nsamp);
	sscanf(&line[88], "%11f",&pwfdisc->smprate);
	sscanf(&line[100], "%f",&pwfdisc->calib);
	sscanf(&line[117], "%f",&pwfdisc->calper);
	sscanf(&line[134], "%s",pwfdisc->instype);
	sscanf(&line[141], "%c",&pwfdisc->segtype);
	sscanf(&line[143], "%s",pwfdisc->datatype);
	sscanf(&line[146], "%c",&pwfdisc->clip);
	sscanf(&line[148], "%s",pwfdisc->dir);
	sscanf(&line[213], "%s",pwfdisc->dfile);
	sscanf(&line[246], "%ld",&pwfdisc->foff);
	sscanf(&line[257], "%ld",&pwfdisc->commid); 
	sscanf(&line[266], "%s",pwfdisc->lddate);
	}
