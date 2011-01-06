c @(#)cvtrc.inc	1.1 9/12/95
c   Format conversion header information structures.
c
      parameter(mxzr=20,mxpl=40)
      structure/rawtim/
         integer*4 iy				! year
         integer*4 jd				! day of year
         integer*4 ih				! hour
         integer*4 im				! minute
         real*4 sec				! second
      end structure
      structure/cvtcal/
         character*5 scode			! station code
         character*3 chn			! channel code
         character*2 comp			! component code
         character*6 inst			! instrument type
         record/rawtim/btim			! response valid begin time
         real*4 ctpu				! constant
         integer*4 nz				! number of zeros
         complex z(mxzr)			! zeros
         integer*4 np				! number of poles
         complex p(mxpl)			! poles
      end structure
c
      structure/cvtwid/
         character*5 scode			! station code
         character*3 chn			! channel code
         character*2 comp			! component code
         character*6 inst			! instrument type
         record/rawtim/stim			! start time of series
         integer*4 npts				! number of samples
         real*4 rate				! sample rate (Hz)
         real*4 gain				! gain (nm/s**iunits/ct)
         integer*4 iunits			! units code (0=d,1=v,2=a)
         real*4 tref				! gain reference period
         real*4 slat				! station latitude (deg)
         real*4 slon				! station longitude (deg)
         real*4 elev				! station elevation (m)
         real*4 sdep				! station burial depth (m)
         real*4 caz				! component azimuth (deg)
         real*4 cpl				! component plunge (deg)
      end structure

c Revision History
c
c $Log: cvtrc.inc,v $
c Revision 1.1.1.1  2000/02/08 20:20:09  dec
c import existing IDA/NRTS sources
c
