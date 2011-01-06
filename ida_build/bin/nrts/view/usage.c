#pragma ident "$Id: usage.c,v 1.1.1.1 2000/02/08 20:20:15 dec Exp $"
#include <stdio.h>

void help(myname)
char *myname;
{

		fprintf(stderr,"\n");
		fprintf(stderr,"usage: %s [if=sys1@host[,sys2@host...]] [options]\n", myname);
		fprintf(stderr,"\n");
		fprintf(stderr,"Options:\n");
		fprintf(stderr,"port=val     => TCP/IP port number\n");
		fprintf(stderr,"beg=string   => beg time (yyyy:ddd-hh:mm:ss)\n");
		fprintf(stderr,"end=string   => end time (yyyy:ddd-hh:mm:ss)\n");
		fprintf(stderr,"chan=list    => list of channels to request\n");
		fprintf(stderr,"keepup=val   => if non zero keep connection alive\n");
		fprintf(stderr,"to=secs      => set read timeout\n");
		fprintf(stderr,"log=name     => set log file name\n");
		fprintf(stderr,"debug=level  => set initial debug level\n");
		fprintf(stderr,"home=name    => set NRTS_HOME\n");
		fprintf(stderr,"comp=val     => compression (0 = none, 1 = 1st diff)\n");
		fprintf(stderr,"gain=val     => gain coeff\n");
		/*
		fprintf(stderr,"sort=string  => sort by channels or systems(chan or sys)");
		*/
		fprintf(stderr,"\n");
		fprintf(stderr, "defaults:\n");
		fprintf(stderr, "beg=<youngest>,end=<youngest>,chan=<all>,keepup=30,\n");
		fprintf(stderr, "to=120,log=<stderr>,debug=1,home=/usr/nrts,comp=0\n");
		fprintf(stderr, "gain=1, sort=chan\n");
		fprintf(stderr,"\n");
		exit(1);

}


/* Revision History
 *
 * $Log: usage.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
