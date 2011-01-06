#include "..\\stdafx.h"
#include <stdio.h>
#include "filter.h"

//#include "coords.h"
extern struct gather_plot_filter_ gpl_filters_def[];




int gpl_filter_seg(double tstart, double  dt, int nsamps, float *data, struct gather_plot_filter_ *filter,
float *data_filtered)
	{
    int             i;
    double           data_mean;
    int             itaper;
    double           durstr,
                    durend,
                    tstap;
    float           dttap;
    int             nttap;
    int             ol,
                    ou;
    int             isave = 1;

    /* De-mean and taper the data. */
    /*data_mean = 0.0;*/
    /* for (i=0; i<nsamps; i++) data_mean += data[i]; data_mean /= nsamps; */
    for (i = 0; i < nsamps; i++)
	data_filtered[i] = data[i] - data_mean;
    itaper = 1;
    durstr = 0.8 * filter->start_pad;
    durend = 0.8 * filter->end_pad;
    tstap = 0.0;
    dttap = dt;
    nttap = nsamps;
    /* taper_ (&itaper, &durstr, &durend, &tstap, &dttap, &nttap,
     * data_filtered); */
    /* Filter the data. */
    switch (filter->type)
      {
      case GPL_BUTTERWORTH:
		ol = filter->param2 + 0.5;
		ou = filter->param4 + 0.5;
		setbfl_(&filter->param1, &ol, &filter->param3, &ou, &dttap);
		inifil_(data_filtered);
		filrec_(&nttap, data_filtered, &isave, data_filtered);
		freefl_();
		break;
      default:
		break;
      }
    return (1);
}

int _filter_seg(int nFilter, double tstart, double  dt, int nsamps, float *data, float *data_filtered)
	{
	return gpl_filter_seg(tstart, dt, nsamps, data, &gpl_filters_def[nFilter],data_filtered);
	}

int _open_filter(double  dt, int nFilter)
	{
    int             i;
    double           data_mean;
    int             itaper;
    double           durstr,
                    durend,
                    tstap;
    float           dttap;
    int             nttap;
    int             ol,
                    ou;
    int             isave = 1;

	struct gather_plot_filter_ *filter=&gpl_filters_def[nFilter];

    /* De-mean and taper the data. */
    /*data_mean = 0.0;*/
    /* for (i=0; i<nsamps; i++) data_mean += data[i]; data_mean /= nsamps; */
    itaper = 1;
    durstr = 0.8 * filter->start_pad;
    durend = 0.8 * filter->end_pad;
    tstap = 0.0;
    dttap = dt;
    /* taper_ (&itaper, &durstr, &durend, &tstap, &dttap, &nttap,
     * data_filtered); */
    /* Filter the data. */
    switch (filter->type)
      {
      case GPL_BUTTERWORTH:
		ol = filter->param2 + 0.5;
		ou = filter->param4 + 0.5;
		setbfl_(&filter->param1, &ol, &filter->param3, &ou, &dttap);
		break;
      default:
		break;
      }
    return 1;
	}
void _init_filter(float *data)
	{
	inifil_(data);
	}
void _do_filter(float *data)
	{
	double data_mean=0.0;
    int isave = 1;
    int nttap = 1;
	filrec_(&nttap, data, &isave, data);
	}
void _close_filter()
	{
	freefl_();
	}
/* $Id: gpl_filter_seg.cpp,v 1.1 2000/07/29 01:10:37 akimov Exp $ */
