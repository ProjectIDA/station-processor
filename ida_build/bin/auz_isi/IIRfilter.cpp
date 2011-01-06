#pragma ident "$Id: IIRfilter.cpp,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
// IIRfilter.cpp: implementation of the CIIRfilter class.
//
//////////////////////////////////////////////////////////////////////

#include "IIRfilter.h"
#include <math.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIIRfilter::CIIRfilter()
	{
	}

CIIRfilter::~CIIRfilter()
	{
	}
/*
 * NAME
 *	iirdes -- (filters) design iir digital filters from analog prototypes
 *
 *	Copyright 1988  regents of the university of california
 *
 *
 * SYNOPSIS
 *	call iirdes( iord, type, fl, fh, ts, sn, sd, nsects )
 *
 *
 *	integer iord		(i)filter order (10 maximum)
 *	character*2 type	(i)filter type
 *				lowpass (LP)
 *				highpass (HP)
 *				bandpass (BP)
 *				bandreject (BR)
 *	real fl			(i)low-frequency cutoff
 *	real fh			(i)high-frequency cutoff
 *	real ts			(i)sampling interval (in seconds)
 *	real sn(*)		(o)array containing numerator coefficients of
 *				second-order sections packed head-to-tail.
 *	real sd(*)		(o)array containing denominator coefficients
 *				of second-order sections packed head-to-tail.
 *	integer nsects		(o)number of second-order sections.
 *
 * DESCRIPTION
 *	Subroutine to design iir digital filters from analog prototypes.
 *	
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
*/

#define M_PI  3.14159265358979323846

void CIIRfilter::iir_design( int iord, char *type, float fl, float fh, float ts )
	{
	double	flw;
	double	fhw;
	complex	p[10];
	int	nps;
	char	ptype[10];

	butter_poles(p, ptype, &nps, iord);
	if (strncmp(type, "BP", 2) == 0)
		{
		fl = fl*ts/2.f;
		fh = fh*ts/2.f;
		flw = tangent_warp( fl, 2.f );
		fhw = tangent_warp( fh, 2.f );
		lp_to_bp(p, ptype, nps, flw, fhw, sn, sd, &nsects);
		}
    else if (strncmp(type, "BR", 2) == 0 ) 
			{
			fl = fl*ts/2.f;
			fh = fh*ts/2.f;
			flw = tangent_warp( fl, 2.f );
			fhw = tangent_warp( fh, 2.f );
			lp_to_br(p, ptype, nps, flw, fhw, sn, sd, &nsects);
			}
        else if (strncmp(type, "LP", 2) == 0 )
				{
				fh = fh*ts/2.f;
				fhw = tangent_warp( fh, 2.f );
				lowpass( p, ptype, nps, sn, sd, &nsects );
				cutoff_alter( sn, sd, nsects, fhw );
				}
			else if (strncmp(type, "HP", 2) == 0 )
					{
					fl = fl*ts/2.f;
					flw = tangent_warp( fl, 2.f );
					lp_to_hp( p, ptype, nps, sn, sd, &nsects );
					cutoff_alter( sn, sd, nsects, flw );
					}
    bilinear( sn, sd, nsects );

	for (int i = 0; i < nsects; i++)
		{
		x1[i] = x2[i] = y1[i] = y2[i] = 0.f;
		}

	}
/*
 * NAME
 * 	apiir  -- (filters) apply iir filter to a data sequence
 * 
 * 	copyright 1988  regents of the university of california
 * 
 *  SYNOPSIS
 * 	call apiir( data, nsamps, zp, sn, sd, nsects )
 * 	real data		(i/o) array containing data
 * 	integer nsamps		(i) number of data samples
 * 	logical zp              (i) true for zero phase filtering,
 * 				false for single pass filtering
 * 	real sn			(i) numerator polynomials for second
 * 				order sections.
 * 	real sd			(i) denominator polynomials for second
 * 				order sections.
 * 	integer nsects		(i) number of second-order sections
 * 
 *  DESCRIPTION
 * 	Subroutine to apply an iir filter to a data sequence.
 * 	The filter is assumed to be stored as second order sections.
 * 	Filtering is in-place.
 * 	Zero-phase (forward and reverse) is an option.
 * 
 *  WARNINGS
 * 	Zero-phase filtering doubles the falloff rate outside of
 * 	the band pass; number of poles is effectively doubled.
 * 
 *  AUTHOR
 * 	Dave Harris
 * 	Lawrence Livermore National Laboratory
 * 	L-205
 * 	P.O. Box 808
 * 	Livermore, CA  94550
 * 	(415) 423-0617
 *
 *	Translated from Fortran (apiir.f) to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
 */

