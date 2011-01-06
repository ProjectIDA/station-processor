#include "..\\stdafx.h"
#include <math.h>
#include <stdlib.h>
#include "filter.h"
#include "stock.h"
#define M_PI 3.141592
#define MAX_ORDER 100
typedef struct Butterworth {
    int         ns,
                    is[MAX_ORDER];
    double          b[MAX_ORDER],
                    c[MAX_ORDER],
                    gain,
                    f[MAX_ORDER],
                    fm1[MAX_ORDER],
                    fm2[MAX_ORDER],
                    rm1[MAX_ORDER],
                    rm2[MAX_ORDER],
                    xim1[MAX_ORDER],
                    xim2[MAX_ORDER],
                    xom1[MAX_ORDER],
                    xom2[MAX_ORDER];
}      Butterworth ; 


static int butsta (int norder, int  *nsout, int *is, double *b, double  *c)
	{
    int  nodd,i,m;
    double     xk,xn;

    *nsout = 0;
    if (norder < 1) {
	return 0;
    }
    if (norder == norder / 2 << 1) {
	nodd = 0;
    } else {
	nodd = 1;
    }
    if (nodd == 1) {
	*nsout = 1;
	is[0] = 0;
	c[0] = (double) 1.;
	if (norder == 1) {
	    return 0;
	}
	m = (norder - 1) / 2;
	xk = (double) 1.;
    } else {
	m = norder / 2;
	xk = (double) .5;
    }
    xn = M_PI / (double) (norder);
    for (i = 0; i < m ; ++i) {
	is[*nsout] = 1;
	c[*nsout] = (double) 1.;
	b[*nsout] = cos (xk * xn) * (double) 2.;
	xk += (double) 1.;
	++(*nsout);

    }

    return 0;
}

static int l2l(double fl, int  ns, int is[], double b[], double c[], double *gain)
{
    int  i;
    double     w,
                    w2;

    *gain = (double) 1.;
    w = fl * (double) 2. * M_PI ;
    w2 = w * w;
    for (i = 0; i < ns; ++i) {
	if (is[i] == 0) {
	    *gain *= w;
	    c[i] *= w;
	} else if (is[i] == 1) {
	    *gain *= w2;
	    b[i] *= w;
	    c[i] *= w2;
	} else if (is[i] == 2) {
	    c[i] *= w;
	} else {
	    b[i] *= w;
	    c[i] *= w2;
	}
    }

    return 0;
}

static int l2h(double fl, int ns, int is[], double b[], double c[], double *gain)
	{
    int  i;
    double     w,
                    w2;


    *gain = (double) 1.;
    w = fl * (double) 2. * M_PI ;
    w2 = w * w;
    for (i = 0; i < ns; ++i) {
	if (is[i] == 0) {
	    *gain /= c[i];
	    is[i] = 2;
	    c[i] = w / c[i];
	} else if (is[i] == 1) {
	    is[i] = 3;
	    *gain /= c[i];
	    b[i] = b[i] * w / c[i];
	    c[i] = w2 / c[i];
	} else if (is[i] == 2) {
	    is[i] = 0;
	    *gain = *gain * w / c[i];
	    c[i] = w / c[i];
	} else {
	    is[i] = 1;
	    *gain = *gain * w2 / c[i];
	    b[i] = b[i] * w / c[i];
	    c[i] = w2 / c[i];
	}
    }

    return 0;
}

