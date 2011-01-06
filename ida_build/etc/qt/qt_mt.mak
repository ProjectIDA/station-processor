# Microsoft Developer Studio Generated NMAKE File, Based on qt_mt.dsp
!IF "$(CFG)" == ""
CFG=qtmt - Win32 Release
!MESSAGE No configuration specified. Defaulting to qtmt - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "qtmt - Win32 Release" && "$(CFG)" != "qtmt - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qt_mt.mak" CFG="qtmt - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qtmt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "qtmt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "qtmt - Win32 Release"

OUTDIR=$(QTDIR)\lib
INTDIR=tmp\obj\release_mt_shared

ALL : "..\lib\qt-mt334.dll"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\jcapimin.obj"
	-@erase "$(INTDIR)\jcapistd.obj"
	-@erase "$(INTDIR)\jccoefct.obj"
	-@erase "$(INTDIR)\jccolor.obj"
	-@erase "$(INTDIR)\jcdctmgr.obj"
	-@erase "$(INTDIR)\jchuff.obj"
	-@erase "$(INTDIR)\jcinit.obj"
	-@erase "$(INTDIR)\jcmainct.obj"
	-@erase "$(INTDIR)\jcmarker.obj"
	-@erase "$(INTDIR)\jcmaster.obj"
	-@erase "$(INTDIR)\jcomapi.obj"
	-@erase "$(INTDIR)\jcparam.obj"
	-@erase "$(INTDIR)\jcphuff.obj"
	-@erase "$(INTDIR)\jcprepct.obj"
	-@erase "$(INTDIR)\jcsample.obj"
	-@erase "$(INTDIR)\jctrans.obj"
	-@erase "$(INTDIR)\jdapimin.obj"
	-@erase "$(INTDIR)\jdapistd.obj"
	-@erase "$(INTDIR)\jdatadst.obj"
	-@erase "$(INTDIR)\jdatasrc.obj"
	-@erase "$(INTDIR)\jdcoefct.obj"
	-@erase "$(INTDIR)\jdcolor.obj"
	-@erase "$(INTDIR)\jddctmgr.obj"
	-@erase "$(INTDIR)\jdhuff.obj"
	-@erase "$(INTDIR)\jdinput.obj"
	-@erase "$(INTDIR)\jdmainct.obj"
	-@erase "$(INTDIR)\jdmarker.obj"
	-@erase "$(INTDIR)\jdmaster.obj"
	-@erase "$(INTDIR)\jdmerge.obj"
	-@erase "$(INTDIR)\jdphuff.obj"
	-@erase "$(INTDIR)\jdpostct.obj"
	-@erase "$(INTDIR)\jdsample.obj"
	-@erase "$(INTDIR)\jdtrans.obj"
	-@erase "$(INTDIR)\jerror.obj"
	-@erase "$(INTDIR)\jfdctflt.obj"
	-@erase "$(INTDIR)\jfdctfst.obj"
	-@erase "$(INTDIR)\jfdctint.obj"
	-@erase "$(INTDIR)\jidctflt.obj"
	-@erase "$(INTDIR)\jidctfst.obj"
	-@erase "$(INTDIR)\jidctint.obj"
	-@erase "$(INTDIR)\jidctred.obj"
	-@erase "$(INTDIR)\jmemmgr.obj"
	-@erase "$(INTDIR)\jmemnobs.obj"
	-@erase "$(INTDIR)\jquant1.obj"
	-@erase "$(INTDIR)\jquant2.obj"
	-@erase "$(INTDIR)\jutils.obj"
	-@erase "$(INTDIR)\moc_qaccel.obj"
	-@erase "$(INTDIR)\moc_qaction.obj"
	-@erase "$(INTDIR)\moc_qapplication.obj"
	-@erase "$(INTDIR)\moc_qasyncio.obj"
	-@erase "$(INTDIR)\moc_qbutton.obj"
	-@erase "$(INTDIR)\moc_qbuttongroup.obj"
	-@erase "$(INTDIR)\moc_qcanvas.obj"
	-@erase "$(INTDIR)\moc_qcheckbox.obj"
	-@erase "$(INTDIR)\moc_qclipboard.obj"
	-@erase "$(INTDIR)\moc_qcolordialog.obj"
	-@erase "$(INTDIR)\moc_qcombobox.obj"
	-@erase "$(INTDIR)\moc_qcommonstyle.obj"
	-@erase "$(INTDIR)\moc_qdatabrowser.obj"
	-@erase "$(INTDIR)\moc_qdatatable.obj"
	-@erase "$(INTDIR)\moc_qdataview.obj"
	-@erase "$(INTDIR)\moc_qdatetimeedit.obj"
	-@erase "$(INTDIR)\moc_qdesktopwidget.obj"
	-@erase "$(INTDIR)\moc_qdial.obj"
	-@erase "$(INTDIR)\moc_qdialog.obj"
	-@erase "$(INTDIR)\moc_qdialogbuttons_p.obj"
	-@erase "$(INTDIR)\moc_qdns.obj"
	-@erase "$(INTDIR)\moc_qdockarea.obj"
	-@erase "$(INTDIR)\moc_qdockwindow.obj"
	-@erase "$(INTDIR)\moc_qdragobject.obj"
	-@erase "$(INTDIR)\moc_qerrormessage.obj"
	-@erase "$(INTDIR)\moc_qeventloop.obj"
	-@erase "$(INTDIR)\moc_qfiledialog.obj"
	-@erase "$(INTDIR)\moc_qfontdialog.obj"
	-@erase "$(INTDIR)\moc_qframe.obj"
	-@erase "$(INTDIR)\moc_qftp.obj"
	-@erase "$(INTDIR)\moc_qgl.obj"
	-@erase "$(INTDIR)\moc_qgplugin.obj"
	-@erase "$(INTDIR)\moc_qgrid.obj"
	-@erase "$(INTDIR)\moc_qgridview.obj"
	-@erase "$(INTDIR)\moc_qgroupbox.obj"
	-@erase "$(INTDIR)\moc_qguardedptr.obj"
	-@erase "$(INTDIR)\moc_qhbox.obj"
	-@erase "$(INTDIR)\moc_qhbuttongroup.obj"
	-@erase "$(INTDIR)\moc_qheader.obj"
	-@erase "$(INTDIR)\moc_qhgroupbox.obj"
	-@erase "$(INTDIR)\moc_qhttp.obj"
	-@erase "$(INTDIR)\moc_qiconview.obj"
	-@erase "$(INTDIR)\moc_qimageformatplugin.obj"
	-@erase "$(INTDIR)\moc_qinputdialog.obj"
	-@erase "$(INTDIR)\moc_qlabel.obj"
	-@erase "$(INTDIR)\moc_qlayout.obj"
	-@erase "$(INTDIR)\moc_qlcdnumber.obj"
	-@erase "$(INTDIR)\moc_qlineedit.obj"
	-@erase "$(INTDIR)\moc_qlistbox.obj"
	-@erase "$(INTDIR)\moc_qlistview.obj"
	-@erase "$(INTDIR)\moc_qlocalfs.obj"
	-@erase "$(INTDIR)\moc_qmainwindow.obj"
	-@erase "$(INTDIR)\moc_qmenubar.obj"
	-@erase "$(INTDIR)\moc_qmessagebox.obj"
	-@erase "$(INTDIR)\moc_qmultilineedit.obj"
	-@erase "$(INTDIR)\moc_qnetworkprotocol.obj"
	-@erase "$(INTDIR)\moc_qobject.obj"
	-@erase "$(INTDIR)\moc_qobjectcleanuphandler.obj"
	-@erase "$(INTDIR)\moc_qpopupmenu.obj"
	-@erase "$(INTDIR)\moc_qprocess.obj"
	-@erase "$(INTDIR)\moc_qprogressbar.obj"
	-@erase "$(INTDIR)\moc_qprogressdialog.obj"
	-@erase "$(INTDIR)\moc_qpushbutton.obj"
	-@erase "$(INTDIR)\moc_qradiobutton.obj"
	-@erase "$(INTDIR)\moc_qrangecontrol.obj"
	-@erase "$(INTDIR)\moc_qrichtext_p.obj"
	-@erase "$(INTDIR)\moc_qscrollbar.obj"
	-@erase "$(INTDIR)\moc_qscrollview.obj"
	-@erase "$(INTDIR)\moc_qsemimodal.obj"
	-@erase "$(INTDIR)\moc_qserversocket.obj"
	-@erase "$(INTDIR)\moc_qsessionmanager.obj"
	-@erase "$(INTDIR)\moc_qsignal.obj"
	-@erase "$(INTDIR)\moc_qsignalmapper.obj"
	-@erase "$(INTDIR)\moc_qsizegrip.obj"
	-@erase "$(INTDIR)\moc_qslider.obj"
	-@erase "$(INTDIR)\moc_qsocket.obj"
	-@erase "$(INTDIR)\moc_qsocketnotifier.obj"
	-@erase "$(INTDIR)\moc_qsound.obj"
	-@erase "$(INTDIR)\moc_qspinbox.obj"
	-@erase "$(INTDIR)\moc_qsplashscreen.obj"
	-@erase "$(INTDIR)\moc_qsplitter.obj"
	-@erase "$(INTDIR)\moc_qsqldatabase.obj"
	-@erase "$(INTDIR)\moc_qsqldriver.obj"
	-@erase "$(INTDIR)\moc_qsqldriverplugin.obj"
	-@erase "$(INTDIR)\moc_qsqlform.obj"
	-@erase "$(INTDIR)\moc_qsqlquery.obj"
	-@erase "$(INTDIR)\moc_qstatusbar.obj"
	-@erase "$(INTDIR)\moc_qstyle.obj"
	-@erase "$(INTDIR)\moc_qstyleplugin.obj"
	-@erase "$(INTDIR)\moc_qstylesheet.obj"
	-@erase "$(INTDIR)\moc_qtabbar.obj"
	-@erase "$(INTDIR)\moc_qtabdialog.obj"
	-@erase "$(INTDIR)\moc_qtable.obj"
	-@erase "$(INTDIR)\moc_qtabwidget.obj"
	-@erase "$(INTDIR)\moc_qtextbrowser.obj"
	-@erase "$(INTDIR)\moc_qtextcodecplugin.obj"
	-@erase "$(INTDIR)\moc_qtextedit.obj"
	-@erase "$(INTDIR)\moc_qtextview.obj"
	-@erase "$(INTDIR)\moc_qtimer.obj"
	-@erase "$(INTDIR)\moc_qtitlebar_p.obj"
	-@erase "$(INTDIR)\moc_qtoolbar.obj"
	-@erase "$(INTDIR)\moc_qtoolbox.obj"
	-@erase "$(INTDIR)\moc_qtoolbutton.obj"
	-@erase "$(INTDIR)\moc_qtooltip.obj"
	-@erase "$(INTDIR)\moc_qtranslator.obj"
	-@erase "$(INTDIR)\moc_qurloperator.obj"
	-@erase "$(INTDIR)\moc_qvalidator.obj"
	-@erase "$(INTDIR)\moc_qvbox.obj"
	-@erase "$(INTDIR)\moc_qvbuttongroup.obj"
	-@erase "$(INTDIR)\moc_qvgroupbox.obj"
	-@erase "$(INTDIR)\moc_qwidget.obj"
	-@erase "$(INTDIR)\moc_qwidgetplugin.obj"
	-@erase "$(INTDIR)\moc_qwidgetresizehandler_p.obj"
	-@erase "$(INTDIR)\moc_qwidgetstack.obj"
	-@erase "$(INTDIR)\moc_qwindowsstyle.obj"
	-@erase "$(INTDIR)\moc_qwizard.obj"
	-@erase "$(INTDIR)\moc_qworkspace.obj"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\qabstractlayout.obj"
	-@erase "$(INTDIR)\qaccel.obj"
	-@erase "$(INTDIR)\qaccessible.obj"
	-@erase "$(INTDIR)\qaccessible_win.obj"
	-@erase "$(INTDIR)\qaction.obj"
	-@erase "$(INTDIR)\qapplication.obj"
	-@erase "$(INTDIR)\qapplication_win.obj"
	-@erase "$(INTDIR)\qasyncimageio.obj"
	-@erase "$(INTDIR)\qasyncio.obj"
	-@erase "$(INTDIR)\qbig5codec.obj"
	-@erase "$(INTDIR)\qbitarray.obj"
	-@erase "$(INTDIR)\qbitmap.obj"
	-@erase "$(INTDIR)\qbuffer.obj"
	-@erase "$(INTDIR)\qbutton.obj"
	-@erase "$(INTDIR)\qbuttongroup.obj"
	-@erase "$(INTDIR)\qcanvas.obj"
	-@erase "$(INTDIR)\qcheckbox.obj"
	-@erase "$(INTDIR)\qclipboard.obj"
	-@erase "$(INTDIR)\qclipboard_win.obj"
	-@erase "$(INTDIR)\qcolor.obj"
	-@erase "$(INTDIR)\qcolor_p.obj"
	-@erase "$(INTDIR)\qcolor_win.obj"
	-@erase "$(INTDIR)\qcolordialog.obj"
	-@erase "$(INTDIR)\qcombobox.obj"
	-@erase "$(INTDIR)\qcomlibrary.obj"
	-@erase "$(INTDIR)\qcommonstyle.obj"
	-@erase "$(INTDIR)\qcomponentfactory.obj"
	-@erase "$(INTDIR)\qconfig.obj"
	-@erase "$(INTDIR)\qconnection.obj"
	-@erase "$(INTDIR)\qcriticalsection_p.obj"
	-@erase "$(INTDIR)\qcstring.obj"
	-@erase "$(INTDIR)\qcursor.obj"
	-@erase "$(INTDIR)\qcursor_win.obj"
	-@erase "$(INTDIR)\qdatabrowser.obj"
	-@erase "$(INTDIR)\qdatastream.obj"
	-@erase "$(INTDIR)\qdatatable.obj"
	-@erase "$(INTDIR)\qdataview.obj"
	-@erase "$(INTDIR)\qdatetime.obj"
	-@erase "$(INTDIR)\qdatetimeedit.obj"
	-@erase "$(INTDIR)\qdeepcopy.obj"
	-@erase "$(INTDIR)\qdesktopwidget_win.obj"
	-@erase "$(INTDIR)\qdial.obj"
	-@erase "$(INTDIR)\qdialog.obj"
	-@erase "$(INTDIR)\qdialogbuttons.obj"
	-@erase "$(INTDIR)\qdir.obj"
	-@erase "$(INTDIR)\qdir_win.obj"
	-@erase "$(INTDIR)\qdnd_win.obj"
	-@erase "$(INTDIR)\qdns.obj"
	-@erase "$(INTDIR)\qdockarea.obj"
	-@erase "$(INTDIR)\qdockwindow.obj"
	-@erase "$(INTDIR)\qdom.obj"
	-@erase "$(INTDIR)\qdragobject.obj"
	-@erase "$(INTDIR)\qdrawutil.obj"
	-@erase "$(INTDIR)\qdropsite.obj"
	-@erase "$(INTDIR)\qeditorfactory.obj"
	-@erase "$(INTDIR)\qeffects.obj"
	-@erase "$(INTDIR)\qerrormessage.obj"
	-@erase "$(INTDIR)\qeucjpcodec.obj"
	-@erase "$(INTDIR)\qeuckrcodec.obj"
	-@erase "$(INTDIR)\qevent.obj"
	-@erase "$(INTDIR)\qeventloop.obj"
	-@erase "$(INTDIR)\qeventloop_win.obj"
	-@erase "$(INTDIR)\qfile.obj"
	-@erase "$(INTDIR)\qfile_win.obj"
	-@erase "$(INTDIR)\qfiledialog.obj"
	-@erase "$(INTDIR)\qfiledialog_win.obj"
	-@erase "$(INTDIR)\qfileinfo.obj"
	-@erase "$(INTDIR)\qfileinfo_win.obj"
	-@erase "$(INTDIR)\qfocusdata.obj"
	-@erase "$(INTDIR)\qfont.obj"
	-@erase "$(INTDIR)\qfont_win.obj"
	-@erase "$(INTDIR)\qfontdatabase.obj"
	-@erase "$(INTDIR)\qfontdialog.obj"
	-@erase "$(INTDIR)\qfontengine_win.obj"
	-@erase "$(INTDIR)\qframe.obj"
	-@erase "$(INTDIR)\qftp.obj"
	-@erase "$(INTDIR)\qgarray.obj"
	-@erase "$(INTDIR)\qgb18030codec.obj"
	-@erase "$(INTDIR)\qgcache.obj"
	-@erase "$(INTDIR)\qgdict.obj"
	-@erase "$(INTDIR)\qgl.obj"
	-@erase "$(INTDIR)\qgl_win.obj"
	-@erase "$(INTDIR)\qglcolormap.obj"
	-@erase "$(INTDIR)\qglist.obj"
	-@erase "$(INTDIR)\qglobal.obj"
	-@erase "$(INTDIR)\qgplugin.obj"
	-@erase "$(INTDIR)\qgpluginmanager.obj"
	-@erase "$(INTDIR)\qgrid.obj"
	-@erase "$(INTDIR)\qgridview.obj"
	-@erase "$(INTDIR)\qgroupbox.obj"
	-@erase "$(INTDIR)\qguardedptr.obj"
	-@erase "$(INTDIR)\qgvector.obj"
	-@erase "$(INTDIR)\qhbox.obj"
	-@erase "$(INTDIR)\qhbuttongroup.obj"
	-@erase "$(INTDIR)\qheader.obj"
	-@erase "$(INTDIR)\qhgroupbox.obj"
	-@erase "$(INTDIR)\qhostaddress.obj"
	-@erase "$(INTDIR)\qhttp.obj"
	-@erase "$(INTDIR)\qiconset.obj"
	-@erase "$(INTDIR)\qiconview.obj"
	-@erase "$(INTDIR)\qimage.obj"
	-@erase "$(INTDIR)\qimageformatplugin.obj"
	-@erase "$(INTDIR)\qinputcontext_win.obj"
	-@erase "$(INTDIR)\qinputdialog.obj"
	-@erase "$(INTDIR)\qinternal.obj"
	-@erase "$(INTDIR)\qiodevice.obj"
	-@erase "$(INTDIR)\qisciicodec.obj"
	-@erase "$(INTDIR)\qjiscodec.obj"
	-@erase "$(INTDIR)\qjpegio.obj"
	-@erase "$(INTDIR)\qjpunicode.obj"
	-@erase "$(INTDIR)\qkeysequence.obj"
	-@erase "$(INTDIR)\qlabel.obj"
	-@erase "$(INTDIR)\qlayout.obj"
	-@erase "$(INTDIR)\qlayoutengine.obj"
	-@erase "$(INTDIR)\qlcdnumber.obj"
	-@erase "$(INTDIR)\qlibrary.obj"
	-@erase "$(INTDIR)\qlibrary_win.obj"
	-@erase "$(INTDIR)\qlineedit.obj"
	-@erase "$(INTDIR)\qlistbox.obj"
	-@erase "$(INTDIR)\qlistview.obj"
	-@erase "$(INTDIR)\qlocale.obj"
	-@erase "$(INTDIR)\qlocalfs.obj"
	-@erase "$(INTDIR)\qmainwindow.obj"
	-@erase "$(INTDIR)\qmap.obj"
	-@erase "$(INTDIR)\qmenubar.obj"
	-@erase "$(INTDIR)\qmenudata.obj"
	-@erase "$(INTDIR)\qmessagebox.obj"
	-@erase "$(INTDIR)\qmetaobject.obj"
	-@erase "$(INTDIR)\qmime.obj"
	-@erase "$(INTDIR)\qmime_win.obj"
	-@erase "$(INTDIR)\qmngio.obj"
	-@erase "$(INTDIR)\qmovie.obj"
	-@erase "$(INTDIR)\qmultilineedit.obj"
	-@erase "$(INTDIR)\qmutex_win.obj"
	-@erase "$(INTDIR)\qmutexpool.obj"
	-@erase "$(INTDIR)\qnetwork.obj"
	-@erase "$(INTDIR)\qnetworkprotocol.obj"
	-@erase "$(INTDIR)\qobject.obj"
	-@erase "$(INTDIR)\qobjectcleanuphandler.obj"
	-@erase "$(INTDIR)\qole_win.obj"
	-@erase "$(INTDIR)\qpaintdevice_win.obj"
	-@erase "$(INTDIR)\qpaintdevicemetrics.obj"
	-@erase "$(INTDIR)\qpainter.obj"
	-@erase "$(INTDIR)\qpainter_win.obj"
	-@erase "$(INTDIR)\qpalette.obj"
	-@erase "$(INTDIR)\qpicture.obj"
	-@erase "$(INTDIR)\qpixmap.obj"
	-@erase "$(INTDIR)\qpixmap_win.obj"
	-@erase "$(INTDIR)\qpixmapcache.obj"
	-@erase "$(INTDIR)\qpngio.obj"
	-@erase "$(INTDIR)\qpoint.obj"
	-@erase "$(INTDIR)\qpointarray.obj"
	-@erase "$(INTDIR)\qpolygonscanner.obj"
	-@erase "$(INTDIR)\qpopupmenu.obj"
	-@erase "$(INTDIR)\qprinter.obj"
	-@erase "$(INTDIR)\qprinter_win.obj"
	-@erase "$(INTDIR)\qprocess.obj"
	-@erase "$(INTDIR)\qprocess_win.obj"
	-@erase "$(INTDIR)\qprogressbar.obj"
	-@erase "$(INTDIR)\qprogressdialog.obj"
	-@erase "$(INTDIR)\qptrcollection.obj"
	-@erase "$(INTDIR)\qpushbutton.obj"
	-@erase "$(INTDIR)\qradiobutton.obj"
	-@erase "$(INTDIR)\qrangecontrol.obj"
	-@erase "$(INTDIR)\qrect.obj"
	-@erase "$(INTDIR)\qregexp.obj"
	-@erase "$(INTDIR)\qregion.obj"
	-@erase "$(INTDIR)\qregion_win.obj"
	-@erase "$(INTDIR)\qrichtext.obj"
	-@erase "$(INTDIR)\qrichtext_p.obj"
	-@erase "$(INTDIR)\qrtlcodec.obj"
	-@erase "$(INTDIR)\qscriptengine.obj"
	-@erase "$(INTDIR)\qscrollbar.obj"
	-@erase "$(INTDIR)\qscrollview.obj"
	-@erase "$(INTDIR)\qsemaphore.obj"
	-@erase "$(INTDIR)\qserversocket.obj"
	-@erase "$(INTDIR)\qsettings.obj"
	-@erase "$(INTDIR)\qsettings_win.obj"
	-@erase "$(INTDIR)\qsignal.obj"
	-@erase "$(INTDIR)\qsignalmapper.obj"
	-@erase "$(INTDIR)\qsimplerichtext.obj"
	-@erase "$(INTDIR)\qsize.obj"
	-@erase "$(INTDIR)\qsizegrip.obj"
	-@erase "$(INTDIR)\qsjiscodec.obj"
	-@erase "$(INTDIR)\qslider.obj"
	-@erase "$(INTDIR)\qsocket.obj"
	-@erase "$(INTDIR)\qsocketdevice.obj"
	-@erase "$(INTDIR)\qsocketdevice_win.obj"
	-@erase "$(INTDIR)\qsocketnotifier.obj"
	-@erase "$(INTDIR)\qsound.obj"
	-@erase "$(INTDIR)\qsound_win.obj"
	-@erase "$(INTDIR)\qspinbox.obj"
	-@erase "$(INTDIR)\qspinwidget.obj"
	-@erase "$(INTDIR)\qsplashscreen.obj"
	-@erase "$(INTDIR)\qsplitter.obj"
	-@erase "$(INTDIR)\qsql_odbc.obj"
	-@erase "$(INTDIR)\qsqlcachedresult.obj"
	-@erase "$(INTDIR)\qsqlcursor.obj"
	-@erase "$(INTDIR)\qsqldatabase.obj"
	-@erase "$(INTDIR)\qsqldriver.obj"
	-@erase "$(INTDIR)\qsqldriverplugin.obj"
	-@erase "$(INTDIR)\qsqleditorfactory.obj"
	-@erase "$(INTDIR)\qsqlerror.obj"
	-@erase "$(INTDIR)\qsqlextension_p.obj"
	-@erase "$(INTDIR)\qsqlfield.obj"
	-@erase "$(INTDIR)\qsqlform.obj"
	-@erase "$(INTDIR)\qsqlindex.obj"
	-@erase "$(INTDIR)\qsqlmanager_p.obj"
	-@erase "$(INTDIR)\qsqlpropertymap.obj"
	-@erase "$(INTDIR)\qsqlquery.obj"
	-@erase "$(INTDIR)\qsqlrecord.obj"
	-@erase "$(INTDIR)\qsqlresult.obj"
	-@erase "$(INTDIR)\qsqlselectcursor.obj"
	-@erase "$(INTDIR)\qstatusbar.obj"
	-@erase "$(INTDIR)\qstring.obj"
	-@erase "$(INTDIR)\qstringlist.obj"
	-@erase "$(INTDIR)\qstyle.obj"
	-@erase "$(INTDIR)\qstylefactory.obj"
	-@erase "$(INTDIR)\qstyleplugin.obj"
	-@erase "$(INTDIR)\qstylesheet.obj"
	-@erase "$(INTDIR)\qsvgdevice.obj"
	-@erase "$(INTDIR)\qsyntaxhighlighter.obj"
	-@erase "$(INTDIR)\qt.res"
	-@erase "$(INTDIR)\qtabbar.obj"
	-@erase "$(INTDIR)\qtabdialog.obj"
	-@erase "$(INTDIR)\qtable.obj"
	-@erase "$(INTDIR)\qtabwidget.obj"
	-@erase "$(INTDIR)\qtextbrowser.obj"
	-@erase "$(INTDIR)\qtextcodec.obj"
	-@erase "$(INTDIR)\qtextcodecfactory.obj"
	-@erase "$(INTDIR)\qtextcodecplugin.obj"
	-@erase "$(INTDIR)\qtextedit.obj"
	-@erase "$(INTDIR)\qtextengine.obj"
	-@erase "$(INTDIR)\qtextlayout.obj"
	-@erase "$(INTDIR)\qtextstream.obj"
	-@erase "$(INTDIR)\qtextview.obj"
	-@erase "$(INTDIR)\qthread.obj"
	-@erase "$(INTDIR)\qthread_win.obj"
	-@erase "$(INTDIR)\qthreadstorage_win.obj"
	-@erase "$(INTDIR)\qtimer.obj"
	-@erase "$(INTDIR)\qtitlebar.obj"
	-@erase "$(INTDIR)\qtoolbar.obj"
	-@erase "$(INTDIR)\qtoolbox.obj"
	-@erase "$(INTDIR)\qtoolbutton.obj"
	-@erase "$(INTDIR)\qtooltip.obj"
	-@erase "$(INTDIR)\qtranslator.obj"
	-@erase "$(INTDIR)\qtsciicodec.obj"
	-@erase "$(INTDIR)\qucom.obj"
	-@erase "$(INTDIR)\qucomextra.obj"
	-@erase "$(INTDIR)\qunicodetables.obj"
	-@erase "$(INTDIR)\qurl.obj"
	-@erase "$(INTDIR)\qurlinfo.obj"
	-@erase "$(INTDIR)\qurloperator.obj"
	-@erase "$(INTDIR)\qutfcodec.obj"
	-@erase "$(INTDIR)\quuid.obj"
	-@erase "$(INTDIR)\qvalidator.obj"
	-@erase "$(INTDIR)\qvariant.obj"
	-@erase "$(INTDIR)\qvbox.obj"
	-@erase "$(INTDIR)\qvbuttongroup.obj"
	-@erase "$(INTDIR)\qvgroupbox.obj"
	-@erase "$(INTDIR)\qwaitcondition_win.obj"
	-@erase "$(INTDIR)\qwhatsthis.obj"
	-@erase "$(INTDIR)\qwidget.obj"
	-@erase "$(INTDIR)\qwidget_win.obj"
	-@erase "$(INTDIR)\qwidgetplugin.obj"
	-@erase "$(INTDIR)\qwidgetresizehandler.obj"
	-@erase "$(INTDIR)\qwidgetstack.obj"
	-@erase "$(INTDIR)\qwindowsstyle.obj"
	-@erase "$(INTDIR)\qwizard.obj"
	-@erase "$(INTDIR)\qwmatrix.obj"
	-@erase "$(INTDIR)\qworkspace.obj"
	-@erase "$(INTDIR)\qxml.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "..\lib\qt-mt334.dll"
	-@erase "..\lib\qt-mt334.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /FI"qt_pch.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fp"$(INTDIR)\qt_mt.pch" /Yu"qt_pch.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\qt.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(QTDIR)\lib/qt_mt.bsc" 
