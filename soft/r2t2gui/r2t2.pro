QT       += core gui network widgets

TARGET = r2t2 

CONFIG += qt debug_and_release

QMAKE_CXXFLAGS += -std=c++0x
TEMPLATE = app

CONFIG(release, debug|release): DEFINES += NDEBUG

win32 {
	DEFINES += WIN32
	CONFIG   += console
	LIBS +=  -lwinmm
	SVNDEV = 0
	DEFINES += SVNREV="0" 
	QT += multimedia
}

unix {
	DEFINES += UNIX 
	CONFIG   += console
	SVNDEV = $$system(svnversion)
	DEFINES += SVNREV=\'\"$$SVNDEV\"\'
	#LIBS += -lasound
	#LIBS += -lwiringPi
	QT += multimedia
} 


# INCLUDEPATH += /usr/local/include/

# PKG-Config is a more robust way to import libaries on Linux / Mac
# since the libraries are not always installed at /usr/local/include
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += protobuf
}

SOURCES += main.cpp control.cpp sdrr2t2.cpp sdrqtradio.cpp
SOURCES += display_touch.cpp display_lcd.cpp smeter.cpp clock.cpp
SOURCES += textbutton.cpp  numeric.cpp fftGraph.cpp filterGraph.cpp analog.cpp
SOURCES += lib.cpp label.cpp sdrgraphicsitem.cpp
SOURCES += g711.cpp
SOURCES += r2t2gui.pb.cc
SOURCES += conf.cpp

LIBS += -lprotobuf

unix {
	#SOURCES += keyreader.cpp audio.cpp
	#SOURCES += audioQt.cpp
	#SOURCES += audio.cpp
	SOURCES += audioQt.cpp
}

win32 {
	SOURCES += audioQt.cpp
	# SOURCES += audio_win32.cpp
}

HEADERS += control.h sdr.h sdrr2t2.h sdrqtradio.h display_lcd.h display_touch.h display_base.h smeter.h clock.h
HEADERS += textbutton.h numeric.h fftGraph.h filterGraph.h analog.h label.h sdrgraphicsitem.h

unix:!mac {
        #HEADERS += keyreader.h audio.h
         HEADERS += audioQt.h
#        HEADERS += audio.h
}

mac: {
        HEADERS += audioQt.h
}

win32 {
        # HEADERS += audio_win32.h
        HEADERS += audioQt.h
}

FORMS    += display_touch.ui