static int recoef(Butterworth *butflc,double dt, int itran, int iwarp)
	{
    int         nstages;
    int  i;
    double     omega,x,z,bb,cc,den;

    nstages = butflc->ns;
    for (i = 0; i < nstages; ++i) {
	if (butflc->is[i] == 0) {
	    omega = butflc->c[i];
	    if (iwarp == 1) {
		x = omega * dt;
		if (x >= M_PI ) {
		    x = M_PI  * (double) .999;
		}
		omega = tan (x * (double) .5) * (float) 2. / dt;
	    }
	    den = omega * dt + (double) 2.;
	    butflc->f[i] = omega * dt / den;
	    butflc->fm1[i] = butflc->f[i];
	    butflc->fm2[i] = (double) 0.;
	    butflc->rm1[i] = ((double) 2. - omega * dt) / den;
	    butflc->rm2[i] = (double) 0.;
	} else if (butflc->is[i] == 1) {
	    omega = sqrt (butflc->c[i]);
	    z = butflc->b[i] * (double) .5 / omega;
	    if (iwarp == 1) {
		x = omega * dt;
		if (x >= M_PI ) {
		    x = M_PI  * (double) .999;
		}
		omega = tan (x * (double) .5) * (float) 2. / dt;
	    }
	    cc = omega * omega;
	    bb = z * (double) 2. *omega;
	    den = dt * (double) 2. *bb + (float) 4. + cc * dt * dt;
	    butflc->f[i] = cc * dt * dt / den;
	    butflc->fm1[i] = butflc->f[i] * (double) 2.;
	    butflc->fm2[i] = butflc->f[i];
	    butflc->rm1[i] = ((double) 8. - cc * (float) 2. * dt * dt) /
		den;
	    butflc->rm2[i] = (dt * (double) 2. * bb - (float) 4. - cc * 
				   dt * dt) / den;
	} else if (butflc->is[i] == 2) {
	    omega = butflc->c[i];
	    if (iwarp == 1) {
		x = omega * dt;
		if (x >= M_PI ) {
		    x = M_PI  * (double) .999;
		}
		omega = tan (x * (double) .5) * (float) 2. / dt;
	    }
	    den = omega * dt + (double) 2.;
	    butflc->f[i] = (double) 2. / den;
	    butflc->fm1[i] = -(double) butflc->f[i];
	    butflc->fm2[i] = (double) 0.;
	    butflc->rm1[i] = ((double) 2. - omega * dt) / den;
	    butflc->rm2[i] = (double) 0.;
	} else if (butflc->is[i] == 3) {
	    omega = sqrt (butflc->c[i]);
	    z = butflc->b[i] * (double) .5 / omega;
	    if (iwarp == 1) {
		x = omega * dt;
		if (x >= M_PI ) {
		    x = M_PI  * (double) .999;
		}
		omega = tan (x * (double) .5) * (float) 2. / dt;
	    }
	    cc = omega * omega;
	    bb = z * (double) 2. *omega;
	    den = dt * (double) 2. *bb + (float) 4. + cc * dt * dt;
	    butflc->f[i] = (double) 4. / den;
	    butflc->fm1[i] = butflc->f[i] * (double) -2.;
	    butflc->fm2[i] = butflc->f[i];
	    butflc->rm1[i] = ((double) 8. - cc * (float) 2. * dt * dt) /
		den;
	    butflc->rm2[i] = (dt * (double) 2. * bb - (float) 4. - cc * 
				   dt * dt) / den;
	} else if (butflc->is[i] == 4) {
	    butflc->f[i] = 0.5*dt;
	    butflc->fm1[i] = butflc->f[i];
	    butflc->fm2[i] = 0.0;
	    butflc->rm1[i] = 1.0;
	    butflc->rm2[i] = 0.0;
	}

    }
    return 0;
}

static Hook *bwt_new(double fl, int ol, double fu, int ou, double dt)
	{
    int  nsout;
    double     gn;
    Butterworth *butflc ;
    Hook	*hook ;

//    allot(Butterworth *, butflc, 1); 
	butflc=new Butterworth();
    hook = new_hook(free) ; 
    hook->p = (void *) butflc ;

    butflc->ns = 0;
    if (ol + ou > MAX_ORDER) {
	return 0;
    }
    if (fu == (double) 0. || ou == 0) {
	butflc->ns = 0;
	butflc->gain = (double) 1.;
    } else {
	butsta (ou, &nsout, butflc->is, butflc->b, butflc->c);
	l2l (fu, nsout, butflc->is, butflc->b, butflc->c, &gn);
	butflc->ns = nsout;
	butflc->gain = gn;
    }
    if (fl != (double) 0. && ol != 0) {
	butsta (ol, &nsout, &butflc->is[butflc->ns], &butflc->b[
				    butflc->ns], &butflc->c[butflc->ns]);
	l2h (fl, nsout, &butflc->is[butflc->ns], &butflc->b[butflc->ns],
	      &butflc->c[butflc->ns], &gn);
	butflc->ns += nsout;
	butflc->gain *= gn;
    }
    butflc->gain = (double) 1.;

    recoef (butflc, dt, 0, 1);

    return hook;
	}

