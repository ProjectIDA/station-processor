#pragma ident "$Id: ReleaseNotes.c,v 1.15 2009/02/26 22:47:02 dechavez Exp $"

char *VersionIdentString = "Release 2.5.0";

/* Release notes

2.5.0  02/26/2009
       Added tee option

2.4.0  02/23/2009
       Replaced -v with more elaborate print flags
       
2.3.0  10/07/2008
       Added ida10 option

2.2.1  03/05/2007
       Relink with libqdp 1.2.0 for CNP316 (aux packet) support

2.2.0  12/20/2007
       Decode C1_STAT packets

2.1.4  06/28/2007
       Fixed decompression error (libqdp 1.0.2), improved help message

2.1.3  05/11/2007
       Relink with libqdp with compiled root time tag instead of macro

2.1.2  01/31/2007
       Relink with libqdp 0.9.14 to fix decompression problem

2.1.1  12/21/2006
       Changes to accomodate reworked structure fields

2.1.0  12/13/2006
       Added meta= option for specifying metadata directory.  Load and use
       metadata to preinit the logical channel queues, if available.

2.0.0  12/06/2006
       Use QDPLUS logical channel queues, much print stuff moved off to library

1.0.2  11/13/2006
       Added hex dumps of compressed DT_DATA blockettes

1.0.1  10/13/2006
       Preliminary DT_DATA support

1.0.0  07/06/2006
       Initial release

*/