BSC32_SBRS= \

IntDir=tmp\obj\release_mt_shared
	
LINK32=link.exe
LINK32_FLAGS="opengl32.lib" "glu32.lib" "delayimp.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "odbc32.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" delayimp.lib "$(IntDir)\qt_mt.obj" /nologo /base:"0x39D00000" /version:3.34 /subsystem:windows /dll /incremental:no /pdb:"$(QTDIR)\lib/qt-mt334.pdb" /machine:IX86 /out:"C:\Qt\3.3.4\lib\qt-mt334.dll" /implib:"C:\Qt\3.3.4\lib\qt-mt334.lib" /libpath:"$(QTDIR)\lib" /DELAYLOAD:opengl32.dll /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\jcapimin.obj" \
	"$(INTDIR)\jcapistd.obj" \
	"$(INTDIR)\jccoefct.obj" \
	"$(INTDIR)\jccolor.obj" \
	"$(INTDIR)\jcdctmgr.obj" \
	"$(INTDIR)\jchuff.obj" \
	"$(INTDIR)\jcinit.obj" \
	"$(INTDIR)\jcmainct.obj" \
	"$(INTDIR)\jcmarker.obj" \
	"$(INTDIR)\jcmaster.obj" \
	"$(INTDIR)\jcomapi.obj" \
	"$(INTDIR)\jcparam.obj" \
	"$(INTDIR)\jcphuff.obj" \
	"$(INTDIR)\jcprepct.obj" \
	"$(INTDIR)\jcsample.obj" \
	"$(INTDIR)\jctrans.obj" \
	"$(INTDIR)\jdapimin.obj" \
	"$(INTDIR)\jdapistd.obj" \
	"$(INTDIR)\jdatadst.obj" \
	"$(INTDIR)\jdatasrc.obj" \
	"$(INTDIR)\jdcoefct.obj" \
	"$(INTDIR)\jdcolor.obj" \
	"$(INTDIR)\jddctmgr.obj" \
	"$(INTDIR)\jdhuff.obj" \
	"$(INTDIR)\jdinput.obj" \
	"$(INTDIR)\jdmainct.obj" \
	"$(INTDIR)\jdmarker.obj" \
	"$(INTDIR)\jdmaster.obj" \
	"$(INTDIR)\jdmerge.obj" \
	"$(INTDIR)\jdphuff.obj" \
	"$(INTDIR)\jdpostct.obj" \
	"$(INTDIR)\jdsample.obj" \
	"$(INTDIR)\jdtrans.obj" \
	"$(INTDIR)\jerror.obj" \
	"$(INTDIR)\jfdctflt.obj" \
	"$(INTDIR)\jfdctfst.obj" \
	"$(INTDIR)\jfdctint.obj" \
	"$(INTDIR)\jidctflt.obj" \
	"$(INTDIR)\jidctfst.obj" \
	"$(INTDIR)\jidctint.obj" \
	"$(INTDIR)\jidctred.obj" \
	"$(INTDIR)\jmemmgr.obj" \
	"$(INTDIR)\jmemnobs.obj" \
	"$(INTDIR)\jquant1.obj" \
	"$(INTDIR)\jquant2.obj" \
	"$(INTDIR)\jutils.obj" \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"$(INTDIR)\qabstractlayout.obj" \
	"$(INTDIR)\qaccel.obj" \
	"$(INTDIR)\qaccessible.obj" \
	"$(INTDIR)\qaccessible_win.obj" \
	"$(INTDIR)\qaction.obj" \
	"$(INTDIR)\qapplication.obj" \
	"$(INTDIR)\qapplication_win.obj" \
	"$(INTDIR)\qasyncimageio.obj" \
	"$(INTDIR)\qasyncio.obj" \
	"$(INTDIR)\qbig5codec.obj" \
	"$(INTDIR)\qbitarray.obj" \
	"$(INTDIR)\qbitmap.obj" \
	"$(INTDIR)\qbuffer.obj" \
	"$(INTDIR)\qbutton.obj" \
	"$(INTDIR)\qbuttongroup.obj" \
	"$(INTDIR)\qcanvas.obj" \
	"$(INTDIR)\qcheckbox.obj" \
	"$(INTDIR)\qclipboard.obj" \
	"$(INTDIR)\qclipboard_win.obj" \
	"$(INTDIR)\qcolor.obj" \
	"$(INTDIR)\qcolor_p.obj" \
	"$(INTDIR)\qcolor_win.obj" \
	"$(INTDIR)\qcolordialog.obj" \
	"$(INTDIR)\qcombobox.obj" \
	"$(INTDIR)\qcomlibrary.obj" \
	"$(INTDIR)\qcommonstyle.obj" \
	"$(INTDIR)\qcomponentfactory.obj" \
	"$(INTDIR)\qconfig.obj" \
	"$(INTDIR)\qconnection.obj" \
	"$(INTDIR)\qcriticalsection_p.obj" \
	"$(INTDIR)\qcstring.obj" \
	"$(INTDIR)\qcursor.obj" \
	"$(INTDIR)\qcursor_win.obj" \
	"$(INTDIR)\qdatabrowser.obj" \
	"$(INTDIR)\qdatastream.obj" \
	"$(INTDIR)\qdatatable.obj" \
	"$(INTDIR)\qdataview.obj" \
	"$(INTDIR)\qdatetime.obj" \
	"$(INTDIR)\qdatetimeedit.obj" \
	"$(INTDIR)\qdeepcopy.obj" \
	"$(INTDIR)\qdesktopwidget_win.obj" \
	"$(INTDIR)\qdial.obj" \
	"$(INTDIR)\qdialog.obj" \
	"$(INTDIR)\qdialogbuttons.obj" \
	"$(INTDIR)\qdir.obj" \
	"$(INTDIR)\qdir_win.obj" \
	"$(INTDIR)\qdnd_win.obj" \
	"$(INTDIR)\qdns.obj" \
	"$(INTDIR)\qdockarea.obj" \
	"$(INTDIR)\qdockwindow.obj" \
	"$(INTDIR)\qdom.obj" \
	"$(INTDIR)\qdragobject.obj" \
	"$(INTDIR)\qdrawutil.obj" \
	"$(INTDIR)\qdropsite.obj" \
	"$(INTDIR)\qeditorfactory.obj" \
	"$(INTDIR)\qeffects.obj" \
	"$(INTDIR)\qerrormessage.obj" \
	"$(INTDIR)\qeucjpcodec.obj" \
	"$(INTDIR)\qeuckrcodec.obj" \
	"$(INTDIR)\qevent.obj" \
	"$(INTDIR)\qeventloop.obj" \
	"$(INTDIR)\qeventloop_win.obj" \
	"$(INTDIR)\qfile.obj" \
	"$(INTDIR)\qfile_win.obj" \
	"$(INTDIR)\qfiledialog.obj" \
	"$(INTDIR)\qfiledialog_win.obj" \
	"$(INTDIR)\qfileinfo.obj" \
	"$(INTDIR)\qfileinfo_win.obj" \
	"$(INTDIR)\qfocusdata.obj" \
	"$(INTDIR)\qfont.obj" \
	"$(INTDIR)\qfont_win.obj" \
	"$(INTDIR)\qfontdatabase.obj" \
	"$(INTDIR)\qfontdialog.obj" \
	"$(INTDIR)\qfontengine_win.obj" \
	"$(INTDIR)\qframe.obj" \
	"$(INTDIR)\qftp.obj" \
	"$(INTDIR)\qgarray.obj" \
	"$(INTDIR)\qgb18030codec.obj" \
	"$(INTDIR)\qgcache.obj" \
	"$(INTDIR)\qgdict.obj" \
	"$(INTDIR)\qgl.obj" \
	"$(INTDIR)\qgl_win.obj" \
	"$(INTDIR)\qglcolormap.obj" \
	"$(INTDIR)\qglist.obj" \
	"$(INTDIR)\qglobal.obj" \
	"$(INTDIR)\qgplugin.obj" \
	"$(INTDIR)\qgpluginmanager.obj" \
	"$(INTDIR)\qgrid.obj" \
	"$(INTDIR)\qgridview.obj" \
	"$(INTDIR)\qgroupbox.obj" \
	"$(INTDIR)\qguardedptr.obj" \
	"$(INTDIR)\qgvector.obj" \
	"$(INTDIR)\qhbox.obj" \
	"$(INTDIR)\qhbuttongroup.obj" \
	"$(INTDIR)\qheader.obj" \
	"$(INTDIR)\qhgroupbox.obj" \
	"$(INTDIR)\qhostaddress.obj" \
	"$(INTDIR)\qhttp.obj" \
	"$(INTDIR)\qiconset.obj" \
	"$(INTDIR)\qiconview.obj" \
	"$(INTDIR)\qimage.obj" \
	"$(INTDIR)\qimageformatplugin.obj" \
	"$(INTDIR)\qinputcontext_win.obj" \
	"$(INTDIR)\qinputdialog.obj" \
	"$(INTDIR)\qinternal.obj" \
	"$(INTDIR)\qiodevice.obj" \
	"$(INTDIR)\qisciicodec.obj" \
	"$(INTDIR)\qjiscodec.obj" \
	"$(INTDIR)\qjpegio.obj" \
	"$(INTDIR)\qjpunicode.obj" \
	"$(INTDIR)\qkeysequence.obj" \
	"$(INTDIR)\qlabel.obj" \
	"$(INTDIR)\qlayout.obj" \
	"$(INTDIR)\qlayoutengine.obj" \
	"$(INTDIR)\qlcdnumber.obj" \
	"$(INTDIR)\qlibrary.obj" \
	"$(INTDIR)\qlibrary_win.obj" \
	"$(INTDIR)\qlineedit.obj" \
	"$(INTDIR)\qlistbox.obj" \
	"$(INTDIR)\qlistview.obj" \
	"$(INTDIR)\qlocale.obj" \
	"$(INTDIR)\qlocalfs.obj" \
	"$(INTDIR)\qmainwindow.obj" \
	"$(INTDIR)\qmap.obj" \
	"$(INTDIR)\qmenubar.obj" \
	"$(INTDIR)\qmenudata.obj" \
	"$(INTDIR)\qmessagebox.obj" \
	"$(INTDIR)\qmetaobject.obj" \
	"$(INTDIR)\qmime.obj" \
	"$(INTDIR)\qmime_win.obj" \
	"$(INTDIR)\qmngio.obj" \
	"$(INTDIR)\qmovie.obj" \
	"$(INTDIR)\qmultilineedit.obj" \
	"$(INTDIR)\qmutex_win.obj" \
	"$(INTDIR)\qmutexpool.obj" \
	"$(INTDIR)\qnetwork.obj" \
	"$(INTDIR)\qnetworkprotocol.obj" \
	"$(INTDIR)\qobject.obj" \
	"$(INTDIR)\qobjectcleanuphandler.obj" \
	"$(INTDIR)\qole_win.obj" \
	"$(INTDIR)\qpaintdevice_win.obj" \
	"$(INTDIR)\qpaintdevicemetrics.obj" \
	"$(INTDIR)\qpainter.obj" \
	"$(INTDIR)\qpainter_win.obj" \
	"$(INTDIR)\qpalette.obj" \
	"$(INTDIR)\qpicture.obj" \
	"$(INTDIR)\qpixmap.obj" \
	"$(INTDIR)\qpixmap_win.obj" \
	"$(INTDIR)\qpixmapcache.obj" \
	"$(INTDIR)\qpngio.obj" \
	"$(INTDIR)\qpoint.obj" \
	"$(INTDIR)\qpointarray.obj" \
	"$(INTDIR)\qpolygonscanner.obj" \
	"$(INTDIR)\qpopupmenu.obj" \
	"$(INTDIR)\qprinter.obj" \
	"$(INTDIR)\qprinter_win.obj" \
	"$(INTDIR)\qprocess.obj" \
	"$(INTDIR)\qprocess_win.obj" \
	"$(INTDIR)\qprogressbar.obj" \
	"$(INTDIR)\qprogressdialog.obj" \
	"$(INTDIR)\qptrcollection.obj" \
	"$(INTDIR)\qpushbutton.obj" \
	"$(INTDIR)\qradiobutton.obj" \
	"$(INTDIR)\qrangecontrol.obj" \
	"$(INTDIR)\qrect.obj" \
	"$(INTDIR)\qregexp.obj" \
	"$(INTDIR)\qregion.obj" \
	"$(INTDIR)\qregion_win.obj" \
	"$(INTDIR)\qrichtext.obj" \
	"$(INTDIR)\qrichtext_p.obj" \
	"$(INTDIR)\qrtlcodec.obj" \
	"$(INTDIR)\qscriptengine.obj" \
	"$(INTDIR)\qscrollbar.obj" \
	"$(INTDIR)\qscrollview.obj" \
	"$(INTDIR)\qsemaphore.obj" \
	"$(INTDIR)\qserversocket.obj" \
	"$(INTDIR)\qsettings.obj" \
	"$(INTDIR)\qsettings_win.obj" \
	"$(INTDIR)\qsignal.obj" \
	"$(INTDIR)\qsignalmapper.obj" \
	"$(INTDIR)\qsimplerichtext.obj" \
	"$(INTDIR)\qsize.obj" \
	"$(INTDIR)\qsizegrip.obj" \
	"$(INTDIR)\qsjiscodec.obj" \
	"$(INTDIR)\qslider.obj" \
	"$(INTDIR)\qsocket.obj" \
	"$(INTDIR)\qsocketdevice.obj" \
	"$(INTDIR)\qsocketdevice_win.obj" \
	"$(INTDIR)\qsocketnotifier.obj" \
	"$(INTDIR)\qsound.obj" \
	"$(INTDIR)\qsound_win.obj" \
	"$(INTDIR)\qspinbox.obj" \
	"$(INTDIR)\qspinwidget.obj" \
	"$(INTDIR)\qsplashscreen.obj" \
	"$(INTDIR)\qsplitter.obj" \
	"$(INTDIR)\qsql_odbc.obj" \
	"$(INTDIR)\qsqlcachedresult.obj" \
	"$(INTDIR)\qsqlcursor.obj" \
	"$(INTDIR)\qsqldatabase.obj" \
	"$(INTDIR)\qsqldriver.obj" \
	"$(INTDIR)\qsqldriverplugin.obj" \
	"$(INTDIR)\qsqleditorfactory.obj" \
	"$(INTDIR)\qsqlerror.obj" \
	"$(INTDIR)\qsqlextension_p.obj" \
	"$(INTDIR)\qsqlfield.obj" \
	"$(INTDIR)\qsqlform.obj" \
	"$(INTDIR)\qsqlindex.obj" \
	"$(INTDIR)\qsqlmanager_p.obj" \
	"$(INTDIR)\qsqlpropertymap.obj" \
	"$(INTDIR)\qsqlquery.obj" \
	"$(INTDIR)\qsqlrecord.obj" \
	"$(INTDIR)\qsqlresult.obj" \
	"$(INTDIR)\qsqlselectcursor.obj" \
	"$(INTDIR)\qstatusbar.obj" \
	"$(INTDIR)\qstring.obj" \
	"$(INTDIR)\qstringlist.obj" \
	"$(INTDIR)\qstyle.obj" \
	"$(INTDIR)\qstylefactory.obj" \
	"$(INTDIR)\qstyleplugin.obj" \
	"$(INTDIR)\qstylesheet.obj" \
	"$(INTDIR)\qsvgdevice.obj" \
	"$(INTDIR)\qsyntaxhighlighter.obj" \
	"$(INTDIR)\qtabbar.obj" \
	"$(INTDIR)\qtabdialog.obj" \
	"$(INTDIR)\qtable.obj" \
	"$(INTDIR)\qtabwidget.obj" \
	"$(INTDIR)\qtextbrowser.obj" \
	"$(INTDIR)\qtextcodec.obj" \
	"$(INTDIR)\qtextcodecfactory.obj" \
	"$(INTDIR)\qtextcodecplugin.obj" \
	"$(INTDIR)\qtextedit.obj" \
	"$(INTDIR)\qtextengine.obj" \
	"$(INTDIR)\qtextlayout.obj" \
	"$(INTDIR)\qtextstream.obj" \
	"$(INTDIR)\qtextview.obj" \
	"$(INTDIR)\qthread.obj" \
	"$(INTDIR)\qthread_win.obj" \
	"$(INTDIR)\qthreadstorage_win.obj" \
	"$(INTDIR)\qtimer.obj" \
	"$(INTDIR)\qtitlebar.obj" \
	"$(INTDIR)\qtoolbar.obj" \
	"$(INTDIR)\qtoolbox.obj" \
	"$(INTDIR)\qtoolbutton.obj" \
	"$(INTDIR)\qtooltip.obj" \
	"$(INTDIR)\qtranslator.obj" \
	"$(INTDIR)\qtsciicodec.obj" \
	"$(INTDIR)\qucom.obj" \
	"$(INTDIR)\qucomextra.obj" \
	"$(INTDIR)\qunicodetables.obj" \
	"$(INTDIR)\qurl.obj" \
	"$(INTDIR)\qurlinfo.obj" \
	"$(INTDIR)\qurloperator.obj" \
	"$(INTDIR)\qutfcodec.obj" \
	"$(INTDIR)\quuid.obj" \
	"$(INTDIR)\qvalidator.obj" \
	"$(INTDIR)\qvariant.obj" \
	"$(INTDIR)\qvbox.obj" \
	"$(INTDIR)\qvbuttongroup.obj" \
	"$(INTDIR)\qvgroupbox.obj" \
	"$(INTDIR)\qwaitcondition_win.obj" \
	"$(INTDIR)\qwhatsthis.obj" \
	"$(INTDIR)\qwidget.obj" \
	"$(INTDIR)\qwidget_win.obj" \
	"$(INTDIR)\qwidgetplugin.obj" \
	"$(INTDIR)\qwidgetresizehandler.obj" \
	"$(INTDIR)\qwidgetstack.obj" \
	"$(INTDIR)\qwindowsstyle.obj" \
	"$(INTDIR)\qwizard.obj" \
	"$(INTDIR)\qwmatrix.obj" \
	"$(INTDIR)\qworkspace.obj" \
	"$(INTDIR)\qxml.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\moc_qaccel.obj" \
	"$(INTDIR)\moc_qaction.obj" \
	"$(INTDIR)\moc_qapplication.obj" \
	"$(INTDIR)\moc_qasyncio.obj" \
	"$(INTDIR)\moc_qbutton.obj" \
	"$(INTDIR)\moc_qbuttongroup.obj" \
	"$(INTDIR)\moc_qcanvas.obj" \
	"$(INTDIR)\moc_qcheckbox.obj" \
	"$(INTDIR)\moc_qclipboard.obj" \
	"$(INTDIR)\moc_qcolordialog.obj" \
	"$(INTDIR)\moc_qcombobox.obj" \
	"$(INTDIR)\moc_qcommonstyle.obj" \
	"$(INTDIR)\moc_qdatabrowser.obj" \
	"$(INTDIR)\moc_qdatatable.obj" \
	"$(INTDIR)\moc_qdataview.obj" \
	"$(INTDIR)\moc_qdatetimeedit.obj" \
	"$(INTDIR)\moc_qdesktopwidget.obj" \
	"$(INTDIR)\moc_qdial.obj" \
	"$(INTDIR)\moc_qdialog.obj" \
	"$(INTDIR)\moc_qdialogbuttons_p.obj" \
	"$(INTDIR)\moc_qdns.obj" \
	"$(INTDIR)\moc_qdockarea.obj" \
	"$(INTDIR)\moc_qdockwindow.obj" \
	"$(INTDIR)\moc_qdragobject.obj" \
	"$(INTDIR)\moc_qerrormessage.obj" \
	"$(INTDIR)\moc_qeventloop.obj" \
	"$(INTDIR)\moc_qfiledialog.obj" \
	"$(INTDIR)\moc_qfontdialog.obj" \
	"$(INTDIR)\moc_qframe.obj" \
	"$(INTDIR)\moc_qftp.obj" \
	"$(INTDIR)\moc_qgl.obj" \
	"$(INTDIR)\moc_qgplugin.obj" \
	"$(INTDIR)\moc_qgrid.obj" \
	"$(INTDIR)\moc_qgridview.obj" \
	"$(INTDIR)\moc_qgroupbox.obj" \
	"$(INTDIR)\moc_qguardedptr.obj" \
	"$(INTDIR)\moc_qhbox.obj" \
	"$(INTDIR)\moc_qhbuttongroup.obj" \
	"$(INTDIR)\moc_qheader.obj" \
	"$(INTDIR)\moc_qhgroupbox.obj" \
	"$(INTDIR)\moc_qhttp.obj" \
	"$(INTDIR)\moc_qiconview.obj" \
	"$(INTDIR)\moc_qimageformatplugin.obj" \
	"$(INTDIR)\moc_qinputdialog.obj" \
	"$(INTDIR)\moc_qlabel.obj" \
	"$(INTDIR)\moc_qlayout.obj" \
	"$(INTDIR)\moc_qlcdnumber.obj" \
	"$(INTDIR)\moc_qlineedit.obj" \
	"$(INTDIR)\moc_qlistbox.obj" \
	"$(INTDIR)\moc_qlistview.obj" \
	"$(INTDIR)\moc_qlocalfs.obj" \
	"$(INTDIR)\moc_qmainwindow.obj" \
	"$(INTDIR)\moc_qmenubar.obj" \
	"$(INTDIR)\moc_qmessagebox.obj" \
	"$(INTDIR)\moc_qmultilineedit.obj" \
	"$(INTDIR)\moc_qnetworkprotocol.obj" \
	"$(INTDIR)\moc_qobject.obj" \
	"$(INTDIR)\moc_qobjectcleanuphandler.obj" \
	"$(INTDIR)\moc_qpopupmenu.obj" \
	"$(INTDIR)\moc_qprocess.obj" \
	"$(INTDIR)\moc_qprogressbar.obj" \
	"$(INTDIR)\moc_qprogressdialog.obj" \
	"$(INTDIR)\moc_qpushbutton.obj" \
	"$(INTDIR)\moc_qradiobutton.obj" \
	"$(INTDIR)\moc_qrangecontrol.obj" \
	"$(INTDIR)\moc_qrichtext_p.obj" \
	"$(INTDIR)\moc_qscrollbar.obj" \
	"$(INTDIR)\moc_qscrollview.obj" \
	"$(INTDIR)\moc_qsemimodal.obj" \
	"$(INTDIR)\moc_qserversocket.obj" \
	"$(INTDIR)\moc_qsessionmanager.obj" \
	"$(INTDIR)\moc_qsignal.obj" \
	"$(INTDIR)\moc_qsignalmapper.obj" \
	"$(INTDIR)\moc_qsizegrip.obj" \
	"$(INTDIR)\moc_qslider.obj" \
	"$(INTDIR)\moc_qsocket.obj" \
	"$(INTDIR)\moc_qsocketnotifier.obj" \
	"$(INTDIR)\moc_qsound.obj" \
	"$(INTDIR)\moc_qspinbox.obj" \
	"$(INTDIR)\moc_qsplashscreen.obj" \
	"$(INTDIR)\moc_qsplitter.obj" \
	"$(INTDIR)\moc_qsqldatabase.obj" \
	"$(INTDIR)\moc_qsqldriver.obj" \
	"$(INTDIR)\moc_qsqldriverplugin.obj" \
	"$(INTDIR)\moc_qsqlform.obj" \
	"$(INTDIR)\moc_qsqlquery.obj" \
	"$(INTDIR)\moc_qstatusbar.obj" \
	"$(INTDIR)\moc_qstyle.obj" \
	"$(INTDIR)\moc_qstyleplugin.obj" \
	"$(INTDIR)\moc_qstylesheet.obj" \
	"$(INTDIR)\moc_qtabbar.obj" \
	"$(INTDIR)\moc_qtabdialog.obj" \
	"$(INTDIR)\moc_qtable.obj" \
	"$(INTDIR)\moc_qtabwidget.obj" \
	"$(INTDIR)\moc_qtextbrowser.obj" \
	"$(INTDIR)\moc_qtextcodecplugin.obj" \
	"$(INTDIR)\moc_qtextedit.obj" \
	"$(INTDIR)\moc_qtextview.obj" \
	"$(INTDIR)\moc_qtimer.obj" \
	"$(INTDIR)\moc_qtitlebar_p.obj" \
	"$(INTDIR)\moc_qtoolbar.obj" \
	"$(INTDIR)\moc_qtoolbox.obj" \
	"$(INTDIR)\moc_qtoolbutton.obj" \
	"$(INTDIR)\moc_qtooltip.obj" \
	"$(INTDIR)\moc_qtranslator.obj" \
	"$(INTDIR)\moc_qurloperator.obj" \
	"$(INTDIR)\moc_qvalidator.obj" \
	"$(INTDIR)\moc_qvbox.obj" \
	"$(INTDIR)\moc_qvbuttongroup.obj" \
	"$(INTDIR)\moc_qvgroupbox.obj" \
	"$(INTDIR)\moc_qwidget.obj" \
	"$(INTDIR)\moc_qwidgetplugin.obj" \
	"$(INTDIR)\moc_qwidgetresizehandler_p.obj" \
	"$(INTDIR)\moc_qwidgetstack.obj" \
	"$(INTDIR)\moc_qwindowsstyle.obj" \
	"$(INTDIR)\moc_qwizard.obj" \
	"$(INTDIR)\moc_qworkspace.obj" \
	"$(INTDIR)\qt.res"

