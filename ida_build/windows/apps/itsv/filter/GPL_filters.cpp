
/*
 *	This include file (GPL_filters.h) contains "hardwired" GatherPlot 
 *	internal trace filters.
 */
#include "..\\stdafx.h"
#include "filter.h"

struct gather_plot_filter_ gpl_filters_def[] = {
	GPL_BUTTERWORTH,	"0.02 LP",	350.0,	0.0, 0.0, 0, 0.02, 5,
	GPL_BUTTERWORTH,	"0.1 LP",	120.0,	0.0, 0.0, 0, 0.1, 5,
	GPL_BUTTERWORTH,	"0.5 LP",	14.0,	0.0, 0.0, 0, 0.5, 5,
	GPL_BUTTERWORTH,	"1.0 LP",	12.0,	0.0, 0.0, 0, 1.0, 5,
	GPL_BUTTERWORTH,	"2.0 LP",	3.5,	0.0, 0.0, 0, 2.0, 5,
	GPL_BUTTERWORTH,	"5.0 LP",	2.0,	0.0, 0.0, 0, 5.0, 5,
	GPL_BUTTERWORTH,	"0.1-1.0 BP",	120.0,	0.0, 0.1, 5, 1.0, 5,
	GPL_BUTTERWORTH,	"1.0-2.0 BP",	12.0,	0.0, 1.0, 5, 2.0, 5,
	GPL_BUTTERWORTH,	"1.0-5.0 BP",	12.0,	0.0, 1.0, 5, 5.0, 5,
	GPL_BUTTERWORTH,	"1.0-10.0 BP",	12.0,	0.0, 1.0, 5, 10.0, 5,
	GPL_BUTTERWORTH,	"5.0-10.0 BP",	2.0,	0.0, 5.0, 5, 10.0, 5
};

int GPL_NFILTERS_DEF = sizeof ( gpl_filters_def ) / sizeof ( struct gather_plot_filter_ ) ; 

/* $Id: GPL_filters.cpp,v 1.1 2000/07/29 01:10:37 akimov Exp $ */
