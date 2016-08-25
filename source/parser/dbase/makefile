COMPILER ?= g++
#COMPILER = the c++ compiler to use.
#  or, on command line, supply the value:
#    make COMPILER=g++
###########################################
yymaincc:
	$(COMPILER) -c \
-Wno-unused-parameter \
-Wno-unused-variable \
-Wno-unused-local-typedefs \
-Wno-unused-but-set-variable \
-Wno-write-strings \
-Wno-extra \
-Wno-reorder \
-Wunused-function \
-Woverloaded-virtual \
-fpermissive \
-ftemplate-depth=200 -frtti -fexceptions -std=c++17 \
-D__BYTE_ORDER=__LITTLE_ENDIAN -D_REENTRANT -fPIC \
-DBUILDTIME=\"14:43:09\" -DBUILDDATE=\"2016-08-22\" \
-I. \
-I/usr/include \
\
-o yymain.o yymain.cc

show:
	@echo "COMPILER=$(COMPILER)"
