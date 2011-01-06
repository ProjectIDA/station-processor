c $Id: cvtgse.f,v 1.2 2001/12/20 18:28:52 dec Exp $
c
c   CVTGSE reads a mail message received from the NEIC AutoDRM and looks 
c   for the results of CALIB and WAVEF commands.  Calibrations are copied 
c   into GSE.scode.chn.rsp files for later use.  If the a calibration for 
c   the same station is found in a later message, it will overwrite the 
c   earlier version.  When a waveform is found, the corresponding 
c   calibration file (if any) is read and the combined results are used 
c   to write the waveform out in either RETRV or SAC format.  All 
c   calibrations and waveforms in a mail message are handled in a single 
c   run.  If the conversion is to RETRV, all waveforms from the same 
c   channel type are written into the same file (named stage.chn, where 
c   chn is the channel code converted to lower case).  If the conversion 
c   is to SAC, the response look aside file named SAC.scode.chn.rsp is 
c   written as well as the waveform in a file named 
c   Syyyymmdd.hhmmss.scode.chn, where the station and channel codes are 
c   uppercase (with blanks replaced by underscores).  It is also possible 
c   to convert to SAC ASCII or back to the GSE format in which case all 
c   conventions are the same except that the waveform files begin with 
c   'C' for SAC ASCII and 'G' for GSE.  The GSE conversion has the effect 
c   of associating the appropriate response information with the waveform 
c   if it was gotten in an earlier request and of converting the CMP6/2 
c   format to INTV/0.  The command syntax is:
c
c	cvtgse fmt_flag mail_file
c
c   the fmt_flag may be r for RETRV, s for SAC binary, and c for SAC 
c   ASCII, and g for GSE.  If no fmt_flag is given, it defaults to r.  
c   All resulting files are created in the current working directory.  
c   Prior to compiling this routine, you will need to extract the two 
c   include files at the end of this distribution file (cvtrc.inc and 
c   SAChed.inc).  Notice that they are marked with '!'s.  The include 
c   files need to be in the same directory as cvtgse.f when it is 
c   compiled.
c                                                       Ray Buland
c
      program cvtgse
c
c   Convert mail messages containing calibrations and/or waveforms in GSE 
c   format into either RETRV or SAC format.
c
      parameter(nser=250000,nlin=(2*nser+79)/80,nchr=80*nlin)
      character*1 fmt
      character*3 chn
      character*4 code
      character*5 scode
      character*17 rsfl
      character*60 flnm
      character*80 cbuf(nlin)
      character*132 line
      include 'cvtrc.inc'
      record/cvtcal/cal
      record/cvtwid/wid
      dimension ia(nser),a(nser)
      equivalence (ia,a)
      data in,nrsp,nout/1,2,3/,fmt/'r'/
c
c   Parse out and validate the command line arguments.
      call getarg(1,flnm)
      if(flnm.eq.' ') then
         write(*,100)
 100     format('usage:  cvtgse fmt_flag mail_file')
         call exit(1)
      endif
      if(flnm(2:).eq.' ') then
         fmt=flnm(1:1)
         if(fmt.ne.'s'.and.fmt.ne.'r'.and.fmt.ne.'c'.and.
     1    fmt.ne.'g') then
            write(*,113)
 113        format('Cvtgse:  the fmt_flag must be r (RETRV, ',
     1       'default), g (GSE), s (SAC binary), or c (SAC ASCII).')
            call exit(1)
         endif
         call getarg(2,flnm)
         if(flnm.eq.' ') then
            write(*,100)
            call exit(1)
         endif
      endif
c
c   Open the mail file.
      open(in,file=flnm,access='sequential',
     1 form='formatted',status='old',iostat=ios)
      if(ios.ne.0) then
         write(*,101)ios
 101     format('Cvtgse:  unable to open mail file (',i3,').')
         call exit(1)
      endif
c
c   Read the mail message looking for calibration and waveform headers.
 1    read(in,102,iostat=ios)line
 102  format(a)
      if(ios)13,2,14
c
c   If we get a DAT1 or CHK1 without a WID1 line, it is an error.
 2    if((line(1:3).eq.'DAT'.or.line(1:3).eq.'CHK').and.
     1 (line(4:4).eq.'1'.or.line(4:4).eq.'2')) then
         write(*,103)line(1:4)
 103     format('Cvtgse:  ',a,' found without waveform header.  ',
     1    'Data skipped.')
      endif
c
c   Handle CALs by simply copying the GSE formatted response to a response 
c   file.  This is necessary because the waveform may not be accompanied 
c   by a response and even if it is, they won't be together.
      if(line(1:4).eq.'CAL1') then
c   Read the station/channel codes in order to make up the name of the 
c   response file.
         read(line,104,err=15)scode,chn
 104     format(5x,a,2x,a)
         rsfl='GSE.'//scode//'.'//chn//'.rsp'
         do j=5,13
            if(rsfl(j:j).eq.' ') rsfl(j:j)='_'
         enddo
c   Open the response file and copy the response into it.
         open(nrsp,file=rsfl,access='sequential',form='formatted',
     1    status='unknown',iostat=ios)
         if(ios.ne.0) then
            write(*,114)scode(1:ntrbl(scode)),chn,ios
 114        format('Cvtgse:  unable to open a response file for ',a,
     1       '/',a,' (',i3,').')
            go to 1
         endif
         write(*,102)line
         write(nrsp,102)line
 4       read(in,102,iostat=ios)line
         if(ios)16,3,14
 3       if(line.eq.' ') then
            close(nrsp)
            go to 1
         else
            write(nrsp,102)line
            go to 4
         endif
      endif
