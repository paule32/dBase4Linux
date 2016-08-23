#------------------------------------------------------------------------
# dBase4Linux 1.0
# RDP - Rapid Database Programming
#
# (c) 2015-2016 Jens Kallup
#------------------------------------------------------------------------
TEMPLATE = app
TARGET   = dbase
CONFIG  += debug

QT = core gui widgets help network

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
        -Woverloaded-virtual -fpermissive \
        -ftemplate-depth=200 \
        -frtti -fexceptions  -std=c++14 \
        -I$${TOPDIR}. -I$${SRCDIR} -I$${SRCDIR}/includes \
        -D__BYTE_ORDER=__LITTLE_ENDIAN \
        -DQT_DEPRECATED \
        -DQT_DISABLE_DEPRECATED_BEFORE

INCLUDEPATH += \
        /usr/include \
        $${TOPDIR}/.uic \
        $${SRCDIR}/ \
        $${SRCDIR}/includes

SOURCES += \
        $${SRCDIR}/dbase/main.cc \
        $${SRCDIR}/dbase/mainwindow.cc \
        $${SRCDIR}/dbase/editorgutter.cc \
        $${SRCDIR}/dbase/scene.cc \
        $${SRCDIR}/dbase/designeritem.cc \
        $${SRCDIR}/dbase/designergraphicsview.cc \
        $${SRCDIR}/dbase/components_draglist.cpp \
        $${SRCDIR}/dbase/events_and_methods.cc \
        $${SRCDIR}/dbase/helpbrowser.cc \
        $${SRCDIR}/dbase/dbaseexception.cc \
        $${SRCDIR}/dbase/dbasemainmenu.cc \
        $${SRCDIR}/dbase/helpdocker.cc \
        $${SRCDIR}/dbase/click.cc \
        $${SRCDIR}/dbase/keypress.cc \
        \
        $${SRCDIR}/parser/dbase/yymain.cc \
        \
    source/dbase/settings_dialog.cc \
    source/dbase/update_dbase.cc \
    source/parser/rfc/rfc_update_dbase.cc \
    source/dbase/update_thread.cc

HEADERS += \
	$${HDRDIR}/mainwindow.h \
	$${HDRDIR}/editorgutter.h \
	$${HDRDIR}/scene.h \
	$${HDRDIR}/designeritem.h \
	$${HDRDIR}/designergraphicsview.h \
        $${HDRDIR}/components_draglist.h \
        $${HDRDIR}/events_and_methods.h \
        $${HDRDIR}/helpbrowser.h \
        $${HDRDIR}/dbaseexception.h \
        $${HDRDIR}/dbasemainmenu.h \
        $${HDRDIR}/pch.h \
        $${HDRDIR}/editorgutter.h \
        \
        \
        $${HDRDIR}/helpdocker.h \
        $${HDRDIR}/click.h \
        $${HDRDIR}/keypress.h \
        \
        \
        $${HDRDIR}/settings_dialog.h \
        $${HDRDIR}/update_dbase.h \
        $${HDRDIR}/update_thread.h

FORMS   += \
        $${SRCDIR}/forms/mainwindow.ui \
        $${SRCDIR}/forms/settings_dialog.ui \
        $${SRCDIR}/forms/update_dbase.ui

RESOURCES += \
        $${SRCDIR}/dbase/icons.qrc \
    source/dbase/icons.qrc

DISTFILES += \
        $${SRCDIR}/helper/help.qhp \
        $${SRCDIR}/help/dBaseHelp.qhcp \
        $${SRCDIR}/help/dBaseHelp.qhp \
    LICENSE \
    mygit.sh \
    README.md \
    examples/test1.prg \
    source/images/addierer.png \
    source/serverfiles/users.php

LIBS += -L"/usr/local/lib64" -lstdc++
LIBS += -L"/dbase" -lmod_dbase