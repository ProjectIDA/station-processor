TEMPLATE	= app
LANGUAGE	= C++
INCLUDEPATH	+= ../../include
TARGET = isitapqt

LIBS += -lisi \
        -lcssio \
        -liacp \
        -lutil \
        -llogio \
        -L../../../lib/$(PLATFORM)

DEFINES += QT_CLEAN_NAMESPACE 
DEFINES += $(OSNAME) 
DEFINES += sparc

CONFIG	+= qt warn_on release thread

SOURCES		=	main.cpp \
				configDlg.cpp \
				DateTimeEdit.cpp \
				GettingDataThread.cpp \
				isitapdlg.cpp \
				isiTapResults.cpp \
				isitap_globals.cpp \
				isitap_time.cpp \
				ISI_GENERIC_TS_Packet.cpp \
				nrtschaninfo.cpp  \
				nrtsinfo.cpp \
				nrtsstainfo.cpp \
				ReadFileCache.cpp \
				thread.cpp \
				wfdisc.cpp

HEADERS		=  	configDlg.h \
				DateTimeEdit.h \
				GettingDataThread.h \
				isitapdlg.h \
				isiTapResults.h \
				isitap_globals.h \
				isitap_time.h \
				nrtschaninfo.h \
				nrtsinfo.h \
				nrtsstainfo.h \
				prcinfo.h \
				ReadFileCache.h \
				resource.h \
				thread.h \
				wfdisc.h

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}



