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

SOURCES += mainclient.cpp r2t2clientqtradio.cpp r2t2clientdspqtradio.cpp r2t2clientconsole.cpp r2t2clientstandard.cpp
SOURCES += lib.cpp 
SOURCES += r2t2.pb.cc r2t2gui.pb.cc
SOURCES += r2t2radiolistener.cpp
SOURCES += TCPSink.cpp TCPSinkQtRadio.cpp
SOURCES += dsp/ProcessBlock.cpp dsp/ProcessBuffer.cpp dsp/Null.cpp
SOURCES += dsp/RX.cpp dsp/Split.cpp dsp/FFTInterpol.cpp
SOURCES += dsp/SSBDemod.cpp dsp/AMDemod.cpp dsp/FMDemod.cpp dsp/G711Encode.cpp dsp/g711.cpp dsp/FIRDecim.cpp dsp/Rotate.cpp
SOURCES += dsp/FFT.cpp dsp/kiss_fft.c dsp/AGC.cpp dsp/SMeter.cpp dsp/Notch.cpp
# SOURCES += dsp/fft_fftw.cpp 

HEADERS += r2t2clientqtradio.h r2t2clientdspqtradio.h r2t2clientconsole.h r2t2clientstandard.h
HEADERS += r2t2radiolistener.h

LIBS += -lprotobuf -lfftw3f -lvolk
LIBS += -lgnuradio-filter
