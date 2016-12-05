#------------------------------------------------------------------------
# dBase4Linux 1.0
# RDP - Rapid Database Programming
#
# (c) 2015-2016 Jens Kallup
#------------------------------------------------------------------------
TEMPLATE = app
CONFIG += console debug
CONFIG -= app_bundle
CONFIG -= qt

TOPDIR=$$PWD
SRCDIR=$${TOPDIR}
HDRDIR=../includes

GENFOLDER =$${TOPDIR}/build/

UI_DIR =$${TOPDIR}/.uic
MOC_DIR=$${TOPDIR}/.moc
OBJ_DIR=$${TOPDIR}/.obj
RCC_DIR=$${TOPDIR}/.res

DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M:%S')\\\"
DEFINES += BUILDDATE=\\\"$$system(date '+%Y-%m-%d')\\\"

QMAKE_CXXFLAGS += \
    -std=c++11 \
    -Wno-write-strings \
    -Wno-unused-parameter \
    -Wno-unused-variable \
    -Wno-unused-local-typedefs \
    -Wno-unused-but-set-variable


INCLUDEPATH += \
    /usr/local/include \
    $${TOPDIR}/.uic \
    ./ ../includes ../includes/tv \
    /usr/include

SOURCES += \
    main.cc \
    ascii.cc \
    mousedlg.cc \
    fileview.cc \
    gadgets.cc \
    tdebughelpwindow.cc

DISTFILES += \
    debughelp.txt

HEADERS += \
    tdebughelpwindow.h \
    fileview.h \
    tvdemo.h \
    help.h

LIBS += -L/usr/local/lib -L/dbase/lib -lboost_system -lboost_serialization -lgpm -lrhtv -lz
