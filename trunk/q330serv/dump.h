/*   Seneca Operating Status Dump definitions
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
    0 2006-10-03 rdr Created
*/
#ifndef dump_h
#define dump_h
#define VER_DUMP 4

#ifndef libtypes_h
#include "libtypes.h"
#endif
#ifndef libclient_h
#include "libclient.h"
#endif
#ifndef globals_h
#include "globals.h"
#endif

extern void show_opstat (void) ;
extern void show_modules (void) ;

#endif