c
c   Process the waveform.
      if(line(1:3).eq.'WID') then
C   Crack The Header.
         if(line(1:4).eq.'WID1') then
c   Do GSE1.0:
            read(line,105,err=15)wid.stim.iy,wid.stim.jd,wid.stim.ih,
     1       wid.stim.im,is,ms,wid.npts,wid.scode,wid.chn,wid.comp,
     2       wid.rate,wid.inst,code,idif
 105        format(6x,i4,4i3,i4,i9,1x,a,2x,a,6x,a,f12.7,1x,a,1x,a,i2)
            write(*,102)line
            wid.stim.sec=is+.001*ms
            read(in,102,iostat=ios)line
            if(ios)16,5,14
 5          read(line,106,err=15)wid.gain,wid.iunits,wid.tref,wid.slat,
     1       wid.slon,wid.elev,wid.sdep,wid.caz
 106        format(f9.6,i1,f7.4,4f10.4,16x,f7.1)
c   GSE1.0 doesn't support the component inclination, so make it up.
            wid.caz=amax1(wid.caz,0.)
            if(wid.chn(3:3).eq.'Z') then
               wid.cpl=-90.
            else
               wid.cpl=0.
            endif
         else if(line(1:4).eq.'WID2') then
c   Do GSE2.0:
            read(line,117,err=15)wid.stim.iy,mn,id,wid.stim.ih,
     1       wid.stim.im,wid.stim.sec,wid.scode,wid.chn,wid.comp,code,
     2       wid.npts,wid.rate,wid.gain,wid.tref,wid.inst,wid.caz,
     3       wid.cpl
 117        format(5x,i4,4(1x,i2),1x,f6.3,1x,a5,1x,a3,1x,a2,3x,a3,i9,
     1        f12.6,e11.2,f8.3,1x,a6,f6.1,f5.1)
            write(*,102)line
            call juldat(wid.stim.iy,mn,id,wid.stim.jd)
            wid.caz=amax1(wid.caz,0.)
            wid.cpl=wid.cpl-90.
            print *,'GSE2.0:  ',wid.stim.iy,mn,id,wid.stim.jd,
     1       wid.stim.ih,wid.stim.im,wid.stim.sec,'|',wid.scode,'|',
     2       wid.chn,'|',wid.comp,'|',code,'|',wid.npts,wid.rate,
     3       wid.gain,wid.tref,'|',wid.inst,'|',wid.caz,wid.cpl
            if(code(3:3).eq.'6') then
               idif=2
            else
               idif=0
            endif
            if((code(1:2).eq.'CM').or.(code(1:2).eq.'AU'.and.
     1       code(3:3).ne.'T')) code='CMP'//code(3:3)
c   Temporary patch for hosed format generation.
            if(code.eq.'CMP2') then
               code='CMP6'
               idif=2
            endif
            print *,'GSE2.0:  new code = ',code
         else
            write(*,115)line(1:4)
 115        format('Cvtgse:  bad waveform header (',a,').')
            go to 1
         endif
c   Skip lines looking for the start of the data.
 7       read(in,102,iostat=ios)line
         if(ios)16,6,14
 6       if(line(1:3).eq.'CHK') then
            write(*,107)line(4:4)
 107        format('Cvtgse:  DAT',a,' omitted.  Data skipped.')
            go to 1
         else if(line(1:3).ne.'DAT') then
            go to 7
         endif
c   We have the start of data.
         nl=0
 9       read(in,102,iostat=ios)line
         if(ios)16,8,14
 8       if(line(1:4).ne.'CHK1'.and.line(1:4).ne.'CHK2') then
c   Collect the data.
            if(nl.eq.nlin) then
               write(*,108)
 108           format('Cvtgse:  data character buffer overflow.')
            endif
            nl=nl+1
            if(nl.le.nlin) cbuf(nl)=line
            go to 9
         else
c   We have the end of data.  Decompress and check the checksum.
            if(line(4:4).eq.'1') then
               read(line,109,err=15)ichk
 109           format(9x,i15)
            else
               read(line,118,err=15)ichk
 118           format(5x,i8)
            endif
            write(*,102)line
            if(code.eq.'CMP6'.or.idif.lt.0.or.idif.gt.2) then
               nc=nser
c   We only support the CMP6 compression for now.
               call dcomp6(nchr,cbuf,nc,ia,ierr)
               if(ierr.lt.0) then
                  write(*,110)
 110              format('Cvtgse:  waveform truncated.')
                  wid.npts=nc
               endif
c   Undo the differencing.
               do j=1,idif
                  call remdif1(ia,nc)
               enddo
c   Compute the checksum.
               jchk=0
               if(line(4:4).eq.'1') then
                  do j=1,nc
                     jchk=jchk+ia(j)
                  enddo
               else if(line(4:4).eq.'2') then
                  do j=1,nc
                     jchk=mod(jchk+mod(ia(j),100000000),100000000)
                  enddo
                  jchk=iabs(jchk)
               else
                 write(*,116)line(4:4)
 116             format('Cvtgse:  CHK',a,' unknown.')
               endif
               if(ichk.ne.jchk) write(*,111)
     1          wid.scode(1:ntrbl(wid.scode)),wid.chn,line(4:4)
 111           format('Cvtgse:  ',a,'/',a,' CHK',a,' mismatch.')
               print *,line(1:4),ichk,jchk