"..\lib\qt-mt334.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\Qt\3.3.4\lib\qt-mt334.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Post Build Step
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\lib\qt-mt334.dll"
   copy "\Qt\3.3.4\lib\qt-mt334.dll" "..\bin"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

OUTDIR=Debug
INTDIR=Debug

ALL : "tmp\moc\release_mt_shared\qworkspace.moc" "tmp\moc\release_mt_shared\qwhatsthis.moc" "tmp\moc\release_mt_shared\qtooltip.moc" "tmp\moc\release_mt_shared\qtoolbar.moc" "tmp\moc\release_mt_shared\qtable.moc" "tmp\moc\release_mt_shared\qsplitter.moc" "tmp\moc\release_mt_shared\qsound_win.moc" "tmp\moc\release_mt_shared\qscrollview.moc" "tmp\moc\release_mt_shared\qmovie.moc" "tmp\moc\release_mt_shared\qmessagebox.moc" "tmp\moc\release_mt_shared\qmainwindow.moc" "tmp\moc\release_mt_shared\qftp.moc" "tmp\moc\release_mt_shared\qfiledialog.moc" "tmp\moc\release_mt_shared\qeffects.moc" "tmp\moc\release_mt_shared\qdockwindow.moc" "tmp\moc\release_mt_shared\qdatetimeedit.moc" "tmp\moc\release_mt_shared\qcolordialog.moc" "..\lib\qt-mt334.dll"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\jcapimin.obj"
	-@erase "$(INTDIR)\jcapistd.obj"
	-@erase "$(INTDIR)\jccoefct.obj"
	-@erase "$(INTDIR)\jccolor.obj"
	-@erase "$(INTDIR)\jcdctmgr.obj"
	-@erase "$(INTDIR)\jchuff.obj"
	-@erase "$(INTDIR)\jcinit.obj"
	-@erase "$(INTDIR)\jcmainct.obj"
	-@erase "$(INTDIR)\jcmarker.obj"
	-@erase "$(INTDIR)\jcmaster.obj"
	-@erase "$(INTDIR)\jcomapi.obj"
	-@erase "$(INTDIR)\jcparam.obj"
	-@erase "$(INTDIR)\jcphuff.obj"
	-@erase "$(INTDIR)\jcprepct.obj"
	-@erase "$(INTDIR)\jcsample.obj"
	-@erase "$(INTDIR)\jctrans.obj"
	-@erase "$(INTDIR)\jdapimin.obj"
	-@erase "$(INTDIR)\jdapistd.obj"
	-@erase "$(INTDIR)\jdatadst.obj"
	-@erase "$(INTDIR)\jdatasrc.obj"
	-@erase "$(INTDIR)\jdcoefct.obj"
	-@erase "$(INTDIR)\jdcolor.obj"
	-@erase "$(INTDIR)\jddctmgr.obj"
	-@erase "$(INTDIR)\jdhuff.obj"
	-@erase "$(INTDIR)\jdinput.obj"
	-@erase "$(INTDIR)\jdmainct.obj"
	-@erase "$(INTDIR)\jdmarker.obj"
	-@erase "$(INTDIR)\jdmaster.obj"
	-@erase "$(INTDIR)\jdmerge.obj"
	-@erase "$(INTDIR)\jdphuff.obj"
	-@erase "$(INTDIR)\jdpostct.obj"
	-@erase "$(INTDIR)\jdsample.obj"
	-@erase "$(INTDIR)\jdtrans.obj"
	-@erase "$(INTDIR)\jerror.obj"
	-@erase "$(INTDIR)\jfdctflt.obj"
	-@erase "$(INTDIR)\jfdctfst.obj"
	-@erase "$(INTDIR)\jfdctint.obj"
	-@erase "$(INTDIR)\jidctflt.obj"
	-@erase "$(INTDIR)\jidctfst.obj"
	-@erase "$(INTDIR)\jidctint.obj"
	-@erase "$(INTDIR)\jidctred.obj"
	-@erase "$(INTDIR)\jmemmgr.obj"
	-@erase "$(INTDIR)\jmemnobs.obj"
	-@erase "$(INTDIR)\jquant1.obj"
	-@erase "$(INTDIR)\jquant2.obj"
	-@erase "$(INTDIR)\jutils.obj"
	-@erase "$(INTDIR)\moc_qaccel.obj"
	-@erase "$(INTDIR)\moc_qaction.obj"
	-@erase "$(INTDIR)\moc_qapplication.obj"
	-@erase "$(INTDIR)\moc_qasyncio.obj"
	-@erase "$(INTDIR)\moc_qbutton.obj"
	-@erase "$(INTDIR)\moc_qbuttongroup.obj"
	-@erase "$(INTDIR)\moc_qcanvas.obj"
	-@erase "$(INTDIR)\moc_qcheckbox.obj"
	-@erase "$(INTDIR)\moc_qclipboard.obj"
	-@erase "$(INTDIR)\moc_qcolordialog.obj"
	-@erase "$(INTDIR)\moc_qcombobox.obj"
	-@erase "$(INTDIR)\moc_qcommonstyle.obj"
	-@erase "$(INTDIR)\moc_qdatabrowser.obj"
	-@erase "$(INTDIR)\moc_qdatatable.obj"
	-@erase "$(INTDIR)\moc_qdataview.obj"
	-@erase "$(INTDIR)\moc_qdatetimeedit.obj"
	-@erase "$(INTDIR)\moc_qdesktopwidget.obj"
	-@erase "$(INTDIR)\moc_qdial.obj"
	-@erase "$(INTDIR)\moc_qdialog.obj"
	-@erase "$(INTDIR)\moc_qdialogbuttons_p.obj"
	-@erase "$(INTDIR)\moc_qdns.obj"
	-@erase "$(INTDIR)\moc_qdockarea.obj"
	-@erase "$(INTDIR)\moc_qdockwindow.obj"
	-@erase "$(INTDIR)\moc_qdragobject.obj"
	-@erase "$(INTDIR)\moc_qerrormessage.obj"
	-@erase "$(INTDIR)\moc_qeventloop.obj"
	-@erase "$(INTDIR)\moc_qfiledialog.obj"
	-@erase "$(INTDIR)\moc_qfontdialog.obj"
	-@erase "$(INTDIR)\moc_qframe.obj"
	-@erase "$(INTDIR)\moc_qftp.obj"
	-@erase "$(INTDIR)\moc_qgl.obj"
	-@erase "$(INTDIR)\moc_qgplugin.obj"
	-@erase "$(INTDIR)\moc_qgrid.obj"
	-@erase "$(INTDIR)\moc_qgridview.obj"
	-@erase "$(INTDIR)\moc_qgroupbox.obj"
	-@erase "$(INTDIR)\moc_qguardedptr.obj"
	-@erase "$(INTDIR)\moc_qhbox.obj"
	-@erase "$(INTDIR)\moc_qhbuttongroup.obj"
	-@erase "$(INTDIR)\moc_qheader.obj"
	-@erase "$(INTDIR)\moc_qhgroupbox.obj"
	-@erase "$(INTDIR)\moc_qhttp.obj"
	-@erase "$(INTDIR)\moc_qiconview.obj"
	-@erase "$(INTDIR)\moc_qimageformatplugin.obj"
	-@erase "$(INTDIR)\moc_qinputdialog.obj"
	-@erase "$(INTDIR)\moc_qlabel.obj"
	-@erase "$(INTDIR)\moc_qlayout.obj"
	-@erase "$(INTDIR)\moc_qlcdnumber.obj"
	-@erase "$(INTDIR)\moc_qlineedit.obj"
	-@erase "$(INTDIR)\moc_qlistbox.obj"
	-@erase "$(INTDIR)\moc_qlistview.obj"
	-@erase "$(INTDIR)\moc_qlocalfs.obj"
	-@erase "$(INTDIR)\moc_qmainwindow.obj"
	-@erase "$(INTDIR)\moc_qmenubar.obj"
	-@erase "$(INTDIR)\moc_qmessagebox.obj"
	-@erase "$(INTDIR)\moc_qmultilineedit.obj"
	-@erase "$(INTDIR)\moc_qnetworkprotocol.obj"
	-@erase "$(INTDIR)\moc_qobject.obj"
	-@erase "$(INTDIR)\moc_qobjectcleanuphandler.obj"
	-@erase "$(INTDIR)\moc_qpopupmenu.obj"
	-@erase "$(INTDIR)\moc_qprocess.obj"
	-@erase "$(INTDIR)\moc_qprogressbar.obj"
	-@erase "$(INTDIR)\moc_qprogressdialog.obj"
	-@erase "$(INTDIR)\moc_qpushbutton.obj"
	-@erase "$(INTDIR)\moc_qradiobutton.obj"
	-@erase "$(INTDIR)\moc_qrangecontrol.obj"
	-@erase "$(INTDIR)\moc_qrichtext_p.obj"
	-@erase "$(INTDIR)\moc_qscrollbar.obj"
	-@erase "$(INTDIR)\moc_qscrollview.obj"
	-@erase "$(INTDIR)\moc_qsemimodal.obj"
	-@erase "$(INTDIR)\moc_qserversocket.obj"
	-@erase "$(INTDIR)\moc_qsessionmanager.obj"
	-@erase "$(INTDIR)\moc_qsignal.obj"
	-@erase "$(INTDIR)\moc_qsignalmapper.obj"
	-@erase "$(INTDIR)\moc_qsizegrip.obj"
	-@erase "$(INTDIR)\moc_qslider.obj"
	-@erase "$(INTDIR)\moc_qsocket.obj"
	-@erase "$(INTDIR)\moc_qsocketnotifier.obj"
	-@erase "$(INTDIR)\moc_qsound.obj"
	-@erase "$(INTDIR)\moc_qspinbox.obj"
	-@erase "$(INTDIR)\moc_qsplashscreen.obj"
	-@erase "$(INTDIR)\moc_qsplitter.obj"
	-@erase "$(INTDIR)\moc_qsqldatabase.obj"
	-@erase "$(INTDIR)\moc_qsqldriver.obj"
	-@erase "$(INTDIR)\moc_qsqldriverplugin.obj"
	-@erase "$(INTDIR)\moc_qsqlform.obj"
	-@erase "$(INTDIR)\moc_qsqlquery.obj"
	-@erase "$(INTDIR)\moc_qstatusbar.obj"
	-@erase "$(INTDIR)\moc_qstyle.obj"
	-@erase "$(INTDIR)\moc_qstyleplugin.obj"
	-@erase "$(INTDIR)\moc_qstylesheet.obj"
	-@erase "$(INTDIR)\moc_qtabbar.obj"
	-@erase "$(INTDIR)\moc_qtabdialog.obj"
	-@erase "$(INTDIR)\moc_qtable.obj"
	-@erase "$(INTDIR)\moc_qtabwidget.obj"
	-@erase "$(INTDIR)\moc_qtextbrowser.obj"
	-@erase "$(INTDIR)\moc_qtextcodecplugin.obj"
	-@erase "$(INTDIR)\moc_qtextedit.obj"
	-@erase "$(INTDIR)\moc_qtextview.obj"
	-@erase "$(INTDIR)\moc_qtimer.obj"
	-@erase "$(INTDIR)\moc_qtitlebar_p.obj"
	-@erase "$(INTDIR)\moc_qtoolbar.obj"
	-@erase "$(INTDIR)\moc_qtoolbox.obj"
	-@erase "$(INTDIR)\moc_qtoolbutton.obj"
	-@erase "$(INTDIR)\moc_qtooltip.obj"
	-@erase "$(INTDIR)\moc_qtranslator.obj"
	-@erase "$(INTDIR)\moc_qurloperator.obj"
	-@erase "$(INTDIR)\moc_qvalidator.obj"
	-@erase "$(INTDIR)\moc_qvbox.obj"
	-@erase "$(INTDIR)\moc_qvbuttongroup.obj"
	-@erase "$(INTDIR)\moc_qvgroupbox.obj"
	-@erase "$(INTDIR)\moc_qwidget.obj"
	-@erase "$(INTDIR)\moc_qwidgetplugin.obj"
	-@erase "$(INTDIR)\moc_qwidgetresizehandler_p.obj"
	-@erase "$(INTDIR)\moc_qwidgetstack.obj"
	-@erase "$(INTDIR)\moc_qwindowsstyle.obj"
	-@erase "$(INTDIR)\moc_qwizard.obj"
	-@erase "$(INTDIR)\moc_qworkspace.obj"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\qabstractlayout.obj"
	-@erase "$(INTDIR)\qaccel.obj"
	-@erase "$(INTDIR)\qaccessible.obj"
	-@erase "$(INTDIR)\qaccessible_win.obj"
	-@erase "$(INTDIR)\qaction.obj"
	-@erase "$(INTDIR)\qapplication.obj"
	-@erase "$(INTDIR)\qapplication_win.obj"
	-@erase "$(INTDIR)\qasyncimageio.obj"
	-@erase "$(INTDIR)\qasyncio.obj"
	-@erase "$(INTDIR)\qbig5codec.obj"
	-@erase "$(INTDIR)\qbitarray.obj"
	-@erase "$(INTDIR)\qbitmap.obj"
	-@erase "$(INTDIR)\qbuffer.obj"
	-@erase "$(INTDIR)\qbutton.obj"
	-@erase "$(INTDIR)\qbuttongroup.obj"
	-@erase "$(INTDIR)\qcanvas.obj"
	-@erase "$(INTDIR)\qcheckbox.obj"
	-@erase "$(INTDIR)\qclipboard.obj"
	-@erase "$(INTDIR)\qclipboard_win.obj"
	-@erase "$(INTDIR)\qcolor.obj"
	-@erase "$(INTDIR)\qcolor_p.obj"
	-@erase "$(INTDIR)\qcolor_win.obj"
	-@erase "$(INTDIR)\qcolordialog.obj"
	-@erase "$(INTDIR)\qcombobox.obj"
	-@erase "$(INTDIR)\qcomlibrary.obj"
	-@erase "$(INTDIR)\qcommonstyle.obj"
	-@erase "$(INTDIR)\qcomponentfactory.obj"
	-@erase "$(INTDIR)\qconfig.obj"
	-@erase "$(INTDIR)\qconnection.obj"
	-@erase "$(INTDIR)\qcriticalsection_p.obj"
	-@erase "$(INTDIR)\qcstring.obj"
	-@erase "$(INTDIR)\qcursor.obj"
	-@erase "$(INTDIR)\qcursor_win.obj"
	-@erase "$(INTDIR)\qdatabrowser.obj"
	-@erase "$(INTDIR)\qdatastream.obj"
	-@erase "$(INTDIR)\qdatatable.obj"
	-@erase "$(INTDIR)\qdataview.obj"
	-@erase "$(INTDIR)\qdatetime.obj"
	-@erase "$(INTDIR)\qdatetimeedit.obj"
	-@erase "$(INTDIR)\qdeepcopy.obj"
	-@erase "$(INTDIR)\qdesktopwidget_win.obj"
	-@erase "$(INTDIR)\qdial.obj"
	-@erase "$(INTDIR)\qdialog.obj"
	-@erase "$(INTDIR)\qdialogbuttons.obj"
	-@erase "$(INTDIR)\qdir.obj"
	-@erase "$(INTDIR)\qdir_win.obj"
	-@erase "$(INTDIR)\qdnd_win.obj"
	-@erase "$(INTDIR)\qdns.obj"
	-@erase "$(INTDIR)\qdockarea.obj"
	-@erase "$(INTDIR)\qdockwindow.obj"
	-@erase "$(INTDIR)\qdom.obj"
	-@erase "$(INTDIR)\qdragobject.obj"
	-@erase "$(INTDIR)\qdrawutil.obj"
	-@erase "$(INTDIR)\qdropsite.obj"
	-@erase "$(INTDIR)\qeditorfactory.obj"
	-@erase "$(INTDIR)\qeffects.obj"
	-@erase "$(INTDIR)\qerrormessage.obj"
	-@erase "$(INTDIR)\qeucjpcodec.obj"
	-@erase "$(INTDIR)\qeuckrcodec.obj"
	-@erase "$(INTDIR)\qevent.obj"
	-@erase "$(INTDIR)\qeventloop.obj"
	-@erase "$(INTDIR)\qeventloop_win.obj"
	-@erase "$(INTDIR)\qfile.obj"
	-@erase "$(INTDIR)\qfile_win.obj"
	-@erase "$(INTDIR)\qfiledialog.obj"
	-@erase "$(INTDIR)\qfiledialog_win.obj"
	-@erase "$(INTDIR)\qfileinfo.obj"
	-@erase "$(INTDIR)\qfileinfo_win.obj"
	-@erase "$(INTDIR)\qfocusdata.obj"
	-@erase "$(INTDIR)\qfont.obj"
	-@erase "$(INTDIR)\qfont_win.obj"
	-@erase "$(INTDIR)\qfontdatabase.obj"
	-@erase "$(INTDIR)\qfontdialog.obj"
	-@erase "$(INTDIR)\qfontengine_win.obj"
	-@erase "$(INTDIR)\qframe.obj"
	-@erase "$(INTDIR)\qftp.obj"
	-@erase "$(INTDIR)\qgarray.obj"
	-@erase "$(INTDIR)\qgb18030codec.obj"
	-@erase "$(INTDIR)\qgcache.obj"
	-@erase "$(INTDIR)\qgdict.obj"
	-@erase "$(INTDIR)\qgl.obj"
	-@erase "$(INTDIR)\qgl_win.obj"
	-@erase "$(INTDIR)\qglcolormap.obj"
	-@erase "$(INTDIR)\qglist.obj"
	-@erase "$(INTDIR)\qglobal.obj"
	-@erase "$(INTDIR)\qgplugin.obj"
	-@erase "$(INTDIR)\qgpluginmanager.obj"
	-@erase "$(INTDIR)\qgrid.obj"
	-@erase "$(INTDIR)\qgridview.obj"
	-@erase "$(INTDIR)\qgroupbox.obj"
	-@erase "$(INTDIR)\qguardedptr.obj"
	-@erase "$(INTDIR)\qgvector.obj"
	-@erase "$(INTDIR)\qhbox.obj"
	-@erase "$(INTDIR)\qhbuttongroup.obj"
	-@erase "$(INTDIR)\qheader.obj"
	-@erase "$(INTDIR)\qhgroupbox.obj"
	-@erase "$(INTDIR)\qhostaddress.obj"
	-@erase "$(INTDIR)\qhttp.obj"
	-@erase "$(INTDIR)\qiconset.obj"
	-@erase "$(INTDIR)\qiconview.obj"
	-@erase "$(INTDIR)\qimage.obj"
	-@erase "$(INTDIR)\qimageformatplugin.obj"
	-@erase "$(INTDIR)\qinputcontext_win.obj"
	-@erase "$(INTDIR)\qinputdialog.obj"
	-@erase "$(INTDIR)\qinternal.obj"
	-@erase "$(INTDIR)\qiodevice.obj"
	-@erase "$(INTDIR)\qisciicodec.obj"
	-@erase "$(INTDIR)\qjiscodec.obj"
	-@erase "$(INTDIR)\qjpegio.obj"
	-@erase "$(INTDIR)\qjpunicode.obj"
	-@erase "$(INTDIR)\qkeysequence.obj"
	-@erase "$(INTDIR)\qlabel.obj"
	-@erase "$(INTDIR)\qlayout.obj"
	-@erase "$(INTDIR)\qlayoutengine.obj"
	-@erase "$(INTDIR)\qlcdnumber.obj"
	-@erase "$(INTDIR)\qlibrary.obj"
	-@erase "$(INTDIR)\qlibrary_win.obj"
	-@erase "$(INTDIR)\qlineedit.obj"
	-@erase "$(INTDIR)\qlistbox.obj"
	-@erase "$(INTDIR)\qlistview.obj"
	-@erase "$(INTDIR)\qlocale.obj"
	-@erase "$(INTDIR)\qlocalfs.obj"
	-@erase "$(INTDIR)\qmainwindow.obj"
	-@erase "$(INTDIR)\qmap.obj"
	-@erase "$(INTDIR)\qmenubar.obj"
	-@erase "$(INTDIR)\qmenudata.obj"
	-@erase "$(INTDIR)\qmessagebox.obj"
	-@erase "$(INTDIR)\qmetaobject.obj"
	-@erase "$(INTDIR)\qmime.obj"
	-@erase "$(INTDIR)\qmime_win.obj"
	-@erase "$(INTDIR)\qmngio.obj"
	-@erase "$(INTDIR)\qmovie.obj"
	-@erase "$(INTDIR)\qmultilineedit.obj"
	-@erase "$(INTDIR)\qmutex_win.obj"
	-@erase "$(INTDIR)\qmutexpool.obj"
	-@erase "$(INTDIR)\qnetwork.obj"
	-@erase "$(INTDIR)\qnetworkprotocol.obj"
	-@erase "$(INTDIR)\qobject.obj"
	-@erase "$(INTDIR)\qobjectcleanuphandler.obj"
	-@erase "$(INTDIR)\qole_win.obj"
	-@erase "$(INTDIR)\qpaintdevice_win.obj"
	-@erase "$(INTDIR)\qpaintdevicemetrics.obj"
	-@erase "$(INTDIR)\qpainter.obj"
	-@erase "$(INTDIR)\qpainter_win.obj"
	-@erase "$(INTDIR)\qpalette.obj"
	-@erase "$(INTDIR)\qpicture.obj"
	-@erase "$(INTDIR)\qpixmap.obj"
	-@erase "$(INTDIR)\qpixmap_win.obj"
	-@erase "$(INTDIR)\qpixmapcache.obj"
	-@erase "$(INTDIR)\qpngio.obj"
	-@erase "$(INTDIR)\qpoint.obj"
	-@erase "$(INTDIR)\qpointarray.obj"
	-@erase "$(INTDIR)\qpolygonscanner.obj"
	-@erase "$(INTDIR)\qpopupmenu.obj"
	-@erase "$(INTDIR)\qprinter.obj"
	-@erase "$(INTDIR)\qprinter_win.obj"
	-@erase "$(INTDIR)\qprocess.obj"
	-@erase "$(INTDIR)\qprocess_win.obj"
	-@erase "$(INTDIR)\qprogressbar.obj"
	-@erase "$(INTDIR)\qprogressdialog.obj"
	-@erase "$(INTDIR)\qptrcollection.obj"
	-@erase "$(INTDIR)\qpushbutton.obj"
	-@erase "$(INTDIR)\qradiobutton.obj"
	-@erase "$(INTDIR)\qrangecontrol.obj"
	-@erase "$(INTDIR)\qrect.obj"
	-@erase "$(INTDIR)\qregexp.obj"
	-@erase "$(INTDIR)\qregion.obj"
	-@erase "$(INTDIR)\qregion_win.obj"
	-@erase "$(INTDIR)\qrichtext.obj"
	-@erase "$(INTDIR)\qrichtext_p.obj"
	-@erase "$(INTDIR)\qrtlcodec.obj"
	-@erase "$(INTDIR)\qscriptengine.obj"
	-@erase "$(INTDIR)\qscrollbar.obj"
	-@erase "$(INTDIR)\qscrollview.obj"
	-@erase "$(INTDIR)\qsemaphore.obj"
	-@erase "$(INTDIR)\qserversocket.obj"
	-@erase "$(INTDIR)\qsettings.obj"
	-@erase "$(INTDIR)\qsettings_win.obj"
	-@erase "$(INTDIR)\qsignal.obj"
	-@erase "$(INTDIR)\qsignalmapper.obj"
	-@erase "$(INTDIR)\qsimplerichtext.obj"
	-@erase "$(INTDIR)\qsize.obj"
	-@erase "$(INTDIR)\qsizegrip.obj"
	-@erase "$(INTDIR)\qsjiscodec.obj"
	-@erase "$(INTDIR)\qslider.obj"
	-@erase "$(INTDIR)\qsocket.obj"
	-@erase "$(INTDIR)\qsocketdevice.obj"
	-@erase "$(INTDIR)\qsocketdevice_win.obj"
	-@erase "$(INTDIR)\qsocketnotifier.obj"
	-@erase "$(INTDIR)\qsound.obj"
	-@erase "$(INTDIR)\qsound_win.obj"
	-@erase "$(INTDIR)\qspinbox.obj"
	-@erase "$(INTDIR)\qspinwidget.obj"
	-@erase "$(INTDIR)\qsplashscreen.obj"
	-@erase "$(INTDIR)\qsplitter.obj"
	-@erase "$(INTDIR)\qsql_odbc.obj"
	-@erase "$(INTDIR)\qsqlcachedresult.obj"
	-@erase "$(INTDIR)\qsqlcursor.obj"
	-@erase "$(INTDIR)\qsqldatabase.obj"
	-@erase "$(INTDIR)\qsqldriver.obj"
	-@erase "$(INTDIR)\qsqldriverplugin.obj"
	-@erase "$(INTDIR)\qsqleditorfactory.obj"
	-@erase "$(INTDIR)\qsqlerror.obj"
	-@erase "$(INTDIR)\qsqlextension_p.obj"
	-@erase "$(INTDIR)\qsqlfield.obj"
	-@erase "$(INTDIR)\qsqlform.obj"
	-@erase "$(INTDIR)\qsqlindex.obj"
	-@erase "$(INTDIR)\qsqlmanager_p.obj"
	-@erase "$(INTDIR)\qsqlpropertymap.obj"
	-@erase "$(INTDIR)\qsqlquery.obj"
	-@erase "$(INTDIR)\qsqlrecord.obj"
	-@erase "$(INTDIR)\qsqlresult.obj"
	-@erase "$(INTDIR)\qsqlselectcursor.obj"
	-@erase "$(INTDIR)\qstatusbar.obj"
	-@erase "$(INTDIR)\qstring.obj"
	-@erase "$(INTDIR)\qstringlist.obj"
	-@erase "$(INTDIR)\qstyle.obj"
	-@erase "$(INTDIR)\qstylefactory.obj"
	-@erase "$(INTDIR)\qstyleplugin.obj"
	-@erase "$(INTDIR)\qstylesheet.obj"
	-@erase "$(INTDIR)\qsvgdevice.obj"
	-@erase "$(INTDIR)\qsyntaxhighlighter.obj"
	-@erase "$(INTDIR)\qt.res"
	-@erase "$(INTDIR)\qtabbar.obj"
	-@erase "$(INTDIR)\qtabdialog.obj"
	-@erase "$(INTDIR)\qtable.obj"
	-@erase "$(INTDIR)\qtabwidget.obj"
	-@erase "$(INTDIR)\qtextbrowser.obj"
	-@erase "$(INTDIR)\qtextcodec.obj"
	-@erase "$(INTDIR)\qtextcodecfactory.obj"
	-@erase "$(INTDIR)\qtextcodecplugin.obj"
	-@erase "$(INTDIR)\qtextedit.obj"
	-@erase "$(INTDIR)\qtextengine.obj"
	-@erase "$(INTDIR)\qtextlayout.obj"
	-@erase "$(INTDIR)\qtextstream.obj"
	-@erase "$(INTDIR)\qtextview.obj"
	-@erase "$(INTDIR)\qthread.obj"
	-@erase "$(INTDIR)\qthread_win.obj"
	-@erase "$(INTDIR)\qthreadstorage_win.obj"
	-@erase "$(INTDIR)\qtimer.obj"
	-@erase "$(INTDIR)\qtitlebar.obj"
	-@erase "$(INTDIR)\qtoolbar.obj"
	-@erase "$(INTDIR)\qtoolbox.obj"
	-@erase "$(INTDIR)\qtoolbutton.obj"
	-@erase "$(INTDIR)\qtooltip.obj"
	-@erase "$(INTDIR)\qtranslator.obj"
	-@erase "$(INTDIR)\qtsciicodec.obj"
	-@erase "$(INTDIR)\qucom.obj"
	-@erase "$(INTDIR)\qucomextra.obj"
	-@erase "$(INTDIR)\qunicodetables.obj"
	-@erase "$(INTDIR)\qurl.obj"
	-@erase "$(INTDIR)\qurlinfo.obj"
	-@erase "$(INTDIR)\qurloperator.obj"
	-@erase "$(INTDIR)\qutfcodec.obj"
	-@erase "$(INTDIR)\quuid.obj"
	-@erase "$(INTDIR)\qvalidator.obj"
	-@erase "$(INTDIR)\qvariant.obj"
	-@erase "$(INTDIR)\qvbox.obj"
	-@erase "$(INTDIR)\qvbuttongroup.obj"
	-@erase "$(INTDIR)\qvgroupbox.obj"
	-@erase "$(INTDIR)\qwaitcondition_win.obj"
	-@erase "$(INTDIR)\qwhatsthis.obj"
	-@erase "$(INTDIR)\qwidget.obj"
	-@erase "$(INTDIR)\qwidget_win.obj"
	-@erase "$(INTDIR)\qwidgetplugin.obj"
	-@erase "$(INTDIR)\qwidgetresizehandler.obj"
	-@erase "$(INTDIR)\qwidgetstack.obj"
	-@erase "$(INTDIR)\qwindowsstyle.obj"
	-@erase "$(INTDIR)\qwizard.obj"
	-@erase "$(INTDIR)\qwmatrix.obj"
	-@erase "$(INTDIR)\qworkspace.obj"
	-@erase "$(INTDIR)\qxml.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\qt-mt334.pdb"
	-@erase "..\lib\qt-mt334.dll"
	-@erase "..\lib\qt-mt334.exp"
	-@erase "tmp\moc\release_mt_shared\qcolordialog.moc"
	-@erase "tmp\moc\release_mt_shared\qdatetimeedit.moc"
	-@erase "tmp\moc\release_mt_shared\qdockwindow.moc"
	-@erase "tmp\moc\release_mt_shared\qeffects.moc"
	-@erase "tmp\moc\release_mt_shared\qfiledialog.moc"
	-@erase "tmp\moc\release_mt_shared\qftp.moc"
	-@erase "tmp\moc\release_mt_shared\qmainwindow.moc"
	-@erase "tmp\moc\release_mt_shared\qmessagebox.moc"
	-@erase "tmp\moc\release_mt_shared\qmovie.moc"
	-@erase "tmp\moc\release_mt_shared\qscrollview.moc"
	-@erase "tmp\moc\release_mt_shared\qsound_win.moc"
	-@erase "tmp\moc\release_mt_shared\qsplitter.moc"
	-@erase "tmp\moc\release_mt_shared\qtable.moc"
	-@erase "tmp\moc\release_mt_shared\qtoolbar.moc"
	-@erase "tmp\moc\release_mt_shared\qtooltip.moc"
	-@erase "tmp\moc\release_mt_shared\qwhatsthis.moc"
	-@erase "tmp\moc\release_mt_shared\qworkspace.moc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /FI"qt_pch.h" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fp"$(INTDIR)\qt_mt.pch" /Yu"qt_pch.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\qt.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\qt_mt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS="opengl32.lib" "glu32.lib" "delayimp.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "odbc32.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "$(IntDir)\qt_mt.obj" /nologo /base:"0x39D00000" /version:3.34 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\qt-mt334.pdb" /debug /machine:IX86 /out:"C:\Qt\3.3.4\lib\qt-mt334.dll" /implib:"C:\Qt\3.3.4\lib\qt-mt334.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib" /DELAYLOAD:opengl32.dll 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\jcapimin.obj" \
	"$(INTDIR)\jcapistd.obj" \
	"$(INTDIR)\jccoefct.obj" \
	"$(INTDIR)\jccolor.obj" \
	"$(INTDIR)\jcdctmgr.obj" \
	"$(INTDIR)\jchuff.obj" \
	"$(INTDIR)\jcinit.obj" \
	"$(INTDIR)\jcmainct.obj" \
	"$(INTDIR)\jcmarker.obj" \
	"$(INTDIR)\jcmaster.obj" \
	"$(INTDIR)\jcomapi.obj" \
	"$(INTDIR)\jcparam.obj" \
	"$(INTDIR)\jcphuff.obj" \
	"$(INTDIR)\jcprepct.obj" \
	"$(INTDIR)\jcsample.obj" \
	"$(INTDIR)\jctrans.obj" \
	"$(INTDIR)\jdapimin.obj" \
	"$(INTDIR)\jdapistd.obj" \
	"$(INTDIR)\jdatadst.obj" \
	"$(INTDIR)\jdatasrc.obj" \
	"$(INTDIR)\jdcoefct.obj" \
	"$(INTDIR)\jdcolor.obj" \
	"$(INTDIR)\jddctmgr.obj" \
	"$(INTDIR)\jdhuff.obj" \
	"$(INTDIR)\jdinput.obj" \
	"$(INTDIR)\jdmainct.obj" \
	"$(INTDIR)\jdmarker.obj" \
	"$(INTDIR)\jdmaster.obj" \
	"$(INTDIR)\jdmerge.obj" \
	"$(INTDIR)\jdphuff.obj" \
	"$(INTDIR)\jdpostct.obj" \
	"$(INTDIR)\jdsample.obj" \
	"$(INTDIR)\jdtrans.obj" \
	"$(INTDIR)\jerror.obj" \
	"$(INTDIR)\jfdctflt.obj" \
	"$(INTDIR)\jfdctfst.obj" \
	"$(INTDIR)\jfdctint.obj" \
	"$(INTDIR)\jidctflt.obj" \
	"$(INTDIR)\jidctfst.obj" \
	"$(INTDIR)\jidctint.obj" \
	"$(INTDIR)\jidctred.obj" \
	"$(INTDIR)\jmemmgr.obj" \
	"$(INTDIR)\jmemnobs.obj" \
	"$(INTDIR)\jquant1.obj" \
	"$(INTDIR)\jquant2.obj" \
	"$(INTDIR)\jutils.obj" \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"$(INTDIR)\qabstractlayout.obj" \
	"$(INTDIR)\qaccel.obj" \
	"$(INTDIR)\qaccessible.obj" \
	"$(INTDIR)\qaccessible_win.obj" \
	"$(INTDIR)\qaction.obj" \
	"$(INTDIR)\qapplication.obj" \
	"$(INTDIR)\qapplication_win.obj" \
	"$(INTDIR)\qasyncimageio.obj" \
	"$(INTDIR)\qasyncio.obj" \
	"$(INTDIR)\qbig5codec.obj" \
	"$(INTDIR)\qbitarray.obj" \
	"$(INTDIR)\qbitmap.obj" \
	"$(INTDIR)\qbuffer.obj" \
	"$(INTDIR)\qbutton.obj" \
	"$(INTDIR)\qbuttongroup.obj" \
	"$(INTDIR)\qcanvas.obj" \
	"$(INTDIR)\qcheckbox.obj" \
	"$(INTDIR)\qclipboard.obj" \
	"$(INTDIR)\qclipboard_win.obj" \
	"$(INTDIR)\qcolor.obj" \
	"$(INTDIR)\qcolor_p.obj" \
	"$(INTDIR)\qcolor_win.obj" \
	"$(INTDIR)\qcolordialog.obj" \
	"$(INTDIR)\qcombobox.obj" \
	"$(INTDIR)\qcomlibrary.obj" \
	"$(INTDIR)\qcommonstyle.obj" \
	"$(INTDIR)\qcomponentfactory.obj" \
	"$(INTDIR)\qconfig.obj" \
	"$(INTDIR)\qconnection.obj" \
	"$(INTDIR)\qcriticalsection_p.obj" \
	"$(INTDIR)\qcstring.obj" \
	"$(INTDIR)\qcursor.obj" \
	"$(INTDIR)\qcursor_win.obj" \
	"$(INTDIR)\qdatabrowser.obj" \
	"$(INTDIR)\qdatastream.obj" \
	"$(INTDIR)\qdatatable.obj" \
	"$(INTDIR)\qdataview.obj" \
	"$(INTDIR)\qdatetime.obj" \
	"$(INTDIR)\qdatetimeedit.obj" \
	"$(INTDIR)\qdeepcopy.obj" \
	"$(INTDIR)\qdesktopwidget_win.obj" \
	"$(INTDIR)\qdial.obj" \
	"$(INTDIR)\qdialog.obj" \
	"$(INTDIR)\qdialogbuttons.obj" \
	"$(INTDIR)\qdir.obj" \
	"$(INTDIR)\qdir_win.obj" \
	"$(INTDIR)\qdnd_win.obj" \
	"$(INTDIR)\qdns.obj" \
	"$(INTDIR)\qdockarea.obj" \
	"$(INTDIR)\qdockwindow.obj" \
	"$(INTDIR)\qdom.obj" \
	"$(INTDIR)\qdragobject.obj" \
	"$(INTDIR)\qdrawutil.obj" \
	"$(INTDIR)\qdropsite.obj" \
	"$(INTDIR)\qeditorfactory.obj" \
	"$(INTDIR)\qeffects.obj" \
	"$(INTDIR)\qerrormessage.obj" \
	"$(INTDIR)\qeucjpcodec.obj" \
	"$(INTDIR)\qeuckrcodec.obj" \
	"$(INTDIR)\qevent.obj" \
	"$(INTDIR)\qeventloop.obj" \
	"$(INTDIR)\qeventloop_win.obj" \
	"$(INTDIR)\qfile.obj" \
	"$(INTDIR)\qfile_win.obj" \
	"$(INTDIR)\qfiledialog.obj" \
	"$(INTDIR)\qfiledialog_win.obj" \
	"$(INTDIR)\qfileinfo.obj" \
	"$(INTDIR)\qfileinfo_win.obj" \
	"$(INTDIR)\qfocusdata.obj" \
	"$(INTDIR)\qfont.obj" \
	"$(INTDIR)\qfont_win.obj" \
	"$(INTDIR)\qfontdatabase.obj" \
	"$(INTDIR)\qfontdialog.obj" \
	"$(INTDIR)\qfontengine_win.obj" \
	"$(INTDIR)\qframe.obj" \
	"$(INTDIR)\qftp.obj" \
	"$(INTDIR)\qgarray.obj" \
	"$(INTDIR)\qgb18030codec.obj" \
	"$(INTDIR)\qgcache.obj" \
	"$(INTDIR)\qgdict.obj" \
	"$(INTDIR)\qgl.obj" \
	"$(INTDIR)\qgl_win.obj" \
	"$(INTDIR)\qglcolormap.obj" \
	"$(INTDIR)\qglist.obj" \
	"$(INTDIR)\qglobal.obj" \
	"$(INTDIR)\qgplugin.obj" \
	"$(INTDIR)\qgpluginmanager.obj" \
	"$(INTDIR)\qgrid.obj" \
	"$(INTDIR)\qgridview.obj" \
	"$(INTDIR)\qgroupbox.obj" \
	"$(INTDIR)\qguardedptr.obj" \
	"$(INTDIR)\qgvector.obj" \
	"$(INTDIR)\qhbox.obj" \
	"$(INTDIR)\qhbuttongroup.obj" \
	"$(INTDIR)\qheader.obj" \
	"$(INTDIR)\qhgroupbox.obj" \
	"$(INTDIR)\qhostaddress.obj" \
	"$(INTDIR)\qhttp.obj" \
	"$(INTDIR)\qiconset.obj" \
	"$(INTDIR)\qiconview.obj" \
	"$(INTDIR)\qimage.obj" \
	"$(INTDIR)\qimageformatplugin.obj" \
	"$(INTDIR)\qinputcontext_win.obj" \
	"$(INTDIR)\qinputdialog.obj" \
	"$(INTDIR)\qinternal.obj" \
	"$(INTDIR)\qiodevice.obj" \
	"$(INTDIR)\qisciicodec.obj" \
	"$(INTDIR)\qjiscodec.obj" \
	"$(INTDIR)\qjpegio.obj" \
	"$(INTDIR)\qjpunicode.obj" \
	"$(INTDIR)\qkeysequence.obj" \
	"$(INTDIR)\qlabel.obj" \
	"$(INTDIR)\qlayout.obj" \
	"$(INTDIR)\qlayoutengine.obj" \
	"$(INTDIR)\qlcdnumber.obj" \
	"$(INTDIR)\qlibrary.obj" \
	"$(INTDIR)\qlibrary_win.obj" \
	"$(INTDIR)\qlineedit.obj" \
	"$(INTDIR)\qlistbox.obj" \
	"$(INTDIR)\qlistview.obj" \
	"$(INTDIR)\qlocale.obj" \
	"$(INTDIR)\qlocalfs.obj" \
	"$(INTDIR)\qmainwindow.obj" \
	"$(INTDIR)\qmap.obj" \
	"$(INTDIR)\qmenubar.obj" \
	"$(INTDIR)\qmenudata.obj" \
	"$(INTDIR)\qmessagebox.obj" \
	"$(INTDIR)\qmetaobject.obj" \
	"$(INTDIR)\qmime.obj" \
	"$(INTDIR)\qmime_win.obj" \
	"$(INTDIR)\qmngio.obj" \
	"$(INTDIR)\qmovie.obj" \
	"$(INTDIR)\qmultilineedit.obj" \
	"$(INTDIR)\qmutex_win.obj" \
	"$(INTDIR)\qmutexpool.obj" \
	"$(INTDIR)\qnetwork.obj" \
	"$(INTDIR)\qnetworkprotocol.obj" \
	"$(INTDIR)\qobject.obj" \
	"$(INTDIR)\qobjectcleanuphandler.obj" \
	"$(INTDIR)\qole_win.obj" \
	"$(INTDIR)\qpaintdevice_win.obj" \
	"$(INTDIR)\qpaintdevicemetrics.obj" \
	"$(INTDIR)\qpainter.obj" \
	"$(INTDIR)\qpainter_win.obj" \
	"$(INTDIR)\qpalette.obj" \
	"$(INTDIR)\qpicture.obj" \
	"$(INTDIR)\qpixmap.obj" \
	"$(INTDIR)\qpixmap_win.obj" \
	"$(INTDIR)\qpixmapcache.obj" \
	"$(INTDIR)\qpngio.obj" \
	"$(INTDIR)\qpoint.obj" \
	"$(INTDIR)\qpointarray.obj" \
	"$(INTDIR)\qpolygonscanner.obj" \
	"$(INTDIR)\qpopupmenu.obj" \
	"$(INTDIR)\qprinter.obj" \
	"$(INTDIR)\qprinter_win.obj" \
	"$(INTDIR)\qprocess.obj" \
	"$(INTDIR)\qprocess_win.obj" \
	"$(INTDIR)\qprogressbar.obj" \
	"$(INTDIR)\qprogressdialog.obj" \
	"$(INTDIR)\qptrcollection.obj" \
	"$(INTDIR)\qpushbutton.obj" \
	"$(INTDIR)\qradiobutton.obj" \
	"$(INTDIR)\qrangecontrol.obj" \
	"$(INTDIR)\qrect.obj" \
	"$(INTDIR)\qregexp.obj" \
	"$(INTDIR)\qregion.obj" \
	"$(INTDIR)\qregion_win.obj" \
	"$(INTDIR)\qrichtext.obj" \
	"$(INTDIR)\qrichtext_p.obj" \
	"$(INTDIR)\qrtlcodec.obj" \
	"$(INTDIR)\qscriptengine.obj" \
	"$(INTDIR)\qscrollbar.obj" \
	"$(INTDIR)\qscrollview.obj" \
	"$(INTDIR)\qsemaphore.obj" \
	"$(INTDIR)\qserversocket.obj" \
	"$(INTDIR)\qsettings.obj" \
	"$(INTDIR)\qsettings_win.obj" \
	"$(INTDIR)\qsignal.obj" \
	"$(INTDIR)\qsignalmapper.obj" \
	"$(INTDIR)\qsimplerichtext.obj" \
	"$(INTDIR)\qsize.obj" \
	"$(INTDIR)\qsizegrip.obj" \
	"$(INTDIR)\qsjiscodec.obj" \
	"$(INTDIR)\qslider.obj" \
	"$(INTDIR)\qsocket.obj" \
	"$(INTDIR)\qsocketdevice.obj" \
	"$(INTDIR)\qsocketdevice_win.obj" \
	"$(INTDIR)\qsocketnotifier.obj" \
	"$(INTDIR)\qsound.obj" \
	"$(INTDIR)\qsound_win.obj" \
	"$(INTDIR)\qspinbox.obj" \
	"$(INTDIR)\qspinwidget.obj" \
	"$(INTDIR)\qsplashscreen.obj" \
	"$(INTDIR)\qsplitter.obj" \
	"$(INTDIR)\qsql_odbc.obj" \
	"$(INTDIR)\qsqlcachedresult.obj" \
	"$(INTDIR)\qsqlcursor.obj" \
	"$(INTDIR)\qsqldatabase.obj" \
	"$(INTDIR)\qsqldriver.obj" \
	"$(INTDIR)\qsqldriverplugin.obj" \
	"$(INTDIR)\qsqleditorfactory.obj" \
	"$(INTDIR)\qsqlerror.obj" \
	"$(INTDIR)\qsqlextension_p.obj" \
	"$(INTDIR)\qsqlfield.obj" \
	"$(INTDIR)\qsqlform.obj" \
	"$(INTDIR)\qsqlindex.obj" \
	"$(INTDIR)\qsqlmanager_p.obj" \
	"$(INTDIR)\qsqlpropertymap.obj" \
	"$(INTDIR)\qsqlquery.obj" \
	"$(INTDIR)\qsqlrecord.obj" \
	"$(INTDIR)\qsqlresult.obj" \
	"$(INTDIR)\qsqlselectcursor.obj" \
	"$(INTDIR)\qstatusbar.obj" \
	"$(INTDIR)\qstring.obj" \
	"$(INTDIR)\qstringlist.obj" \
	"$(INTDIR)\qstyle.obj" \
	"$(INTDIR)\qstylefactory.obj" \
	"$(INTDIR)\qstyleplugin.obj" \
	"$(INTDIR)\qstylesheet.obj" \
	"$(INTDIR)\qsvgdevice.obj" \
	"$(INTDIR)\qsyntaxhighlighter.obj" \
	"$(INTDIR)\qtabbar.obj" \
	"$(INTDIR)\qtabdialog.obj" \
	"$(INTDIR)\qtable.obj" \
	"$(INTDIR)\qtabwidget.obj" \
	"$(INTDIR)\qtextbrowser.obj" \
	"$(INTDIR)\qtextcodec.obj" \
	"$(INTDIR)\qtextcodecfactory.obj" \
	"$(INTDIR)\qtextcodecplugin.obj" \
	"$(INTDIR)\qtextedit.obj" \
	"$(INTDIR)\qtextengine.obj" \
	"$(INTDIR)\qtextlayout.obj" \
	"$(INTDIR)\qtextstream.obj" \
	"$(INTDIR)\qtextview.obj" \
	"$(INTDIR)\qthread.obj" \
	"$(INTDIR)\qthread_win.obj" \
	"$(INTDIR)\qthreadstorage_win.obj" \
	"$(INTDIR)\qtimer.obj" \
	"$(INTDIR)\qtitlebar.obj" \
	"$(INTDIR)\qtoolbar.obj" \
	"$(INTDIR)\qtoolbox.obj" \
	"$(INTDIR)\qtoolbutton.obj" \
	"$(INTDIR)\qtooltip.obj" \
	"$(INTDIR)\qtranslator.obj" \
	"$(INTDIR)\qtsciicodec.obj" \
	"$(INTDIR)\qucom.obj" \
	"$(INTDIR)\qucomextra.obj" \
	"$(INTDIR)\qunicodetables.obj" \
	"$(INTDIR)\qurl.obj" \
	"$(INTDIR)\qurlinfo.obj" \
	"$(INTDIR)\qurloperator.obj" \
	"$(INTDIR)\qutfcodec.obj" \
	"$(INTDIR)\quuid.obj" \
	"$(INTDIR)\qvalidator.obj" \
	"$(INTDIR)\qvariant.obj" \
	"$(INTDIR)\qvbox.obj" \
	"$(INTDIR)\qvbuttongroup.obj" \
	"$(INTDIR)\qvgroupbox.obj" \
	"$(INTDIR)\qwaitcondition_win.obj" \
	"$(INTDIR)\qwhatsthis.obj" \
	"$(INTDIR)\qwidget.obj" \
	"$(INTDIR)\qwidget_win.obj" \
	"$(INTDIR)\qwidgetplugin.obj" \
	"$(INTDIR)\qwidgetresizehandler.obj" \
	"$(INTDIR)\qwidgetstack.obj" \
	"$(INTDIR)\qwindowsstyle.obj" \
	"$(INTDIR)\qwizard.obj" \
	"$(INTDIR)\qwmatrix.obj" \
	"$(INTDIR)\qworkspace.obj" \
	"$(INTDIR)\qxml.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\moc_qaccel.obj" \
	"$(INTDIR)\moc_qaction.obj" \
	"$(INTDIR)\moc_qapplication.obj" \
	"$(INTDIR)\moc_qasyncio.obj" \
	"$(INTDIR)\moc_qbutton.obj" \
	"$(INTDIR)\moc_qbuttongroup.obj" \
	"$(INTDIR)\moc_qcanvas.obj" \
	"$(INTDIR)\moc_qcheckbox.obj" \
	"$(INTDIR)\moc_qclipboard.obj" \
	"$(INTDIR)\moc_qcolordialog.obj" \
	"$(INTDIR)\moc_qcombobox.obj" \
	"$(INTDIR)\moc_qcommonstyle.obj" \
	"$(INTDIR)\moc_qdatabrowser.obj" \
	"$(INTDIR)\moc_qdatatable.obj" \
	"$(INTDIR)\moc_qdataview.obj" \
	"$(INTDIR)\moc_qdatetimeedit.obj" \
	"$(INTDIR)\moc_qdesktopwidget.obj" \
	"$(INTDIR)\moc_qdial.obj" \
	"$(INTDIR)\moc_qdialog.obj" \
	"$(INTDIR)\moc_qdialogbuttons_p.obj" \
	"$(INTDIR)\moc_qdns.obj" \
	"$(INTDIR)\moc_qdockarea.obj" \
	"$(INTDIR)\moc_qdockwindow.obj" \
	"$(INTDIR)\moc_qdragobject.obj" \
	"$(INTDIR)\moc_qerrormessage.obj" \
	"$(INTDIR)\moc_qeventloop.obj" \
	"$(INTDIR)\moc_qfiledialog.obj" \
	"$(INTDIR)\moc_qfontdialog.obj" \
	"$(INTDIR)\moc_qframe.obj" \
	"$(INTDIR)\moc_qftp.obj" \
	"$(INTDIR)\moc_qgl.obj" \
	"$(INTDIR)\moc_qgplugin.obj" \
	"$(INTDIR)\moc_qgrid.obj" \
	"$(INTDIR)\moc_qgridview.obj" \
	"$(INTDIR)\moc_qgroupbox.obj" \
	"$(INTDIR)\moc_qguardedptr.obj" \
	"$(INTDIR)\moc_qhbox.obj" \
	"$(INTDIR)\moc_qhbuttongroup.obj" \
	"$(INTDIR)\moc_qheader.obj" \
	"$(INTDIR)\moc_qhgroupbox.obj" \
	"$(INTDIR)\moc_qhttp.obj" \
	"$(INTDIR)\moc_qiconview.obj" \
	"$(INTDIR)\moc_qimageformatplugin.obj" \
	"$(INTDIR)\moc_qinputdialog.obj" \
	"$(INTDIR)\moc_qlabel.obj" \
	"$(INTDIR)\moc_qlayout.obj" \
	"$(INTDIR)\moc_qlcdnumber.obj" \
	"$(INTDIR)\moc_qlineedit.obj" \
	"$(INTDIR)\moc_qlistbox.obj" \
	"$(INTDIR)\moc_qlistview.obj" \
	"$(INTDIR)\moc_qlocalfs.obj" \
	"$(INTDIR)\moc_qmainwindow.obj" \
	"$(INTDIR)\moc_qmenubar.obj" \
	"$(INTDIR)\moc_qmessagebox.obj" \
	"$(INTDIR)\moc_qmultilineedit.obj" \
	"$(INTDIR)\moc_qnetworkprotocol.obj" \
	"$(INTDIR)\moc_qobject.obj" \
	"$(INTDIR)\moc_qobjectcleanuphandler.obj" \
	"$(INTDIR)\moc_qpopupmenu.obj" \
	"$(INTDIR)\moc_qprocess.obj" \
	"$(INTDIR)\moc_qprogressbar.obj" \
	"$(INTDIR)\moc_qprogressdialog.obj" \
	"$(INTDIR)\moc_qpushbutton.obj" \
	"$(INTDIR)\moc_qradiobutton.obj" \
	"$(INTDIR)\moc_qrangecontrol.obj" \
	"$(INTDIR)\moc_qrichtext_p.obj" \
	"$(INTDIR)\moc_qscrollbar.obj" \
	"$(INTDIR)\moc_qscrollview.obj" \
	"$(INTDIR)\moc_qsemimodal.obj" \
	"$(INTDIR)\moc_qserversocket.obj" \
	"$(INTDIR)\moc_qsessionmanager.obj" \
	"$(INTDIR)\moc_qsignal.obj" \
	"$(INTDIR)\moc_qsignalmapper.obj" \
	"$(INTDIR)\moc_qsizegrip.obj" \
	"$(INTDIR)\moc_qslider.obj" \
	"$(INTDIR)\moc_qsocket.obj" \
	"$(INTDIR)\moc_qsocketnotifier.obj" \
	"$(INTDIR)\moc_qsound.obj" \
	"$(INTDIR)\moc_qspinbox.obj" \
	"$(INTDIR)\moc_qsplashscreen.obj" \
	"$(INTDIR)\moc_qsplitter.obj" \
	"$(INTDIR)\moc_qsqldatabase.obj" \
	"$(INTDIR)\moc_qsqldriver.obj" \
	"$(INTDIR)\moc_qsqldriverplugin.obj" \
	"$(INTDIR)\moc_qsqlform.obj" \
	"$(INTDIR)\moc_qsqlquery.obj" \
	"$(INTDIR)\moc_qstatusbar.obj" \
	"$(INTDIR)\moc_qstyle.obj" \
	"$(INTDIR)\moc_qstyleplugin.obj" \
	"$(INTDIR)\moc_qstylesheet.obj" \
	"$(INTDIR)\moc_qtabbar.obj" \
	"$(INTDIR)\moc_qtabdialog.obj" \
	"$(INTDIR)\moc_qtable.obj" \
	"$(INTDIR)\moc_qtabwidget.obj" \
	"$(INTDIR)\moc_qtextbrowser.obj" \
	"$(INTDIR)\moc_qtextcodecplugin.obj" \
	"$(INTDIR)\moc_qtextedit.obj" \
	"$(INTDIR)\moc_qtextview.obj" \
	"$(INTDIR)\moc_qtimer.obj" \
	"$(INTDIR)\moc_qtitlebar_p.obj" \
	"$(INTDIR)\moc_qtoolbar.obj" \
	"$(INTDIR)\moc_qtoolbox.obj" \
	"$(INTDIR)\moc_qtoolbutton.obj" \
	"$(INTDIR)\moc_qtooltip.obj" \
	"$(INTDIR)\moc_qtranslator.obj" \
	"$(INTDIR)\moc_qurloperator.obj" \
	"$(INTDIR)\moc_qvalidator.obj" \
	"$(INTDIR)\moc_qvbox.obj" \
	"$(INTDIR)\moc_qvbuttongroup.obj" \
	"$(INTDIR)\moc_qvgroupbox.obj" \
	"$(INTDIR)\moc_qwidget.obj" \
	"$(INTDIR)\moc_qwidgetplugin.obj" \
	"$(INTDIR)\moc_qwidgetresizehandler_p.obj" \
	"$(INTDIR)\moc_qwidgetstack.obj" \
	"$(INTDIR)\moc_qwindowsstyle.obj" \
	"$(INTDIR)\moc_qwizard.obj" \
	"$(INTDIR)\moc_qworkspace.obj" \
	"$(INTDIR)\qt.res"

