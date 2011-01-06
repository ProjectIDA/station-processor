#pragma ident "$Id: utils.c,v 1.1.1.1 2000/02/08 20:20:15 dec Exp $"
#include <stdlib.h>
#include <sys/types.h>
#include "nrts.h"
#include "view.h"

int packetcpy(output, input, new_packet)
struct nrts_packet *output;
struct nrts_packet *input;
int new_packet;
{
Cardinal dlen, hlen;


	if(input->hlen <= 0 || input->dlen <= 0
		 || input->header == NULL || input->data == NULL) {
		/*
		util_log(1, "packetcpy: nothing to copy(input == NULL)");
		*/
		return;
	}

  strcpy(output->sname, input->sname);
  strcpy(output->cname, input->cname);
	strcpy(output->instype, input->instype);
  output->beg.time = input->beg.time;
  output->beg.code = input->beg.code;
  output->beg.qual = input->beg.qual;
 	output->end.time = input->end.time;
  output->end.code = input->end.code;
  output->end.qual = input->end.qual;
  output->tear     = input->tear;
  output->sint     = input->sint;
	output->calib    = input->calib;
	output->calper   = input->calper;
	output->vang     = input->vang;
	output->hang     = input->hang;
  output->nsamp    = input->nsamp;
  output->wrdsiz   = input->wrdsiz;
  output->order    = input->order;
  output->type     = input->type;

	hlen = (Cardinal)input->hlen;
	dlen = (Cardinal)input->dlen;

	if(new_packet) {
		output->header = XtMalloc(hlen);
		output->data = XtMalloc(dlen);
	} else {
		output->header = XtRealloc(output->header, hlen);
		output->data = XtRealloc(output->data, dlen);
	}
  output->hlen     = input->hlen;
  output->dlen     = input->dlen;

	if(output->header == NULL) 
		util_log(1, "packetcopy: can't alloc memory for header");

	if(output->data == NULL) 
		util_log(1, "packetcopy: can't alloc memory for data");

	if(output->data == NULL || output->header == NULL) 
		quit();

	memcpy(output->data, input->data, (int)dlen);
	memcpy(output->header, input->header, (int)hlen);
	if(!new_packet) {
		XtFree(input->data);
		input->data = NULL;
		XtFree(input->header);
		input->header = NULL;
	}

}

int shiftpackets(plot_stuff)
Plot_stuff *plot_stuff;
{
int pi, p_index;
	
	for(pi = 0, p_index = plot_stuff->packet_index; pi < plot_stuff->packet_index; pi++) {
		packetcpy(&(plot_stuff->packet[pi]), &(plot_stuff->packet[pi+1]), 0);
	}
	plot_stuff->packet_index -= 1;

}

/* Revision History
 *
 * $Log: utils.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
