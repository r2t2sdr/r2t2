QT += core network
CONFIG += debug_and_release

QMAKE_CXXFLAGS += -std=c++0x
#QMAKE_CXXFLAGS += -mfloat-abi=softfp -ftree-vectorize -mfpu=neon -mvectorize-with-neon-quad -funsafe-math-optimizations -ffast-math -march=armv7-a -mcpu=cortex-a9  

TEMPLATE = app

TARGET = r2t2srv 

SOURCES += mainsrv.cpp r2t2srv.cpp  
SOURCES += lib.cpp 
SOURCES += r2t2.cpp reg.cpp si5344_init.cpp spi.cpp i2c.cpp socket.cpp
SOURCES += r2t2.pb.cc

HEADERS += r2t2srv.h

LIBS += -lprotobuf