"..\lib\qt-mt334.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\Qt\3.3.4\lib\qt-mt334.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Post Build Step
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "tmp\moc\release_mt_shared\qworkspace.moc" "tmp\moc\release_mt_shared\qwhatsthis.moc" "tmp\moc\release_mt_shared\qtooltip.moc" "tmp\moc\release_mt_shared\qtoolbar.moc" "tmp\moc\release_mt_shared\qtable.moc" "tmp\moc\release_mt_shared\qsplitter.moc" "tmp\moc\release_mt_shared\qsound_win.moc" "tmp\moc\release_mt_shared\qscrollview.moc" "tmp\moc\release_mt_shared\qmovie.moc" "tmp\moc\release_mt_shared\qmessagebox.moc" "tmp\moc\release_mt_shared\qmainwindow.moc" "tmp\moc\release_mt_shared\qftp.moc" "tmp\moc\release_mt_shared\qfiledialog.moc" "tmp\moc\release_mt_shared\qeffects.moc" "tmp\moc\release_mt_shared\qdockwindow.moc" "tmp\moc\release_mt_shared\qdatetimeedit.moc" "tmp\moc\release_mt_shared\qcolordialog.moc" "..\lib\qt-mt334.dll"
   copy "\Qt\3.3.4\lib\qt-mt334.dll" "..\bin"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("qt_mt.dep")
