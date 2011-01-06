#pragma ident "$Id: ReleaseNotes.c,v 1.8 2008/04/03 17:24:33 dechavez Exp $"

char *VersionIdentString = "Release 1.6.1";

/* Release notes

1.6.1  04/03/2008
       Fixed core dump on -t in the presence of IDA10.5 (-t still not
       supported for 10.5 packets, they just get ignored)

1.6.0  04/02/2008
       IDA10.5 support (raw and -v options only)

1.5.2  03/06/2006
       Added -h (help) option.

1.5.1  01/04/2006
       Add multiple digitizer/same stream name support to time tear checker

1.5.0  12/21/2006
       IDA10.4 support

1.4.4  11/13/2006
       Added -t (time tag) and -noseqno options, moved print fuctions 
       to ida10 library

1.4.3  08/14/2006
       Relink with libida10 2.5.0 for 10.3 (OFIS) support

1.4.2  10/06/2005
       Detect and correct for faulty nbytes in 10.2 CF records.
       Terse listing includes raw packet format

1.4.1  09/30/2005
       Added -v for full output, default to common header only

1.4.0  08/25/2005
       IDA10.2 support

1.3.0  12/09/2004 (cvs rtag ida10_1_3_0 ida10)
       added ttag=path option for dumping time tag information

1.2.1  04/29/2002 (cvs rtag ida10_1_2_1 ida10)
       use new ida10TStoString to generate TS summary

1.2.0  03/15/2002 (cvs rtag ida10_1_2_0 ida10)
       support IDA10.x (and hence transparent buffer lengths)

1.1.0  09/06/2001 (cvs rtag ida10_1_1_0bis ida10)
       allow user to specify alternate buffer lengths

1.0.0  04/02/2001 (cvs rtag ida10_1_0_0 ida10)
       created
*/
