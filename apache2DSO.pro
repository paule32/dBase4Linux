#------------------------------------------------------------------------
# dBase4Linux 1.0
# RDP - Rapid Database Programming
#
# (c) 2015-2016 Jens Kallup
#------------------------------------------------------------------------
TEMPLATE = lib
TARGET   = mod_dbase
CONFIG  += debug

QT = core gui widgets help network

TOPDIR =$$PWD
SRCDIR =$${TOPDIR}/source
HDRDIR =$${SRCDIR}/includes

GENFOLDER =$${TOPDIR}/build/

UI_DIR =$${TOPDIR}/.uic
MOC_DIR=$${TOPDIR}/.moc
OBJ_DIR=$${TOPDIR}/.obj
RCC_DIR=$${TOPDIR}/.res

DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M:%S')\\\"
DEFINES += BUILDDATE=\\\"$$system(date '+%Y-%m-%d')\\\"

#----------------------------------------------------------------------
# if you would not use pch - pre-compiled-header, just remove -H block
#----------------------------------------------------------------------
QMAKE_CXXFLAGS += -H \
	-Wno-unused-parameter \
	-Wno-unused-variable \
	-Wno-unused-local-typedefs \
        -Wno-unused-but-set-variable \
        -Wno-write-strings \
        -Wno-extra \
        -Wno-reorder \
        -Wunused-function \
        -lmod_dbase \
        -Woverloaded-virtual -fpermissive \
        -frtti -fexceptions  -std=c++17 \
        -I$${TOPDIR}. -I$${SRCDIR} -I$${SRCDIR}/includes \
        -D__BYTE_ORDER=__LITTLE_ENDIAN \
        -DQT_DEPRECATED \
        -DQT_DISABLE_DEPRECATED_BEFORE

INCLUDEPATH += \
        /usr/include \
        $${SRCDIR}/ \
        $${SRCDIR}/includes

SOURCES += $${SRCDIR}/parser/dbase/yymain.cc

HEADERS +=
FORMS   +=

RESOURCES +=

LIBS += -L"/usr/local/lib64" -lstdc++

DISTFILES += \
    source/html/index.php

