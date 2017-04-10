QT += core network
CONFIG += debug_and_release

QMAKE_CXXFLAGS += -std=c++0x  -Werror
#QMAKE_CXXFLAGS += -DDEBUG_TIME 
QMAKE_CXXFLAGS_DEBUG += -DDEBUG 

# neon opts now in /usr/lib/qt/mkspecs/common/gcc-base.conf 
# QMAKE_CXXFLAGS += -mfloat-abi=hard -march=armv7-a -mtune=cortex-a9 -mfpu=neon -mvectorize-with-neon-quad -funsafe-math-optimizations -Wformat=0 
# QMAKE_CXXFLAGS += -DNEON_OPTS

TEMPLATE = app

TARGET = r2t2client 

SOURCES += mainclient.cpp r2t2client.cpp  
SOURCES += lib.cpp 
SOURCES += r2t2.pb.cc
SOURCES += r2t2qtradiolistener.cpp
SOURCES += dsp/ProcessBlock.cpp dsp/ProcessBuffer.cpp
SOURCES += dsp/RX.cpp dsp/TCPSink.cpp dsp/Split.cpp dsp/FFTInterpol.cpp
SOURCES += dsp/SSBDemod.cpp dsp/AMDemod.cpp dsp/FMDemod.cpp dsp/G711Encode.cpp dsp/g711.cpp dsp/FIRDecim.cpp dsp/Rotate.cpp
SOURCES += dsp/FFT.cpp dsp/kiss_fft.c dsp/AGC.cpp dsp/SMeter.cpp dsp/Notch.cpp
# SOURCES += dsp/fft_fftw.cpp 

HEADERS += r2t2client.h
HEADERS += r2t2qtradiolistener.h

LIBS += -lprotobuf -lfftw3f -lvolk
LIBS += -lgnuradio-filter