c     write(99,301)scode,chn,(ia(j),j=1,npts)
c301  format('DAT1',1x,a,1x,a/(10i8))
c   Fetch the corresponding response file.  Note that valid intervals are 
c   not yet supported.
               call getcal(nrsp,wid.scode,wid.chn,cal)
c   Handle the integer formats.
               if(fmt.eq.'g') then
c   Convert the data to GSE INTV format if called for.
                  call wgse(nout,wid,cal,jchk,ia)
               else
c   Else, convert the data to floating point for the floating point 
c   formats.
                  do j=1,wid.npts
                    a(j)=ia(j)
                  enddo
c   Write the data out either as SAC or as RETRV format.
                  if(fmt.eq.'s'.or.fmt.eq.'c') then
                     call wsac(nrsp,nout,wid,cal,a,fmt)
                  else
                     call wretrv(wid,cal,a)
                  endif
               endif
            else
               write(*,112)code,idif
 112           format('Cvtgse:  unknown compression (',a,i2,').')
            endif
         endif
      endif
      go to 1
c
 13   write(*,200)
 200  format('EOD')
      go to 17
c
 14   write(*,201)ios
 201  format('Cvtgse:  read error on mail file (',i3,').')
      go to 17
c
 15   write(*,202)line(1:4)
 202  format('Cvtgse:  illegal format in GSE record (',a,').')
      go to 17
c
 16   write(*,203)
 203  format('Cvtgse:  unexpected EOD.')
 17   call cretrv
      close(in)
      call exit(1)
      end
      subroutine getcal(nrsp,scode,chn,cal)
c
c   Getcal finds the appropriate GSE style calibration file and reads it 
c   into the cal structure.  If the response isn't found, getcal returns 
c   appropriate defaults.
c
      character*3 chn
      character*4 code
      character*5 scode
      character*17 rsfl
      character*132 line
      logical exst
      include 'cvtrc.inc'
      record/cvtcal/cal
c
c   Make up the response file name.
      rsfl='GSE.'//scode//'.'//chn//'.rsp'
      do j=5,13
         if(rsfl(j:j).eq.' ') rsfl(j:j)='_'
      enddo
c   See if the response file exists.
      inquire(file=rsfl,exist=exst)
      if(.not.exst) then
         write(*,104)scode(1:ntrbl(scode)),chn
 104     format('Getcal:  ',a,'/',a,' response file not found.')
         go to 18
      endif
c   If so, open it.
      open(nrsp,file=rsfl,access='sequential',form='formatted',
     1    status='old',err=13,iostat=ios)
c
c   Read the response file and this time crack the format.
      read(nrsp,100,err=15,end=16,iostat=ios)cal.scode,cal.chn,
     1 cal.comp,cal.inst,code,cal.btim.iy,mn,id,cal.btim.ih,cal.btim.im
 100  format(5x,a,2x,a,6x,a,1x,a,1x,a,3i2,1x,2i2)
      call juldat(cal.btim.iy,mn,id,cal.btim.jd)
      cal.btim.sec=0.
c   If there are analogue poles and zeros, do them too.  Note that FIR 
c   filters, etc. are not yet supported.
      if(code(1:3).eq.'PAZ') then
         exst=.false.
         read(nrsp,101,err=15,end=16,iostat=ios)cal.np
 101     format(i8)
         do j=1,cal.np
            read(nrsp,102,err=15,end=16,iostat=ios)cal.p(j)
 102        format(2g8.1)
         enddo
         read(nrsp,101,err=15,end=16,iostat=ios)cal.nz
         do j=1,cal.nz
            read(nrsp,102,err=15,end=16,iostat=ios)cal.z(j)
         enddo
         read(nrsp,103,err=15,end=16,iostat=ios)cal.ctpu
 103     format(e10.3)
      else
         go to 14
      endif
      go to 20
c
 13   write(*,105)scode(1:ntrbl(scode)),chn,ios
 105  format('Getcal:  ',a,'/',a,' unable to open response file (',i3,
     1 ').')
      go to 18
c
 14   write(*,106)scode(1:ntrbl(scode)),chn
 106  format('Getcal:  ',a,'/',a,' PAZ section not found.')
      go to 19
c
 15   write(*,107)scode(1:ntrbl(scode)),chn,ios
 107  format('Getcal:  ',a,'/',a,' response file read error (',i3,').')
      go to 17
c
 16   write(*,108)scode(1:ntrbl(scode)),chn
 108  format('Getcal:  ',a,'/',a,' unexpected EOF on response file.')
c
 17   if(.not.exst) go to 19
 18   cal.scode=scode
      cal.chn=chn
      cal.comp=chn(1:1)//chn(3:3)
      cal.inst='-'
      cal.btim.iy=0
      cal.btim.jd=0
      cal.btim.ih=0
      cal.btim.im=0
c
 19   cal.ctpu=0.
      cal.nz=0
      cal.np=0
c
 20   continue
      return
      end
      subroutine wgse(nout,wid,cal,jchk,ia)
      character*16 fmtpnz,fmt
      character*26 flnm
      include 'cvtrc.inc'
      record/cvtcal/cal
      record/cvtwid/wid
      dimension ia(*)
c
c   Make up a file name.
      call gredat(wid.stim.iy,mn,id,wid.stim.jd)
      write(flnm,100)wid.stim.iy,mn,id,wid.stim.ih,wid.stim.im,
     1 int(wid.stim.sec),wid.scode,wid.chn
 100  format('G',i4,2i2.2,'.',3i2.2,'.',a5,'.',a)
      do j=18,22
         if(flnm(j:j).eq.' ') flnm(j:j)='_'
      enddo
