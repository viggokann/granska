###############################################
#
#  Makefile -- builds the project
#
#  First you should call
#     make depend
#  to add the dependencies (at the bottom of this file)
#  Then call "make" as usual
#
CPPC = g++ # c++-kompilator
CC   = gcc # c-kompilator
WARN_PERFORMANCE = -Winline -Wpadded -Wdisabled-optimization
WARN_STRICT = -Wchar-subscripts -Wcomment -Wimplicit -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wuninitialized -Wreorder -Wno-system-headers -Wpointer-arith -Wno-cast-qual -Wcast-align -Wwrite-strings -Wno-missing-declarations -Wunreachable-code 
WARN =  -Wall -Wno-missing-braces -Wno-unknown-pragmas -Wno-unused-function -Wno-unused-label #warning do not use the -pedantic flag, since it reports error for standalone semicolons.
DEBUG_FLAGS = -g -DENSURE
RELEASE_FLAGS = -O3 #$(WARN_PERFORMANCE)

#CFLAGS = $(RELEASE_FLAGS) $(WARN)# general settings
CFLAGS = -DDEFAULTCODE=UTF8CODE $(DEBUG_FLAGS) $(WARN)# general settings

#
#  Directories and paths
#
CD = common# common files directory
TD = tagger# tagger files directory
SD = scrut# scrutinizer files directory
PD = prob# probcheck files directory
STD = stava# stava files directory
TOD = token# tokenizer files directory
CGID = cgigranska# cgigranska files directory
#XERCES = /misc/tcs/language/granska/src/xerces-c-src1_6_0 //W
XERCES = /usr/local/include/xercesc
FLAGS = $(CFLAGS) -I$(CD) -I$(TD) -I$(SD) -I$(PD) -I$(STD) \
  -I$(TOD) -I.. $(INCL)
#LIB_PATH_RT = -R$(XERCES)/lib #set runtime-path at compile time

STAVA_OBJ = $(STD)/stavaapi.o $(STD)/suffix.o $(STD)/rattstava.o
STAVA_FLAGS = -DAPI

TOK_OBJ = $(TOD)/tokenizer.o

MAIN_OBJ = $(CD)/main.o
COMMON_OBJ = $(CD)/file.o $(CD)/message.o $(CD)/timer.o
COMMON_FLAGS = 

WWW_OBJ = $(CGID)/wwwmain.o $(CGID)/wwwscrutinizer.o

TAGGER_OBJ = $(TD)/hashscatter.o $(TD)/inflectlexicon.o\
  $(TD)/inflectrule.o $(TD)/letter.o $(TD)/lexicon.o\
  $(TD)/morflexicon.o $(TD)/newwordlexicon.o $(TD)/sentence.o\
  $(TD)/settings.o $(TD)/tag.o $(TD)/tagger.o $(TD)/taglexicon.o\
  $(TD)/text.o $(TD)/word.o $(TD)/wordlexicon.o $(TD)/wordtag.o\
  $(TD)/wordtoken.o
TAGGER_FLAGS = -DPROB_DOUBLE # jonas, I get lots of prob = 0 (smaller than float limit) otherwise

# leave lex/yacc files first because of the files they generate
# and the dependencies that other files have
SCRUT_OBJ = $(SD)/rules.tab.o $(SD)/element.o $(SD)/expr.o\
  $(SD)/gramerror.o $(SD)/identry.o $(SD)/matching.o\
  $(SD)/matchingset.o $(SD)/operation.o $(SD)/rule.o $(SD)/ruleset.o\
  $(SD)/rulesettings.o $(SD)/scrutinizer.o
SCRUT_FLAGS = -DMESSAGES -DTIMER -DDEVELOPER_OUTPUT 

PROBCHECK_OBJ = $(PD)/annot.o $(PD)/output.o\
  $(PD)/probcheck.o $(PD)/trans.o $(PD)/report.o $(PD)/probdevelop.o $(PD)/probinfo.o $(PD)/probmatch.o \
  $(PD)/represent.o $(PD)/probadmin.o \
  ../misc/xmloutput.o
PROBCHECK_FLAGS = -DPROBCHECK -DNO_SENTENCE_DELIMITERS  
  #-DTRANSITIVITY -DNO_XML_SUPPORT -DNO_ANNOT_FROM_FILE 

WEB_OBJ = $(SD)/web.o $(SD)/socket.o $(SD)/server_socket.o
WEB_FLAGS = -DWEB_SCRUTINIZER

API_OBJ = granskaapi.o
ALLWWW_OBJ = $(WWW_OBJ) $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ) $(TAGGER_OBJ) $(PROBCHECK_OBJ)
ALL_OBJ = $(MAIN_OBJ) $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ)\
  $(TAGGER_OBJ) $(PROBCHECK_OBJ) $(WEB_OBJ) 
API = $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ)\
  $(TAGGER_OBJ) $(PROBCHECK_OBJ) $(API_OBJ)
