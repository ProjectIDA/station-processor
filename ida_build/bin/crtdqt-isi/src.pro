CFLAGS   += -D$(OSNAME) \
            -DQT_CLEAN_NAMESPACE
CXXFLAGS += -D$(OSNAME) \
            -DQT_CLEAN_NAMESPACE
INCPATH  += ../../include
LIBS += -L../../../lib/$(PLATFORM) \
        -lisi \
        -lcssio \
        -liacp \
        -lutil \
        -llogio 
TARGET = crtdqt 
CONFIG += release \
          warn_on \
          qt \
          thread 
TEMPLATE = app 
HEADERS += nrtschaninfo.h \
           nrtsinfo.h \
           nrtsstainfo.h \
           set_all_parameters.h \
           DataBlock.h \
           MainFrame.h \
           Preview.h \
           CRTDglob.h \
           QmyButton.h \
           QmyToolBar.h \
           RTDisplay.h \
	       StopAcquisitionThread.h \
	       GettingDataThread.h \
           DrawWaveformThread.h \ 
           IsiInterruptThread.cpp
SOURCES += datablock.cpp \
           main.cpp \
           nrtschaninfo.cpp \
           nrtsinfo.cpp \
           nrtsstainfo.cpp \
           qmake_image_collection.cpp \
           set_all_parameters.cpp \
           thread.cpp \
           MainFrame.cpp \
           MWindow.cpp \
           Preview.cpp \
           CRTDglob.cpp \
           QmyButton.cpp \
           QmyToolBar.cpp \
           RTDisplay.cpp \
	       StopAcquisitionThread.cpp \
	       GettingDataThread.cpp \
           DrawWaveformThread.cpp \
           IsiInterruptThread.cpp
