
CPPFLAGS += -DGTEST_USE_OWN_TR1_TUPLE=0

TARGET = bin

MODULEROOT = ../..

MODULEPATH = sample_io/

SUBDIRS = .

STATICS =

LIBRARIES = pthread gtest gtest_main

PKGCONFIGS =

BINPATH = thirdparty/staging/bin

SEARCHPATH = thirdparty/staging/include \
	     flinter/output/include 

LIBRARYPATH = thirdparty/staging/lib \
	      flinter/output/lib

DESTDIR =
prefix = /usr
EXTRA_INSTALLS =
CROSS_COMPILE=

GOAL = $(TEST)
GOALd = $(TESTd)

include ../../flinter/include.mk