double CIIRfilter::apply_iir(double fData)//, int nsamps,  float *sn, float *sd, int nsects)
	{
	int	 j, jptr;

	jptr = 0;
	input = fData;
	for (j = 0; j < nsects; j++)
		{
        second_order( &output, &y1[j], &y2[j], sd[jptr+1], 
		sd[jptr+2], input, &x1[j], &x2[j], sn[jptr],
		sn[jptr+1], sn[jptr+2]);
		jptr = jptr + 3;
		input = output;
		}
	return output;
	}


/*
 * NAME
 *	bilin2  -- (filters) transforms an analog filter to a digital filter 
 *	via the bilinear transformation. Assumes both are stored as second
 *	order sections.  The transformation is done in-place.
 *
 *	Copyright 1988  regents of the university of california
 *
 * SYNOPSIS
 *	call bilin2( sn, sd, nsects )
 *	real sn(*)		(i)Array containing numerator polynomial 
 *				coefficients for second order sections.
 *				Packed head-to-tail.
 *	real sd(*)		(i)Array containing denominator polynomial 
 *				coefficients for second order sections. 
 *				Packed head-to-tail.
 *	integer nsects		(i)Number of second order sections.
 *
 * DESCRIPTION
 *	Transforms an analog filter to a digital filter via the bilinear 
 *	transformation. Assumes both are stored as second order sections.  
 *	The transformation is done in-place.
 *
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
*/

void CIIRfilter::bilinear( double *sn, double *sd, int nsects )
	{
	double a0, a1, a2, scale;
	int	iptr, i;
        iptr = 0;

	for (i = 0; i < nsects; i++)
		{
		a0 = sd[iptr];
		a1 = sd[iptr+1];
		a2 = sd[iptr+2];
		scale = a2 + a1 + a0;
		sd[iptr]   = 1.f;
		sd[iptr+1] = (2.f*(a0 - a2)) / scale;
		sd[iptr+2] = (a2 - a1 + a0) / scale;
		a0 = sn[iptr];
		a1 = sn[iptr+1];
		a2 = sn[iptr+2];
		sn[iptr]   = (a2 + a1 + a0) / scale;
		sn[iptr+1] = (2.f*(a0 - a2)) / scale;
		sn[iptr+2] = (a2 - a1 + a0) / scale;
		iptr = iptr + 3;
		}
	}
/*
 * NAME
 *	bupoles -- (filters) compute butterworth poles for lowpass filter
 *
 *	Copyright 1988  regents of the university of california
 *
 *
 * SYNOPSIS
 *	call  bupoles( p, type, n, iord )
 *	real p			(o)complex array containing poles
 *				contains only one from each
 *				complex conjugate pair, and
 *				all real poles
 *      character*1(1) type	(o)character array indicating pole
 *				type:
 *				'S' -- single real
 *				'C' -- complex conjugate pair
 *	integer n		(o)number of second order sections
 *	iord			(i)desired number of poles
 *
 * DESCRIPTION
 *	bupoles -- subroutine to compute butterworth poles for
 *	normalized lowpass filter
 *
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *
 *
 * last modified:  august 4, 1988
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
*/

void CIIRfilter::butter_poles(complex *p, char *type, int *n, int iord )
	{
	double	pi, angle;
	int	half, k;

        pi=M_PI;
        half = iord/2;

	/* test for odd order, and add pole at -1 */

        *n = 0;
        if (    2*half < iord )
			{
			p[0] = cmplx( -1., 0. );
			type[0] = 'S';
			*n = 1;
			}
	for (k = 0; k < half; k++)
		{
		angle = pi * ( 0.5f + (double)(2*(k+1)-1) / (double)(2*iord) );
		p[*n] = cmplx( (double)cos((double)angle), 
			(double)sin((double)angle) );
		type[*n] = 'C';
		*n = *n + 1;
		}
}

complex CIIRfilter::cmplx_add(complex c1,complex c2)
	{
	complex	csum;
	csum.x = c1.x + c2.x;
	csum.y = c1.y + c2.y;
	return (csum);
	}
complex CIIRfilter::cmplx_sub(complex c1,complex c2)
	{
	complex	csum;
	csum.x = c1.x - c2.x;
	csum.y = c1.y - c2.y;
	return (csum);
	}