c   Open the output file.
      open(nout,file=flnm,access='sequential',form='formatted',
     1 status='new')
c   Write the response.
      call gredat(cal.btim.iy,mnc,idc,cal.btim.jd)
      write(nout,101)cal.scode,cal.chn,cal.comp,cal.inst,cal.btim.iy,
     1 mnc,idc,cal.btim.ih,cal.btim.im
 101  format('CAL1 ' ,a,2x,a,6x,a,1x,a,' PAZ ',3i2.2,1x,2i2.2)
      call juldat(cal.btim.iy,mn,id,cal.btim.jd)
      write(nout,102)cal.np
 102  format(i8)
      do j=1,cal.np
         fmt=fmtpnz(cal.p(j))
         write(nout,103)fmt
 103     format(a)
      enddo
      write(nout,102)cal.nz
      do j=1,cal.nz
         fmt=fmtpnz(cal.z(j))
         write(nout,103)fmt
      enddo
      write(nout,104)cal.ctpu
 104  format(1pe10.3)
c   Write the data header.
      is=wid.stim.sec
      write(nout,105)wid.stim.iy,wid.stim.jd,wid.stim.ih,wid.stim.im,
     1 is,int(1000.*(wid.stim.sec-is)),wid.npts,wid.scode,wid.chn,
     2 wid.comp,wid.rate,wid.inst,wid.gain,wid.iunits,wid.tref,
     3 wid.slat,wid.slon,wid.elev,wid.sdep,wid.caz
 105  format('WID1  ',i4,i3.3,3i3.2,i4.3,i9,1x,a,2x,a,6x,a,f12.7,1x,a,
     1 ' INTV 0'/f9.6,i1,f7.4,4f10.4,'   -1.00   -1.00',f7.1/'DAT1')
c   Write the data.
      write(nout,*)(ia(i),i=1,wid.npts)
      write(nout,106)jchk
 106  format('CHK1',5x,i15)
      close(nout)
      return
      end
      character*16 function fmtpnz(c)
c
c $$$$$ calls only library routines $$$$$
c
c   Fmtpnz formats poles and zeros to fit into the miserly allocation 
c   offered by the GSE format.
c
      character*15 fmtbuf(2)
      character*30 fmtlin
      complex c
      equivalence (fmtlin,fmtbuf)
c
      write(fmtlin,100,iostat=ios)c
 100  format(2f15.6)
c
      do k=1,2
         do i=2,7
            if(fmtbuf(k)(i:i).ne.' '.and.fmtbuf(k)(i:i).ne.'-') go to 2
         enddo
         i=8
 2       j=i-1
         fmtbuf(k)=fmtbuf(k)(j:)
         if(fmtbuf(k)(2:2).eq.'0') fmtbuf(k)(2:)=fmtbuf(k)(3:)
         if(fmtbuf(k)(1:8).eq.'-.000000') fmtbuf(k)(1:1)=' '
      enddo
      fmtpnz=fmtbuf(1)(1:8)//fmtbuf(2)(1:8)
      return
      end
      subroutine wretrv(wid,cal,a)
c
c   Write out a seismogram in RETRV format.
c
      parameter(mxchn=12,nbuf=500,nbuf1=nbuf-1)
      character*3 fchn(mxchn),rchn(mxchn)
      character*4 icod(8),ecod(4)
      logical opn(mxchn)
      include 'cvtrc.inc'
      record/cvtcal/cal
      record/cvtwid/wid
      dimension a(*),lun(mxchn),iflg(3)
      data lun,opn/8,9,10,11,12,13,14,15,16,17,18,19,12*.false./
      data rchn/'BHZ','BHN','BHE','SHZ','SHN','SHE','LHZ','LHN','LHE',
     1 'MHZ','MHN','MHE'/
      data fchn/'bhz','bhn','bhe','shz','shn','she','lhz','lhn','lhe',
     1 'mhz','mhn','mhe'/
      data iflg/1,0,0/,icod,ecod/8*' ','E','N','D',' '/
c
c   Be sure we have some data.
      if(wid.npts.le.0) then
         write(*,100)wid.scode(1:ntrbl(wid.scode)),wid.chn
 100     format('Wretrv:  no data found for ',a,'/',a,'.')
         return
      endif
c
c   Find the right output file.
      do jndx=1,mxchn
         if(wid.chn.eq.rchn(jndx)) go to 1
      enddo
      write(*,101)wid.chn,wid.scode(1:ntrbl(wid.scode))
 101  format('Wretrv:  channel ',a,' not supported (',a,').')
      return
c
c   Open the output file if necessary.
 1    if(.not.opn(jndx)) then
         open(lun(jndx),file='stage.'//fchn(jndx),form='unformatted',
     1    access='sequential',status='unknown',iostat=ios)
         if(ios.ne.0) then
            write(*,102)wid.scode(1:ntrbl(scode)),wid.chn
 102        format('Wretrv:  unable to open RETRV file (',a,'/',a,').')
            return
         endif
         opn(jndx)=.true.
      endif
c
c   Handle some of the peculiarities of RETRV.
      do j=1,4
         icod(j)=wid.scode(j:j)
      enddo
      do j=1,3
         icod(j+4)=wid.chn(j:j)
      enddo
      if(cal.np.le.0) then
         cal.np=cal.np
         cal.p(1)=(0.,0.)
      endif
      if(cal.nz.le.0) then
         cal.nz=cal.nz
         cal.z(1)=(0.,0.)
      endif
