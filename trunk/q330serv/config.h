/*   Seneca Configuration definitions
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
    0 2006-10-01 rdr Created
*/
#ifndef config_h
#define config_h
#define VER_CONFIG 2

#ifndef libtypes_h
#include "libtypes.h"
#endif
#ifndef libclient_h
#include "libclient.h"
#endif

extern void getline (string *s) ;
extern void load_configuration (const char *configfile) ;
extern void save_configuration (void) ;
extern void show_create_pars (void) ;
extern void show_idle_menu (void) ;
extern void show_register_pars (void) ;
extern void show_run_menu (void) ;
extern void show_file_menu (void) ;
extern void get_t64 (string *prompt, t64 *result) ;
extern word get_word (string *prompt) ;
extern integer get_integer (string *prompt) ;
extern longword get_ip_address (string *prompt) ;
extern char *get_string (string *prompt, integer maxlth, string *result) ;
extern word get_verbosity (void) ;
extern boolean get_yesno (string *prompt, boolean default_yes) ;
extern integer get_frequency (string *prompt) ;
extern char *uppercase (pchar s) ;

#ifndef OMIT_SERIAL
extern enum thost_mode get_hostmode (void) ;
#endif

extern word get_timeout (string *prompt) ;
extern word get_onesec_filter (void) ;

#ifndef OMIT_SEED
extern word get_seed_filter (string *prompt) ;
#endif

#endif