!INCLUDE "qt_mt.dep"
!ELSE 
!MESSAGE Warning: cannot find "qt_mt.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "qtmt - Win32 Release" || "$(CFG)" == "qtmt - Win32 Debug"
SOURCE=3rdparty\zlib\adler32.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\adler32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\adler32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\zlib\compress.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\compress.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\compress.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\zlib\crc32.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\crc32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\crc32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\zlib\deflate.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\deflate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\deflate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\zlib\gzio.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\gzio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\gzio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\zlib\inffast.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\zlib\inflate.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\zlib\inftrees.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcapimin.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcapimin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcapimin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcapistd.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcapistd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcapistd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jccoefct.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jccoefct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jccoefct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jccolor.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jccolor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jccolor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcdctmgr.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcdctmgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcdctmgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jchuff.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jchuff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jchuff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcinit.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcinit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcinit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcmainct.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcmainct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcmainct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcmarker.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcmarker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcmarker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcmaster.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcmaster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcmaster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcomapi.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcomapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcomapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcparam.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcparam.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcparam.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcphuff.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcphuff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcphuff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcprepct.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcprepct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcprepct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jcsample.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jcsample.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jcsample.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jctrans.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jctrans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jctrans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdapimin.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdapimin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdapimin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdapistd.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdapistd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdapistd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdatadst.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdatadst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdatadst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdatasrc.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdatasrc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdatasrc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdcoefct.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdcoefct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdcoefct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdcolor.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdcolor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdcolor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jddctmgr.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jddctmgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jddctmgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdhuff.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdhuff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdhuff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdinput.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdinput.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdinput.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdmainct.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdmainct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdmainct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdmarker.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdmarker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdmarker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdmaster.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdmaster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdmaster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdmerge.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdmerge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdmerge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdphuff.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdphuff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdphuff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdpostct.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdpostct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdpostct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdsample.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdsample.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdsample.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jdtrans.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jdtrans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jdtrans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jerror.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jerror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jerror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jfdctflt.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jfdctflt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jfdctflt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jfdctfst.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jfdctfst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jfdctfst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jfdctint.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jfdctint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jfdctint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jidctflt.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jidctflt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jidctflt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jidctfst.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jidctfst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jidctfst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jidctint.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jidctint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jidctint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jidctred.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jidctred.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jidctred.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jmemmgr.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jmemmgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jmemmgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jmemnobs.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jmemnobs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jmemnobs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jquant1.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jquant1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jquant1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jquant2.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jquant2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jquant2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libjpeg\jutils.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\jutils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\jutils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\png.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngerror.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngerror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngerror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngget.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngget.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngget.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngmem.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngpread.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngpread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngpread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngread.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngrio.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngrio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngrio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngrtran.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngrtran.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngrtran.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngrutil.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngrutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngrutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngset.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngtrans.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngtrans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngtrans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngwio.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngwio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngwio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngwrite.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngwrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngwrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngwtran.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngwtran.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngwtran.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\libpng\pngwutil.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\pngwutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\pngwutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=kernel\qabstractlayout.cpp

"$(INTDIR)\qabstractlayout.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qdockwindow.h" "..\include\qmenubar.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qaccel.cpp

"$(INTDIR)\qaccel.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qstatusbar.h" "..\include\qdockwindow.h" "..\include\qsignal.h" "..\include\qframe.h" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qaccessible.cpp

"$(INTDIR)\qaccessible.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qguardedptr.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qaccessible_win.cpp

"$(INTDIR)\qaccessible_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qmessagebox.h" "..\include\qdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qaction.cpp

"$(INTDIR)\qaction.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qcombobox.h" "..\include\qtoolbar.h" "..\include\qstatusbar.h" "..\include\qtooltip.h" "..\include\qdockwindow.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qaccel.h" "..\include\qaction.h" "..\include\qtoolbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qapplication.cpp

"$(INTDIR)\qapplication.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsessionmanager.h" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj" "..\include\qmessagebox.h" "..\include\qclipboard.h" "..\include\qdialog.h" "..\include\qeventloop.h" "..\include\qsocketnotifier.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qapplication_win.cpp

"$(INTDIR)\qapplication_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qclipboard.h" "..\include\qsessionmanager.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qasyncimageio.cpp

"$(INTDIR)\qasyncimageio.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qasyncio.cpp

"$(INTDIR)\qasyncio.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qasyncio.h" "..\include\qsignal.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qbig5codec.cpp

"$(INTDIR)\qbig5codec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qbitarray.cpp

"$(INTDIR)\qbitarray.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qbitmap.cpp

"$(INTDIR)\qbitmap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qbuffer.cpp

"$(INTDIR)\qbuffer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qbutton.cpp

"$(INTDIR)\qbutton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qbutton.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qbuttongroup.h" "..\include\qgroupbox.h" "..\include\qpushbutton.h" "..\include\qradiobutton.h" "..\include\qframe.h" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qbuttongroup.cpp

"$(INTDIR)\qbuttongroup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qradiobutton.h" "..\include\qbutton.h" "..\include\qframe.h" "..\include\qbuttongroup.h" "..\include\qgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=canvas\qcanvas.cpp

"$(INTDIR)\qcanvas.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "$(INTDIR)\jcinit.obj" "..\include\qcanvas.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qcheckbox.cpp

"$(INTDIR)\qcheckbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qcheckbox.h" "..\include\qbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qclipboard.cpp

"$(INTDIR)\qclipboard.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qclipboard.h" "..\include\qdragobject.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qclipboard_win.cpp

"$(INTDIR)\qclipboard_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qeventloop.h" "..\include\qsocketnotifier.h" "..\include\qclipboard.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qcolor.cpp

"$(INTDIR)\qcolor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qcolor_p.cpp

"$(INTDIR)\qcolor_p.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qcolor_win.cpp

"$(INTDIR)\qcolor_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qcolordialog.cpp

"$(INTDIR)\qcolordialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qrangecontrol.h" "..\include\qbutton.h" "..\include\qlineedit.h" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj" "..\include\qvalidator.h" "..\include\qgridview.h" "..\include\qsignal.h" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qpopupmenu.h" ".\tmp\moc\release_mt_shared\qcolordialog.moc" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" "..\include\qcolordialog.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qcombobox.cpp

"$(INTDIR)\qcombobox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qlineedit.h" "..\include\qguardedptr.h" "..\include\qcombobox.h" "$(INTDIR)\jcinit.obj" "..\include\qlistbox.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qcomlibrary.cpp

"$(INTDIR)\qcomlibrary.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=styles\qcommonstyle.cpp

"$(INTDIR)\qcommonstyle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qrangecontrol.h" "..\include\qdockarea.h" "..\include\qcheckbox.h" "..\include\qbutton.h" "..\include\qheader.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qspinbox.h" "..\include\qdockwindow.h" "..\include\qslider.h" "..\include\qlistview.h" "..\include\qgroupbox.h" "..\include\qmenubar.h" "..\include\qsignal.h" "..\include\qprogressbar.h" "..\include\qpushbutton.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qradiobutton.h" "..\include\qlayout.h" ".\widgets\qtitlebar_p.h" "..\include\qcommonstyle.h" "..\include\qscrollview.h" "..\include\qscrollbar.h" "..\include\qtabbar.h" "..\include\qtoolbox.h" "..\include\qtoolbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qcomponentfactory.cpp

"$(INTDIR)\qcomponentfactory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qconfig.cpp

"$(INTDIR)\qconfig.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qconnection.cpp

"$(INTDIR)\qconnection.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qcriticalsection_p.cpp

"$(INTDIR)\qcriticalsection_p.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qcstring.cpp

"$(INTDIR)\qcstring.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qcursor.cpp

"$(INTDIR)\qcursor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qcursor_win.cpp

"$(INTDIR)\qcursor_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qdatabrowser.cpp

"$(INTDIR)\qdatabrowser.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qsqlform.h" "..\include\qdatabrowser.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qdatastream.cpp

"$(INTDIR)\qdatastream.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qdatatable.cpp

"$(INTDIR)\qdatatable.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qheader.h" "..\include\qguardedptr.h" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qsqldriver.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qdatatable.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qtable.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qdataview.cpp

"$(INTDIR)\qdataview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsqlquery.h" "..\include\qdataview.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qdatetime.cpp

"$(INTDIR)\qdatetime.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qdatetimeedit.cpp

"$(INTDIR)\qdatetimeedit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" ".\tmp\moc\release_mt_shared\qdatetimeedit.moc" "..\include\qdatetimeedit.h" "..\include\qframe.h" "..\include\qlayout.h" ".\kernel\qrichtext_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qdeepcopy.cpp

"$(INTDIR)\qdeepcopy.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qdesktopwidget_win.cpp

"$(INTDIR)\qdesktopwidget_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qdial.cpp

"$(INTDIR)\qdial.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qdial.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qdialog.cpp

"$(INTDIR)\qdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qbutton.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qsignal.h" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qpopupmenu.h" "..\include\qsizegrip.h" "..\include\qframe.h" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qdialogbuttons.cpp

"$(INTDIR)\qdialogbuttons.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qbutton.h" ".\widgets\qdialogbuttons_p.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qdir.cpp

"$(INTDIR)\qdir.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qdir_win.cpp

"$(INTDIR)\qdir_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qdnd_win.cpp

"$(INTDIR)\qdnd_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qdragobject.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=network\qdns.cpp

"$(INTDIR)\qdns.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdns.h" "..\include\qsocketnotifier.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qdockarea.cpp

"$(INTDIR)\qdockarea.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qdockarea.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qdockwindow.h" "..\include\qsplitter.h" "..\include\qmainwindow.h" "..\include\qframe.h" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qdockwindow.cpp

"$(INTDIR)\qdockwindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qdockarea.h" "..\include\qbutton.h" ".\widgets\qwidgetresizehandler_p.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qtooltip.h" "..\include\qdockwindow.h" ".\tmp\moc\release_mt_shared\qdockwindow.moc" "..\include\qmainwindow.h" "..\include\qframe.h" "..\include\qlayout.h" ".\widgets\qtitlebar_p.h" "..\include\qtoolbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=xml\qdom.cpp

"$(INTDIR)\qdom.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qdragobject.cpp

"$(INTDIR)\qdragobject.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qdrawutil.cpp

"$(INTDIR)\qdrawutil.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qdropsite.cpp

"$(INTDIR)\qdropsite.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qeditorfactory.cpp

"$(INTDIR)\qeditorfactory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qrangecontrol.h" "..\include\qlineedit.h" "..\include\qcombobox.h" "$(INTDIR)\jcinit.obj" "..\include\qspinbox.h" "..\include\qframe.h" "..\include\qdatetimeedit.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qeffects.cpp

"$(INTDIR)\qeffects.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" ".\tmp\moc\release_mt_shared\qeffects.moc" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qerrormessage.cpp

"$(INTDIR)\qerrormessage.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qcheckbox.h" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qmessagebox.h" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" "..\include\qerrormessage.h" "..\include\qscrollbar.h" "..\include\qtextview.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qeucjpcodec.cpp

"$(INTDIR)\qeucjpcodec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qeuckrcodec.cpp

"$(INTDIR)\qeuckrcodec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qevent.cpp

"$(INTDIR)\qevent.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qeventloop.cpp

"$(INTDIR)\qeventloop.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qeventloop.h" "..\include\qsocketnotifier.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qeventloop_win.cpp

"$(INTDIR)\qeventloop_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qeventloop.h" "..\include\qsocketnotifier.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qfile.cpp

"$(INTDIR)\qfile.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qfile_win.cpp

"$(INTDIR)\qfile_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qfiledialog.cpp

"$(INTDIR)\qfiledialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" ".\tmp\moc\release_mt_shared\qfiledialog.moc" "..\include\qsignal.h" "..\include\qlistview.h" "$(INTDIR)\jcinit.obj" "..\include\qsplitter.h" "..\include\qprogressbar.h" "..\include\qguardedptr.h" "..\include\qfiledialog.h" "..\include\qcheckbox.h" "..\include\qpopupmenu.h" "..\include\qtoolbutton.h" "..\include\qbuttongroup.h" "..\include\qurloperator.h" "..\include\qscrollview.h" "..\include\qframe.h" "..\include\qbutton.h" "..\include\qgroupbox.h" "..\include\qscrollbar.h" "..\include\qcombobox.h" "..\include\qpushbutton.h" "..\include\qnetworkprotocol.h" "..\include\qlabel.h" "..\include\qrangecontrol.h" "..\include\qtooltip.h" "..\include\qlineedit.h" "..\include\qlistbox.h" "..\include\qlayout.h" "..\include\qmessagebox.h" "..\include\qhbox.h" "..\include\qsemimodal.h" "..\include\qdialog.h" "..\include\qvbox.h" "..\include\qheader.h" "..\include\qwidgetstack.h" "..\include\qdragobject.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qfiledialog_win.cpp

"$(INTDIR)\qfiledialog_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qnetworkprotocol.h" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h" "..\include\qfiledialog.h" "..\include\qurloperator.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qfileinfo.cpp

"$(INTDIR)\qfileinfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qfileinfo_win.cpp

"$(INTDIR)\qfileinfo_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qfocusdata.cpp

"$(INTDIR)\qfocusdata.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qfont.cpp

"$(INTDIR)\qfont.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qfont_win.cpp

"$(INTDIR)\qfont_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qfontdatabase.cpp

"$(INTDIR)\qfontdatabase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qfontdialog.cpp

"$(INTDIR)\qfontdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qrangecontrol.h" "..\include\qcheckbox.h" "..\include\qbutton.h" "..\include\qlineedit.h" "$(INTDIR)\jcinit.obj" "..\include\qcombobox.h" "..\include\qvalidator.h" "..\include\qfontdialog.h" "..\include\qlistbox.h" "..\include\qgroupbox.h" "..\include\qvgroupbox.h" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qhgroupbox.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qfontengine_win.cpp

"$(INTDIR)\qfontengine_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qframe.cpp

"$(INTDIR)\qframe.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=network\qftp.cpp

"$(INTDIR)\qftp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" ".\tmp\moc\release_mt_shared\qftp.moc" "..\include\qnetworkprotocol.h" "$(INTDIR)\jcinit.obj" "..\include\qftp.h" "..\include\qurloperator.h" "..\include\qsocket.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qgarray.cpp

"$(INTDIR)\qgarray.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qgb18030codec.cpp

"$(INTDIR)\qgb18030codec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qgcache.cpp

"$(INTDIR)\qgcache.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qgdict.cpp

"$(INTDIR)\qgdict.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=opengl\qgl.cpp

"$(INTDIR)\qgl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgl.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=opengl\qgl_win.cpp

"$(INTDIR)\qgl_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgl.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=opengl\qglcolormap.cpp

"$(INTDIR)\qglcolormap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qglist.cpp

"$(INTDIR)\qglist.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qglobal.cpp

"$(INTDIR)\qglobal.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qgplugin.cpp

"$(INTDIR)\qgplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qgpluginmanager.cpp

"$(INTDIR)\qgpluginmanager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qgrid.cpp

"$(INTDIR)\qgrid.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlayout.h" "..\include\qgrid.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qgridview.cpp

"$(INTDIR)\qgridview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qscrollview.h" "..\include\qframe.h" "..\include\qscrollbar.h" "..\include\qgridview.h" "..\include\qrangecontrol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qgroupbox.cpp

"$(INTDIR)\qgroupbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qcheckbox.h" "..\include\qbutton.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qgroupbox.h" "..\include\qradiobutton.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qguardedptr.cpp

"$(INTDIR)\qguardedptr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qguardedptr.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qgvector.cpp

"$(INTDIR)\qgvector.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qhbox.cpp

"$(INTDIR)\qhbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qhbox.h" "..\include\qlayout.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qhbuttongroup.cpp

"$(INTDIR)\qhbuttongroup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qhbuttongroup.h" "..\include\qbuttongroup.h" "..\include\qgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qheader.cpp

"$(INTDIR)\qheader.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qheader.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qhgroupbox.cpp

"$(INTDIR)\qhgroupbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qgroupbox.h" "..\include\qhgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=network\qhostaddress.cpp

"$(INTDIR)\qhostaddress.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=network\qhttp.cpp

"$(INTDIR)\qhttp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qnetworkprotocol.h" "$(INTDIR)\jcinit.obj" "..\include\qhttp.h" "..\include\qurloperator.h" "..\include\qsocket.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qiconset.cpp

"$(INTDIR)\qiconset.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=iconview\qiconview.cpp

"$(INTDIR)\qiconview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qtooltip.h" "..\include\qhbox.h" "..\include\qiconview.h" "..\include\qvbox.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qimage.cpp

"$(INTDIR)\qimage.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qimageformatplugin.cpp

"$(INTDIR)\qimageformatplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h" "..\include\qimageformatplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qinputcontext_win.cpp

"$(INTDIR)\qinputcontext_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qinputdialog.cpp

"$(INTDIR)\qinputdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qrangecontrol.h" "..\include\qbutton.h" "..\include\qlineedit.h" "$(INTDIR)\jcinit.obj" "..\include\qcombobox.h" "..\include\qvalidator.h" "..\include\qspinbox.h" "..\include\qinputdialog.h" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qwidgetstack.h" "..\include\qframe.h" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qinternal.cpp

"$(INTDIR)\qinternal.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qiodevice.cpp

"$(INTDIR)\qiodevice.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qisciicodec.cpp

"$(INTDIR)\qisciicodec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qjiscodec.cpp

"$(INTDIR)\qjiscodec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qjpegio.cpp

"$(INTDIR)\qjpegio.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qjpunicode.cpp

"$(INTDIR)\qjpunicode.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qkeysequence.cpp

"$(INTDIR)\qkeysequence.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qlabel.cpp

"$(INTDIR)\qlabel.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qframe.h" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qlayout.cpp

"$(INTDIR)\qlayout.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qlayoutengine.cpp

"$(INTDIR)\qlayoutengine.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qlcdnumber.cpp

"$(INTDIR)\qlcdnumber.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlcdnumber.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qlibrary.cpp

"$(INTDIR)\qlibrary.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qlibrary_win.cpp

"$(INTDIR)\qlibrary_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qlineedit.cpp

"$(INTDIR)\qlineedit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlineedit.h" "..\include\qdragobject.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qvalidator.h" "..\include\qclipboard.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qlistbox.cpp

"$(INTDIR)\qlistbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qlistbox.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qlistview.cpp

"$(INTDIR)\qlistview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qlineedit.h" "..\include\qdragobject.h" "..\include\qheader.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qtooltip.h" "..\include\qhbox.h" "..\include\qlistview.h" "..\include\qsignal.h" "..\include\qvbox.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qlocale.cpp

"$(INTDIR)\qlocale.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qlocalfs.cpp

"$(INTDIR)\qlocalfs.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qnetworkprotocol.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qlocalfs.h" "..\include\qurloperator.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qmainwindow.cpp

"$(INTDIR)\qmainwindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qdockarea.h" ".\tmp\moc\release_mt_shared\qmainwindow.moc" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qstatusbar.h" "..\include\qtooltip.h" "..\include\qdockwindow.h" "..\include\qmenubar.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qmainwindow.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qmap.cpp

"$(INTDIR)\qmap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qmenubar.cpp

"$(INTDIR)\qmenubar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qdockwindow.h" "..\include\qmenubar.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qmainwindow.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qmenudata.cpp

"$(INTDIR)\qmenudata.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qmenubar.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qmessagebox.cpp

"$(INTDIR)\qmessagebox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qbutton.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qmessagebox.h" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qframe.h" ".\tmp\moc\release_mt_shared\qmessagebox.moc" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qmetaobject.cpp

"$(INTDIR)\qmetaobject.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qmime.cpp

"$(INTDIR)\qmime.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj" "..\include\qclipboard.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qmime_win.cpp

"$(INTDIR)\qmime_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qmngio.cpp

"$(INTDIR)\qmngio.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qmovie.cpp

"$(INTDIR)\qmovie.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsignal.h" ".\tmp\moc\release_mt_shared\qmovie.moc" "..\include\qasyncio.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qmultilineedit.cpp

"$(INTDIR)\qmultilineedit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qmultilineedit.h" "..\include\qclipboard.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" ".\kernel\qrichtext_p.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qmutex_win.cpp

"$(INTDIR)\qmutex_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qmutexpool.cpp

"$(INTDIR)\qmutexpool.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=network\qnetwork.cpp

"$(INTDIR)\qnetwork.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qftp.h" "..\include\qhttp.h" "..\include\qnetworkprotocol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qnetworkprotocol.cpp

"$(INTDIR)\qnetworkprotocol.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qnetworkprotocol.h" "$(INTDIR)\jcinit.obj" "..\include\qlocalfs.h" "..\include\qurloperator.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qobject.cpp

"$(INTDIR)\qobject.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qobjectcleanuphandler.cpp

"$(INTDIR)\qobjectcleanuphandler.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qobjectcleanuphandler.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qole_win.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\qole_win.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\qole_win.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=kernel\qpaintdevice_win.cpp

"$(INTDIR)\qpaintdevice_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpaintdevicemetrics.cpp

"$(INTDIR)\qpaintdevicemetrics.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpainter.cpp

"$(INTDIR)\qpainter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qlayout.h" ".\kernel\qrichtext_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpainter_win.cpp

"$(INTDIR)\qpainter_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpalette.cpp

"$(INTDIR)\qpalette.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpicture.cpp

"$(INTDIR)\qpicture.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpixmap.cpp

"$(INTDIR)\qpixmap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpixmap_win.cpp

"$(INTDIR)\qpixmap_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpixmapcache.cpp

"$(INTDIR)\qpixmapcache.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpngio.cpp

"$(INTDIR)\qpngio.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpoint.cpp

"$(INTDIR)\qpoint.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpointarray.cpp

"$(INTDIR)\qpointarray.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qpolygonscanner.cpp

"$(INTDIR)\qpolygonscanner.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qpopupmenu.cpp

"$(INTDIR)\qpopupmenu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qmenubar.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qprinter.cpp