c
c   Write the RETRV waveform header.  Note that GSE does things in 
c   nanometers while RETRV expects microns.
      write(lun(jndx))icod,wid.slat,wid.slon,wid.elev,wid.stim.iy,
     1 wid.stim.jd,wid.stim.ih,wid.stim.im,wid.stim.sec,wid.rate,
     2 1e3*cal.ctpu,cal.np,(cal.p(i),i=1,cal.np),cal.nz,
     3 (cal.z(i),i=1,cal.nz),iflg
c
c   Write the data.
      do j=1,wid.npts
         a(j)=amin1(amax1(a(j),-500000000.),500000000.)
      enddo
      do j=1,wid.npts,nbuf
         k=min0(j+nbuf1,wid.npts)
         nn=k-j+1
         write(lun(jndx))nn,(a(i),i=j,k)
      enddo
c   Terminate the time series.
      write(lun(jndx))1,10000000000.
      return
c
c   Entry cretrv terminates and closes all open RETRV files.
c
      entry cretrv
      do jndx=1,mxchn
         if(opn(jndx)) then
c   Write the null header that terminates the RETRV file.
            write(lun(jndx))ecod,ecod,0.,0.,0.,0,0,0,0,0.,0.,0.,1,
     1       (0.,0.),1,(0.,0.),0,0,0
            close(lun(jndx))
         endif
      enddo
      return
      end
      subroutine wsac(nrsp,nout,wid,cal,a,fmt)
c
c   Write out a seismogram in SAC binary format.
c
      character*1 fmt
      character*17 rsfl
      logical bin
      include 'cvtrc.inc'
      record/cvtcal/cal
      record/cvtwid/wid
      dimension a(*)
c
c   Be sure we have some data.
      if(wid.npts.le.0) then
         write(*,100)wid.scode(1:ntrbl(wid.scode)),wid.chn
 100     format('Wsac:  no data found for ',a,'/',a,'.')
         return
      endif
c
c   Make up the response file name.
      rsfl='SAC.'//wid.scode//'.'//wid.chn//'.rsp'
      do j=5,13
         if(rsfl(j:j).eq.' ') rsfl(j:j)='_'
      enddo
c   Open the file and write the response into it.
      open(nrsp,file=rsfl,access='sequential',form='formatted',
     1 status='unknown',iostat=ios)
      if(ios.eq.0) then
         write(nrsp,*)'ZEROS ',cal.nz
         do i=1,cal.nz
            write(nrsp,*)real(cal.z(i)),aimag(cal.z(i))
         enddo
         write(nrsp,*)'POLES ',cal.np
         do i=1,cal.np
            write(nrsp,*)real(cal.p(i)),aimag(cal.p(i))
         enddo
         write(nrsp,*)'CONSTANT ',1e3*cal.ctpu
         close(nrsp)
      else
         write(*,101)wid.scode(1:ntrbl(wid.scode)),wid.chn,ios
 101     format('Wsac:  unable to open a response file for ',a,'/',a,
     1    ' (',i3,').')
      endif
c
c   Set up some SAC dependent stuff.
      dt=1./wid.rate
      depmin=a(1)
      depmax=depmin
      depmen=depmin
      do j=2,wid.npts
         depmin=amin1(depmin,a(j))
         depmax=amax1(depmax,a(j))
         depmen=depmen+a(j)
      enddo
      depmen=depmen/wid.npts
      if(fmt.eq.'c') then
         bin=.false.
      else
         bin=.true.
      endif
c
c   Write out the SAC header.
      call wsachd(nout,wid.scode,wid.chn,dt,wid.npts,wid.slat,
     1 wid.slon,wid.elev,wid.stim.iy,wid.stim.jd,wid.stim.ih,
     2 wid.stim.im,wid.stim.sec,depmin,depmax,depmen,bin)
c
c   Write out the time series.
      call wsacdt(nout,wid.npts,a,bin)
c   Flush the last buffer.
      call wsacdt(nout,0,a,bin)
      return
      end
      subroutine wsachd(nout,scode,chn,dt,npts,slat,slon,elev,iy,jd,
     1 ih,im,sec,depmin,depmax,depmen,bin)
c
c   Wsachd writes in the header portion of a binary SAC file.  
c   Programmed on 11 November 1992 by R. Buland.
c
      character*(*) scode,chn
      character*26 flnm
      logical bin
      common/sacio/irec,lpt,ipt,buf(128,2)
c
c   Implicitly equivalence the data buffer with the header structure 
c   through the frmthd call and extract what is needed.
      call frmthd(buf,scode,chn,dt,npts,slat,slon,elev,iy,jd,ih,im,
     1 sec,depmin,depmax,depmen)
c
      call gredat(iy,mn,id,jd)
      write(flnm,101)iy,mn,id,ih,im,int(sec),scode,chn
 101  format('S',i4,2i2.2,'.',3i2.2,'.',a5,'.',a)
      if(.not.bin) flnm(1:1)='C'
      do j=18,22
         if(flnm(j:j).eq.' ') flnm(j:j)='_'
      enddo
c
c   Write the header (or part of it at least).
      if(bin) then
