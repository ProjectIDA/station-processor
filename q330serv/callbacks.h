/*   Seneca Callback related definitions
     Copyright 2006 Certified Software Corporation

    This file is part of Seneca

    Seneca is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Seneca is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Seneca; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Edit History:
   Ed Date       By  Changes
   -- ---------- --- ---------------------------------------------------
    0 2006-10-02 rdr Created
*/
#ifndef callbacks_h
#define callbacks_h
#define VER_CALLBACKS 6

#ifndef libtypes_h
#include "libtypes.h"
#endif
#ifndef libclient_h
#include "libclient.h"
#endif
#ifndef globals_h
#include "globals.h"
#endif
#ifndef libpoc_h
#include "libpoc.h"
#endif

extern void sen_state_callback (pointer p) ;
extern void msgs_callback (pointer p) ;
extern void sen_file_callback (pointer p) ;
extern void dump_messages (void) ;
extern void onesec_callback (pointer p) ;

#ifndef OMIT_SEED
extern void mini_callback (pointer p) ;
extern void amini_callback (pointer p) ;
extern void show_detectors (void) ;
extern void show_lcqs (void) ;
#endif

#ifndef OMIT_NETWORK
extern void poc_handler (enum tpocstate pocstate, tpoc_recvd *poc_recv) ;
#endif
extern void send_tunneled_request (void) ;
extern void arp_status (void) ;
extern void ep_status (void) ;

#endif
