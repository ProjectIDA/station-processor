#pragma ident "$Id: IIRfilter.h,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
// IIRfilter.h: interface for the CIIRfilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IIRFILTER_H__65ED5FA5_7EDE_11D3_B61D_FAC543000000__INCLUDED_)
#define AFX_IIRFILTER_H__65ED5FA5_7EDE_11D3_B61D_FAC543000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct
{
	double   x;
	double   y;
} complex;


class CIIRfilter  
{
public:
	CIIRfilter();
	virtual ~CIIRfilter();
	void iir_design( int iord, char *type, float fl, float fh, float ts);
	double apply_iir(double fData);


private:
	double	x1[20], x2[20], y1[20], y2[20];
	double sn[20],sd[20];
	int nsects;
	double	input, output;


void lp_to_hp(complex *p, char *ptype, int np, double *sn, double *sd, int *ns );
void lp_to_bp( complex *p, char *ptype, int np, double fl, double fh,
			  double *sn, double *sd, int *ns );
void bilinear( double *sn, double *sd, int nsects );

void butter_poles(complex *p, char *type, int *n, int iord );
double tangent_warp(double f, double t);
void lp_to_br(complex *p, char *ptype, int np, double fl, double fh, double *sn, double *sd, int *ns );
void lowpass(complex *p, char *ptype, int np, double *sn, double *sd, int *ns );
void cutoff_alter(double *sn, double *sd, int ns, double f );
void second_order(double *y, double *y1, double *y2, double a1, double a2, 
	  double x, double *x1, double *x2, double b, double b1, double b2 );
complex  cmplx_add(complex c1,complex c2);
complex  cmplx_sub(complex c1,complex c2);
complex  cmplx_mul(complex c1,complex c2);
complex  cmplx_div(complex c1,complex c2);
complex  cmplx(double c1, double c2);
complex  cmplx_conjg(complex c1);
complex  cmplx_sqrt(complex);
complex  real_cmplx_mul(double r, complex c);
double    cmplx_abs(complex c);
double    real_part(complex c);

};

#endif // !defined(AFX_IIRFILTER_H__65ED5FA5_7EDE_11D3_B61D_FAC543000000__INCLUDED_)

/* Revision History
 *
 * $Log: IIRfilter.h,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