complex CIIRfilter::cmplx_mul(complex c1, complex c2)
	{
	complex	c;
	c.x = c1.x * c2.x - c1.y * c2.y;
	c.y = c1.y * c2.x + c1.x * c2.y;
	return (c);
	}
complex CIIRfilter::cmplx_div(complex	c1, complex	c2)
	{
	double	a, b, c, d;
	double	f;
	complex	ratio;
	a = c1.x;
	b = c1.y;
	c = c2.x;
	d = c2.y;
	f = c*c + d*d;
	ratio.x = (a*c + b*d) / f;
	ratio.y = (b*c - a*d) / f;
	return (ratio);
	}
complex CIIRfilter::cmplx(double fr, double fi)
	{
	complex	c;
	c.x = fr;
	c.y = fi;
	return (c);
	}
complex CIIRfilter::cmplx_conjg(complex	c)
	{
	c.y = -c.y;
	return (c);
	}
complex CIIRfilter::cmplx_sqrt(complex	c)
	{
	double	radius, theta;
	double	c_r, c_i;
	c_r = c.x;
	c_i = c.y;
	radius = sqrt(sqrt(c_r*c_r + c_i*c_i));
	if (c_r == 0.)
	{
		theta = M_PI / 4.;
	}
	theta = 0.5 * atan2(c_i, c_r);
	c.x = radius * cos(theta);
	c.y = radius * sin(theta);
	return (c);
}
complex CIIRfilter::real_cmplx_mul(double r, complex c)
	{
	c.x *= r;
	c.y *= r;
	return (c);
	}
double CIIRfilter::cmplx_abs(complex c)
	{
	double	f;
	f = (double)sqrt(c.x * c.x + c.y * c.y);
	return (f);
	}
double CIIRfilter::real_part(complex c)
	{
	return (c.x);
	}
/*
 * NAME
 *	cutoffs -- (filters) Subroutine to alter the cutoff of a filter.
 *
 *	Copyright 1988  regents of the university of california
 *
 * SYNOPSIS
 *	call cutoffs( sn, sd, ns, f )
 *
 *	real sn			(i/o) numerator polynomials for second
 *				order sections.
 *	real sd			(i/o) denominator polynomials for second
 *				order sections.
 *	integer ns		(i/o) number of second-order sections
 *	real f			(i) new cutoff frequency
 *
 * DESCRIPTION
 *	Subroutine to alter the cutoff of a filter. Assumes that the	
 *	filter is structured as second order sections.  Changes
 *	the cutoffs of normalized lowpass and highpass filters through
 *	a simple polynomial transformation.
 *
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
*/

void CIIRfilter::cutoff_alter( double *sn, double *sd, int ns, double f )
{
	double	scale;
	int	iptr, i;

        scale = 2.*M_PI*f;
        iptr = 0;
	for (i = 0; i < ns; i++)
	{
		sn[ iptr + 1 ] = sn[ iptr + 1 ] / scale;
		sn[ iptr + 2 ] = sn[ iptr + 2 ] / (scale*scale);
		sd[ iptr + 1 ] = sd[ iptr + 1 ] / scale;
		sd[ iptr + 2 ] = sd[ iptr + 2 ] / (scale*scale);
		iptr = iptr + 3;
	}
}
/*
 * NAME
 *	lpa -- (filters) generate second order sections from all-pole description for lowpass filters.
 *
 *	Copyright 1988  regents of the university of california
 *
 * SYNOPSIS
 *	call lpa( p, ptype, np, sn, sd, ns )
 *	real p(*)		(i)array containing poles
 *	character*1 ptype(1)	(i)character array containing type information
 *				'S' -- single real pole or
 *				'C' -- complex conjugate pair
 *	integer np		(i)number of real poles and complex 
 *				conjugate pairs
 *	real sn			(o)numerator polynomials for second order
 *				sections.
 *	real sd			(o)denominator polynomials for second order
 *				sections.
 *	integer ns		(o)number of second order sections
 *
 * DESCRIPTION
 *
 *	Subroutine to generate second order section parameterization
 *	from an all-pole description for lowpass filters.
 *
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
*/