"$(INTDIR)\qprinter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qprinter_win.cpp

"$(INTDIR)\qprinter_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qprocess.cpp

"$(INTDIR)\qprocess.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qprocess.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qprocess_win.cpp

"$(INTDIR)\qprocess_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qprocess.h" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qprogressbar.cpp

"$(INTDIR)\qprogressbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qprogressbar.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qprogressdialog.cpp

"$(INTDIR)\qprogressdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qsemimodal.h" "..\include\qprogressdialog.h" "..\include\qprogressbar.h" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qframe.h" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qptrcollection.cpp

"$(INTDIR)\qptrcollection.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qpushbutton.cpp

"$(INTDIR)\qpushbutton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qbutton.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qdockwindow.h" "..\include\qsignal.h" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qpopupmenu.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qradiobutton.cpp

"$(INTDIR)\qradiobutton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qradiobutton.h" "..\include\qbutton.h" "..\include\qframe.h" "..\include\qbuttongroup.h" "..\include\qgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qrangecontrol.cpp

"$(INTDIR)\qrangecontrol.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qrangecontrol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qrect.cpp

"$(INTDIR)\qrect.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qregexp.cpp

"$(INTDIR)\qregexp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qregion.cpp

"$(INTDIR)\qregion.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qregion_win.cpp

"$(INTDIR)\qregion_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qrichtext.cpp

"$(INTDIR)\qrichtext.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qlayout.h" ".\kernel\qrichtext_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qrichtext_p.cpp

"$(INTDIR)\qrichtext_p.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qlayout.h" ".\kernel\qrichtext_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qrtlcodec.cpp

"$(INTDIR)\qrtlcodec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qscriptengine.cpp

"$(INTDIR)\qscriptengine.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qscrollbar.cpp

"$(INTDIR)\qscrollbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qscrollview.cpp

"$(INTDIR)\qscrollview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qscrollview.h" ".\tmp\moc\release_mt_shared\qscrollview.moc" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qsemaphore.cpp

"$(INTDIR)\qsemaphore.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=network\qserversocket.cpp

"$(INTDIR)\qserversocket.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsocketnotifier.h" "..\include\qserversocket.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qsettings.cpp

"$(INTDIR)\qsettings.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qsettings_win.cpp

"$(INTDIR)\qsettings_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qsignal.cpp

"$(INTDIR)\qsignal.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsignal.h" "..\include\qguardedptr.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qsignalmapper.cpp

"$(INTDIR)\qsignalmapper.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsignalmapper.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qsimplerichtext.cpp

"$(INTDIR)\qsimplerichtext.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qlayout.h" ".\kernel\qrichtext_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qsize.cpp

"$(INTDIR)\qsize.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qsizegrip.cpp

"$(INTDIR)\qsizegrip.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsizegrip.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qsjiscodec.cpp

"$(INTDIR)\qsjiscodec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qslider.cpp

"$(INTDIR)\qslider.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qslider.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=network\qsocket.cpp

"$(INTDIR)\qsocket.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsocketnotifier.h" "..\include\qdns.h" "..\include\qsocket.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=network\qsocketdevice.cpp

"$(INTDIR)\qsocketdevice.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=network\qsocketdevice_win.cpp

"$(INTDIR)\qsocketdevice_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qsocketnotifier.cpp

"$(INTDIR)\qsocketnotifier.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qeventloop.h" "..\include\qsocketnotifier.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qsound.cpp

"$(INTDIR)\qsound.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsound.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qsound_win.cpp

"$(INTDIR)\qsound_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsound.h" "..\include\qguardedptr.h" ".\tmp\moc\release_mt_shared\qsound_win.moc"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qspinbox.cpp

"$(INTDIR)\qspinbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qbutton.h" "..\include\qlineedit.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qvalidator.h" "..\include\qspinbox.h" "..\include\qpushbutton.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qspinwidget.cpp

"$(INTDIR)\qspinwidget.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qrangecontrol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qsplashscreen.cpp

"$(INTDIR)\qsplashscreen.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsplashscreen.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qsplitter.cpp

"$(INTDIR)\qsplitter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" ".\tmp\moc\release_mt_shared\qsplitter.moc" "$(INTDIR)\jcinit.obj" "..\include\qsplitter.h" "..\include\qframe.h" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\drivers\odbc\qsql_odbc.cpp

"$(INTDIR)\qsql_odbc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qsqldriver.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\drivers\cache\qsqlcachedresult.cpp

"$(INTDIR)\qsqlcachedresult.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlcursor.cpp

"$(INTDIR)\qsqlcursor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qsqldriver.h" "..\include\qsqldatabase.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqldatabase.cpp

"$(INTDIR)\qsqldatabase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qsqldriver.h" "..\include\qsqldatabase.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqldriver.cpp

"$(INTDIR)\qsqldriver.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qsqldriver.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqldriverplugin.cpp

"$(INTDIR)\qsqldriverplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h" "..\include\qsqldriverplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqleditorfactory.cpp

"$(INTDIR)\qsqleditorfactory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qrangecontrol.h" "..\include\qlineedit.h" "..\include\qcombobox.h" "$(INTDIR)\jcinit.obj" "..\include\qspinbox.h" "..\include\qframe.h" "..\include\qdatetimeedit.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlerror.cpp

"$(INTDIR)\qsqlerror.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qmessagebox.h" "..\include\qdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlextension_p.cpp

"$(INTDIR)\qsqlextension_p.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlfield.cpp

"$(INTDIR)\qsqlfield.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlform.cpp

"$(INTDIR)\qsqlform.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsqlform.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlindex.cpp

"$(INTDIR)\qsqlindex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsqlquery.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlmanager_p.cpp

"$(INTDIR)\qsqlmanager_p.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qsqldriver.h" "..\include\qsqlform.h" "..\include\qmessagebox.h" "..\include\qdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlpropertymap.cpp

"$(INTDIR)\qsqlpropertymap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlquery.cpp

"$(INTDIR)\qsqlquery.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qsqldriver.h" "..\include\qsqldatabase.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlrecord.cpp

"$(INTDIR)\qsqlrecord.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlresult.cpp

"$(INTDIR)\qsqlresult.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=sql\qsqlselectcursor.cpp

"$(INTDIR)\qsqlselectcursor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qsqldriver.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qstatusbar.cpp

"$(INTDIR)\qstatusbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qstatusbar.h" "$(INTDIR)\jcinit.obj" "..\include\qlayout.h" "..\include\qsizegrip.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qstring.cpp

"$(INTDIR)\qstring.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qstringlist.cpp

"$(INTDIR)\qstringlist.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qstyle.cpp

"$(INTDIR)\qstyle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=styles\qstylefactory.cpp

"$(INTDIR)\qstylefactory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qwindowsstyle.h" "..\include\qcommonstyle.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=styles\qstyleplugin.cpp

"$(INTDIR)\qstyleplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h" "..\include\qobjectcleanuphandler.h" "..\include\qstyleplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qstylesheet.cpp

"$(INTDIR)\qstylesheet.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=xml\qsvgdevice.cpp

"$(INTDIR)\qsvgdevice.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qsyntaxhighlighter.cpp

"$(INTDIR)\qsyntaxhighlighter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" ".\kernel\qrichtext_p.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=qt.rc

"$(INTDIR)\qt.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=widgets\qtabbar.cpp

"$(INTDIR)\qtabbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qbutton.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qtooltip.h" "..\include\qaccel.h" "..\include\qtabbar.h" "..\include\qtoolbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qtabdialog.cpp

"$(INTDIR)\qtabdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qtabwidget.h" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qwidgetstack.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qtabbar.h" "..\include\qtabdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=table\qtable.cpp

"$(INTDIR)\qtable.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qcheckbox.h" "..\include\qbutton.h" "..\include\qlineedit.h" "..\include\qdragobject.h" "..\include\qheader.h" "..\include\qguardedptr.h" "..\include\qsqlquery.h" "..\include\qcombobox.h" "$(INTDIR)\jcinit.obj" "..\include\qvalidator.h" "..\include\qlistbox.h" ".\tmp\moc\release_mt_shared\qtable.moc" "..\include\qdatatable.h" "..\include\qframe.h" "..\include\qtable.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qtabwidget.cpp

"$(INTDIR)\qtabwidget.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qtabwidget.h" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qwidgetstack.h" "..\include\qframe.h" "..\include\qaccel.h" "..\include\qtabbar.h" "..\include\qtoolbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qtextbrowser.cpp

"$(INTDIR)\qtextbrowser.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qtextbrowser.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" ".\kernel\qrichtext_p.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qtextcodec.cpp

"$(INTDIR)\qtextcodec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qtextcodecfactory.cpp

"$(INTDIR)\qtextcodecfactory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qtextcodecplugin.cpp

"$(INTDIR)\qtextcodecplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qtextcodecplugin.h" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qtextedit.cpp

"$(INTDIR)\qtextedit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "..\include\qdragobject.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qhbox.h" "..\include\qfontdialog.h" "..\include\qlistbox.h" "..\include\qtextbrowser.h" "..\include\qclipboard.h" "..\include\qsignal.h" "..\include\qvbox.h" "..\include\qdialog.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" ".\kernel\qrichtext_p.h" "..\include\qcolordialog.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qtextengine.cpp

"$(INTDIR)\qtextengine.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qtextlayout.cpp

"$(INTDIR)\qtextlayout.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qtextstream.cpp

"$(INTDIR)\qtextstream.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qtextview.cpp

"$(INTDIR)\qtextview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h" "..\include\qtextview.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qthread.cpp

"$(INTDIR)\qthread.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qthread_win.cpp

"$(INTDIR)\qthread_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qthreadstorage_win.cpp

"$(INTDIR)\qthreadstorage_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qtimer.cpp

"$(INTDIR)\qtimer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsignal.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qtitlebar.cpp

"$(INTDIR)\qtitlebar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qtooltip.h" "..\include\qframe.h" "..\include\qworkspace.h" ".\widgets\qtitlebar_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qtoolbar.cpp

"$(INTDIR)\qtoolbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qbutton.h" ".\tmp\moc\release_mt_shared\qtoolbar.moc" "$(INTDIR)\jcinit.obj" "..\include\qcombobox.h" "..\include\qtoolbar.h" "..\include\qtooltip.h" "..\include\qdockwindow.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qmainwindow.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qtoolbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qtoolbox.cpp

"$(INTDIR)\qtoolbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qtooltip.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qscrollview.h" "..\include\qeventloop.h" "..\include\qscrollbar.h" "..\include\qtoolbox.h" "..\include\qsocketnotifier.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qtoolbutton.cpp

"$(INTDIR)\qtoolbutton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qbutton.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qtooltip.h" "..\include\qdockwindow.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qmainwindow.h" "..\include\qframe.h" "..\include\qtoolbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qtooltip.cpp

"$(INTDIR)\qtooltip.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlabel.h" ".\tmp\moc\release_mt_shared\qtooltip.moc" "..\include\qtooltip.h" "..\include\qframe.h" "..\include\qguardedptr.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qtranslator.cpp

"$(INTDIR)\qtranslator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qtsciicodec.cpp

"$(INTDIR)\qtsciicodec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qucom.cpp

"$(INTDIR)\qucom.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qucomextra.cpp

"$(INTDIR)\qucomextra.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qunicodetables.cpp

"$(INTDIR)\qunicodetables.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qurl.cpp

"$(INTDIR)\qurl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qurlinfo.cpp

"$(INTDIR)\qurlinfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qurloperator.h" "..\include\qnetworkprotocol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qurloperator.cpp

"$(INTDIR)\qurloperator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qnetworkprotocol.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qurloperator.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=codecs\qutfcodec.cpp

"$(INTDIR)\qutfcodec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\quuid.cpp

"$(INTDIR)\quuid.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qvalidator.cpp

"$(INTDIR)\qvalidator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qvalidator.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qvariant.cpp

"$(INTDIR)\qvariant.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qvbox.cpp

"$(INTDIR)\qvbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qhbox.h" "..\include\qvbox.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qvbuttongroup.cpp

"$(INTDIR)\qvbuttongroup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qbuttongroup.h" "..\include\qvbuttongroup.h" "..\include\qgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qvgroupbox.cpp

"$(INTDIR)\qvgroupbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qgroupbox.h" "..\include\qvgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tools\qwaitcondition_win.cpp

"$(INTDIR)\qwaitcondition_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qwhatsthis.cpp

"$(INTDIR)\qwhatsthis.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qtoolbutton.h" "..\include\qbutton.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qtooltip.h" ".\tmp\moc\release_mt_shared\qwhatsthis.moc"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qwidget.cpp

"$(INTDIR)\qwidget.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qwidget_win.cpp

"$(INTDIR)\qwidget_win.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlayout.h" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qwidgetplugin.cpp

"$(INTDIR)\qwidgetplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h" "..\include\qwidgetplugin.h" "..\include\qobjectcleanuphandler.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qwidgetresizehandler.cpp

"$(INTDIR)\qwidgetresizehandler.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" ".\widgets\qwidgetresizehandler_p.h" "..\include\qframe.h" "..\include\qsizegrip.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=widgets\qwidgetstack.cpp

"$(INTDIR)\qwidgetstack.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qbuttongroup.h" "..\include\qgroupbox.h" "..\include\qwidgetstack.h" "..\include\qframe.h" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=styles\qwindowsstyle.cpp

"$(INTDIR)\qwindowsstyle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qrangecontrol.h" "..\include\qtabwidget.h" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qcombobox.h" "..\include\qslider.h" "..\include\qdockwindow.h" "..\include\qwindowsstyle.h" "..\include\qlistview.h" "..\include\qlistbox.h" "..\include\qmenubar.h" "..\include\qsignal.h" "..\include\qpushbutton.h" "..\include\qpopupmenu.h" "..\include\qframe.h" "..\include\qcommonstyle.h" "..\include\qscrollview.h" "..\include\qscrollbar.h" "..\include\qtabbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=dialogs\qwizard.cpp

"$(INTDIR)\qwizard.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h" "..\include\qpushbutton.h" "..\include\qwizard.h" "..\include\qwidgetstack.h" "..\include\qframe.h" "..\include\qlayout.h" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=kernel\qwmatrix.cpp

"$(INTDIR)\qwmatrix.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=workspace\qworkspace.cpp

"$(INTDIR)\qworkspace.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qrangecontrol.h" "..\include\qdockarea.h" "..\include\qbutton.h" "..\include\qguardedptr.h" ".\widgets\qwidgetresizehandler_p.h" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qstatusbar.h" "..\include\qtooltip.h" "..\include\qdockwindow.h" "..\include\qhbox.h" ".\tmp\moc\release_mt_shared\qworkspace.moc" "..\include\qmenubar.h" "..\include\qsignal.h" "..\include\qvbox.h" "..\include\qpopupmenu.h" "..\include\qmainwindow.h" "..\include\qframe.h" "..\include\qworkspace.h" "..\include\qlayout.h" ".\widgets\qtitlebar_p.h" "..\include\qaccel.h" "..\include\qscrollbar.h" "..\include\qtoolbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=xml\qxml.cpp

"$(INTDIR)\qxml.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=3rdparty\zlib\trees.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\trees.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\trees.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\zlib\uncompr.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\uncompr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\uncompr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=3rdparty\zlib\zutil.c

!IF  "$(CFG)" == "qtmt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "QT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_CUPS" /D "QT_NO_NIS" /D "QT_SQL_ODBC" /D "QT_NO_IMAGEIO_MNG" /D "QT_NO_STYLE_MAC" /D "QT_NO_STYLE_AQUA" /D "QT_NO_STYLE_CDE" /D "QT_NO_STYLE_MOTIFPLUS" /D "QT_NO_STYLE_INTERLACE" /D "QT_NO_STYLE_PLATINUM" /D "QT_NO_STYLE_WINDOWSXP" /D "QT_NO_STYLE_SGI" /D "QT_NO_STYLE_COMPACT" /D "QT_NO_STYLE_POCKETPC" /D "QT_NO_STYLE_MOTIF" /D "QT_DLL" /D "QT_SHARED" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\include\qaccel.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qaccel.h
USERDEP__QACCE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qaccel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QACCE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qaccel.h -o tmp\moc\release_mt_shared\moc_qaccel.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qaccel.h
USERDEP__QACCE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qaccel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QACCE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qaccel.h -o tmp\moc\release_mt_shared\moc_qaccel.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qaction.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qaction.h
USERDEP__QACTI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qaction.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QACTI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qaction.h -o tmp\moc\release_mt_shared\moc_qaction.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qaction.h
USERDEP__QACTI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qaction.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QACTI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qaction.h -o tmp\moc\release_mt_shared\moc_qaction.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qapplication.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qapplication.h
USERDEP__QAPPL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qapplication.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QAPPL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qapplication.h -o tmp\moc\release_mt_shared\moc_qapplication.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qapplication.h
USERDEP__QAPPL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qapplication.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QAPPL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qapplication.h -o tmp\moc\release_mt_shared\moc_qapplication.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qasyncio.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qasyncio.h
USERDEP__QASYN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qasyncio.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QASYN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qasyncio.h -o tmp\moc\release_mt_shared\moc_qasyncio.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qasyncio.h
USERDEP__QASYN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qasyncio.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QASYN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qasyncio.h -o tmp\moc\release_mt_shared\moc_qasyncio.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qbutton.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qbutton.h
USERDEP__QBUTT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qbutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QBUTT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qbutton.h -o tmp\moc\release_mt_shared\moc_qbutton.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qbutton.h
USERDEP__QBUTT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qbutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QBUTT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qbutton.h -o tmp\moc\release_mt_shared\moc_qbutton.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qbuttongroup.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qbuttongroup.h
USERDEP__QBUTTO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qbuttongroup.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QBUTTO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qbuttongroup.h -o tmp\moc\release_mt_shared\moc_qbuttongroup.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qbuttongroup.h
USERDEP__QBUTTO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qbuttongroup.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QBUTTO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qbuttongroup.h -o tmp\moc\release_mt_shared\moc_qbuttongroup.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qcanvas.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qcanvas.h
USERDEP__QCANV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcanvas.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCANV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcanvas.h -o tmp\moc\release_mt_shared\moc_qcanvas.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qcanvas.h
USERDEP__QCANV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcanvas.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCANV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcanvas.h -o tmp\moc\release_mt_shared\moc_qcanvas.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qcheckbox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qcheckbox.h
USERDEP__QCHEC=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcheckbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCHEC)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcheckbox.h -o tmp\moc\release_mt_shared\moc_qcheckbox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qcheckbox.h
USERDEP__QCHEC=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcheckbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCHEC)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcheckbox.h -o tmp\moc\release_mt_shared\moc_qcheckbox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qclipboard.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qclipboard.h
USERDEP__QCLIP=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qclipboard.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCLIP)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qclipboard.h -o tmp\moc\release_mt_shared\moc_qclipboard.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qclipboard.h
USERDEP__QCLIP=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qclipboard.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCLIP)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qclipboard.h -o tmp\moc\release_mt_shared\moc_qclipboard.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qcolordialog.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qcolordialog.h
USERDEP__QCOLO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcolordialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCOLO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcolordialog.h -o tmp\moc\release_mt_shared\moc_qcolordialog.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qcolordialog.h
USERDEP__QCOLO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcolordialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCOLO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcolordialog.h -o tmp\moc\release_mt_shared\moc_qcolordialog.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qcombobox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qcombobox.h
USERDEP__QCOMB=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcombobox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCOMB)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcombobox.h -o tmp\moc\release_mt_shared\moc_qcombobox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qcombobox.h
USERDEP__QCOMB=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcombobox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCOMB)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcombobox.h -o tmp\moc\release_mt_shared\moc_qcombobox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qcommonstyle.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qcommonstyle.h
USERDEP__QCOMM=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcommonstyle.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCOMM)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcommonstyle.h -o tmp\moc\release_mt_shared\moc_qcommonstyle.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qcommonstyle.h
USERDEP__QCOMM=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qcommonstyle.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCOMM)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qcommonstyle.h -o tmp\moc\release_mt_shared\moc_qcommonstyle.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdatabrowser.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdatabrowser.h
USERDEP__QDATA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdatabrowser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdatabrowser.h -o tmp\moc\release_mt_shared\moc_qdatabrowser.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdatabrowser.h
USERDEP__QDATA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdatabrowser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdatabrowser.h -o tmp\moc\release_mt_shared\moc_qdatabrowser.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdatatable.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdatatable.h
USERDEP__QDATAT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdatatable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATAT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdatatable.h -o tmp\moc\release_mt_shared\moc_qdatatable.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdatatable.h
USERDEP__QDATAT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdatatable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATAT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdatatable.h -o tmp\moc\release_mt_shared\moc_qdatatable.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdataview.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdataview.h
USERDEP__QDATAV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdataview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATAV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdataview.h -o tmp\moc\release_mt_shared\moc_qdataview.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdataview.h
USERDEP__QDATAV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdataview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATAV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdataview.h -o tmp\moc\release_mt_shared\moc_qdataview.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdatetimeedit.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdatetimeedit.h
USERDEP__QDATE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdatetimeedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdatetimeedit.h -o tmp\moc\release_mt_shared\moc_qdatetimeedit.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdatetimeedit.h
USERDEP__QDATE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdatetimeedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdatetimeedit.h -o tmp\moc\release_mt_shared\moc_qdatetimeedit.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdesktopwidget.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdesktopwidget.h
USERDEP__QDESK=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdesktopwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDESK)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdesktopwidget.h -o tmp\moc\release_mt_shared\moc_qdesktopwidget.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdesktopwidget.h
USERDEP__QDESK=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdesktopwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDESK)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdesktopwidget.h -o tmp\moc\release_mt_shared\moc_qdesktopwidget.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdial.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdial.h
USERDEP__QDIAL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdial.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDIAL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdial.h -o tmp\moc\release_mt_shared\moc_qdial.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdial.h
USERDEP__QDIAL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdial.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDIAL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdial.h -o tmp\moc\release_mt_shared\moc_qdial.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdialog.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdialog.h
USERDEP__QDIALO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDIALO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdialog.h -o tmp\moc\release_mt_shared\moc_qdialog.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdialog.h
USERDEP__QDIALO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDIALO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdialog.h -o tmp\moc\release_mt_shared\moc_qdialog.cpp
<< 
	

!ENDIF 