c   For binary SAC files, write the first physical records out.  The 
c   header occupies the first 158 longwords of the 256 allocated.  The 
c   remainder of the header will be written when the rest of the 
c   second record has been filled with data.
         open(nout,file=flnm,access='direct',form='unformatted',
     1    recl=512,status='new')
         write(nout,rec=1,iostat=ios)(buf(j,1),j=1,128)
         if(ios.ne.0) go to 13
c   Set up buffer pointers so that the data packing will start right.
         irec=1
         lpt=2
         ipt=30
      else
c   For ASCII SAC files, write all of the header.
         open(nout,file=flnm,access='sequential',form='formatted',
     1    status='new')
         call asched(nout,buf,ios)
         if(ios.ne.0) go to 13
c   Set up buffer pointers so that the data packing will start right.
         lpt=1
         ipt=0
      endif
      return
c
 13   write(8,100)ios
 100  format('Wsachd:  write error (',i3,').')
      npts=0
      return
      end
      subroutine frmthd(hed,scode,chn,dt,npts,slat,slon,elev,iy,jd,ih,
     1 im,sec,depmin,depmax,depmen)
c
c   Fsachd formats the header portion of a SAC trace.  Programmed on 
c   11 November 1992 by R. Buland.
c
      character*(*) scode,chn
      include 'SAChed.inc'
      record/sached/hed
      record/sacini/hini
c
      call moveb(632,hini,hed)
c
      hed.delta=dt
      hed.depmin=depmin
      hed.depmax=depmax
      hed.b=0.
      hed.e=dt*(npts-1)
      hed.stla=slat
      hed.stlo=slon
      hed.stel=elev
      hed.depmen=depmen
      if(chn(3:3).eq.'N') then
         hed.cmpaz=0.
         hed.cmpinc=90.
      else if(chn(3:3).eq.'E') then
         hed.cmpaz=90.
         hed.cmpinc=90.
      else if(chn(3:3).eq.'Z') then
         hed.cmpaz=0.
         hed.cmpinc=0.
      else
         hed.cmpaz=-12345.
         hed.cmpinc=-12345.
      endif
c
      hed.nzyear=iy
      hed.nzjday=jd
      hed.nzhour=ih
      hed.nzmin=im
      hed.nzsec=int(sec)
      hed.nzmsec=int(1000.*(sec-hed.nzsec))
      hed.nvhdr=6
      hed.npts=npts
      hed.iftype=1
      hed.idep=5
      hed.iztype=9
c
      hed.leven=.true.
      if(scode.eq.'DUG') then
         hed.lpspol=.false.
      else
         hed.lpspol=.true.
      endif
c
      hed.kstnm=scode
      hed.kcmpnm=chn
      return
      end
      subroutine asched(nout,hed,ios)
c
c   Write out the SAC ASCII header.
c
      include 'SAChed.inc'
      record/sached/hed
c
      write(nout,101,iostat=ios)hed.delta,hed.depmin,hed.depmax,
     1 hed.scale,hed.odelta,hed.b,hed.e,hed.o,hed.a,hed.int1,hed.t,
     2 hed.f,hed.resp,hed.stla,hed.stlo,hed.stel,hed.stdp,hed.evla,
     3 hed.evlo,hed.evel,hed.evdp,hed.unu1,hed.user,hed.dist,hed.az,
     4 hed.baz,hed.gcarc,hed.int2,hed.depmen,hed.cmpaz,hed.cmpinc,
     5 hed.unu2
 101  format(5g15.7)
      if(ios.ne.0) return
      write(nout,102,iostat=ios)hed.nzyear,hed.nzjday,hed.nzhour,
     1 hed.nzmin,hed.nzsec,hed.nzmsec,hed.nvhdr,hed.int3,hed.npts,
     2 hed.int4,hed.unu3,hed.iftype,hed.idep,hed.iztype,hed.unu4,
     3 hed.iinst,hed.istreg,hed.ievreg,hed.ievtyp,hed.iqual,
     4 hed.isynth,hed.unu5
 102  format(5i10)
      if(ios.ne.0) return
      write(nout,102,iostat=ios)hed.leven,hed.lpspol,hed.lovrok,
     1 hed.lcalda,hed.unu6
      if(ios.ne.0) return
      write(nout,103,iostat=ios)hed.kstnm,hed.kevnm,hed.khole,hed.ko,
     1 hed.ka,hed.kt,hed.kf,hed.kuser,hed.kcmpnm,hed.knetwk,
     2 hed.kdatrd,hed.kinst
 103  format(a8,a16/(3a8))
      return
      end
      subroutine wsacdt(nout,kpts,a,bin)
c
c   Write time series data out in SAC format.
c
      logical bin
      dimension a(*)
      common/sacio/irec,lpt,ipt,buf(128,2)
c
      if(kpts.gt.0) then
c   Copy the data into the internal buffer, flushing it to disk as needed.
         do i=1,kpts
c   See if the buffer is full.
            if(ipt.ge.128) then
c   Handle binary data.
               if(bin) then
                  irec=irec+1
                  write(nout,rec=irec,iostat=ios)(buf(j,lpt),j=1,128)
                  if(ios.ne.0) go to 13
                  ipt=0
c   Handle ASCII data.
               else
                  write(nout,101,iostat=ios)(buf(j,lpt),j=1,125)
 101              format(5g15.7)
                  if(ios.ne.0) go to 13
                  ipt=0
                  do j=126,128
                     ipt=ipt+1
                     buf(ipt,lpt)=buf(j,lpt)
                  enddo
               endif
            endif
c   Buffer the next point.
            ipt=ipt+1
            buf(ipt,lpt)=a(i)
         enddo