void CIIRfilter::lowpass(complex *p, char *ptype, int np, double *sn, double *sd, int *ns )
{
	int	iptr, i;
        *ns = 0;
        iptr = 0;
	for (i = 0; i < np; i++)
	{
		if (    ptype[i] == 'C' )
		{
			sn[iptr]     = 1.;
			sn[iptr + 1] = 0.;
			sn[iptr + 2] = 0.;
			sd[iptr] = real_part(cmplx_mul(p[i],cmplx_conjg(p[i])));
			sd[iptr + 1] = -2. * real_part( p[i] );
			sd[iptr + 2] = 1.;
			iptr = iptr + 3;
			*ns = *ns + 1;
		}
		else if (  ptype[i] == 'S' )
		{
			sn[ iptr ]     = 1.;
			sn[ iptr + 1 ] = 0.;
			sn[ iptr + 2 ] = 0.;
			sd[ iptr ]     = -real_part( p[i] );
			sd[ iptr + 1 ] = 1.;
			sd[ iptr + 2 ] = 0.;
			iptr = iptr + 3;
			
			*ns = *ns + 1;
		}
	}
}
/*
 * NAME
 *	lptbpa -- (filters)  convert all-pole lowpass to bandpass filter
 *	via the analog polynomial transformation.
 *
 *	Copyright 1988  regents of the university of california
 *
 * SYNOPSIS
 *	call lptbpa( p, ptype, np, fl, fh, sn, sd, ns )
 *	real p(*)		(i)array containing poles
 *	character*1(1) ptype	(i)array containing type information
 *				'S' -- single real pole or
 *				'C' -- complex conjugate pair
 *	integer np		(i)number of real poles and complex 
 *				conjugate pairs
 *	real fl			(i)low-frequency cutoff
 *	real fh			(i)high-frequency cutoff
 *	real sn			(o)numerator polynomials for second
 *				order sections.
 *	real sd			(o)denominator polynomials for second
 *				order sections.
 *	integer ns		(o)number of second-order sections
 *
 * DESCRIPTION
 *
 *	Subroutine to convert an all-pole lowpass filter to a bandpass
 *	filter via the analog polynomial transformation. The lowpass filter
 *	is described in terms of its poles (as input to this routine).
 *	the output consists of the parameters for second order sections.
 *	
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
*/

void CIIRfilter::lp_to_bp( complex *p, char *ptype, int np, double fl, double fh,
			  double *sn, double *sd, int *ns )
	{
	complex	ctemp, p1, p2;
	double	pi, twopi, a, b;
	int	i, iptr;
        pi = M_PI;
        twopi = 2.*pi;
        a = twopi*twopi*fl*fh;
        b = twopi*( fh - fl );
        *ns = 0;
        iptr = 0;
	for (i = 0; i < np; i++)
	{
		if (    ptype[i] == 'C' )
		{
			ctemp = real_cmplx_mul(b, p[i]);
			ctemp = cmplx_mul(ctemp, ctemp);
			ctemp = cmplx_sub(ctemp, cmplx(4.*a, 0.));
			ctemp = cmplx_sqrt( ctemp );
			p1 = real_cmplx_mul(0.5, 
				cmplx_add(real_cmplx_mul(b,p[i]), ctemp));
			p2 = real_cmplx_mul(0.5, 
				cmplx_sub(real_cmplx_mul(b,p[i]), ctemp));
			sn[ iptr ]     = 0.;
			sn[ iptr + 1 ] = b;
			sn[ iptr + 2 ] = 0.;
			sd[iptr] = real_part(cmplx_mul(p1, cmplx_conjg(p1)));
			sd[ iptr + 1 ] = -2. * real_part( p1 );
			sd[ iptr + 2 ] = 1.;
			iptr = iptr + 3;
			sn[ iptr ]     = 0.;
			sn[ iptr + 1 ] = b;
			sn[ iptr + 2 ] = 0.;
			sd[iptr] = real_part(cmplx_mul(p2, cmplx_conjg(p2)));
			sd[ iptr + 1 ] = -2. * real_part( p2 );
			sd[ iptr + 2 ] = 1.;
			iptr = iptr + 3;
			*ns = *ns + 2;
		}
		else if ( ptype[i] == 'S' )
		{
			sn[ iptr ]     = 0.;
			sn[ iptr + 1 ] = b;
			sn[ iptr + 2 ] = 0.;
			sd[ iptr ]     = a;
			sd[ iptr + 1 ] = -b*real_part( p[i] );
			sd[ iptr + 2 ] = 1.;
			iptr = iptr + 3;
			*ns = *ns + 1;
		}
	}
}
/*
 * NAME
 *	lptbra -- (filters)  convert all-pole lowpass to band reject filter
 *	via the analog polynomial transformation.
 *
 *	Copyright 1988  regents of the university of california
 *
 * SYNOPSIS
 *
 *	call lptbra( p, ptype, np, fl, fh, sn, sd, ns )
 *	real p(*)		(i)array containing poles
 *	character*1(1) ptype	(i)array containing type information
 *				'S' -- single real pole or
 *				'C' -- complex conjugate pair
 *	integer np		(i)number of real poles and complex 
 *				conjugate pairs
 *	real fl			(i)low-frequency cutoff
 *	real fh			(i)high-frequency cutoff
 *	real sn			(o)numerator polynomials for second
 *				order sections.
 *	real sd			(o)denominator polynomials for second
 *				order sections.
 *	integer ns		(o)number of second-order sections
 *
 * DESCRIPTION
 *
 *	Subroutine to convert an all-pole lowpass filter to a band reject
 *	filter via the analog polynomial transformation. The lowpass filter
 *	is described in terms of its poles (as input to this routine).
 *	the output consists of the parameters for second order sections.
 *	
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
*/

