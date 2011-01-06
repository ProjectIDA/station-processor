#pragma ident "$Id: ReleaseNotes.c,v 1.1 2008/09/11 16:41:59 dechavez Exp $"

char *VersionIdentString = "Release 3.1.0";

/* Release Notes

3.1.0  09/11/2008
       Fixed infinite loop bug in main loop, linked with libnrts 4.9.1 to
       remove strict time check (suspect bit ignored)
       Started using ReleaseNotes.c instead of VERSION constant in nrts_alpha.h

3.0.0  02/06/2006
       mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)

2.12.1 09/12/2005
       Use IDA_BUFSIZ to allocate read buffer instead of (no longer existant) sys raw len

2.12.0 04/26/2004
       MySQL support added (via dbspec instead of dbdir)

2.11.0 03/13/2003 (cvs rtag nrts_alpha_2_11_0 nrts_alpha)
       map 5 char names to 3 if -sc option NOT given, change
       +/-uc, +/-lc options to +/-suc and +/-slc (for site name)
       and added +/-cuc and +/-luc (for channel names)

2.10.5 02/08/2001 (cvs rtag nrts_alpha_2_10_5 nrts_alpha)
       Fixed bug in mapping 5 char chan names to 2 char

2.10.4 04/23/2001 (cvs rtag nrts_alpha_2_10_4 nrts_alpha)
       Minor changes in how next time is forced

2.10.3 01/31/2001 (cvs rtag nrts_alpha_2_10_3 nrts_alpha)
       Fixed 6 year old(!) bug in status index in main.c

2.10.2 01/11/2001 (cvs rtag nrts_alpha_2_10_2 nrts_alpha)
       Add sname=string option to allow the user to force the output
       station name.

2.10.1 03/08/2000 (cvs rtag nrts_alpha_2_10_1 nrts_alpha)
       Started sane version numbering and release notes after 9 years!
       Luna2 and Lynks authentication supported, Makefile set to
       include neither.
 */
