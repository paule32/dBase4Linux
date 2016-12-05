#-------------------------------------------------
#
# Project created by QtCreator 2016-06-02T22:48:27
#
#-------------------------------------------------

QT       -= core gui

TARGET = asmemu
TEMPLATE = lib

TOPDIR =$$PWD
SRCDIR =$${TOPDIR}/source/assembler
HDRDIR =$${TOPDIR}/source/assembler

GENFOLDER =$${TOPDIR}/build/

UI_DIR      =$${TOPDIR}/.uic
MOC_DIR     =$${TOPDIR}/.moc
OBJECTS_DIR =$${TOPDIR}/.obj
RCC_DIR     =$${TOPDIR}/.res

QMAKE_CXXFLAGS += \
    -Wno-unused-parameter \
    -Wno-unused-variable \
    -Wno-unused-local-typedefs \
    -Wno-unused-but-set-variable \
    -Wno-write-strings \
    -Wno-extra \
    -Wunused-function \
    -Woverloaded-virtual -fpermissive \
    -frtti -fexceptions  -std=c++11 \
    -I$${TOPDIR}. -I$${TOPDIR}/source -I$${TOPDIR}/source/includes \
    -D__BYTE_ORDER=__LITTLE_ENDIAN \
    -DQT_DEPRECATED \
    -DQT_DISABLE_DEPRECATED_BEFORE \
    -fpermissive -fno-operator-names

SOURCES += \
    $${SRCDIR}/Assembler.cpp \
    $${SRCDIR}/CodeGenerator.cpp \
    $${SRCDIR}/Emulator.cpp \
    $${SRCDIR}/Encoding.cpp \
    $${SRCDIR}/Error.cpp \
    $${SRCDIR}/Instruction.cpp \
    $${SRCDIR}/InstructionSet.cpp \
    $${SRCDIR}/Linker.cpp \
    $${SRCDIR}/Loader.cpp \
    $${SRCDIR}/Macro.cpp \
    $${SRCDIR}/Operand.cpp \
    $${SRCDIR}/Optimizer.cpp \
    $${SRCDIR}/Parser.cpp \
    $${SRCDIR}/RegisterAllocator.cpp \
    $${SRCDIR}/Scanner.cpp \
    $${SRCDIR}/Synthesizer.cpp \
    $${SRCDIR}/Token.cpp \
    $${SRCDIR}/TokenList.cpp

HEADERS += \
    $${HDRDIR}/Assembler.hpp \
    $${HDRDIR}/CharType.hpp \
    $${HDRDIR}/CodeGenerator.hpp \
    $${HDRDIR}/Emulator.hpp \
    $${HDRDIR}/Encoding.hpp \
    $${HDRDIR}/Error.hpp \
    $${HDRDIR}/File.hpp \
    $${HDRDIR}/Instruction.hpp \
    $${HDRDIR}/InstructionSet.hpp \
    $${HDRDIR}/Intrinsics.hpp \
    $${HDRDIR}/Link.hpp \
    $${HDRDIR}/Linker.hpp \
    $${HDRDIR}/Loader.hpp \
    $${HDRDIR}/Macro.hpp \
    $${HDRDIR}/Operand.hpp \
    $${HDRDIR}/Optimizer.hpp \
    $${HDRDIR}/Parser.hpp \
    $${HDRDIR}/RegisterAllocator.hpp \
    $${HDRDIR}/Scanner.hpp \
    $${HDRDIR}/SoftWire.hpp \
    $${HDRDIR}/String.hpp \
    $${HDRDIR}/Synthesizer.hpp \
    $${HDRDIR}/Token.hpp \
    $${HDRDIR}/TokenList.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
