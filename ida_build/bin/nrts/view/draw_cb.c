#pragma ident "$Id: draw_cb.c,v 1.1.1.1 2000/02/08 20:20:14 dec Exp $"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/param.h>
#include <unistd.h>
#include <sys/stat.h>
#include <Xm/MainW.h>
#include <Xm/DrawingA.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/Form.h>
#include <Xm/PanedW.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Frame.h>
#include <Xm/CascadeB.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/ArrowBG.h>
#include <Xm/FileSB.h>
#include <X11/cursorfont.h>
#include "nrts.h"
#include "util.h"
#include "view.h"


extern GC gc, gc_clear, gc_phase;
extern Dimension plot_form_height;
static long buf[1024];
extern int first_call;
static int gap = 0;

Boolean CheckForInterrupt();
void handle_input();

void
draw_cb(client_data, timer)
XtPointer client_data;
XtIntervalId *timer;
{
short *sbuf;
long *lbuf;
GatherStuff *gather_stuff = (GatherStuff *) client_data;
XtAppContext app = gather_stuff->app;
Widget main_w = gather_stuff->menu_stuff.main_w;
Widget file_menu = gather_stuff->menu_stuff.file_menu;
Plot_stuff *plot_stuff;
Sys_stuff *sys_stuff;
int p = (int) gather_stuff->index.sys;
User_info *userinfo = gather_stuff->userinfo;
Widget widget, cur_widget;
int latency, first_entry;
static int i, j, n, k, m, t, u, s, pi, len;
static float fmax, fmin, ytop, ybot, center, ygain;
static long prev_value, curr_value, prev_max, prev_min;
int screen_width, screen_height;
static Position x, y, x_delta, x_offset, prev_x, prev_y, r_edge;
static double beg, end;
static char string[40];
static char max_min_str[1024];
struct nrts_packet packet;
int status;
XEvent event;
Display *dpy;

  if(first_call) {
		util_log(2, "initializing drawing stuff(nsys = %d, nchn = %d)", userinfo->nsys, userinfo->nchn);
		XRaiseWindow(XtDisplay(XtParent(main_w)), XtWindow(XtParent(main_w)));
    first_call = 0;
    x_delta = (Position) 1;
    x_offset = 0;
    ygain = userinfo->ygain;
    i = 0;
    k = 0;
    screen_width = WidthOfScreen(XtScreen(main_w));
    screen_height = HeightOfScreen(XtScreen(main_w));
		/*
		for(j = 0; j < userinfo->nsys; j++) {
			sys_stuff = &(gather_stuff->sys_stuff[j]);
      for(i = 0; i < sys_stuff->nchn; i++) {
				plot_stuff = &(gather_stuff->plot_stuff[j][i]);
				XtFree(plot_stuff->array); plot_stuff->array = NULL;
				XtFree(plot_stuff->long_value); plot_stuff->long_value = NULL;
				XtFree(plot_stuff->segs); plot_stuff->segs = NULL;
				XtFree(plot_stuff->packet); plot_stuff->packet = NULL;
			}
		}
		*/
    for(j = 0; j < userinfo->nsys; j++) {
			sys_stuff = &(gather_stuff->sys_stuff[j]);
      for(i = 0; i < sys_stuff->nchn; i++) {
				plot_stuff = &(gather_stuff->plot_stuff[j][i]);
       	plot_stuff->array = (XPoint *) XtRealloc((char *)plot_stuff->array, sizeof(XPoint)*screen_width*2);
        if(plot_stuff->array == NULL) {
          fprintf(stderr," cannot XtRealloc memory for array of %s %s\n", sys_stuff->info.input,  sys_stuff->cname[i]);
          exit(1);
        }
        plot_stuff->long_value = (long *) XtRealloc((char *)plot_stuff->long_value, sizeof(long)*screen_width*2);
        if(plot_stuff->long_value == NULL) {
          fprintf(stderr," cannot XtRealloc memory for long_values of %s %s\n", sys_stuff->info.input,  sys_stuff->cname[i]);
          exit(1);
        }
       	plot_stuff->segs = (XSegment *) XtRealloc((char *)plot_stuff->segs, sizeof(XSegment)*screen_width);
        if(plot_stuff->segs == NULL) {
          fprintf(stderr," cannot XtRealloc memory for segs of %s %s\n", sys_stuff->info.input,  sys_stuff->cname[i]);
          exit(1);
        }
        plot_stuff->packet = (struct nrts_packet *) XtRealloc((char *)plot_stuff->packet, sizeof(struct nrts_packet)*screen_width);
        if(plot_stuff->packet == NULL) {
          fprintf(stderr," cannot XtRealloc memory for packet struct of %s %s\n", sys_stuff->info.input,  sys_stuff->cname[i]);
          exit(1);
        }
				/*
				for(k = 0; k < screen_width; k++) {
					XtFree(plot_stuff->packet[k].data);
					plot_stuff->packet[k].data = NULL;
					plot_stuff->packet[k].dlen = 0;
					XtFree(plot_stuff->packet[k].header);
					plot_stuff->packet[k].header = NULL;
					plot_stuff->packet[k].hlen = 0;
				}
				*/
        plot_stuff->seconds = 0;
        plot_stuff->to_save = 1;
        plot_stuff->first_point = 1;
        plot_stuff->scale = 1;
        plot_stuff->index = 0;
        plot_stuff->packet_index = 0;
        XtVaGetValues(plot_stuff->drawing_a, XmNwidth, &(plot_stuff->width), XmNheight, &(plot_stuff->height), NULL);
      }
    }
    for(j = 0; j < userinfo->nsys; j++) {
			sys_stuff = &(gather_stuff->sys_stuff[j]);
			sys_stuff->connected = 0;
			sys_stuff->pause = 1;
			if(sys_stuff->sys != NULL) {
      	sys_stuff->status = NRTS_OK;
				util_log(2, "connecting %s", sys_stuff->info.input);
				sys_stuff->sd = Nrts_tap_connect(j);
				sys_stuff->connected = 1;
				util_log(2, "setting input handler for %s", sys_stuff->info.input);
				sys_stuff->InputId = XtAppAddInput(app, sys_stuff->sd,
						(XtPointer)XtInputReadMask, handle_input, (XtPointer) j);
				sys_stuff->pause = 0;
			}
		}
    i = 0;
    k = 0;
		return;
  }

	sys_stuff = &(gather_stuff->sys_stuff[p]);	
	status = sys_stuff->status;
  if((sys_stuff->status = Nrts_tap(&packet, &len, p)) == NRTS_OK) { 
		if (packet.wrdsiz == sizeof(short)) {
			sbuf = (short *) packet.data;
			for(i = 0; i < packet.nsamp; i++)
				buf[i] = (long) sbuf[i];
		} else if(packet.wrdsiz == sizeof(long)) {
				lbuf = (long *) packet.data;
				for(i = 0; i < packet.nsamp; i++)
					buf[i] = (long) lbuf[i];
		}

    n = packet.nsamp;
    for(t = 0; t < sys_stuff->nchn; t++) {
      if(strncmp(packet.cname, sys_stuff->cname[t], strlen(packet.cname)) == 0) {
        m = t;
        break;
      }
    }
		plot_stuff = &(gather_stuff->plot_stuff[p][m]);
		pi = plot_stuff->packet_index;
		packetcpy(&(plot_stuff->packet[pi]), &packet, 1);
		plot_stuff->packet_index += 1;
		ygain = plot_stuff->ygain.value;
		cur_widget = plot_stuff->drawing_a;
		dpy = XtDisplay(cur_widget);
		XFlush(dpy);
																
    XtVaGetValues(plot_stuff->drawing_a, XmNwidth, &(plot_stuff->width), XmNheight, &(plot_stuff->height), NULL);
		if(plot_stuff->prev_width != 0 && plot_stuff->prev_width < plot_stuff->width)
			redraw_plot(&(plot_stuff->ygain));
   	plot_stuff->prev_width = plot_stuff->width;
    r_edge = (int) plot_stuff->width - 2;

    if(!plot_stuff->first_point && packet.tear > (float) 0)  {
      x_offset = (Position) ((packet.tear)/(packet.sint*plot_stuff->decim));
      s = 0;
      plot_stuff->s = s;
    }

    if(!plot_stuff->first_point && x_offset > r_edge) {

      XFillRectangle (XtDisplay (plot_stuff->drawing_a), plot_stuff->pixmap, gc_clear, 0, 0, plot_stuff->width, plot_stuff->height);

       XCopyArea (XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, XtWindow(plot_stuff->drawing_a), gc,
           0, 0, plot_stuff->width, plot_stuff->height, 0, 0);

      plot_stuff->first_point = 1;
      plot_stuff->to_save = 1;
      plot_stuff->seconds = 0;
      i = 0;

      util_log(1, "%s %s %ld secs time jump (%ld samples, %d points)", sys_stuff->info.input, packet.cname, (long) packet.tear, (long)(packet.tear/packet.sint), x_offset );
    }
	
    beg = packet.beg.time;
		plot_stuff->beg = beg;
    end = packet.beg.time + (double)(n*packet.sint); 
		plot_stuff->sint = packet.sint;
		plot_stuff->end = end;
    if(plot_stuff->first_point) {
      string[0] = 0;
      sprintf(string, "%s",util_dttostr(beg,0));
      string[strlen(string)] = 0;
      XmTextFieldSetString(plot_stuff->beg_time_w, string);
    }

    for(k = 0; k < n; k++) {
      if(plot_stuff->first_point) {
        plot_stuff->points += 1;
      } else {
        plot_stuff->points += (1 + x_offset);
      }
      if(plot_stuff->first_point) {
        plot_stuff->s = 0;
        s = plot_stuff->s;
        x_offset = 0;
        plot_stuff->max_value = buf[0];
        plot_stuff->min_value = buf[0];
        plot_stuff->mean_value = 0.0;
        for(t = 0; t < n; t++) {
          plot_stuff->mean_value += (float) buf[t];
          if(plot_stuff->max_value <= buf[t])
            plot_stuff->max_value = buf[t];
          if(plot_stuff->min_value >= buf[t])
            plot_stuff->min_value = buf[t];
        }
        fmax = (float)plot_stuff->max_value;
        fmin = (float)plot_stuff->min_value;
        plot_stuff->mean_value /= t;
        
        if(fmax < 0)
          fmax = -fmax;
        if(fmin < 0)
          fmin = -fmin;
        if(fmin > fmax)
          fmax = fmin;
        ytop = fmax;
        ybot = -fmax;
        ytop += 0.5;
        ybot -= 0.5;
        center = (ybot + ytop) * 0.5;
        plot_stuff->ytp = center - (center - ytop)/ygain;
        plot_stuff->scale = (float) plot_stuff->height/(ybot - ytop); 
        plot_stuff->scale *= ygain;
        max_min_str[0] = 0;
        sprintf(max_min_str, "MAX: %10ld, MIN: %10ld, ", plot_stuff->max_value, plot_stuff->min_value);
        max_min_str[strlen(max_min_str)] = 0;
        XmTextFieldSetString(plot_stuff->max_min_w, max_min_str);
        if(!userinfo->decim)
          plot_stuff->decim = 1/packet.sint;
        else
          plot_stuff->decim = userinfo->decim;
        if(!plot_stuff->decim)
          plot_stuff->decim = 1;
        plot_stuff->index = 0;
				plot_stuff->p_nsamp = packet.nsamp;;
        i = plot_stuff->index;
        prev_x = 0; prev_y = 0;
        x = (Position) 0;
        plot_stuff->long_value[i] = buf[k];
        prev_value = plot_stuff->long_value[i];
        curr_value = plot_stuff->long_value[i];
        y = (Position)  (plot_stuff->scale * ((float)plot_stuff->long_value[i] - plot_stuff->mean_value - plot_stuff->ytp));
        
        XDrawPoint(XtDisplay(plot_stuff->drawing_a), XtWindow(plot_stuff->drawing_a), gc, x, y);
        XDrawPoint(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, x, y);
        plot_stuff->array[i].x = (short)x; plot_stuff->array[i].y = (short)y;
        prev_x = (Position) plot_stuff->array[i].x; prev_y = (Position) plot_stuff->array[i].y;
        plot_stuff->first_point = 0;
        plot_stuff->points = 0;
      } else if(plot_stuff->points >= plot_stuff->decim) {
        plot_stuff->points = 0;
        i = plot_stuff->index;
        prev_x = (Position) plot_stuff->array[i].x; prev_y = (Position) plot_stuff->array[i].y;
        prev_value = plot_stuff->long_value[i];
        plot_stuff->index += 1;
        i = plot_stuff->index;
        if(x_offset > 0) {
          util_log(1, "%s %s %ld secs time jump (%ld samples, %d points)", sys_stuff->info.input, packet.cname, (long) packet.tear, (long)(packet.tear/packet.sint), x_offset ); 
        }
        x = prev_x;
        plot_stuff->long_value[i] = buf[k];

        prev_max = plot_stuff->max_value;
        prev_min = plot_stuff->min_value;
        if(plot_stuff->max_value <= buf[k])
          plot_stuff->max_value = buf[k];
        if(plot_stuff->min_value >= buf[k])
          plot_stuff->min_value = buf[k];

        if(prev_max != plot_stuff->max_value || prev_min != plot_stuff->min_value)
          plot_stuff->calculate = 1;
        else 
          plot_stuff->calculate = 0;

        y = (Position)  (plot_stuff->scale * ((float)plot_stuff->long_value[i] - plot_stuff->mean_value - plot_stuff->ytp));
        curr_value = plot_stuff->long_value[i];
        if(x_offset > 0) {
          for(t = i; t < i + x_offset; t++) {
            x += x_delta;
            plot_stuff->array[t].x = x; plot_stuff->array[t].y = -1000;
          }
          i += x_offset - 1;
          plot_stuff->index = i;
          XDrawPoint(XtDisplay(plot_stuff->drawing_a), XtWindow(plot_stuff->drawing_a), gc, x, y);
          XDrawPoint(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, x, y);
          plot_stuff->array[i].x = (short)x; plot_stuff->array[i].y = (short)y;
          plot_stuff->long_value[i] = curr_value;
          prev_value = plot_stuff->long_value[i];
          prev_x = (Position) plot_stuff->array[i].x; prev_y = (Position) plot_stuff->array[i].y;
          x_offset = 0;
          if(plot_stuff->array[i].x >= (short) r_edge) {
            plot_stuff->to_save = 0;
            x = (Position) r_edge;
            plot_stuff->array[i].x = (short)x;
            while(i >= r_edge) {
              for(j = 0; j <= i; j++) {
                plot_stuff->array[j].y = plot_stuff->array[j+1].y;
                plot_stuff->long_value[j] = plot_stuff->long_value[j+1];
              }
			 				if(plot_stuff->phases.p && plot_stuff->phases.p_x > 0) {
								plot_stuff->phases.p_x -= 1;
								if(plot_stuff->phases.p_x <= 0)
									plot_stuff->phases.p = 0;
							}
			 				if(plot_stuff->phases.s && plot_stuff->phases.s_x > 0) {
								plot_stuff->phases.s_x -= 1;
								if(plot_stuff->phases.s_x <= 0)
									plot_stuff->phases.s = 0;
							} 
              plot_stuff->index -= 1; 
              i = plot_stuff->index;
            }
          }
          s = 0;
          plot_stuff->s = 0;
          continue;
        }
        x += x_delta;
        plot_stuff->array[i].x = (short)x; plot_stuff->array[i].y = (short) y;
        

        if(plot_stuff->array[i].x >= (short) r_edge) {
          plot_stuff->to_save = 0;
          x = (Position) r_edge;
          plot_stuff->array[i].x = (short)x;
					while(i >= r_edge) {
        	  for(j = 0; j <= plot_stuff->index; j++) {
         	   plot_stuff->array[j].y = plot_stuff->array[j+1].y;
         	   plot_stuff->long_value[j] = plot_stuff->long_value[j+1];
         		 }
						 if(plot_stuff->phases.p && plot_stuff->phases.p_x > 0) {
								plot_stuff->phases.p_x -= 1;
								if(plot_stuff->phases.p_x <= 0)
									plot_stuff->phases.p = 0;
							}
			 				if(plot_stuff->phases.s && plot_stuff->phases.s_x > 0) {
								plot_stuff->phases.s_x -= 1;
								if(plot_stuff->phases.s_x <= 0)
									plot_stuff->phases.s = 0;
							}
         	 	plot_stuff->index -= 1; 
         	 	i = plot_stuff->index;
					}
          continue;
        }

        if(plot_stuff->array[i].y >= plot_stuff->height || plot_stuff->array[i].y <= 0) {
          plot_stuff->mean_value = 0.0;
					first_entry = 1;
          for(t = 0, s = 0; t <= plot_stuff->index; t++) {
            if(plot_stuff->array[t].y == -1000) {
              continue;
            } else {
							if(first_entry) {
          			plot_stuff->max_value = plot_stuff->long_value[0];
          			plot_stuff->min_value = plot_stuff->long_value[0];
								first_entry = 0;
							}
              ++s;
              plot_stuff->mean_value += (float)plot_stuff->long_value[t];
              if(plot_stuff->max_value <= plot_stuff->long_value[t])
                plot_stuff->max_value = plot_stuff->long_value[t];
              if(plot_stuff->min_value >= plot_stuff->long_value[t])
                plot_stuff->min_value = plot_stuff->long_value[t];
            }
          }
          fmax = (float)plot_stuff->max_value;
          fmin = (float)plot_stuff->min_value;
          plot_stuff->mean_value /= s;
        
          if(fmax < 0)
            fmax = -fmax;
          if(fmin < 0)
            fmin = -fmin;
          if(fmin > fmax)
            fmax = fmin;
          ytop = fmax;
          ybot = -fmax;
          ytop += 0.5;
          ybot -= 0.5;
          center = (ybot + ytop) * 0.5;
          plot_stuff->ytp = center - (center - ytop)/ygain;
          plot_stuff->scale = (float) plot_stuff->height/(ybot - ytop); 
          plot_stuff->scale *= ygain;
          XFillRectangle (XtDisplay (plot_stuff->drawing_a), plot_stuff->pixmap, gc_clear, 0, 0, plot_stuff->width, plot_stuff->height);
          gap = 0;
          for(t = 0; t <= plot_stuff->index; t++) {
            if(plot_stuff->array[t].y == -1000) {
              gap = 0;
            } else if(!gap) {
              gap = 1;
              plot_stuff->array[t].y = (short)  (plot_stuff->scale * ((float)plot_stuff->long_value[t] - plot_stuff->mean_value - plot_stuff->ytp));
              XDrawPoint(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, (Position)plot_stuff->array[t].x, (Position)plot_stuff->array[t].y);
            } else {
              plot_stuff->array[t-1].y = (short)  (plot_stuff->scale * ((float)plot_stuff->long_value[t-1] - plot_stuff->mean_value - plot_stuff->ytp));
              plot_stuff->array[t].y = (short)  (plot_stuff->scale * ((float)plot_stuff->long_value[t] - plot_stuff->mean_value - plot_stuff->ytp));
              XDrawLine(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, (Position)plot_stuff->array[t-1].x, (Position)plot_stuff->array[t-1].y, (Position)plot_stuff->array[t].x, (Position)plot_stuff->array[t].y);
            }
          }
			 		if(plot_stuff->phases.p) {
				 		XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)plot_stuff->phases.p_x, (Position)0, (Position)plot_stuff->phases.p_x, (Position)plot_stuff->height);
					 }
			 		if(plot_stuff->phases.s) {
				 		XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)plot_stuff->phases.s_x, (Position)0, (Position)plot_stuff->phases.s_x, (Position)plot_stuff->height);
					 }
          XCopyArea (XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, XtWindow(plot_stuff->drawing_a), gc,
            0, 0, plot_stuff->width, plot_stuff->height, 0, 0);
          max_min_str[0] = 0;
          sprintf(max_min_str, "MAX: %10ld, MIN: %10ld, ", plot_stuff->max_value, plot_stuff->min_value);
          max_min_str[strlen(max_min_str)] = 0;
          XmTextFieldSetString(plot_stuff->max_min_w, max_min_str);
          s = 0;
          plot_stuff->s = s;
          continue;
        }
        s = plot_stuff->s;
        plot_stuff->segs[s].x1 = prev_x; plot_stuff->segs[s].y1 = prev_y;
        plot_stuff->segs[s].x2 = x; plot_stuff->segs[s].y2 = y;
        s++;
        plot_stuff->s = s;
      }
    }

    if(x >= (Position) r_edge) {
			shiftpackets(plot_stuff);
      plot_stuff->mean_value = 0.0;
			first_entry = 1;
      for(t = 0, s = 0; t <= plot_stuff->index; t++) {
        if(plot_stuff->array[t].y == -1000) {
          continue;
        } else {
					if(first_entry) {
      			plot_stuff->max_value = plot_stuff->long_value[t];
      			plot_stuff->min_value = plot_stuff->long_value[t];
						first_entry = 0;
					}
          ++s;
          plot_stuff->mean_value += (float)plot_stuff->long_value[t];
          if(plot_stuff->max_value <= plot_stuff->long_value[t])
            plot_stuff->max_value = plot_stuff->long_value[t];
          if(plot_stuff->min_value >= plot_stuff->long_value[t])
            plot_stuff->min_value = plot_stuff->long_value[t];
        }
      }
      fmax = (float) plot_stuff->max_value;
      fmin = (float) plot_stuff->min_value;
      plot_stuff->mean_value /= s;
      
      if(fmax < 0)
        fmax = -fmax;
      if(fmin < 0)
        fmin = -fmin;
      if(fmin > fmax)
        fmax = fmin;
      ytop = fmax;
      ybot = -fmax;
      ytop += 0.5;
      ybot -= 0.5;
      center = (ybot + ytop) * 0.5;
      plot_stuff->ytp = center - (center - ytop)/ygain;
      plot_stuff->scale = (float) plot_stuff->height/(ybot - ytop); 
      plot_stuff->scale *= ygain;
      XFillRectangle (XtDisplay (plot_stuff->drawing_a), plot_stuff->pixmap, gc_clear, 0, 0, plot_stuff->width, plot_stuff->height);
      gap = 0;
      for(t = 0; t <= plot_stuff->index; t++) {
        if(plot_stuff->array[t].y == -1000) {
          gap = 0;
        } else if(!gap) {
          gap = 1;
          plot_stuff->array[t].y = (short)  (plot_stuff->scale * ((float)plot_stuff->long_value[t] - plot_stuff->mean_value - plot_stuff->ytp));
          XDrawPoint(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, (Position)plot_stuff->array[t].x, (Position)plot_stuff->array[t].y);
        } else {
          plot_stuff->array[t-1].y = (short)  (plot_stuff->scale * ((float)plot_stuff->long_value[t-1] - plot_stuff->mean_value - plot_stuff->ytp));
          plot_stuff->array[t].y = (short)  (plot_stuff->scale * ((float)plot_stuff->long_value[t] - plot_stuff->mean_value - plot_stuff->ytp));
          XDrawLine(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, (Position)plot_stuff->array[t-1].x, (Position)plot_stuff->array[t-1].y, (Position)plot_stuff->array[t].x, (Position)plot_stuff->array[t].y);
        }
      }
			if(plot_stuff->phases.p) {
		 		XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)plot_stuff->phases.p_x, (Position)0, (Position)plot_stuff->phases.p_x, (Position)plot_stuff->height);
			 }
			if(plot_stuff->phases.s) {
		 		XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)plot_stuff->phases.s_x, (Position)0, (Position)plot_stuff->phases.s_x, (Position)plot_stuff->height);
			 }
      XCopyArea (XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, XtWindow(plot_stuff->drawing_a), gc,
        0, 0, plot_stuff->width, plot_stuff->height, 0, 0);
      prev_y = (Position)  (plot_stuff->scale * ((float)prev_value - plot_stuff->mean_value - plot_stuff->ytp));
      y = (Position)  (plot_stuff->scale * ((float)curr_value - plot_stuff->mean_value - plot_stuff->ytp));
      XDrawLine(XtDisplay(plot_stuff->drawing_a), XtWindow(plot_stuff->drawing_a), gc, prev_x, prev_y, x, y);
      XDrawLine(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, prev_x, prev_y, x, y);
			beg = plot_stuff->end - (double)(x*plot_stuff->decim*plot_stuff->sint);
			plot_stuff->beg = beg;
   	 string[0] = 0;
   	 sprintf(string, "%s",util_dttostr(beg,0));
   	 string[strlen(string)] = 0;
   	 XmTextFieldSetString(plot_stuff->beg_time_w, string);XFlush(XtDisplay(plot_stuff->beg_time_w));
  	 string[0] = 0;
     sprintf(string, "%s", util_dttostr(end,0));
   	 string[strlen(string)] = 0;
   	 XmTextFieldSetString(plot_stuff->end_time_w, string);
      max_min_str[0] = 0;
      sprintf(max_min_str, "MAX: %10ld, MIN: %10ld, ", plot_stuff->max_value, plot_stuff->min_value);
      max_min_str[strlen(max_min_str)] = 0;
      XmTextFieldSetString(plot_stuff->max_min_w, max_min_str);
        
      plot_stuff->to_save = 1;
    } else if(plot_stuff->s > 0) {
      s = plot_stuff->s;
      XDrawSegments(XtDisplay(plot_stuff->drawing_a), XtWindow(plot_stuff->drawing_a), gc, plot_stuff->segs, s);
      XDrawSegments(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, plot_stuff->segs, s);
      string[0] = 0;
      sprintf(string, "%s", util_dttostr(end,0));
      string[strlen(string)] = 0;
      XmTextFieldSetString(plot_stuff->end_time_w, string);
      s = 0;
      plot_stuff->s = s;
      max_min_str[0] = 0;
      sprintf(max_min_str, "MAX: %10ld, MIN: %10ld, ", plot_stuff->max_value, plot_stuff->min_value);
      max_min_str[strlen(max_min_str)] = 0;
      XmTextFieldSetString(plot_stuff->max_min_w, max_min_str);
    }
    XFlush(XtDisplay(plot_stuff->drawing_a));
  } else if(sys_stuff->status == 3) {
    sys_stuff->time = time(NULL);
    return;
  } else if(sys_stuff->status == NRTS_ERROR) {
		if(sys_stuff->connected) {
			util_log(1, "NRTS_ERROR: removing input handler and close connection for %s", sys_stuff->info.input);
   		XtRemoveInput(sys_stuff->InputId);
			Nrts_closetap(p);
		}
		sys_stuff->pause = 1;
		sys_stuff->connected = 0;
		if(userinfo->nsys == 1) {
			widget = gather_stuff->menu_stuff.file_menu_btns[0];
			XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.file_menu_btns[1];
			XtSetSensitive(widget, False);
		}
    return;
  } else if(sys_stuff->status == 4) {
		if(sys_stuff->connected) {
			util_log(1, "removing input handler and close connection for %s", sys_stuff->info.input);
   		XtRemoveInput(sys_stuff->InputId);
			Nrts_closetap(p);
		}
		sys_stuff->pause = 1;
		sys_stuff->connected = 0;
		if(userinfo->nsys == 1) {
			widget = gather_stuff->menu_stuff.file_menu_btns[0];
			XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.file_menu_btns[1];
			XtSetSensitive(widget, False);
		}
    return;
  } else if(sys_stuff->status == NRTS_EOF) {
		if(sys_stuff->connected) {
			util_log(1, "removing input handler and close connection for %s", sys_stuff->info.input);
   		XtRemoveInput(sys_stuff->InputId);
			Nrts_closetap(p);
		}
		sys_stuff->pause = 1;
		sys_stuff->connected = 0;
		if(userinfo->nsys == 1) {
			widget = gather_stuff->menu_stuff.file_menu_btns[0];
			XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.file_menu_btns[1];
			XtSetSensitive(widget, False);
		}
    return;
  } else {
		util_log(1, "%s status %d", sys_stuff->info.input, sys_stuff->status);
		return;
	}

}

Boolean CheckForInterrupt(gather_stuff)
GatherStuff *gather_stuff;
{
Widget main_w = gather_stuff->menu_stuff.main_w;
Display *dpy = XtDisplay(main_w);
Widget file_menu = gather_stuff->menu_stuff.file_menu;
Window win = XtWindow(main_w);
XEvent event;

	XFlush(dpy);

	while(XCheckMaskEvent(dpy, 
				ButtonPressMask | ButtonReleaseMask , &event)) {
		if(event.xany.window == win) {
			XtDispatchEvent(&event);
			return True;
		} else {
		/*
			XBell(dpy, 50);
		*/
			return False;
		}
	}

}

/* Revision History
 *
 * $Log: draw_cb.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:14  dec
 * import existing IDA/NRTS sources
 *
 */