static Hook *wa_new (int velocity, int nominal, double dt)
{
    Butterworth *butflc ;
    Hook	*hook ;
    int i;
    double omega, damp;

//    allot ( Butterworth *, butflc, 1 ) ; 
	butflc =new Butterworth();
    hook = new_hook(free) ; 
    hook->p = (void *) butflc ;

	for (i=0; i<MAX_ORDER; i++) {
		butflc->xim1[i] = 0.0;
		butflc->xim2[i] = 0.0;
		butflc->xom1[i] = 0.0;
		butflc->xom2[i] = 0.0;
	}
	if (nominal) {
		omega = 2.0*M_PI*(1.0/0.8);
		damp = 0.8;
		butflc->gain = 2800.0 * 1.e-6;
	} else {
		omega = 2.0*M_PI*(1.0/0.8);
		damp = 0.7;
		butflc->gain = 2080.0 * 1.e-6;
	}
 
	/* Get the filter stages in the s-domain. */

	butflc->ns = 2;
	butflc->is[0] = 3;
	butflc->b[0] = 2.0*damp*omega;
	butflc->c[0] = omega*omega;
	omega = 2.0*M_PI*10.0;
	damp = 0.7;
	butflc->is[1] = 1;
	butflc->b[1] = 2.0*damp*omega;
	butflc->c[1] = omega*omega;
 	if (velocity) {
		butflc->ns = 3;
		butflc->is[2] = 4;
	}

    recoef (butflc, dt, 0, 1);

    return hook ;
}

static int bwt_init(Hook *hook, float xin[])
	{

    int         nstages;
    double     yout;
    int  i;
    double     yin;
    Butterworth *butflc ;

    butflc = (Butterworth *) hook->p ; 

    yin = xin[0];
    nstages = butflc->ns;
    for (i = 0; i <= nstages; ++i) {
	if (butflc->is[i] == 4) {
		butflc->xim1[i] = yin;
		butflc->xim2[i] = yin;
		butflc->xom1[i] = yin;
		butflc->xom2[i] = yin;
	} else {
		yout = yin * (butflc->f[i] + butflc->fm1[i] + butflc->fm2[i]) / 
		    ((double) 1. - butflc->rm1[i] - butflc->rm2[i]);
		butflc->xim1[i] = yin;
		butflc->xim2[i] = yin;
		butflc->xom1[i] = yout;
		butflc->xom2[i] = yout;
		yin = yout;
	}
    }

    return 0;
	}

static int bwt_filter(Hook *hook,int npts,float xin[], int isave, float xout[])
	{
    int         nstages;
    double     yout;
    int  i,
                    j;
    double     yin;
    double *f, *xim1, *fm1, *xim2, *fm2, *xom1, *rm1, *xom2, *rm2 ;
    Butterworth *butflc ;

    butflc = (Butterworth *) hook->p ; 

    nstages = butflc->ns ; 
    f = butflc->f ;
    xim1 = butflc->xim1 ;
    fm1 = butflc->fm1 ;
    xim2 = butflc->xim2 ;
    fm2 = butflc->fm2 ;

    xom1 = butflc->xom1 ;
    rm1 = butflc->rm1 ;
    xom2 = butflc->xom2 ;
    rm2 = butflc->rm2 ;

    for (j = 0 ; j < npts ; ++j) {
	yin = xin[j];
	for (i = 0 ; i < nstages ; ++i) {
	    yout = yin * f[i] 
		    + xim1[i] * fm1[i] + xim2[i] * fm2[i] 
		    + xom1[i] * rm1[i] + xom2[i] * rm2[i] ;

	    xom2[i] = xom1[i];
	    xom1[i] = yout;
	    xim2[i] = xim1[i];
	    xim1[i] = yin;
	    yin = yout;
	}
	if (isave != 0) {
	    xout[j] =(float)( yout * butflc->gain);
	}
    }

    return 0;
	}

static Hook *BwtHook = 0 ; 

void setbfl_ (float *fl, int *ol, float *fu, int *ou, float *dt ) 
	{
    if (BwtHook != 0) {
	free_hook(&BwtHook) ; 
    }
    BwtHook = bwt_new ( *fl, *ol, *fu, *ou, *dt ) ;
	}

void setwfl_ (int *velocity, int *nominal, float *dt)
	{
    if (BwtHook != 0) {
	free_hook(&BwtHook) ; 
    }
    BwtHook = wa_new ( *velocity, *nominal, *dt ) ;
	}

void inifil_ ( float *data ) 
	{
//    _ASSERT( BwtHook != 0 ) ;
    bwt_init ( BwtHook, data ) ; 
	}

void filrec_ ( int *npts, float *idata, int *saveflag, float *odata)
	{
//    _ASSERT( BwtHook != 0 ) ;
    bwt_filter ( BwtHook, *npts, idata, *saveflag, odata ) ; 
	}
void freefl_()
	{
    if (BwtHook != 0) 
		{
		free_hook(&BwtHook); 
		}
	}