SOURCE=widgets\qdialogbuttons_p.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=widgets\qdialogbuttons_p.h
USERDEP__QDIALOG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdialogbuttons_p.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDIALOG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qdialogbuttons_p.h -o tmp\moc\release_mt_shared\moc_qdialogbuttons_p.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=widgets\qdialogbuttons_p.h
USERDEP__QDIALOG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdialogbuttons_p.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDIALOG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qdialogbuttons_p.h -o tmp\moc\release_mt_shared\moc_qdialogbuttons_p.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdns.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdns.h
USERDEP__QDNS_=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdns.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDNS_)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdns.h -o tmp\moc\release_mt_shared\moc_qdns.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdns.h
USERDEP__QDNS_=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdns.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDNS_)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdns.h -o tmp\moc\release_mt_shared\moc_qdns.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdockarea.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdockarea.h
USERDEP__QDOCK=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdockarea.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDOCK)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdockarea.h -o tmp\moc\release_mt_shared\moc_qdockarea.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdockarea.h
USERDEP__QDOCK=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdockarea.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDOCK)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdockarea.h -o tmp\moc\release_mt_shared\moc_qdockarea.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdockwindow.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdockwindow.h
USERDEP__QDOCKW=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdockwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDOCKW)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdockwindow.h -o tmp\moc\release_mt_shared\moc_qdockwindow.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdockwindow.h
USERDEP__QDOCKW=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdockwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDOCKW)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdockwindow.h -o tmp\moc\release_mt_shared\moc_qdockwindow.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qdragobject.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qdragobject.h
USERDEP__QDRAG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdragobject.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDRAG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdragobject.h -o tmp\moc\release_mt_shared\moc_qdragobject.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qdragobject.h
USERDEP__QDRAG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qdragobject.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDRAG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qdragobject.h -o tmp\moc\release_mt_shared\moc_qdragobject.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qerrormessage.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qerrormessage.h
USERDEP__QERRO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qerrormessage.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QERRO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qerrormessage.h -o tmp\moc\release_mt_shared\moc_qerrormessage.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qerrormessage.h
USERDEP__QERRO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qerrormessage.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QERRO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qerrormessage.h -o tmp\moc\release_mt_shared\moc_qerrormessage.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qeventloop.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qeventloop.h
USERDEP__QEVEN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qeventloop.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QEVEN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qeventloop.h -o tmp\moc\release_mt_shared\moc_qeventloop.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qeventloop.h
USERDEP__QEVEN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qeventloop.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QEVEN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qeventloop.h -o tmp\moc\release_mt_shared\moc_qeventloop.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qfiledialog.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qfiledialog.h
USERDEP__QFILE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qfiledialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFILE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qfiledialog.h -o tmp\moc\release_mt_shared\moc_qfiledialog.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qfiledialog.h
USERDEP__QFILE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qfiledialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFILE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qfiledialog.h -o tmp\moc\release_mt_shared\moc_qfiledialog.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qfontdialog.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qfontdialog.h
USERDEP__QFONT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qfontdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFONT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qfontdialog.h -o tmp\moc\release_mt_shared\moc_qfontdialog.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qfontdialog.h
USERDEP__QFONT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qfontdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFONT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qfontdialog.h -o tmp\moc\release_mt_shared\moc_qfontdialog.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qframe.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qframe.h
USERDEP__QFRAM=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qframe.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFRAM)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qframe.h -o tmp\moc\release_mt_shared\moc_qframe.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qframe.h
USERDEP__QFRAM=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qframe.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFRAM)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qframe.h -o tmp\moc\release_mt_shared\moc_qframe.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qftp.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qftp.h
USERDEP__QFTP_=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qftp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFTP_)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qftp.h -o tmp\moc\release_mt_shared\moc_qftp.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qftp.h
USERDEP__QFTP_=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qftp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFTP_)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qftp.h -o tmp\moc\release_mt_shared\moc_qftp.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qgl.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qgl.h
USERDEP__QGL_H=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGL_H)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgl.h -o tmp\moc\release_mt_shared\moc_qgl.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qgl.h
USERDEP__QGL_H=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgl.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGL_H)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgl.h -o tmp\moc\release_mt_shared\moc_qgl.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qgplugin.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qgplugin.h
USERDEP__QGPLU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGPLU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgplugin.h -o tmp\moc\release_mt_shared\moc_qgplugin.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qgplugin.h
USERDEP__QGPLU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGPLU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgplugin.h -o tmp\moc\release_mt_shared\moc_qgplugin.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qgrid.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qgrid.h
USERDEP__QGRID=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgrid.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGRID)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgrid.h -o tmp\moc\release_mt_shared\moc_qgrid.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qgrid.h
USERDEP__QGRID=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgrid.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGRID)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgrid.h -o tmp\moc\release_mt_shared\moc_qgrid.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qgridview.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qgridview.h
USERDEP__QGRIDV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgridview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGRIDV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgridview.h -o tmp\moc\release_mt_shared\moc_qgridview.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qgridview.h
USERDEP__QGRIDV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgridview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGRIDV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgridview.h -o tmp\moc\release_mt_shared\moc_qgridview.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qgroupbox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qgroupbox.h
USERDEP__QGROU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgroupbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGROU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgroupbox.h -o tmp\moc\release_mt_shared\moc_qgroupbox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qgroupbox.h
USERDEP__QGROU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qgroupbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGROU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qgroupbox.h -o tmp\moc\release_mt_shared\moc_qgroupbox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qguardedptr.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qguardedptr.h
USERDEP__QGUAR=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qguardedptr.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGUAR)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qguardedptr.h -o tmp\moc\release_mt_shared\moc_qguardedptr.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qguardedptr.h
USERDEP__QGUAR=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qguardedptr.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QGUAR)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qguardedptr.h -o tmp\moc\release_mt_shared\moc_qguardedptr.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qhbox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qhbox.h
USERDEP__QHBOX=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qhbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHBOX)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qhbox.h -o tmp\moc\release_mt_shared\moc_qhbox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qhbox.h
USERDEP__QHBOX=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qhbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHBOX)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qhbox.h -o tmp\moc\release_mt_shared\moc_qhbox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qhbuttongroup.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qhbuttongroup.h
USERDEP__QHBUT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qhbuttongroup.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHBUT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qhbuttongroup.h -o tmp\moc\release_mt_shared\moc_qhbuttongroup.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qhbuttongroup.h
USERDEP__QHBUT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qhbuttongroup.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHBUT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qhbuttongroup.h -o tmp\moc\release_mt_shared\moc_qhbuttongroup.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qheader.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qheader.h
USERDEP__QHEAD=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qheader.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHEAD)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qheader.h -o tmp\moc\release_mt_shared\moc_qheader.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qheader.h
USERDEP__QHEAD=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qheader.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHEAD)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qheader.h -o tmp\moc\release_mt_shared\moc_qheader.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qhgroupbox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qhgroupbox.h
USERDEP__QHGRO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qhgroupbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHGRO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qhgroupbox.h -o tmp\moc\release_mt_shared\moc_qhgroupbox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qhgroupbox.h
USERDEP__QHGRO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qhgroupbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHGRO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qhgroupbox.h -o tmp\moc\release_mt_shared\moc_qhgroupbox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qhttp.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qhttp.h
USERDEP__QHTTP=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qhttp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHTTP)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qhttp.h -o tmp\moc\release_mt_shared\moc_qhttp.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qhttp.h
USERDEP__QHTTP=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qhttp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QHTTP)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qhttp.h -o tmp\moc\release_mt_shared\moc_qhttp.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qiconview.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qiconview.h
USERDEP__QICON=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qiconview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QICON)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qiconview.h -o tmp\moc\release_mt_shared\moc_qiconview.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qiconview.h
USERDEP__QICON=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qiconview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QICON)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qiconview.h -o tmp\moc\release_mt_shared\moc_qiconview.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qimageformatplugin.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qimageformatplugin.h
USERDEP__QIMAG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qimageformatplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QIMAG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qimageformatplugin.h -o tmp\moc\release_mt_shared\moc_qimageformatplugin.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qimageformatplugin.h
USERDEP__QIMAG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qimageformatplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QIMAG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qimageformatplugin.h -o tmp\moc\release_mt_shared\moc_qimageformatplugin.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qinputdialog.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qinputdialog.h
USERDEP__QINPU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qinputdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QINPU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qinputdialog.h -o tmp\moc\release_mt_shared\moc_qinputdialog.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qinputdialog.h
USERDEP__QINPU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qinputdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QINPU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qinputdialog.h -o tmp\moc\release_mt_shared\moc_qinputdialog.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qlabel.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qlabel.h
USERDEP__QLABE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlabel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLABE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlabel.h -o tmp\moc\release_mt_shared\moc_qlabel.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qlabel.h
USERDEP__QLABE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlabel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLABE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlabel.h -o tmp\moc\release_mt_shared\moc_qlabel.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qlayout.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qlayout.h
USERDEP__QLAYO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlayout.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLAYO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlayout.h -o tmp\moc\release_mt_shared\moc_qlayout.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qlayout.h
USERDEP__QLAYO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlayout.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLAYO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlayout.h -o tmp\moc\release_mt_shared\moc_qlayout.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qlcdnumber.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qlcdnumber.h
USERDEP__QLCDN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlcdnumber.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLCDN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlcdnumber.h -o tmp\moc\release_mt_shared\moc_qlcdnumber.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qlcdnumber.h
USERDEP__QLCDN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlcdnumber.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLCDN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlcdnumber.h -o tmp\moc\release_mt_shared\moc_qlcdnumber.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qlineedit.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qlineedit.h
USERDEP__QLINE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlineedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLINE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlineedit.h -o tmp\moc\release_mt_shared\moc_qlineedit.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qlineedit.h
USERDEP__QLINE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlineedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLINE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlineedit.h -o tmp\moc\release_mt_shared\moc_qlineedit.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qlistbox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qlistbox.h
USERDEP__QLIST=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlistbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLIST)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlistbox.h -o tmp\moc\release_mt_shared\moc_qlistbox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qlistbox.h
USERDEP__QLIST=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlistbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLIST)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlistbox.h -o tmp\moc\release_mt_shared\moc_qlistbox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qlistview.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qlistview.h
USERDEP__QLISTV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlistview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLISTV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlistview.h -o tmp\moc\release_mt_shared\moc_qlistview.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qlistview.h
USERDEP__QLISTV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlistview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLISTV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlistview.h -o tmp\moc\release_mt_shared\moc_qlistview.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qlocalfs.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qlocalfs.h
USERDEP__QLOCA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlocalfs.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLOCA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlocalfs.h -o tmp\moc\release_mt_shared\moc_qlocalfs.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qlocalfs.h
USERDEP__QLOCA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qlocalfs.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QLOCA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qlocalfs.h -o tmp\moc\release_mt_shared\moc_qlocalfs.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qmainwindow.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qmainwindow.h
USERDEP__QMAIN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qmainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMAIN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qmainwindow.h -o tmp\moc\release_mt_shared\moc_qmainwindow.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qmainwindow.h
USERDEP__QMAIN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qmainwindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMAIN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qmainwindow.h -o tmp\moc\release_mt_shared\moc_qmainwindow.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qmenubar.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qmenubar.h
USERDEP__QMENU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qmenubar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMENU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qmenubar.h -o tmp\moc\release_mt_shared\moc_qmenubar.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qmenubar.h
USERDEP__QMENU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qmenubar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMENU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qmenubar.h -o tmp\moc\release_mt_shared\moc_qmenubar.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qmessagebox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qmessagebox.h
USERDEP__QMESS=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qmessagebox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMESS)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qmessagebox.h -o tmp\moc\release_mt_shared\moc_qmessagebox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qmessagebox.h
USERDEP__QMESS=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qmessagebox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMESS)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qmessagebox.h -o tmp\moc\release_mt_shared\moc_qmessagebox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qmultilineedit.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qmultilineedit.h
USERDEP__QMULT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qmultilineedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMULT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qmultilineedit.h -o tmp\moc\release_mt_shared\moc_qmultilineedit.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qmultilineedit.h
USERDEP__QMULT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qmultilineedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMULT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qmultilineedit.h -o tmp\moc\release_mt_shared\moc_qmultilineedit.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qnetworkprotocol.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qnetworkprotocol.h
USERDEP__QNETW=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qnetworkprotocol.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QNETW)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qnetworkprotocol.h -o tmp\moc\release_mt_shared\moc_qnetworkprotocol.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qnetworkprotocol.h
USERDEP__QNETW=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qnetworkprotocol.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QNETW)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qnetworkprotocol.h -o tmp\moc\release_mt_shared\moc_qnetworkprotocol.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qobject.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qobject.h
USERDEP__QOBJE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qobject.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QOBJE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qobject.h -o tmp\moc\release_mt_shared\moc_qobject.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qobject.h
USERDEP__QOBJE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qobject.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QOBJE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qobject.h -o tmp\moc\release_mt_shared\moc_qobject.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qobjectcleanuphandler.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qobjectcleanuphandler.h
USERDEP__QOBJEC=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qobjectcleanuphandler.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QOBJEC)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qobjectcleanuphandler.h -o tmp\moc\release_mt_shared\moc_qobjectcleanuphandler.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qobjectcleanuphandler.h
USERDEP__QOBJEC=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qobjectcleanuphandler.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QOBJEC)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qobjectcleanuphandler.h -o tmp\moc\release_mt_shared\moc_qobjectcleanuphandler.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qpopupmenu.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qpopupmenu.h
USERDEP__QPOPU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qpopupmenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPOPU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qpopupmenu.h -o tmp\moc\release_mt_shared\moc_qpopupmenu.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qpopupmenu.h
USERDEP__QPOPU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qpopupmenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPOPU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qpopupmenu.h -o tmp\moc\release_mt_shared\moc_qpopupmenu.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qprocess.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qprocess.h
USERDEP__QPROC=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qprocess.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPROC)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qprocess.h -o tmp\moc\release_mt_shared\moc_qprocess.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qprocess.h
USERDEP__QPROC=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qprocess.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPROC)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qprocess.h -o tmp\moc\release_mt_shared\moc_qprocess.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qprogressbar.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qprogressbar.h
USERDEP__QPROG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qprogressbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPROG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qprogressbar.h -o tmp\moc\release_mt_shared\moc_qprogressbar.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qprogressbar.h
USERDEP__QPROG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qprogressbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPROG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qprogressbar.h -o tmp\moc\release_mt_shared\moc_qprogressbar.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qprogressdialog.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qprogressdialog.h
USERDEP__QPROGR=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qprogressdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPROGR)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qprogressdialog.h -o tmp\moc\release_mt_shared\moc_qprogressdialog.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qprogressdialog.h
USERDEP__QPROGR=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qprogressdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPROGR)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qprogressdialog.h -o tmp\moc\release_mt_shared\moc_qprogressdialog.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qpushbutton.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qpushbutton.h
USERDEP__QPUSH=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qpushbutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPUSH)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qpushbutton.h -o tmp\moc\release_mt_shared\moc_qpushbutton.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qpushbutton.h
USERDEP__QPUSH=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qpushbutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QPUSH)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qpushbutton.h -o tmp\moc\release_mt_shared\moc_qpushbutton.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qradiobutton.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qradiobutton.h
USERDEP__QRADI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qradiobutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QRADI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qradiobutton.h -o tmp\moc\release_mt_shared\moc_qradiobutton.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qradiobutton.h
USERDEP__QRADI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qradiobutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QRADI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qradiobutton.h -o tmp\moc\release_mt_shared\moc_qradiobutton.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qrangecontrol.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qrangecontrol.h
USERDEP__QRANG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qrangecontrol.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QRANG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qrangecontrol.h -o tmp\moc\release_mt_shared\moc_qrangecontrol.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qrangecontrol.h
USERDEP__QRANG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qrangecontrol.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QRANG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qrangecontrol.h -o tmp\moc\release_mt_shared\moc_qrangecontrol.cpp
<< 
	

!ENDIF 

SOURCE=kernel\qrichtext_p.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=kernel\qrichtext_p.h
USERDEP__QRICH=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qrichtext_p.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QRICH)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc kernel\qrichtext_p.h -o tmp\moc\release_mt_shared\moc_qrichtext_p.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=kernel\qrichtext_p.h
USERDEP__QRICH=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qrichtext_p.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QRICH)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc kernel\qrichtext_p.h -o tmp\moc\release_mt_shared\moc_qrichtext_p.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qscrollbar.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qscrollbar.h
USERDEP__QSCRO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qscrollbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSCRO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qscrollbar.h -o tmp\moc\release_mt_shared\moc_qscrollbar.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qscrollbar.h
USERDEP__QSCRO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qscrollbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSCRO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qscrollbar.h -o tmp\moc\release_mt_shared\moc_qscrollbar.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qscrollview.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qscrollview.h
USERDEP__QSCROL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qscrollview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSCROL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qscrollview.h -o tmp\moc\release_mt_shared\moc_qscrollview.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qscrollview.h
USERDEP__QSCROL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qscrollview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSCROL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qscrollview.h -o tmp\moc\release_mt_shared\moc_qscrollview.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsemimodal.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsemimodal.h
USERDEP__QSEMI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsemimodal.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSEMI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsemimodal.h -o tmp\moc\release_mt_shared\moc_qsemimodal.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsemimodal.h
USERDEP__QSEMI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsemimodal.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSEMI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsemimodal.h -o tmp\moc\release_mt_shared\moc_qsemimodal.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qserversocket.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qserversocket.h
USERDEP__QSERV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qserversocket.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSERV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qserversocket.h -o tmp\moc\release_mt_shared\moc_qserversocket.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qserversocket.h
USERDEP__QSERV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qserversocket.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSERV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qserversocket.h -o tmp\moc\release_mt_shared\moc_qserversocket.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsessionmanager.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsessionmanager.h
USERDEP__QSESS=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsessionmanager.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSESS)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsessionmanager.h -o tmp\moc\release_mt_shared\moc_qsessionmanager.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsessionmanager.h
USERDEP__QSESS=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsessionmanager.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSESS)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsessionmanager.h -o tmp\moc\release_mt_shared\moc_qsessionmanager.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsignal.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsignal.h
USERDEP__QSIGN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsignal.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSIGN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsignal.h -o tmp\moc\release_mt_shared\moc_qsignal.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsignal.h
USERDEP__QSIGN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsignal.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSIGN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsignal.h -o tmp\moc\release_mt_shared\moc_qsignal.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsignalmapper.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsignalmapper.h
USERDEP__QSIGNA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsignalmapper.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSIGNA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsignalmapper.h -o tmp\moc\release_mt_shared\moc_qsignalmapper.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsignalmapper.h
USERDEP__QSIGNA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsignalmapper.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSIGNA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsignalmapper.h -o tmp\moc\release_mt_shared\moc_qsignalmapper.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsizegrip.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsizegrip.h
USERDEP__QSIZE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsizegrip.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSIZE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsizegrip.h -o tmp\moc\release_mt_shared\moc_qsizegrip.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsizegrip.h
USERDEP__QSIZE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsizegrip.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSIZE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsizegrip.h -o tmp\moc\release_mt_shared\moc_qsizegrip.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qslider.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qslider.h
USERDEP__QSLID=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qslider.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSLID)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qslider.h -o tmp\moc\release_mt_shared\moc_qslider.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qslider.h
USERDEP__QSLID=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qslider.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSLID)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qslider.h -o tmp\moc\release_mt_shared\moc_qslider.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsocket.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsocket.h
USERDEP__QSOCK=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsocket.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSOCK)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsocket.h -o tmp\moc\release_mt_shared\moc_qsocket.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsocket.h
USERDEP__QSOCK=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsocket.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSOCK)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsocket.h -o tmp\moc\release_mt_shared\moc_qsocket.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsocketnotifier.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsocketnotifier.h
USERDEP__QSOCKE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsocketnotifier.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSOCKE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsocketnotifier.h -o tmp\moc\release_mt_shared\moc_qsocketnotifier.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsocketnotifier.h
USERDEP__QSOCKE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsocketnotifier.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSOCKE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsocketnotifier.h -o tmp\moc\release_mt_shared\moc_qsocketnotifier.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsound.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsound.h
USERDEP__QSOUN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsound.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSOUN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsound.h -o tmp\moc\release_mt_shared\moc_qsound.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsound.h
USERDEP__QSOUN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsound.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSOUN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsound.h -o tmp\moc\release_mt_shared\moc_qsound.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qspinbox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qspinbox.h
USERDEP__QSPIN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qspinbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSPIN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qspinbox.h -o tmp\moc\release_mt_shared\moc_qspinbox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qspinbox.h
USERDEP__QSPIN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qspinbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSPIN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qspinbox.h -o tmp\moc\release_mt_shared\moc_qspinbox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsplashscreen.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsplashscreen.h
USERDEP__QSPLA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsplashscreen.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSPLA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsplashscreen.h -o tmp\moc\release_mt_shared\moc_qsplashscreen.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsplashscreen.h
USERDEP__QSPLA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsplashscreen.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSPLA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsplashscreen.h -o tmp\moc\release_mt_shared\moc_qsplashscreen.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsplitter.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsplitter.h
USERDEP__QSPLI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsplitter.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSPLI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsplitter.h -o tmp\moc\release_mt_shared\moc_qsplitter.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsplitter.h
USERDEP__QSPLI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsplitter.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSPLI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsplitter.h -o tmp\moc\release_mt_shared\moc_qsplitter.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsqldatabase.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsqldatabase.h
USERDEP__QSQLD=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqldatabase.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLD)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqldatabase.h -o tmp\moc\release_mt_shared\moc_qsqldatabase.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsqldatabase.h
USERDEP__QSQLD=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqldatabase.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLD)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqldatabase.h -o tmp\moc\release_mt_shared\moc_qsqldatabase.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsqldriver.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsqldriver.h
USERDEP__QSQLDR=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqldriver.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLDR)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqldriver.h -o tmp\moc\release_mt_shared\moc_qsqldriver.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsqldriver.h
USERDEP__QSQLDR=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqldriver.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLDR)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqldriver.h -o tmp\moc\release_mt_shared\moc_qsqldriver.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsqldriverplugin.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsqldriverplugin.h
USERDEP__QSQLDRI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqldriverplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLDRI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqldriverplugin.h -o tmp\moc\release_mt_shared\moc_qsqldriverplugin.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsqldriverplugin.h
USERDEP__QSQLDRI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqldriverplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLDRI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqldriverplugin.h -o tmp\moc\release_mt_shared\moc_qsqldriverplugin.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsqlform.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsqlform.h
USERDEP__QSQLF=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqlform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLF)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqlform.h -o tmp\moc\release_mt_shared\moc_qsqlform.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsqlform.h
USERDEP__QSQLF=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqlform.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLF)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqlform.h -o tmp\moc\release_mt_shared\moc_qsqlform.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qsqlquery.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qsqlquery.h
USERDEP__QSQLQ=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqlquery.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLQ)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqlquery.h -o tmp\moc\release_mt_shared\moc_qsqlquery.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qsqlquery.h
USERDEP__QSQLQ=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qsqlquery.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSQLQ)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qsqlquery.h -o tmp\moc\release_mt_shared\moc_qsqlquery.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qstatusbar.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qstatusbar.h
USERDEP__QSTAT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qstatusbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSTAT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qstatusbar.h -o tmp\moc\release_mt_shared\moc_qstatusbar.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qstatusbar.h
USERDEP__QSTAT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qstatusbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSTAT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qstatusbar.h -o tmp\moc\release_mt_shared\moc_qstatusbar.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qstyle.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qstyle.h
USERDEP__QSTYL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qstyle.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSTYL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qstyle.h -o tmp\moc\release_mt_shared\moc_qstyle.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qstyle.h
USERDEP__QSTYL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qstyle.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSTYL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qstyle.h -o tmp\moc\release_mt_shared\moc_qstyle.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qstyleplugin.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qstyleplugin.h
USERDEP__QSTYLE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qstyleplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSTYLE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qstyleplugin.h -o tmp\moc\release_mt_shared\moc_qstyleplugin.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qstyleplugin.h
USERDEP__QSTYLE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qstyleplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSTYLE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qstyleplugin.h -o tmp\moc\release_mt_shared\moc_qstyleplugin.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qstylesheet.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qstylesheet.h
USERDEP__QSTYLES=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qstylesheet.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSTYLES)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qstylesheet.h -o tmp\moc\release_mt_shared\moc_qstylesheet.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qstylesheet.h
USERDEP__QSTYLES=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qstylesheet.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSTYLES)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qstylesheet.h -o tmp\moc\release_mt_shared\moc_qstylesheet.cpp
<< 
	

!ENDIF 

SOURCE=kernel\qt_pch.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

IntDir=tmp\obj\release_mt_shared
InputPath=kernel\qt_pch.h

"$(INTDIR)\qt_mt.pch" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	cl.exe /TP /W3 /FD /c /D "WIN32" /Yc /Fp""$(IntDir)\qt_mt.pch"" /Fo""$(IntDir)\qt_mt.obj"" /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D UNICODE /D QT_MAKEDLL /D QT_THREAD_SUPPORT /D QT_NO_CUPS /D QT_NO_NIS /D QT_SQL_ODBC /D QT_NO_IMAGEIO_MNG /D QT_NO_STYLE_MAC /D QT_NO_STYLE_AQUA /D QT_NO_STYLE_CDE /D QT_NO_STYLE_MOTIFPLUS /D QT_NO_STYLE_INTERLACE /D QT_NO_STYLE_PLATINUM /D QT_NO_STYLE_WINDOWSXP /D QT_NO_STYLE_SGI /D QT_NO_STYLE_COMPACT /D QT_NO_STYLE_POCKETPC /D QT_NO_STYLE_MOTIF /D QT_DLL /D QT_MAKEDLL /D QT_SHARED /D QT_THREAD_SUPPORT /D QT_ACCESSIBILITY_SUPPORT -nologo -Zm200 /D "NDEBUG" -O1 -MT /D "QT_NO_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Fd"$(IntDir)\\" kernel\qt_pch.h
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

IntDir=Debug
InputPath=kernel\qt_pch.h

"$(INTDIR)\qt_mt.pch" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	cl.exe /TP /W3 /FD /c /D "WIN32" /Yc /Fp""$(IntDir)\qt_mt.pch"" /Fo""$(IntDir)\qt_mt.obj"" /I "tmp" /I "3rdparty\libjpeg" /I "3rdparty\libpng" /I "3rdparty\zlib" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D UNICODE /D QT_MAKEDLL /D QT_THREAD_SUPPORT /D QT_NO_CUPS /D QT_NO_NIS /D QT_SQL_ODBC /D QT_NO_IMAGEIO_MNG /D QT_NO_STYLE_MAC /D QT_NO_STYLE_AQUA /D QT_NO_STYLE_CDE /D QT_NO_STYLE_MOTIFPLUS /D QT_NO_STYLE_INTERLACE /D QT_NO_STYLE_PLATINUM /D QT_NO_STYLE_WINDOWSXP /D QT_NO_STYLE_SGI /D QT_NO_STYLE_COMPACT /D QT_NO_STYLE_POCKETPC /D QT_NO_STYLE_MOTIF /D QT_DLL /D QT_MAKEDLL /D QT_SHARED /D QT_THREAD_SUPPORT /D QT_ACCESSIBILITY_SUPPORT -nologo -Zm200 /D "_DEBUG" /Od -Zi -MDd /GZ /Zi /D "_WINDOWS" /D "_USRDLL" /Fd"$(IntDir)\\" kernel\qt_pch.h
<< 
	

!ENDIF 

SOURCE=..\include\qtabbar.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtabbar.h
USERDEP__QTABB=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtabbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABB)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtabbar.h -o tmp\moc\release_mt_shared\moc_qtabbar.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtabbar.h
USERDEP__QTABB=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtabbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABB)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtabbar.h -o tmp\moc\release_mt_shared\moc_qtabbar.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtabdialog.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtabdialog.h
USERDEP__QTABD=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtabdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABD)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtabdialog.h -o tmp\moc\release_mt_shared\moc_qtabdialog.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtabdialog.h
USERDEP__QTABD=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtabdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABD)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtabdialog.h -o tmp\moc\release_mt_shared\moc_qtabdialog.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtable.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtable.h
USERDEP__QTABL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtable.h -o tmp\moc\release_mt_shared\moc_qtable.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtable.h
USERDEP__QTABL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtable.h -o tmp\moc\release_mt_shared\moc_qtable.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtabwidget.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtabwidget.h
USERDEP__QTABW=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtabwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABW)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtabwidget.h -o tmp\moc\release_mt_shared\moc_qtabwidget.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtabwidget.h
USERDEP__QTABW=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtabwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABW)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtabwidget.h -o tmp\moc\release_mt_shared\moc_qtabwidget.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtextbrowser.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtextbrowser.h
USERDEP__QTEXT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtextbrowser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTEXT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtextbrowser.h -o tmp\moc\release_mt_shared\moc_qtextbrowser.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtextbrowser.h
USERDEP__QTEXT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtextbrowser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTEXT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtextbrowser.h -o tmp\moc\release_mt_shared\moc_qtextbrowser.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtextcodecplugin.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtextcodecplugin.h
USERDEP__QTEXTC=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtextcodecplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTEXTC)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtextcodecplugin.h -o tmp\moc\release_mt_shared\moc_qtextcodecplugin.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtextcodecplugin.h
USERDEP__QTEXTC=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtextcodecplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTEXTC)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtextcodecplugin.h -o tmp\moc\release_mt_shared\moc_qtextcodecplugin.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtextedit.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtextedit.h
USERDEP__QTEXTE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtextedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTEXTE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtextedit.h -o tmp\moc\release_mt_shared\moc_qtextedit.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtextedit.h
USERDEP__QTEXTE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtextedit.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTEXTE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtextedit.h -o tmp\moc\release_mt_shared\moc_qtextedit.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtextview.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtextview.h
USERDEP__QTEXTV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtextview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTEXTV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtextview.h -o tmp\moc\release_mt_shared\moc_qtextview.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtextview.h
USERDEP__QTEXTV=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtextview.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTEXTV)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtextview.h -o tmp\moc\release_mt_shared\moc_qtextview.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtimer.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtimer.h
USERDEP__QTIME=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtimer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTIME)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtimer.h -o tmp\moc\release_mt_shared\moc_qtimer.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtimer.h
USERDEP__QTIME=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtimer.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTIME)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtimer.h -o tmp\moc\release_mt_shared\moc_qtimer.cpp
<< 
	

!ENDIF 

