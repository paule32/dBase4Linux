#------------------------------------------------------------------------
# dBase4Linux 1.0
# RDP - Rapid Database Programming
#
# (c) 2015-2016 Jens Kallup
#------------------------------------------------------------------------
TEMPLATE = app
TARGET   = dbase
CONFIG  += release

QT = core gui widgets help network

PWD=$$system(pwd)

TOPDIR=$$PWD
SRCDIR=$${TOPDIR}/source
HDRDIR=$${SRCDIR}/includes

GENFOLDER =$${TOPDIR}/build/

TEMPLATE_DEPTH = 516

UI_DIR =$${TOPDIR}/.uic
MOC_DIR=$${TOPDIR}/.moc
OBJ_DIR=$${TOPDIR}/.obj
RCC_DIR=$${TOPDIR}/.res

DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M:%S')\\\"
DEFINES += BUILDDATE=\\\"$$system(date '+%Y-%m-%d')\\\"

#----------------------------------------------------------------------
# if you would not use pch - pre-compiled-header, just remove -H block
#----------------------------------------------------------------------
QMAKE_CXXFLAGS += \
	-std=c++14 -ggdb \
	-Wno-unused-parameter \
	-Wno-unused-variable \
	-Wno-unused-local-typedefs \
    -Wno-unused-but-set-variable \
    -Wno-write-strings \
	-Wno-switch \
    -Wno-extra \
    -Wno-reorder \
    -Wno-multichar \
    -Wno-sign-compare \
    -Wunused-function \
    -Woverloaded-virtual -fpermissive \
    -ftemplate-depth=$${TEMPLATE_DEPTH} \
    -frtti -fexceptions \
    -I/usr/local/include \
    -I/usr/local/include/boost \
    -I$${TOPDIR}. -I$${SRCDIR} -I$${SRCDIR}/includes \
    -D__BYTE_ORDER=__LITTLE_ENDIAN \
    -DQT_DEPRECATED \
    -DQT_DISABLE_DEPRECATED_BEFORE

dBaseHelp.target     = all
dBaseHelp.commands   = @./source/tools/html2qch.sh
dBaseHelp.depends    = FORCE

QMAKE_EXTRA_TARGETS += dBaseHelp

INCLUDEPATH += \
        /usr/local/include \
        $${TOPDIR}/.uic \
        $${SRCDIR}/ \
        $${SRCDIR}/includes \
        /usr/include

SOURCES += \
        $${SRCDIR}/dbase/antifreeze.cc \
        $${SRCDIR}/dbase/assistant.cc \
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
    source/dbase/update_thread.cc \
    source/dbase/mainclass.cc \
    source/parser/dbase/testcase/nstest1.cc \
    source/parser/dbase/dbaseWindow.cc \
    source/parser/dbase/type.cc

HEADERS += \
	$${HDRDIR}/antifreeze.h \
	$${HDRDIR}/assistant.h \
	$${HDRDIR}/helplistview.h \
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
        $${HDRDIR}/update_thread.h \
        $${HDRDIR}/mainclass.h \
    source/parser/dbase/if_expr_op.h \
    source/parser/dbase/expression_ast.h \
    source/parser/dbase/dbase_exception.h \
    source/parser/dbase/dbasevariables.h \
    source/parser/dbase/binary_op.h \
    source/parser/dbase/unary_op.h \
    source/parser/dbase/class_op.h \
    source/parser/dbase/testcase/namespace.h \
    source/parser/dbase/testcase/test1.h \
    source/parser/dbase/dBaseWindow.h \
    source/parser/dbase/type.h \
    source/parser/dbase/skipper.h \
    source/parser/dbase/symbol.h

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
    exec/dbase.sh \
    exec/dbase \
    examples/test1.prg \
    source/images/addierer.png \
    source/serverfiles/users.php \
    source/help/index.html \
    source/help/test.html \
    source/help/printline.html \
    source/help/append.html \
    source/html/bewerbung.html \
    source/html/style.css \
    source/html/bewerber.xml \
    source/html/bewerber.dtd \
    source/html/bewerber.xsl \
    source/html/index.html \
    examples/Form1.frm \
    examples/example1.prg \
    ../media/sdb1/e-learning/mov/speech/doc/images/neuron.png \
    ../media/sdb1/e-learning/mov/speech/doc/notizen.tex

LIBS += -L"/usr/local/lib64" -L/srv/Projekte/dbase/lib -lstdc++
LIBS += -lmod_dbase
