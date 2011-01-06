#ifndef _filter_h_
#define _filter_h_
#define GPL_BUTTERWORTH 1
struct gather_plot_filter_ {	/* This structure contains the GatherPlot
				   "internal" filter info for a single
				   filter. */
	int type;			/* Filter type.
					   = GPL_BUTTERWORTH 
					     - Bandpass, lowpass or highpass
					       Butterworth filter with:
					       param1 = Low cutoff freq.
					       param2 = Low cutoff order.
					       param3 = High cutoff freq.
					       param4 = High cutoff order.
					       Low pass if param1 = 0.0.
					       High pass if param3 = 0.0. */
	char label[32];			/* A filter label used with the menu
					   buttons and for other annotations. */
	double start_pad;		/* Defines filter transient time at the
					   start of the trace. */
	double end_pad;			/* Defines filter transient time at the
					   end of the trace. */
	float param1;			/* Filter parameters. */
	float param2;
	float param3;
	float param4;
};

void setbfl_ (float *fl, int *ol, float *fu, int *ou, float *dt );
void inifil_ ( float *data );
void filrec_ ( int *npts, float *idata, int *saveflag, float *odata);
void freefl_();




#endif