c   Pad out the last buffer and flush it to disk.
      else
         ipt0=ipt
         do i=1,256
            if(ipt.ge.128) then
c   Handle binary data.
               if(bin) then
                  irec=irec+1
                  write(nout,rec=irec,iostat=ios)(buf(j,lpt),j=1,128)
                  close(nout)
                  if(ios.ne.0) go to 13
                  return
c   Handle ASCII data.
               else
                  ln=min0(5*((ipt0+4)/5),125)
                  write(nout,101,iostat=ios)(buf(j,lpt),j=1,ln)
                  if(ios.ne.0) then
                     close(nout)
                     go to 13
                  endif
                  if(ipt0.le.125) then
                     close(nout)
                     return
                  endif
                  ipt=0
                  do j=126,128
                     ipt=ipt+1
                     buf(ipt,lpt)=buf(j,lpt)
                  enddo
                  ipt0=ipt0-125
               endif
            endif
            ipt=ipt+1
            buf(ipt,lpt)=-12345.
         enddo
      endif
      return
c
 13   write(*,100)ios,i-1,kpts
 100  format('Wsacdt:  write error (',i3,').',i6,' of',i5,
     1 ' samples written.')
      return
      end
      SUBROUTINE DCOMP6(LB,IBUF,LOUT,IOUT,IERROR)
C***********************************************************************
C                                                                      *
C     SUBROUTINE DCOMP6(LB,IBUF,LOUT,IOUT,IERROR)                      *
C                                                                      *
C     SUBROUTINE DECOMPRESS INTEGER DATA THAT HAS BEEN COMPRESSED      *
C     INTO ASCII CHARACTERS AND RETURNS VALUES IN INTEGER *4 FORMAT    *
C     SEE SUBROUTINE CMPRS6 FOR COMPRESSION FORMAT                     *
C     INPUT - IBUF AN ARRAY OF CONTAINING  LB CHARACTERS.              *
C     LOUT SHOULD CONTAIN THE DIMENSION OF THE INTEGER *4 ARRAY IOUT.  * 
C     ON RETURN ARRAY LOUT WILL BE SET TO CONTAIN THE NUMBER OF INTEGER*
C     VALUES WHICH HAVE BEEN PUT IN THE ARRAY IOUT.                    *
C     IF THE ARRAY IOUT IS NOT LARGE ENOUGH TO CONTAIN ALL OF THE      *
C     DECOMPRESSED VALUES IERROR WILL BE SET TO -1 OTHERWISE IT WILL   *
C     SET TO ZERO                                                      *
C                                                                      *
C***********************************************************************
      IMPLICIT INTEGER *4 (I-N)
cuk      INTEGER *4 IOUT(*),ICHAR(128)
cuu91      INTEGER *4 IOUT(1),ICHAR(128)
      INTEGER *4 IOUT(lout),ICHAR(128)
cuu91      CHARACTER *1 IBUF(1), ACHAR(4), TEST(4)
      CHARACTER *1 IBUF(lb), ACHAR(4), TEST(4)
      CHARACTER *1 ASPACE,LFEED,CRETN
      EQUIVALENCE (INN,ACHAR),(ITEST,TEST)
      DATA TEST/'1','2','3','4'/
      data ichar/0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     1 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     2 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,2,
     3 3,4,5,6,7,8,9,10,11,0,0,0,0,0,0,0,
     4 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
     5 28,29,30,31,32,33,34,35,36,37,0,0,0,0,0,0,
     6 38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,
     7 54,55,56,57,58,59,60,61,62,63,0,0,0,0,0,0/
      IERROR = 0
      CRETN = CHAR(13)
      LFEED = CHAR(10)
      ASPACE = CHAR(32)
      IMAX = LOUT
      ISIGN = 16
      IOFLOW = 32
      MASK1 = 15
      MASK2 = 31
C     WORK OUT WHICH WAY BYTES ARE STORED IN COMPUTER
      IBYTE = 4
      IF (ITEST.EQ.(((52*256+51)*256+50)*256+49)) IBYTE = 1
      ICOUNT = 0
      I = 0
      J = 0
C     START OF DECODING
   1  CONTINUE
        I = I + 1
        ACHAR(IBYTE) = IBUF(I)
C       IF A CARRIAGE OR LINE FEED IGNORE, IF A SPACE THEN END OF DATA
        IF (ACHAR(IBYTE) .EQ. CRETN) GO TO 1 
        IF (ACHAR(IBYTE) .EQ. LFEED) GO TO 1 
        IF (ACHAR(IBYTE) .EQ. ASPACE) GO TO 5
        ICOUNT = ICOUNT + 1
C       STRIP OFF ANY HIGHER ORDER BITS
        CALL INTAND(INN,127,K)
C       GET NUMBER REPRESENTION OF INPUT CHARACTER
        INN = ICHAR(K)
C       GET SIGN BIT
        CALL INTAND(INN,ISIGN,JSIGN)
C       GET CONTINUATION BIT (IF ANY)
        CALL INTAND(INN,IOFLOW,JOFLOW)
C       REMOVE BITS WE DONT WANT
        CALL INTAND(INN,MASK1,ITEMP)
   2    CONTINUE
          IF(JOFLOW.EQ.0) GO TO 4
C         THERE IS ANOTHER BYTE IN THIS SAMPLE
          ITEMP = ITEMP * 32
   3        CONTINUE
            I = I + 1
            ACHAR(IBYTE) = IBUF(I)
          IF (ACHAR(IBYTE) .EQ. CRETN) GO TO 3 
          IF (ACHAR(IBYTE) .EQ. LFEED) GO TO 3 
          ICOUNT = ICOUNT + 1
