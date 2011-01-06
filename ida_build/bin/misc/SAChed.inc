c $Id: SAChed.inc,v 1.1.1.1 2000/02/08 20:20:08 dec Exp $
c   SAC binary header:
      structure/sached/
         real*4 delta			! nominal dt (s)
         real*4 depmin			! min(A(t))
         real*4 depmax			! max(A(t))
         real*4 scale			! scale factor (not implemented)
         real*4 odelta			! actual dt (s)
         real*4 b			! t0 (s)
         real*4 e			! tn = (npts-1)*dt+t0
         real*4 o			! event origin time (s) (wrt reference)
         real*4 a			! first arrival time (s) (wrt reference)
         real*4 int1			! internal
         real*4 t(10)			! user picks (s) (wrt reference)
         real*4 f			! end-of-event time (s) (wrt reference)
         real*4 resp(10)		! instrument response parameters
         real*4 stla			! station latitude (deg)
         real*4 stlo			! station longitude (deg)
         real*4 stel			! station elevation (m)
         real*4 stdp			! instrument depth (m)
         real*4 evla			! event latitude (deg)
         real*4 evlo			! event longitude (deg)
         real*4 evel			! event elevation (m) ???
         real*4 evdp			! event depth (m)
         real*4 unu1			! unused
         real*4 user(10)		! user storage
         real*4 dist			! event-station distance (km)
         real*4 az			! station azimuth (deg)
         real*4 baz			! event back azimuth (deg)
         real*4 gcarc			! event-station distance (deg)
         real*4 int2(2)			! internal
         real*4 depmen			! mean(A(t))
         real*4 cmpaz			! component azimuth (deg)
         real*4 cmpinc			! component incident angle (deg)
         real*4 unu2(11)		! unused
         integer*4 nzyear		! year (UTC time corresponding to t=0 s)
         integer*4 nzjday		! Julian day
         integer*4 nzhour		! hour
         integer*4 nzmin		! minute
         integer*4 nzsec		! second
         integer*4 nzmsec		! millisecond
         integer*4 nvhdr		! version
         integer*4 int3(2)		! internal
         integer*4 npts			! number of samples
         integer*4 int4(2)		! internal
         integer*4 unu3(3)		! unused
         integer*4 iftype		! file type code
         integer*4 idep			! units code (for A(t))
         integer*4 iztype		! time reference code
         integer*4 unu4			! unused
         integer*4 iinst		! instrument type code
         integer*4 istreg		! station geographic region
         integer*4 ievreg		! event geographic region
         integer*4 ievtyp		! event type code
         integer*4 iqual		! data quality code
         integer*4 isynth		! synthetic data flag
         integer*4 unu5(10)		! unused
         logical*4 leven		! true for equal spaced data
         logical*4 lpspol		! true for correct polarity
         logical*4 lovrok		! true if OK to overwrite this file
         logical*4 lcalda		! true if DIST, AZ, BAZ, GCARC needed
         logical*4 unu6			! unused
         character*8 kstnm		! station name
         character*16 kevnm		! event name
         character*8 khole		! (nuclear shot) hole ID
         character*8 ko			! event origin time ID
         character*8 ka			! first arrival time ID
         character*8 kt(10)		! user pick ID's
         character*8 kf			! end-of-event time ID
         character*8 kuser(3)		! user storage
         character*8 kcmpnm		! component name
         character*8 knetwk		! network name
         character*8 kdatrd		! SAC access date
         character*8 kinst		! instrument name
      end structure
