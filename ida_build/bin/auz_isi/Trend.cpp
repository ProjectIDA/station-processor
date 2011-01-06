#pragma ident "$Id: Trend.cpp,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
void RemoveLinearTrend(double  *y, float dt, long qty, double *a, double *b)
	{
	double a12=0.,a22=0.,b1=0.,b2=0.;
	double xs=0;
	long i;
	double aa,bb;

	for(i=0,xs=0.;i<qty;++i,xs+=dt)
		{
		a12+=xs;
		b1+=y[i];
		b2+=y[i]*xs;
		a22+=xs*xs;
		}
	aa=(b1*a22-b2*a12)/(qty*a22-a12*a12);
	bb=(qty*b2-b1*a12)/(qty*a22-a12*a12);

	for(i=0,xs=0.;i<qty;++i,xs+=dt)
		{
		y[i]-=(aa+bb*xs);
		}
	*a=aa;
	*b=bb;
	}
void RemoveMean(double *y, long nQty)
	{
	long i;
	double dMean;
	dMean=0.;
	for(i=0;i<nQty;++i)
		{
		dMean+=y[i];
		}
	dMean/=nQty;
	for(i=0;i<nQty;++i)
		{
		y[i]-=dMean;
		}
	}

/* Revision History
 *
 * $Log: Trend.cpp,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