void CIIRfilter::lp_to_br(complex *p, char *ptype, int np, double fl, double fh, double *sn, double *sd, int *ns )
{
	complex	pinv, ctemp, p1, p2;
	double	pi, twopi, a, b;
	int	i, iptr;
        pi = M_PI;
        twopi = 2.*pi;
        a = twopi*twopi*fl*fh;
        b = twopi*( fh - fl );
        *ns = 0;
        iptr = 0;
	for (i = 0; i < np; i++)
	{
		if (    ptype[i] == 'C' )
		{
			pinv = cmplx_div(cmplx(1.,0.), p[i]);
			ctemp = cmplx_mul(real_cmplx_mul(b,pinv), 
				real_cmplx_mul(b, pinv));
			ctemp = cmplx_sub(ctemp, cmplx(4.*a, 0.));
			ctemp = cmplx_sqrt( ctemp );
			p1 = real_cmplx_mul(0.5, 
				cmplx_add(real_cmplx_mul(b,pinv), ctemp));
			p2 = real_cmplx_mul(0.5, 
				cmplx_sub(real_cmplx_mul(b,pinv), ctemp));
			sn[ iptr ]     = a;
			sn[ iptr + 1 ] = 0.;
			sn[ iptr + 2 ] = 1.;
			sd[iptr] = real_part(cmplx_mul(p1, cmplx_conjg(p1)));
			sd[ iptr + 1 ] = -2. * real_part(p1);
			sd[ iptr + 2 ] = 1.;
			iptr = iptr + 3;
			sn[ iptr ]     = a;
			sn[ iptr + 1 ] = 0.;
			sn[ iptr + 2 ] = 1.;
			sd[iptr] = real_part(cmplx_mul(p2, cmplx_conjg(p2)));
			sd[ iptr + 1 ] = -2. * real_part(p2);
			sd[ iptr + 2 ] = 1.;
			iptr = iptr + 3;
			*ns = *ns + 2;
		}
		else if (  ptype[i] == 'S' )
		{
			sn[ iptr ]     = a;
			sn[ iptr + 1 ] = 0.;
			sn[ iptr + 2 ] = 1.;
			sd[ iptr ]     = -a*real_part( p[i] );
			sd[ iptr + 1 ] = b;
			sd[ iptr + 2 ] = -real_part( p[i] );
			iptr = iptr + 3;
			*ns = *ns + 1;
		}
	}
}
/*
 * NAME
 *	lpthpa -- (filters)  convert all-pole lowpass to high pass filter
 *	via the analog polynomial transformation.
 *
 *	Copyright 1988  regents of the university of california
 *
 *
 * SYNOPSIS
 *	call lpthpa( p, ptype, np, fl, fh, sn, sd, ns )
 *	real p(*)		(i)array containing poles
 *	character*1(1) ptype	(i)array containing type information
 *				'S' -- single real pole or
 *				'C' -- complex conjugate pair
 *	integer np		(i)number of real poles and complex 
 *				conjugate pairs
 *	real fl			(i)low-frequency cutoff
 *
 *	real fh			(i)high-frequency cutoff
 *	real sn			(o)numerator polynomials for second
 *				order sections.
 *	real sd			(o)denominator polynomials for second
 *				order sections.
 *	integer ns		(o)number of second-order sections
 *
 * DESCRIPTION
 *	Subroutine to convert an all-pole lowpass filter to a high pass
 *	filter via the analog polynomial transformation. The lowpass filter
 *	is described in terms of its poles (as input to this routine).
 *	the output consists of the parameters for second order sections.
 *	
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
*/

