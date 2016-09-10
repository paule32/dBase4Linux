TEMPLATE = app

CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

TOPDIR=$$PWD
SRCDIR=$${TOPDIR}/source
HDRDIR=$${SRCDIR}/includes

GENFOLDER =$${TOPDIR}/build/

UI_DIR =$${TOPDIR}/.uic
MOC_DIR=$${TOPDIR}/.moc
OBJ_DIR=$${TOPDIR}/.obj
RCC_DIR=$${TOPDIR}/.res

DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M:%S')\\\"
DEFINES += BUILDDATE=\\\"$$system(date '+%Y-%m-%d')\\\"

INCLUDEPATH = \
        /usr/include \
        /usr/include/SDL2

SOURCES += main.cc

