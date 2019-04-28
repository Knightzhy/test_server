DisableStackProtector = 0
ReleaseAsDebug = 1
StaticLinkLibraries = 1

CPPFLAGS += -DGTEST_USE_OWN_TR1_TUPLE=0

TARGET = mongocxx
MODULEROOT = ../..
MODULEPATH = sample_mongocxx/

SUBDIRS = .

#  "d" will be appended for each STATICS, ABC will become -lABCd eventually
STATICS =
LIBRARIES = bsoncxx mongocxx
PKGCONFIGS =

BINPATH = thirdparty/staging/bin
SEARCHPATH = thirdparty/staging/include \
		 thirdparty/staging/include/bsoncxx/v_noabi \
		 thirdparty/staging/include/mongocxx/v_noabi
LIBRARYPATH = thirdparty/staging/lib \
		  thirdparty/staging/lib64

# Installing related
DESTDIR =
prefix = /usr
EXTRA_INSTALLS =

### MODIFY ANYTHING BELOW THIS LINE CAREFULLY ###

CROSS_COMPILE=

### DON'T MODIFY ANYTHING BELOW THIS LINE ###

GOAL = $(EXECUTABLE)
GOALd = $(EXECUTABLEd)
#GOAL = $(LIBRARY)
#GOALd = $(LIBRARYd)
#GOAL = $(TEST)
#GOALd = $(TESTd)
include ../../flinter/include.mk

####
#EXECUTABLE := $(TARGET)
#EXECUTABLEd := $(TARGET)_debug
#LIBRARY             := $(patsubst ./%,%,$(dir $(TARGET)))lib$(basename $(notdir $(TARGET))).a
#LIBRARYd            := $(patsubst ./%,%,$(dir $(TARGET)))lib$(basename $(notdir $(TARGET)))_debug.a
#SHLIBRARY           := $(patsubst ./%,%,$(dir $(TARGET)))lib$(basename $(notdir $(TARGET))).so.0
#SHLIBRARYREAL       := $(patsubst ./%,%,$(dir $(TARGET)))lib$(basename $(notdir $(TARGET))).so
#SHLIBRARYd          := $(patsubst ./%,%,$(dir $(TARGET)))lib$(basename $(notdir $(TARGET)))_debug.so
#SHLIBRARYNAME       := lib$(basename $(notdir $(TARGET))).so.0
#SHLIBRARYREALNAME   := lib$(basename $(notdir $(TARGET))).so
#
#CGI_SOURCES := $(patsubst ./%,%,$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*_cgi.cpp)))
#CGI := $(foreach cgi,$(CGI_SOURCES),$(TARGET)/$(patsubst ./%,%,$(dir $(cgi)))$(notdir $(patsubst %_cgi.cpp,%$(CGI_SUFFIX),$(cgi))))
#CGId := $(foreach cgi,$(CGI_SOURCES),$(TARGET)/$(patsubst ./%,%,$(dir $(cgi)))$(notdir $(patsubst %_cgi.cpp,%_debug$(CGI_SUFFIX),$(cgi)))
#
#TEST_SOURCES := $(patsubst ./%,%,$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/test_*.cpp)))
#TEST := $(foreach test,$(TEST_SOURCES),$(TARGET)/$(patsubst ./%,%,$(dir $(test)))$(notdir $(patsubst %.cpp,%,$(test))))
#TESTd := $(foreach test,$(TEST_SOURCES),$(TARGET)/$(patsubst ./%,%,$(dir $(test)))$(notdir $(patsubst %.cpp,%_debug,$(test))))
####