void CIIRfilter::lp_to_hp(complex p[], char ptype[], int np, double sn[], double sd[], int *ns )
	{
	int	i, iptr;
        *ns = 0;
        iptr = 0;
	for (i = 0; i < np; i++)
		{
		if (ptype[i] == 'C' )
			{
			sn[ iptr ]     = 0.f;
			sn[ iptr + 1 ] = 0.f;
			sn[ iptr + 2 ] = 1.f;
			sd[ iptr ]     = 1.f;
			sd[ iptr + 1 ] = -2.f * real_part( p[i] );
			sd[ iptr + 2 ] = real_part(cmplx_mul(p[i], cmplx_conjg(p[i])));
			iptr = iptr + 3;
			*ns = *ns + 1;
			}
		else if (  ptype[i] == 'S' )
			{
			sn[ iptr ]     = 0.;
			sn[ iptr + 1 ] = 1.;
			sn[ iptr + 2 ] = 0.;
			sd[ iptr ]     = 1.;
			sd[ iptr + 1 ] = -real_part( p[i] );
			sd[ iptr + 2 ] = 0.;
			iptr = iptr + 3;
			*ns = *ns + 1;
			}
		}
}

/*
 * NAME
 *	secord  -- (filters) implements one time-step of second order section
 *
 *	Copyright 1988  regents of the university of california
 *
 *
 * FILE
 *	source: geotech/src/libsrc/filter/secord.f
 *	object: geotech/lib/libfilters.a
 *
 *
 * SYNOPSIS
 *	call secord( y, y1, y2, a1, a2, x, x1, x2, b, b1, b2 )

 *	real y1, y2		(i)previous output samples
 *	real a1, a2		(i)denominator coefficients
 *	real x			(i)current input sample
 *	realx1, x2		(i)previous input samples
 *	real b, b1, b2		(i)numerator coefficients
 *	real y			(o)current output sample
 *
 *
 * DESCRIPTION
 *
 *	subroutine implements one time-step of a single second order section.
 *	The recurrence equation is:
 *
 *		y  =  - a1*y1 - a2*y2 + b*x + b1*x1 + b2*x2
 *
 *	the z-transform of the second order section is assumed to be:
 *
 *			b  +  b1 * z**(-1)  +  b2 * z**(-2)
 *			-----------------------------------
 *			1  +  b1 * z**(-1)  +  b2 * z**(-2)
 *
 *
 *	x1, x2, y1, y2 can be thought of as internal states of the filter
 *	which are stored from one call to the next.
 *	
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
*/

void CIIRfilter::second_order(double *y, double *y1, double *y2, double a1, double a2, double x, double *x1, double *x2, double b, double b1, double b2 )
	{
    *y = b*x + b1*(*x1) + b2*(*x2) - ( a1*(*y1) + a2*(*y2) );
    *y2 = *y1;
    *y1 = *y;
    *x2 = *x1;
    *x1 = x;
	}
/*
 * NAME
 *	warp -- (filters) function, applies tangent frequency warping
 *	to compensate for bilinear analog -> digital transformation
 *
 *	Copyright 1988  regents of the university of california
 *
 * SYNOPSIS
 *	real function warp(f,t)
 *
 *		f	original design frequency specification (hertz)
 *		t	sampling interval
 *
 *
 * DESCRIPTION
 *
 *	warp -- function, applies tangent frequency warping to compensate
 *		for bilinear analog -> digital transformation
 *
 * DIAGNOSTICS
 *	none
 *
 * RESTRICTIONS
 *	unknown.
 *
 * BUGS
 *	unknown.
 *
 * AUTHOR
 *	Dave Harris
 *
 *	Lawrence Livermore National Laboratory
 *	L-205
 *	P.O. Box 808
 *	Livermore, CA  94550
 *	USA
 *
 *	(415) 423-0617
 *
 *	Translated from Fortran to C by:
 *	C. S. Lynnes
 *	Teledyne Geotech
 *	314 Montgomery
 *	Alexandria, VA  22314
 *	(703) 739-7316
 *
 *	last modified:  july 7, 1980
 *
 *
 *
 *
*/

double CIIRfilter::tangent_warp(double f, double t)
{
	double	twopi;
	double	angle;
	double	warp;
	twopi = 2*M_PI;
	angle = twopi*f*t/2.;
        warp = 2.*tan(angle)/t;
        warp = warp/twopi;
	return((double)warp);
}




/* Revision History
 *
 * $Log: IIRfilter.cpp,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
