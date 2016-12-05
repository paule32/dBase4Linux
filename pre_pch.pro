QT += core
QT -= gui

TARGET = pre_pch
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += pch.cc

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
        -frtti -fexceptions  -std=c++17 \
        -I/opt/qt5/5.5/gcc_64/include \
        -I. -I../source -I../source/includes \
        -D__BYTE_ORDER=__LITTLE_ENDIAN \
        -DQT_DEPRECATED \
        -DQT_DISABLE_DEPRECATED_BEFORE

INCLUDEPATH += \
        /usr/include \
        ../source \
        ../source/includes/