C         STRIP OFF ANY HIGHER ORDER BITS
          CALL INTAND(INN,127,K)
          INN = ICHAR(K)
C         GET CONTINUATION BIT (IF ANY)
          CALL INTAND(INN,IOFLOW,JOFLOW)
          CALL INTAND(INN,MASK2,K)
          ITEMP = ITEMP + K
        GO TO 2
   4    CONTINUE
        IF (JSIGN.NE.0) ITEMP = -ITEMP
        J = J +1
        IF (J .GT. IMAX) GO TO 5
        IOUT(J) = ITEMP
      IF(ICOUNT.LT.LB) GO TO 1
   5  CONTINUE
      LOUT = J
      IF (J .GT. IMAX) THEN
         LOUT = IMAX
         IERROR = -1
      ENDIF
      RETURN
      END
       SUBROUTINE INTAND(I1,I2,I3) 
C**********************************************************************
C                                                                     *
C      SUBROUTINE INTAND(I1,I2,I3)                                    *
C                                                                     *
C      SUBROUTINE BITWISE "ANDS" THE INTEGERS IN I1 AND I2            *
C      AND RETURNS THE RESULT IN INTEGER I3                           *
C      FOR EXAMPLE                                                    *
C      IF THE 32 BITS IN I1 ARE 11001100 1110001110 11110000 11111111 *
C      AND IN I2 ARE            10101010 1100110011 00110011 00110011 *
C      I3 WILL BECOME           10001000 1100000010 00110000 00110011 *
C                                                                     *
C      NOTE "AND" IS NOT STANDARD FORTRAN 77 FUNCTION SO THIS IS A    *
C      MACHINE DEPENDANT SUBROUTINE                                   *
C                                                                     *
C**********************************************************************
       INTEGER *4 I1,I2,I3
C##SUN## REMOVE COMMENT FROM NEXT LINE FOR SUN COMPUTERS
       I3 = AND(I1,I2)
C##DEC## REMOVE COMMENT FROM NEXT LINE FOR PDP AND VAX COMPUTERS and HP's
C      I3 = JIAND(I1,I2)
C##IBM## REMOVE COMMENT FROM NEXT LINE FOR IBM PC'S (I THINK)
C      I3 = IAND(I1,I2)
       RETURN
       END
      subroutine REMDIF1(iy,nmax)
c
c     Remove Data-compression (first differences)
c
c     Urs Kradolfer, January 1990
c
      implicit none
      integer nmax, iy(nmax), k
c
      do k=2,nmax
         iy(k)=iy(k)+iy(k-1)
      enddo
c
      RETURN
      end ! of subroutine remdif1
      subroutine juldat(year,month,day,julday)
c
c $$$$$ calls only library routines $$$$$
c
c   Given the usual year, month, and day (as integer numbers) subroutine
c   juldat returns integer Julian day julday.  It properly accounts for
c   the leap years through 2099.  If the month or day are illegal dates,
c   then julday is set to zero.  Programmed by R. Buland.
c
      integer year,day,dpm(12),feb
      data dpm/31,28,31,30,31,30,31,31,30,31,30,31/,feb/28/
      dpm(2)=feb
      if(mod(year,4).eq.0) dpm(2)=feb+1
      if(month.le.0.or.month.gt.12.or.day.le.0.or.day.gt.dpm(month))
     1 go to 13
      julday=day
      if(month.le.1) return
      im=month-1
      do 1 i=1,im
 1    julday=julday+dpm(i)
      return
 13   julday=0
      return
c
c   Given the integer year and julian day, subroutine gredat returns the
c   correct integer month and day of the month.  Gredat correctly treats
c   leap years through 2099.  If the Julian day is illegal, then a zero
c   month and day are returned.  Programmed by R. Buland.
c
      entry gredat(year,month,day,julday)
      if(julday.le.0.or.julday.gt.366-min0(mod(year,4),1)) go to 14
      dpm(2)=feb
      if(mod(year,4).eq.0) dpm(2)=feb+1
      day=julday
      month=1
      do 2 i=1,12
      if(day.le.dpm(i)) return
      day=day-dpm(i)
 2    month=month+1
      return
 14   month=0
      day=0
      return
      end
      function ntrbl(ia)
c
c $$$$$ calls no other routine $$$$$
c
c   Ntrbl returns the position of the last non-blank character stored
c   in character variable ia.  If ia is completely blank, then ntrbl
c   returns one so that ia(1:ntrbl(ia)) will always be a legal substring
c   with as many trailing blanks trimmed as possible.
c
      character*(*) ia
      ln=len(ia)
      do 1 i=ln,1,-1
      if(ichar(ia(i:i)).gt.32) go to 2
 1    continue
      i=1
 2    ntrbl=i
      return
      end
      subroutine moveb(n,ia,ib)
c
c   Move n bytes from byte array ia(n) to byte array ib(n).
c
      byte ia(n),ib(n)
      integer n,j
c
      do j=1,n
        ib(j)=ia(j)
      enddo
      return
      end

c Revision History
c
c $Log: cvtgse.f,v $
c Revision 1.2  2001/12/20 18:28:52  dec
c increase hard-coded maximum number of samples
c
c Revision 1.1.1.1  2000/02/08 20:20:08  dec
c import existing IDA/NRTS sources
c
