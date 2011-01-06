#pragma ident "$Id: ReleaseNotes.c,v 1.18 2009/01/28 05:35:47 dechavez Exp $"

char *VersionIdentString = "Release 1.8.0";

/* Release notes

1.8.0  01/26/2009
       Don't create QDP state file, fixed annoying "No such file" message 
       when creating new QDP loop w/o pre-existing metadir

1.7.3  02/20/2008
       Fixed bug where type was required even if explicit sizes were given

1.7.2  10/05/2007
        Link with libdbio 3.2.6, libdbio 1.3.2 and libisidl 1.10.1 for
        support for NRTS_HOME environment variable

1.7.1  09/07/2007
       Create QDP state file

1.7.0  05/03/2007
       Added LISS defaults (type=liss)

1.6.2  04/18/2007
       Note state file, if present

1.6.1  01/11/2007
       Use isidlX function names

1.6.0  12/12/2006
       Added generation of metadata subdirectory for QDP disk loops

1.5.0  06/02/2006
       Added support for QDPLUS packets, and now require type= arg
       on the command line to specify what type of disk loop to build

1.4.1  03/15/2006
       Replaced ini=file command line option with db=spec for global init

1.4.0  02/08/2006
       link with libida 4.0.0, isidb 1.0.0 and neighbors

1.3.1  09/14/2005
       don't print any warning messages related to bad or missing NRTS disk loop

1.3.0  07/25/2005
       added ISI_GLOB support, open after to create to check NRTS status

1.2.0  06/29/2005
       added time stamps to disk loop

1.1.0  06/24/2005
       build new version of disk loop, with "hide" parameter

1.0.1  06/02/2005
       added perm argument to isiOpenDiskLoop call

1.0.0  05/24/2005 (cvs rtag isimkdl_1_0_0 isimkdl)
       Initial release

*/