ALL_FLAGS = -fPIC $(FLAGS) $(COMMON_FLAGS) $(STAVA_FLAGS) $(TOK_FLAGS)\
  $(DEVELOPERS_TAGGER_FLAGS) $(TAGGER_FLAGS) $(SCRUT_FLAGS)\
  $(PROBCHECK_FLAGS) $(WEB_FLAGS) # comment this row when compiling wwwscrutinizer

TAGG_OBJ = $(CD)/file.o $(CD)/message.o $(CD)/timer.o $(TD)/main.o\
  $(TOK_OBJ) $(TAGGER_OBJ)

DTD = developers-tagger
DEVELOPERS_TAGGER_OBJ=$(DTD)/main.o $(DTD)/developers-tagger.o $(DTD)/settings_manip.o
# do not use DEVELOPERS_TAGGER_FLAGS unless building developers tagger (defines optConst, CheckHash)
DEVELOPERS_TAGGER_FLAGS=-DDEVELOPERS
DEVELOPERS_HELPER_OBJ=$(DEVELOPERS_TAGGER_OBJ) $(TOK_OBJ) $(TAGGER_OBJ) $(CD)/file.o $(CD)/message.o $(CD)/timer.o
DEVELOPERS_TAGG_OBJ = $(patsubst %.o, %.do, $(DEVELOPERS_HELPER_OBJ))

#platform specific:
UNAME := $(shell uname)

ifeq ($(UNAME), SunOS)
LIBS = -L/usr/local/lib -R/usr/local/lib -lxerces-c-3.1 -lsocket -lnsl
WWWLIBS = -L/usr/local/lib -R/usr/local/lib -lxerces-c-3.1
INSTALL_ARG1 = -f /usr/local/include/ granskaapi.h
INSTALL_ARG2 = -f /usr/local/lib libgranska.so
LDCONF = crle -l /usr/local/lib -u
endif
ifeq ($(UNAME), $(filter $(UNAME), Linux Darwin))
LIBS = -lxerces-c
WWWLIBS = -lxerces-c
INSTALL_ARG1 = granskaapi.h /usr/local/include/
INSTALL_ARG2 = libgranska.so /usr/local/lib 
LDCONF =  ldconfig /usr/local/lib
endif

TARGETS = depend scrut/rules.tab.h scrutinizer wwwscrutinizer library testlibrary

all: $(TARGETS)

developers-tagg: $(DEVELOPERS_TAGG_OBJ)
	$(CPPC) $(FLAGS) -o $@ $^ $(LIBS) \

library: $(API)
	$(CPPC) -shared $(ALL_FLAGS) -o libgranska.so $(API) $(LIBS)

testlibrary: test.o XMLHandler.o library
	g++ -g -c test.cpp XMLHandler.cpp
	g++ test.o XMLHandler.o -L. -lgranska $(LIBS) -o testlibrary

scrutinizer: $(ALL_OBJ)
	$(CPPC) $(ALL_FLAGS) -o scrutinizer $(ALL_OBJ) $(LIBS)

wwwscrutinizer: $(ALLWWW_OBJ)
	$(CPPC) $(ALL_FLAGS) -Bstatic -o wwwscrutinizer $(ALLWWW_OBJ) $(WWWLIBS)
	strip wwwscrutinizer

tagg: $(TAGG_OBJ)
	$(CPPC) $(FLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CPPC) $(ALL_FLAGS) -Idevelopers-tagger -c $*.cpp -o $*.o

%.o: %.c
	$(CC) $(ALL_FLAGS) -c $*.c -o $*.o

%.do: %.cpp
#	$(CPPC) $(ALL_FLAGS) $(DEVELOPERS_TAGGER_FLAGS) -c $< -o $@
	$(CPPC) $(ALL_FLAGS) -c $< -o $@

$(SD)/rules.tab.cpp: $(SD)/rules.y $(SD)/lex.yy.c
	cd $(SD) ; bison -v -d rules.y
	mv $(SD)/rules.tab.c $(SD)/rules.tab.cpp

$(SD)/rules.tab.h: $(SD)/rules.y $(SD)/lex.yy.c
	cd $(SD); bison -v -d rules.y

$(SD)/lex.yy.c: $(SD)/rules.l 
	flex -f -8 $(SD)/rules.l
	mv lex.yy.c $(SD)

install:
	sudo install $(INSTALL_ARG1)
	sudo install $(INSTALL_ARG2)
	sudo $(LDCONF)

test:
	./test.sh
	 
clean: 
	rm -f *.o *~ */*.o */*~
	rm -f *.do */*.do 
	rm -f $(SD)/*.tab.c $(SD)/*.tab.cpp $(SD)/*.tab.h
	rm -f $(SD)/*.yy.c $(SD)/*.output
	rm -f ../misc/xmloutput.o
	rm -f core messages
	rm scrutinizer testlibrary wwwscrutinizer libgranska.so

realclean: clean 	
	rm -f $(TARGETS)
	rm -f tagg

depend:
	makedepend -- $(ALL_FLAGS) -- */*.cpp */*.c
