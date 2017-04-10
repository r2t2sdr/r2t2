QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = r2t2 

# CONFIG   -= app_bundle
CONFIG += qt debug_and_release

QMAKE_CXXFLAGS += -std=c++0x
TEMPLATE = app


win32 {
	DEFINES += WIN32
	CONFIG   += console
	LIBS +=  -lwinmm
	SVNDEV = 0
	DEFINES += SVNREV=0 
}

unix:!android {
	DEFINES += UNIX 
	CONFIG   += console
	SVNDEV = $$system(svnversion)
	DEFINES += SVNREV=\'\"$$SVNDEV\"\'
	#LIBS += -lasound
	#LIBS += -lwiringPi
	QT += multimedia
} 

unix:android {
	QT += multimedia
	DEFINES += ANDROID
	SVNDEV = $$system(svnversion)
	DEFINES += SVNREV=\'\"$$SVNDEV\"\'
} 

INCLUDEPATH += /usr/local/include/ 

SOURCES += main.cpp control.cpp sdr.cpp 
SOURCES += display_base.cpp display_touch.cpp display_lcd.cpp smeter.cpp clock.cpp
SOURCES += textbutton.cpp  numeric.cpp fftGraph.cpp filterGraph.cpp analog.cpp
SOURCES += lib.cpp label.cpp sdrgraphicsitem.cpp
SOURCES += g711.cpp

unix:!android {
	#SOURCES += keyreader.cpp audio.cpp
	#SOURCES += audioQt.cpp
	#SOURCES += audio.cpp
	SOURCES += audioQt.cpp
}

unix:android {
	SOURCES += audioQt.cpp
}

win32 {
	SOURCES += audio_win32.cpp
}

HEADERS += control.h sdr.h display_lcd.h display_touch.h display_base.h smeter.h clock.h
HEADERS += textbutton.h numeric.h fftGraph.h filterGraph.h analog.h label.h sdrgraphicsitem.h

unix:!android {
	#HEADERS += keyreader.h audio.h
	# HEADERS += audioQt.h
	HEADERS += audio.h
}

unix:android {
	HEADERS += audioQt.h
}

win32 {
	HEADERS += audio_win32.h
}

FORMS    += display_touch.ui

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml

