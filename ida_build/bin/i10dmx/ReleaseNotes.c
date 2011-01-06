#pragma ident "$Id: ReleaseNotes.c,v 1.21 2008/08/20 18:30:01 dechavez Exp $"

char *VersionIdentString = "Release 2.3.1";

/*

i10dmx Release Notes

2.3.1  08/20/2008
       Force new wfdisc record each time clock first sees a valid time string (this
       addresses the problem of bogus 1999 stamped data following femtometer reboots).

2.3.0  04/02/2008
       IDA10.5 support

2.2.6  09/26/2007
       Fixed shared detector channel mixing bug
       Fixed spurious time tears bug when multi-station file has duplicate channel names
       on different stations.

2.2.5  09/25/2007
       Added detect option for testing Q330 detector code

2.2.2  01/11/2007
       Relink with updated isidb libraries

2.2.1  01/04/2007
       Use new incerr header struct to decide to neglect a time tear or not

2.2.0  12/21/2006
       Initial 10.4 support.  Warn about missing database.
       Don't generate new wfdisc records if errors are less than one sample

2.1.2  11/13/2006
       Final 10.3 support, use ida10 library print functions for diagnostic options

2.1.1  08/17/2006
       further 10.3 support

2.1.0  08/14/2006
       IDA 10.3 (OFIS) support

2.0.0  02/08/2006
       libisidb database support

1.5.2  10/11/2005
       Generate descriptor file

1.5.1  08/18/2005
       IDA 10.2 support

1.5.0  05/13/2005 (cvs rtag i10dmx_1_5_0 i10dmx)
       Rework to use BufferedStream I/O 

1.4.2  10/22/2004 (cvs rtag i10dmx_1_4_2 i10dmx)
       Don't abort on incomprehensible records

1.4.1  11/21/2003 (cvs rtag i10dmx_1_4_1 i10dmx)
       Removed Sigfunc casts

1.4.0  10/16/2003 (cvs rtag i10dmx_1_4_0 i10dmx)
       Added (untested) support for types other than INT32

1.3.4  05/13/2003 (cvs rtag i10dmx_1_3_4 i10dmx)
       Replace spaces in station and channel names with blanks.
       Fixed help message to remove reference to abandoned buflen argument (since 1.3.0)

1.3.3  05/05/2003 (cvs rtag i10dmx_1_3_3 i10dmx)
       Increased IDA10_MAXSTREAMS to 128

1.3.2  05/15/2002 (cvs rtag i10dmx_1_3_2 i10dmx)
       Changed numeric decomposition of ttag status to interpreted
       yes/no strings for easier reading, compute and print offset
       change regardless of validity of time stamp

1.3.1  05/15/2002 (cvs rtag i10dmx_1_3_1 i10dmx)
       Print record number, stream name, offset difference and comments
       when printing time tags

1.3.0  03/15/2002 (cvs rtag i10dmx_1_3_0 i10dmx)
       Use ida10ReadRecord() to support all IDA10.x formats, and
       hence transparent variable buffer lengths

1.2.2  01/25/2002 (cvs rtag i10dmx_1_2_2 i10dmx)
       Tolerate stream sample interval changes.
       Recognize, but ignore, CF records.

1.2.1  12/20/2001 (cvs rtag i10dmx_1_2_1 i10dmx)
       Fixed error printing size of data gaps in samples.

1.2.0  09/08/2001 (cvs rtag i10dmx_1_2_0 i10dmx)
       Support arbitrary (fixed) length records via buflen= argument

1.1.1  11/06/2000 (cvs rtag i10dmx_1_1_1 i10dmx)
       Added support for ISP (I~I) log records.

1.1.0  02/17/2000 (cvs rtag i10dmx_1_1_0 i10dmx)
       Initial release.

*/