c
c   SAC binary header with initialization:
      structure/sacini/
         real*4 delta/-12345./		! nominal dt (s)
         real*4 depmin/-12345./		! min(A(t))
         real*4 depmax/-12345./		! max(A(t))
         real*4 scale/-12345./		! scale factor (not implemented)
         real*4 odelta/-12345./		! actual dt (s)
         real*4 b/-12345./		! t0 (s)
         real*4 e/-12345./		! tn = (npts-1)*dt+t0
         real*4 o/-12345./		! event origin time (s) (wrt reference)
         real*4 a/-12345./		! first arrival time (s) (wrt reference)
         real*4 int1/-12345./		! internal
         real*4 t(10)/10*-12345./	! user picks (s) (wrt reference)
         real*4 f/-12345./		! end-of-event time (s) (wrt reference)
         real*4 resp(10)/10*-12345./	! instrument response parameters
         real*4 stla/-12345./		! station latitude (deg)
         real*4 stlo/-12345./		! station longitude (deg)
         real*4 stel/-12345./		! station elevation (m)
         real*4 stdp/-12345./		! instrument depth (m)
         real*4 evla/-12345./		! event latitude (deg)
         real*4 evlo/-12345./		! event longitude (deg)
         real*4 evel/-12345./		! event elevation (m) ???
         real*4 evdp/-12345./		! event depth (m)
         real*4 unu1/-12345./		! unused
         real*4 user(10)/10*-12345./	! user storage
         real*4 dist/-12345./		! event-station distance (km)
         real*4 az/-12345./		! station azimuth (deg)
         real*4 baz/-12345./		! event back azimuth (deg)
         real*4 gcarc/-12345./		! event-station distance (deg)
         real*4 int2(2)/2*-12345./	! internal
         real*4 depmen/-12345./		! mean(A(t))
         real*4 cmpaz/-12345./		! component azimuth (deg)
         real*4 cmpinc/-12345./		! component incident angle (deg)
         real*4 unu2(11)/11*-12345./	! unused
         integer*4 nzyear/-12345/	! year (UTC time corresponding to t=0 s)
         integer*4 nzjday/-12345/	! Julian day
         integer*4 nzhour/-12345/	! hour
         integer*4 nzmin/-12345/	! minute
         integer*4 nzsec/-12345/	! second
         integer*4 nzmsec/-12345/	! millisecond
         integer*4 nvhdr/-12345/	! version
         integer*4 int3(2)/2*-12345/	! internal
         integer*4 npts/-12345/		! number of samples
         integer*4 int4(2)/2*-12345/	! internal
         integer*4 unu3(3)/3*-12345/	! unused
         integer*4 iftype/-12345/	! file type code
         integer*4 idep/-12345/		! units code (for A(t))
         integer*4 iztype/-12345/	! time reference code
         integer*4 unu4/-12345/		! unused
         integer*4 iinst/-12345/	! instrument type code
         integer*4 istreg/-12345/	! station geographic region
         integer*4 ievreg/-12345/	! event geographic region
         integer*4 ievtyp/-12345/	! event type code
         integer*4 iqual/-12345/	! data quality code
         integer*4 isynth/-12345/	! synthetic data flag
         integer*4 unu5(10)/10*-12345/	! unused
         logical*4 leven/.false./	! true for equal spaced data
         logical*4 lpspol/.false./	! true for correct polarity
         logical*4 lovrok/.false./	! true if OK to overwrite this file
         logical*4 lcalda/.false./	! true if DIST, AZ, BAZ, GCARC needed
         logical*4 unu6/.false./	! unused
         character*8 kstnm/'-12345'/	! station name
         character*16 kevnm/'-12345'/	! event name
         character*8 khole/'-12345'/	! (nuclear shot) hole ID
         character*8 ko/'-12345'/	! event origin time ID
         character*8 ka/'-12345'/	! first arrival time ID
         character*8 kt(10)/10*'-12345'/	! user pick ID's
         character*8 kf/'-12345'/	! end-of-event time ID
         character*8 kuser(3)/3*'-12345'/	! user storage
         character*8 kcmpnm/'-12345'/	! component name
         character*8 knetwk/'-12345'/	! network name
         character*8 kdatrd/'-12345'/	! SAC access date
         character*8 kinst/'-12345'/	! instrument name
      end structure

c Revision History
c
c $Log: SAChed.inc,v $
c Revision 1.1.1.1  2000/02/08 20:20:08  dec
c import existing IDA/NRTS sources
c
