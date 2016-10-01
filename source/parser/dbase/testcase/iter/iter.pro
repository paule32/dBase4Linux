TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M:%S')\\\"
DEFINES += BUILDDATE=\\\"$$system(date '+%Y-%m-%d')\\\"

QMAKE_CXXFLAGS += \
    -frtti -fexceptions -fpermissive -Wno-sign-compare

SOURCES += main.cc

