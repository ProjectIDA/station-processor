#pragma ident "$Id: dft.cpp,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

/*
   Direct fourier transform
*/
void DFT(short dir, long m, double *x1, double *y1)
	{
	long i,k;
	long double arg;
	long double cosarg,sinarg;
	long double cosdelta,sindelta,cosn,sinn;

	double *x2, *y2;
	x2=new double[m];
	y2=new double[m];

	for (i=0;i<m;i++) 
		{
		x2[i] = 0;
		y2[i] = 0;
		arg = - dir * 2.0 * M_PI * i / (double)m;

		cosdelta=cos(arg);
		sindelta=sin(arg);
		cosarg=1.;
		sinarg=0.;

		for (k=0;k<m;k++) 
			{
//			cosarg = cos(k * arg);
//			sinarg = sin(k * arg);
			x2[i] += (x1[k] * cosarg - y1[k] * sinarg);
			y2[i] += (x1[k] * sinarg + y1[k] * cosarg);
			cosn = cosarg*cosdelta-sinarg*sindelta;
			sinn = sinarg*cosdelta+cosarg*sindelta;
			cosarg=cosn;
			sinarg=sinn;
			}
		}
   
   /* Copy the data back */
	if (dir == 1) 
		{
		for (i=0;i<m;i++) 
			{
			x1[i] = (x2[i] / m);
			y1[i] = (y2[i] / m);
			}
		} 
	else 
		{
		for (i=0;i<m;i++) 
			{
			x1[i] = x2[i];
			y1[i] = y2[i];
			}
		}
	delete[] x2;
	delete[] y2;
	}

/* Revision History
 *
 * $Log: dft.cpp,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