SOURCE=widgets\qtitlebar_p.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=widgets\qtitlebar_p.h
USERDEP__QTITL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtitlebar_p.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTITL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qtitlebar_p.h -o tmp\moc\release_mt_shared\moc_qtitlebar_p.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=widgets\qtitlebar_p.h
USERDEP__QTITL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtitlebar_p.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTITL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qtitlebar_p.h -o tmp\moc\release_mt_shared\moc_qtitlebar_p.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtoolbar.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtoolbar.h
USERDEP__QTOOL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtoolbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtoolbar.h -o tmp\moc\release_mt_shared\moc_qtoolbar.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtoolbar.h
USERDEP__QTOOL=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtoolbar.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtoolbar.h -o tmp\moc\release_mt_shared\moc_qtoolbar.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtoolbox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtoolbox.h
USERDEP__QTOOLB=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtoolbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLB)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtoolbox.h -o tmp\moc\release_mt_shared\moc_qtoolbox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtoolbox.h
USERDEP__QTOOLB=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtoolbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLB)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtoolbox.h -o tmp\moc\release_mt_shared\moc_qtoolbox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtoolbutton.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtoolbutton.h
USERDEP__QTOOLBU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtoolbutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLBU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtoolbutton.h -o tmp\moc\release_mt_shared\moc_qtoolbutton.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtoolbutton.h
USERDEP__QTOOLBU=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtoolbutton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLBU)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtoolbutton.h -o tmp\moc\release_mt_shared\moc_qtoolbutton.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtooltip.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtooltip.h
USERDEP__QTOOLT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtooltip.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtooltip.h -o tmp\moc\release_mt_shared\moc_qtooltip.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtooltip.h
USERDEP__QTOOLT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtooltip.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtooltip.h -o tmp\moc\release_mt_shared\moc_qtooltip.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qtranslator.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qtranslator.h
USERDEP__QTRAN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtranslator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTRAN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtranslator.h -o tmp\moc\release_mt_shared\moc_qtranslator.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qtranslator.h
USERDEP__QTRAN=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qtranslator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTRAN)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qtranslator.h -o tmp\moc\release_mt_shared\moc_qtranslator.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qurloperator.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qurloperator.h
USERDEP__QURLO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qurloperator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QURLO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qurloperator.h -o tmp\moc\release_mt_shared\moc_qurloperator.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qurloperator.h
USERDEP__QURLO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qurloperator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QURLO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qurloperator.h -o tmp\moc\release_mt_shared\moc_qurloperator.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qvalidator.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qvalidator.h
USERDEP__QVALI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qvalidator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QVALI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qvalidator.h -o tmp\moc\release_mt_shared\moc_qvalidator.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qvalidator.h
USERDEP__QVALI=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qvalidator.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QVALI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qvalidator.h -o tmp\moc\release_mt_shared\moc_qvalidator.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qvbox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qvbox.h
USERDEP__QVBOX=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qvbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QVBOX)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qvbox.h -o tmp\moc\release_mt_shared\moc_qvbox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qvbox.h
USERDEP__QVBOX=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qvbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QVBOX)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qvbox.h -o tmp\moc\release_mt_shared\moc_qvbox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qvbuttongroup.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qvbuttongroup.h
USERDEP__QVBUT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qvbuttongroup.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QVBUT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qvbuttongroup.h -o tmp\moc\release_mt_shared\moc_qvbuttongroup.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qvbuttongroup.h
USERDEP__QVBUT=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qvbuttongroup.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QVBUT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qvbuttongroup.h -o tmp\moc\release_mt_shared\moc_qvbuttongroup.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qvgroupbox.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qvgroupbox.h
USERDEP__QVGRO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qvgroupbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QVGRO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qvgroupbox.h -o tmp\moc\release_mt_shared\moc_qvgroupbox.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qvgroupbox.h
USERDEP__QVGRO=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qvgroupbox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QVGRO)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qvgroupbox.h -o tmp\moc\release_mt_shared\moc_qvgroupbox.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qwidget.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qwidget.h
USERDEP__QWIDG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIDG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwidget.h -o tmp\moc\release_mt_shared\moc_qwidget.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qwidget.h
USERDEP__QWIDG=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwidget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIDG)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwidget.h -o tmp\moc\release_mt_shared\moc_qwidget.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qwidgetplugin.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qwidgetplugin.h
USERDEP__QWIDGE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwidgetplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIDGE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwidgetplugin.h -o tmp\moc\release_mt_shared\moc_qwidgetplugin.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qwidgetplugin.h
USERDEP__QWIDGE=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwidgetplugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIDGE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwidgetplugin.h -o tmp\moc\release_mt_shared\moc_qwidgetplugin.cpp
<< 
	

!ENDIF 

SOURCE=widgets\qwidgetresizehandler_p.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=widgets\qwidgetresizehandler_p.h
USERDEP__QWIDGET=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwidgetresizehandler_p.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIDGET)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qwidgetresizehandler_p.h -o tmp\moc\release_mt_shared\moc_qwidgetresizehandler_p.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=widgets\qwidgetresizehandler_p.h
USERDEP__QWIDGET=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwidgetresizehandler_p.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIDGET)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qwidgetresizehandler_p.h -o tmp\moc\release_mt_shared\moc_qwidgetresizehandler_p.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qwidgetstack.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qwidgetstack.h
USERDEP__QWIDGETS=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwidgetstack.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIDGETS)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwidgetstack.h -o tmp\moc\release_mt_shared\moc_qwidgetstack.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qwidgetstack.h
USERDEP__QWIDGETS=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwidgetstack.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIDGETS)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwidgetstack.h -o tmp\moc\release_mt_shared\moc_qwidgetstack.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qwindowsstyle.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qwindowsstyle.h
USERDEP__QWIND=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwindowsstyle.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIND)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwindowsstyle.h -o tmp\moc\release_mt_shared\moc_qwindowsstyle.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qwindowsstyle.h
USERDEP__QWIND=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwindowsstyle.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIND)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwindowsstyle.h -o tmp\moc\release_mt_shared\moc_qwindowsstyle.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qwizard.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qwizard.h
USERDEP__QWIZA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwizard.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIZA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwizard.h -o tmp\moc\release_mt_shared\moc_qwizard.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qwizard.h
USERDEP__QWIZA=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qwizard.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWIZA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qwizard.h -o tmp\moc\release_mt_shared\moc_qwizard.cpp
<< 
	

!ENDIF 

SOURCE=..\include\qworkspace.h

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=..\include\qworkspace.h
USERDEP__QWORK=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qworkspace.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWORK)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qworkspace.h -o tmp\moc\release_mt_shared\moc_qworkspace.cpp
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=..\include\qworkspace.h
USERDEP__QWORK=""$(QTDIR)\bin\moc.exe""	

".\tmp\moc\release_mt_shared\moc_qworkspace.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWORK)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc ..\include\qworkspace.h -o tmp\moc\release_mt_shared\moc_qworkspace.cpp
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\moc_qaccel.cpp

"$(INTDIR)\moc_qaccel.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qaccel.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qaction.cpp

"$(INTDIR)\moc_qaction.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qaction.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qapplication.cpp

"$(INTDIR)\moc_qapplication.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qasyncio.cpp

"$(INTDIR)\moc_qasyncio.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qasyncio.h" "..\include\qsignal.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qbutton.cpp

"$(INTDIR)\moc_qbutton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qbuttongroup.cpp

"$(INTDIR)\moc_qbuttongroup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qbuttongroup.h" "..\include\qgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qcanvas.cpp

"$(INTDIR)\moc_qcanvas.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qcanvas.h" "..\include\qscrollview.h" "..\include\qframe.h" "..\include\qscrollbar.h" "..\include\qrangecontrol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qcheckbox.cpp

"$(INTDIR)\moc_qcheckbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qcheckbox.h" "..\include\qbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qclipboard.cpp

"$(INTDIR)\moc_qclipboard.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qclipboard.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qcolordialog.cpp

"$(INTDIR)\moc_qcolordialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h" "..\include\qcolordialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qcombobox.cpp

"$(INTDIR)\moc_qcombobox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qcombobox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qcommonstyle.cpp

"$(INTDIR)\moc_qcommonstyle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qcommonstyle.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdatabrowser.cpp

"$(INTDIR)\moc_qdatabrowser.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qdatabrowser.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdatatable.cpp

"$(INTDIR)\moc_qdatatable.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qheader.h" "..\include\qguardedptr.h" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qdatatable.h" "..\include\qframe.h" "..\include\qtable.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdataview.cpp

"$(INTDIR)\moc_qdataview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdataview.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdatetimeedit.cpp

"$(INTDIR)\moc_qdatetimeedit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdatetimeedit.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdesktopwidget.cpp

"$(INTDIR)\moc_qdesktopwidget.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdial.cpp

"$(INTDIR)\moc_qdial.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qrangecontrol.h" "..\include\qdial.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdialog.cpp

"$(INTDIR)\moc_qdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdialogbuttons_p.cpp

"$(INTDIR)\moc_qdialogbuttons_p.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" ".\widgets\qdialogbuttons_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdns.cpp

"$(INTDIR)\moc_qdns.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsocketnotifier.h" "..\include\qdns.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdockarea.cpp

"$(INTDIR)\moc_qdockarea.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qdockarea.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qdockwindow.h" "..\include\qframe.h" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdockwindow.cpp

"$(INTDIR)\moc_qdockwindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdockwindow.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qdragobject.cpp

"$(INTDIR)\moc_qdragobject.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdragobject.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qerrormessage.cpp

"$(INTDIR)\moc_qerrormessage.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qerrormessage.h" "..\include\qdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qeventloop.cpp

"$(INTDIR)\moc_qeventloop.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qeventloop.h" "..\include\qsocketnotifier.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qfiledialog.cpp

"$(INTDIR)\moc_qfiledialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qnetworkprotocol.h" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h" "..\include\qfiledialog.h" "..\include\qurloperator.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qfontdialog.cpp

"$(INTDIR)\moc_qfontdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h" "..\include\qfontdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qframe.cpp

"$(INTDIR)\moc_qframe.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qftp.cpp

"$(INTDIR)\moc_qftp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qnetworkprotocol.h" "$(INTDIR)\jcinit.obj" "..\include\qftp.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qgl.cpp

"$(INTDIR)\moc_qgl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgl.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qgplugin.cpp

"$(INTDIR)\moc_qgplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qgrid.cpp

"$(INTDIR)\moc_qgrid.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgrid.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qgridview.cpp

"$(INTDIR)\moc_qgridview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qscrollview.h" "..\include\qframe.h" "..\include\qscrollbar.h" "..\include\qgridview.h" "..\include\qrangecontrol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qgroupbox.cpp

"$(INTDIR)\moc_qgroupbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qguardedptr.cpp

"$(INTDIR)\moc_qguardedptr.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qguardedptr.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qhbox.cpp

"$(INTDIR)\moc_qhbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qhbox.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qhbuttongroup.cpp

"$(INTDIR)\moc_qhbuttongroup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qhbuttongroup.h" "..\include\qbuttongroup.h" "..\include\qgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qheader.cpp

"$(INTDIR)\moc_qheader.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qheader.h" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qhgroupbox.cpp

"$(INTDIR)\moc_qhgroupbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qgroupbox.h" "..\include\qhgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qhttp.cpp

"$(INTDIR)\moc_qhttp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qnetworkprotocol.h" "$(INTDIR)\jcinit.obj" "..\include\qhttp.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qiconview.cpp

"$(INTDIR)\moc_qiconview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qdragobject.h" "$(INTDIR)\jcinit.obj" "..\include\qiconview.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qimageformatplugin.cpp

"$(INTDIR)\moc_qimageformatplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h" "..\include\qimageformatplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qinputdialog.cpp

"$(INTDIR)\moc_qinputdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlineedit.h" "..\include\qdialog.h" "..\include\qframe.h" "..\include\qinputdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qlabel.cpp

"$(INTDIR)\moc_qlabel.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlabel.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qlayout.cpp

"$(INTDIR)\moc_qlayout.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlayout.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qlcdnumber.cpp

"$(INTDIR)\moc_qlcdnumber.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlcdnumber.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qlineedit.cpp

"$(INTDIR)\moc_qlineedit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlineedit.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qlistbox.cpp

"$(INTDIR)\moc_qlistbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "$(INTDIR)\jcinit.obj" "..\include\qlistbox.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qlistview.cpp

"$(INTDIR)\moc_qlistview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "$(INTDIR)\jcinit.obj" "..\include\qlistview.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qlocalfs.cpp

"$(INTDIR)\moc_qlocalfs.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlocalfs.h" "..\include\qnetworkprotocol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qmainwindow.cpp

"$(INTDIR)\moc_qmainwindow.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qtoolbar.h" "..\include\qdockwindow.h" "..\include\qmainwindow.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qmenubar.cpp

"$(INTDIR)\moc_qmenubar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qmenubar.h" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qmessagebox.cpp

"$(INTDIR)\moc_qmessagebox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qmessagebox.h" "..\include\qdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qmultilineedit.cpp

"$(INTDIR)\moc_qmultilineedit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qmultilineedit.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qnetworkprotocol.cpp

"$(INTDIR)\moc_qnetworkprotocol.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qnetworkprotocol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qobject.cpp

"$(INTDIR)\moc_qobject.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qobjectcleanuphandler.cpp

"$(INTDIR)\moc_qobjectcleanuphandler.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qobjectcleanuphandler.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qpopupmenu.cpp

"$(INTDIR)\moc_qpopupmenu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsignal.h" "..\include\qpopupmenu.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qprocess.cpp

"$(INTDIR)\moc_qprocess.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qprocess.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qprogressbar.cpp

"$(INTDIR)\moc_qprogressbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qprogressbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qprogressdialog.cpp

"$(INTDIR)\moc_qprogressdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qlabel.h" "..\include\qsemimodal.h" "..\include\qdialog.h" "..\include\qframe.h" "..\include\qprogressdialog.h" "..\include\qprogressbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qpushbutton.cpp

"$(INTDIR)\moc_qpushbutton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qbutton.h" "..\include\qpushbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qradiobutton.cpp

"$(INTDIR)\moc_qradiobutton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qradiobutton.h" "..\include\qbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qrangecontrol.cpp

"$(INTDIR)\moc_qrangecontrol.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qrangecontrol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qrichtext_p.cpp

"$(INTDIR)\moc_qrichtext_p.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qlayout.h" ".\kernel\qrichtext_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qscrollbar.cpp

"$(INTDIR)\moc_qscrollbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qscrollbar.h" "..\include\qrangecontrol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qscrollview.cpp

"$(INTDIR)\moc_qscrollview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsemimodal.cpp

"$(INTDIR)\moc_qsemimodal.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsemimodal.h" "..\include\qdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qserversocket.cpp

"$(INTDIR)\moc_qserversocket.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qserversocket.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsessionmanager.cpp

"$(INTDIR)\moc_qsessionmanager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsessionmanager.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsignal.cpp

"$(INTDIR)\moc_qsignal.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsignal.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsignalmapper.cpp

"$(INTDIR)\moc_qsignalmapper.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsignalmapper.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsizegrip.cpp

"$(INTDIR)\moc_qsizegrip.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsizegrip.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qslider.cpp

"$(INTDIR)\moc_qslider.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qslider.h" "..\include\qrangecontrol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsocket.cpp

"$(INTDIR)\moc_qsocket.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsocket.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsocketnotifier.cpp

"$(INTDIR)\moc_qsocketnotifier.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsocketnotifier.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsound.cpp

"$(INTDIR)\moc_qsound.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsound.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qspinbox.cpp

"$(INTDIR)\moc_qspinbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qspinbox.h" "..\include\qframe.h" "..\include\qrangecontrol.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsplashscreen.cpp

"$(INTDIR)\moc_qsplashscreen.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsplashscreen.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsplitter.cpp

"$(INTDIR)\moc_qsplitter.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsplitter.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsqldatabase.cpp

"$(INTDIR)\moc_qsqldatabase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsqlquery.h" "..\include\qsqldatabase.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsqldriver.cpp

"$(INTDIR)\moc_qsqldriver.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qsqlquery.h" "$(INTDIR)\jcinit.obj" "..\include\qsqldriver.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsqldriverplugin.cpp

"$(INTDIR)\moc_qsqldriverplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h" "..\include\qsqldriverplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsqlform.cpp

"$(INTDIR)\moc_qsqlform.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsqlform.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qsqlquery.cpp

"$(INTDIR)\moc_qsqlquery.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qsqlquery.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qstatusbar.cpp

"$(INTDIR)\moc_qstatusbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qstatusbar.h" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qstyle.cpp

"$(INTDIR)\moc_qstyle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qstyleplugin.cpp

"$(INTDIR)\moc_qstyleplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h" "..\include\qstyleplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qstylesheet.cpp

"$(INTDIR)\moc_qstylesheet.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtabbar.cpp

"$(INTDIR)\moc_qtabbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qtabbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtabdialog.cpp

"$(INTDIR)\moc_qtabdialog.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h" "..\include\qtabdialog.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtable.cpp

"$(INTDIR)\moc_qtable.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qheader.h" "..\include\qguardedptr.h" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qtable.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtabwidget.cpp

"$(INTDIR)\moc_qtabwidget.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qtabwidget.h" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtextbrowser.cpp

"$(INTDIR)\moc_qtextbrowser.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qtextbrowser.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtextcodecplugin.cpp

"$(INTDIR)\moc_qtextcodecplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qtextcodecplugin.h" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtextedit.cpp

"$(INTDIR)\moc_qtextedit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtextview.cpp

"$(INTDIR)\moc_qtextview.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qrangecontrol.h" "..\include\qtextedit.h" "$(INTDIR)\jcinit.obj" "..\include\qstylesheet.h" "..\include\qframe.h" "..\include\qscrollview.h" "..\include\qscrollbar.h" "..\include\qtextview.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtimer.cpp

"$(INTDIR)\moc_qtimer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtitlebar_p.cpp

"$(INTDIR)\moc_qtitlebar_p.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qlabel.h" "..\include\qbutton.h" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" ".\widgets\qtitlebar_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtoolbar.cpp

"$(INTDIR)\moc_qtoolbar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qtoolbar.h" "..\include\qdockwindow.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtoolbox.cpp

"$(INTDIR)\moc_qtoolbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qtoolbox.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtoolbutton.cpp

"$(INTDIR)\moc_qtoolbutton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qbutton.h" "..\include\qtoolbutton.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtooltip.cpp

"$(INTDIR)\moc_qtooltip.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qtooltip.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qtranslator.cpp

"$(INTDIR)\moc_qtranslator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qurloperator.cpp

"$(INTDIR)\moc_qurloperator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "..\include\qnetworkprotocol.h" "$(INTDIR)\jcinit.obj" "..\include\qurloperator.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qvalidator.cpp

"$(INTDIR)\moc_qvalidator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qvalidator.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qvbox.cpp

"$(INTDIR)\moc_qvbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qhbox.h" "..\include\qvbox.h" "..\include\qframe.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qvbuttongroup.cpp

"$(INTDIR)\moc_qvbuttongroup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qbuttongroup.h" "..\include\qvbuttongroup.h" "..\include\qgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qvgroupbox.cpp

"$(INTDIR)\moc_qvgroupbox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qgroupbox.h" "..\include\qvgroupbox.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qwidget.cpp

"$(INTDIR)\moc_qwidget.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qwidgetplugin.cpp

"$(INTDIR)\moc_qwidgetplugin.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qgplugin.h" "..\include\qwidgetplugin.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qwidgetresizehandler_p.cpp

"$(INTDIR)\moc_qwidgetresizehandler_p.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" ".\widgets\qwidgetresizehandler_p.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qwidgetstack.cpp

"$(INTDIR)\moc_qwidgetstack.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qframe.h" "..\include\qwidgetstack.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qwindowsstyle.cpp

"$(INTDIR)\moc_qwindowsstyle.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qwindowsstyle.h" "..\include\qcommonstyle.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qwizard.cpp

"$(INTDIR)\moc_qwizard.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qdialog.h" "..\include\qwizard.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\moc_qworkspace.cpp

"$(INTDIR)\moc_qworkspace.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\qt_mt.pch" "$(INTDIR)\jcinit.obj" "..\include\qworkspace.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=tmp\moc\release_mt_shared\qcolordialog.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qcolordialog.moc
USERDEP__QCOLOR=".\dialogs\qcolordialog.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qcolordialog.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCOLOR)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc dialogs\qcolordialog.cpp -o tmp\moc\release_mt_shared\qcolordialog.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qcolordialog.moc
USERDEP__QCOLOR=".\dialogs\qcolordialog.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qcolordialog.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QCOLOR)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc dialogs\qcolordialog.cpp -o tmp\moc\release_mt_shared\qcolordialog.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qdatetimeedit.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qdatetimeedit.moc
USERDEP__QDATET=".\widgets\qdatetimeedit.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qdatetimeedit.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATET)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qdatetimeedit.cpp -o tmp\moc\release_mt_shared\qdatetimeedit.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qdatetimeedit.moc
USERDEP__QDATET=".\widgets\qdatetimeedit.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qdatetimeedit.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDATET)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qdatetimeedit.cpp -o tmp\moc\release_mt_shared\qdatetimeedit.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qdockwindow.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qdockwindow.moc
USERDEP__QDOCKWI=".\widgets\qdockwindow.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qdockwindow.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDOCKWI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qdockwindow.cpp -o tmp\moc\release_mt_shared\qdockwindow.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qdockwindow.moc
USERDEP__QDOCKWI=".\widgets\qdockwindow.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qdockwindow.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QDOCKWI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qdockwindow.cpp -o tmp\moc\release_mt_shared\qdockwindow.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qeffects.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qeffects.moc
USERDEP__QEFFE=".\widgets\qeffects.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qeffects.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QEFFE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qeffects.cpp -o tmp\moc\release_mt_shared\qeffects.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qeffects.moc
USERDEP__QEFFE=".\widgets\qeffects.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qeffects.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QEFFE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qeffects.cpp -o tmp\moc\release_mt_shared\qeffects.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qfiledialog.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qfiledialog.moc
USERDEP__QFILED=".\dialogs\qfiledialog.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qfiledialog.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFILED)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc dialogs\qfiledialog.cpp -o tmp\moc\release_mt_shared\qfiledialog.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qfiledialog.moc
USERDEP__QFILED=".\dialogs\qfiledialog.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qfiledialog.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFILED)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc dialogs\qfiledialog.cpp -o tmp\moc\release_mt_shared\qfiledialog.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qftp.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qftp.moc
USERDEP__QFTP_M=".\network\qftp.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qftp.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFTP_M)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc network\qftp.cpp -o tmp\moc\release_mt_shared\qftp.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qftp.moc
USERDEP__QFTP_M=".\network\qftp.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qftp.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QFTP_M)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc network\qftp.cpp -o tmp\moc\release_mt_shared\qftp.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qmainwindow.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qmainwindow.moc
USERDEP__QMAINW=".\widgets\qmainwindow.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qmainwindow.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMAINW)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qmainwindow.cpp -o tmp\moc\release_mt_shared\qmainwindow.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qmainwindow.moc
USERDEP__QMAINW=".\widgets\qmainwindow.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qmainwindow.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMAINW)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qmainwindow.cpp -o tmp\moc\release_mt_shared\qmainwindow.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qmessagebox.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qmessagebox.moc
USERDEP__QMESSA=".\dialogs\qmessagebox.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qmessagebox.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMESSA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc dialogs\qmessagebox.cpp -o tmp\moc\release_mt_shared\qmessagebox.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qmessagebox.moc
USERDEP__QMESSA=".\dialogs\qmessagebox.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qmessagebox.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMESSA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc dialogs\qmessagebox.cpp -o tmp\moc\release_mt_shared\qmessagebox.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qmovie.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qmovie.moc
USERDEP__QMOVI=".\kernel\qmovie.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qmovie.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMOVI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc kernel\qmovie.cpp -o tmp\moc\release_mt_shared\qmovie.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qmovie.moc
USERDEP__QMOVI=".\kernel\qmovie.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qmovie.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QMOVI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc kernel\qmovie.cpp -o tmp\moc\release_mt_shared\qmovie.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qscrollview.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qscrollview.moc
USERDEP__QSCROLL=".\widgets\qscrollview.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qscrollview.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSCROLL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qscrollview.cpp -o tmp\moc\release_mt_shared\qscrollview.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qscrollview.moc
USERDEP__QSCROLL=".\widgets\qscrollview.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qscrollview.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSCROLL)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qscrollview.cpp -o tmp\moc\release_mt_shared\qscrollview.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qsound_win.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qsound_win.moc
USERDEP__QSOUND=".\kernel\qsound_win.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qsound_win.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSOUND)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc kernel\qsound_win.cpp -o tmp\moc\release_mt_shared\qsound_win.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qsound_win.moc
USERDEP__QSOUND=".\kernel\qsound_win.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qsound_win.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSOUND)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc kernel\qsound_win.cpp -o tmp\moc\release_mt_shared\qsound_win.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qsplitter.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qsplitter.moc
USERDEP__QSPLIT=".\widgets\qsplitter.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qsplitter.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSPLIT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qsplitter.cpp -o tmp\moc\release_mt_shared\qsplitter.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qsplitter.moc
USERDEP__QSPLIT=".\widgets\qsplitter.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qsplitter.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QSPLIT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qsplitter.cpp -o tmp\moc\release_mt_shared\qsplitter.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qtable.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qtable.moc
USERDEP__QTABLE=".\table\qtable.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qtable.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABLE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc table\qtable.cpp -o tmp\moc\release_mt_shared\qtable.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qtable.moc
USERDEP__QTABLE=".\table\qtable.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qtable.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTABLE)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc table\qtable.cpp -o tmp\moc\release_mt_shared\qtable.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qtoolbar.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qtoolbar.moc
USERDEP__QTOOLBA=".\widgets\qtoolbar.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qtoolbar.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLBA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qtoolbar.cpp -o tmp\moc\release_mt_shared\qtoolbar.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qtoolbar.moc
USERDEP__QTOOLBA=".\widgets\qtoolbar.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qtoolbar.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLBA)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qtoolbar.cpp -o tmp\moc\release_mt_shared\qtoolbar.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qtooltip.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qtooltip.moc
USERDEP__QTOOLTI=".\widgets\qtooltip.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qtooltip.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLTI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qtooltip.cpp -o tmp\moc\release_mt_shared\qtooltip.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qtooltip.moc
USERDEP__QTOOLTI=".\widgets\qtooltip.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qtooltip.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QTOOLTI)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qtooltip.cpp -o tmp\moc\release_mt_shared\qtooltip.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qwhatsthis.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qwhatsthis.moc
USERDEP__QWHAT=".\widgets\qwhatsthis.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qwhatsthis.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWHAT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qwhatsthis.cpp -o tmp\moc\release_mt_shared\qwhatsthis.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qwhatsthis.moc
USERDEP__QWHAT=".\widgets\qwhatsthis.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qwhatsthis.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWHAT)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc widgets\qwhatsthis.cpp -o tmp\moc\release_mt_shared\qwhatsthis.moc
<< 
	

!ENDIF 

SOURCE=tmp\moc\release_mt_shared\qworkspace.moc

!IF  "$(CFG)" == "qtmt - Win32 Release"

InputPath=tmp\moc\release_mt_shared\qworkspace.moc
USERDEP__QWORKS=".\workspace\qworkspace.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qworkspace.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWORKS)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc workspace\qworkspace.cpp -o tmp\moc\release_mt_shared\qworkspace.moc
<< 
	

!ELSEIF  "$(CFG)" == "qtmt - Win32 Debug"

InputPath=tmp\moc\release_mt_shared\qworkspace.moc
USERDEP__QWORKS=".\workspace\qworkspace.cpp"	"$(QTDIR)\bin\moc.exe"	

".\tmp\moc\release_mt_shared\qworkspace.moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__QWORKS)
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc workspace\qworkspace.cpp -o tmp\moc\release_mt_shared\qworkspace.moc
<< 
	

!ENDIF 


!ENDIF 

