###############################################
#
#  makefile -- fil för att bygga projektet
#
#  skriv först make depend för att bygga
#    beroenden (tillkommer nederst i makefilen)
#  skriv sedan make som vanligt
#
CPPC = g++ # c++-kompilator
CC   = gcc # c-kompilator
WARN_PERFORMANCE = -Winline -Wpadded -Wdisabled-optimization
WARN_STRICT = -Wchar-subscripts -Wcomment -Wimplicit -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wuninitialized -Wreorder -Wno-system-headers -Wpointer-arith -Wno-cast-qual -Wcast-align -Wwrite-strings -Wno-missing-declarations -Wunreachable-code 
WARN =  -Wall -Wno-missing-braces -Wno-unknown-pragmas -Wno-unused-function -Wno-unused-label #warning do not use the -pedantic flag, since it reports error for standalone semicolons.
DEBUG_FLAGS = -g -DENSURE
RELEASE_FLAGS = -O3 #$(WARN_PERFORMANCE)

#CFLAGS = $(RELEASE_FLAGS) $(WARN)# gemensamma inställningar
CFLAGS = $(DEBUG_FLAGS) $(WARN)# gemensamma inställningar

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
#WEB_LIBS = -lsocket -lnsl //solaris specific?
API_OBJ = granskaapi.o
ALLWWW_OBJ = $(WWW_OBJ) $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ) $(TAGGER_OBJ) $(PROBCHECK_OBJ)
ALL_OBJ = $(MAIN_OBJ) $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ)\
  $(TAGGER_OBJ) $(PROBCHECK_OBJ) $(WEB_OBJ) 
API = $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ)\
  $(TAGGER_OBJ) $(PROBCHECK_OBJ) $(API_OBJ)
API_FLAGS = -fPIC -DLIB_GRANSKA $(FLAGS) $(COMMON_FLAGS) $(STAVA_FLAGS) $(TOK_FLAGS)\
  $(DEVELOPERS_TAGGER_FLAGS) $(TAGGER_FLAGS) $(SCRUT_FLAGS)\
  $(PROBCHECK_FLAGS) $(WEB_FLAGS) # comment this row when compiling wwwscrutinizer
  
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

LIBS = -lxerces-c 
WWWLIBS = -lxerces-c $(WEB_LIBS)

TARGETS = depend scrut/rules.tab.h scrutinizer library testlibrary

all: $(TARGETS)

developers-tagg: $(DEVELOPERS_TAGG_OBJ)
	$(CPPC) $(FLAGS) -o $@ $^ $(LIBS) \

library: $(API)
	$(CPPC) -shared $(API_FLAGS) -o libgranska.so $(API) $(LIBS)
	install granskaapi.h /usr/local/include/
	install libgranska.so /usr/local/lib
	ldconfig /usr/local/lib

testlibrary: test.o
	gcc -g -c test.cpp
	gcc test.o -lgranska -o testlibrary

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

clean: 
	rm -f *.o *~ */*.o */*~
	rm -f *.do */*.do 
	rm -f $(SD)/*.tab.c $(SD)/*.tab.cpp $(SD)/*.tab.h
	rm -f $(SD)/*.yy.c $(SD)/*.output
	rm -f ../misc/xmloutput.o
	rm -f core messages

realclean: clean 	
	rm -f $(TARGETS)
	rm -f tagg

depend:
	makedepend -- $(ALL_FLAGS) -- */*.cpp */*.c

# DO NOT DELETE

cgigranska/wwwmain.o: /usr/include/sys/types.h /usr/include/features.h
cgigranska/wwwmain.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
cgigranska/wwwmain.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
cgigranska/wwwmain.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
cgigranska/wwwmain.o: /usr/include/bits/typesizes.h /usr/include/time.h
cgigranska/wwwmain.o: /usr/include/endian.h /usr/include/bits/endian.h
cgigranska/wwwmain.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
cgigranska/wwwmain.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
cgigranska/wwwmain.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
cgigranska/wwwmain.o: /usr/include/bits/pthreadtypes.h
cgigranska/wwwmain.o: /usr/include/sys/wait.h /usr/include/signal.h
cgigranska/wwwmain.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
cgigranska/wwwmain.o: /usr/include/bits/sigaction.h
cgigranska/wwwmain.o: /usr/include/bits/sigcontext.h
cgigranska/wwwmain.o: /usr/include/bits/sigstack.h
cgigranska/wwwmain.o: /usr/include/sys/ucontext.h
cgigranska/wwwmain.o: /usr/include/bits/sigthread.h
cgigranska/wwwmain.o: /usr/include/sys/resource.h
cgigranska/wwwmain.o: /usr/include/bits/resource.h
cgigranska/wwwmain.o: /usr/include/bits/waitflags.h
cgigranska/wwwmain.o: /usr/include/bits/waitstatus.h
cgigranska/wwwmain.o: cgigranska/wwwscrutinizer.h scrut/scrutinizer.h
cgigranska/wwwmain.o: scrut/matching.h tagger/wordtoken.h tagger/letter.h
cgigranska/wwwmain.o: /usr/include/string.h /usr/include/xlocale.h
cgigranska/wwwmain.o: /usr/include/ctype.h common/basics.h
cgigranska/wwwmain.o: /usr/include/stdio.h /usr/include/libio.h
cgigranska/wwwmain.o: /usr/include/_G_config.h /usr/include/wchar.h
cgigranska/wwwmain.o: /usr/include/bits/stdio_lim.h
cgigranska/wwwmain.o: /usr/include/bits/sys_errlist.h common/ensure.h
cgigranska/wwwmain.o: /usr/include/assert.h tagger/settings.h
cgigranska/wwwmain.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
cgigranska/wwwmain.o: /usr/include/bits/local_lim.h
cgigranska/wwwmain.o: /usr/include/linux/limits.h
cgigranska/wwwmain.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
cgigranska/wwwmain.o: tagger/token.h tagger/word.h tagger/hashscatter.h
cgigranska/wwwmain.o: tagger/inflectrule.h common/message.h tagger/tag.h
cgigranska/wwwmain.o: tagger/bitmap.h common/file.h tagger/feature.h
cgigranska/wwwmain.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
cgigranska/wwwmain.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
cgigranska/wwwmain.o: tagger/sentence.h tagger/taglexicon.h
cgigranska/wwwmain.o: tagger/hasharray.h tagger/tagtrigram.h
cgigranska/wwwmain.o: scrut/matchingset.h scrut/rules.h tagger/text.h
cgigranska/wwwmain.o: tagger/hashtable.h /usr/include/stdlib.h
cgigranska/wwwmain.o: /usr/include/alloca.h scrut/evaluatedvariable.h
cgigranska/wwwmain.o: scrut/semantictype.h scrut/operation.h
cgigranska/wwwmain.o: scrut/rules.tab.h scrut/expr.h scrut/rulesettings.h
cgigranska/wwwmain.o: scrut/gramerror.h tagger/tagger.h tagger/lexicon.h
cgigranska/wwwmain.o: tagger/morflexicon.h tagger/styleword.h
cgigranska/wwwmain.o: tagger/newwordlexicon.h tagger/newword.h
cgigranska/wwwmain.o: tagger/wordlexicon.h tagger/inflectlexicon.h
cgigranska/wwwmain.o: tagger/wordruleterms.h token/tokenizer.h
cgigranska/wwwmain.o: common/FlexLexer.h tagger/trigramgadget.h prob/prob.h
cgigranska/wwwmain.o: scrut/ruleset.h
common/file.o: /usr/include/ctype.h /usr/include/features.h
common/file.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
common/file.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
common/file.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
common/file.o: /usr/include/bits/typesizes.h /usr/include/endian.h
common/file.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
common/file.o: /usr/include/xlocale.h /usr/include/string.h
common/file.o: /usr/include/stdio.h /usr/include/libio.h
common/file.o: /usr/include/_G_config.h /usr/include/wchar.h
common/file.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
common/file.o: common/file.h common/ensure.h /usr/include/assert.h
common/file.o: common/message.h
common/main.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
common/main.o: tagger/letter.h /usr/include/string.h /usr/include/features.h
common/main.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
common/main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
common/main.o: /usr/include/gnu/stubs-64.h /usr/include/xlocale.h
common/main.o: /usr/include/ctype.h /usr/include/bits/types.h
common/main.o: /usr/include/bits/typesizes.h /usr/include/endian.h
common/main.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
common/main.o: common/basics.h /usr/include/stdio.h /usr/include/libio.h
common/main.o: /usr/include/_G_config.h /usr/include/wchar.h
common/main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
common/main.o: common/ensure.h /usr/include/assert.h tagger/settings.h
common/main.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
common/main.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
common/main.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
common/main.o: tagger/token.h tagger/word.h tagger/hashscatter.h
common/main.o: tagger/inflectrule.h common/message.h tagger/tag.h
common/main.o: tagger/bitmap.h common/file.h tagger/feature.h
common/main.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
common/main.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
common/main.o: /usr/include/time.h /usr/include/bits/time.h
common/main.o: /usr/include/sys/select.h /usr/include/bits/select.h
common/main.o: /usr/include/bits/sigset.h tagger/sentence.h
common/main.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
common/main.o: scrut/matchingset.h scrut/rules.h tagger/text.h
common/main.o: tagger/hashtable.h /usr/include/stdlib.h
common/main.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
common/main.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
common/main.o: scrut/evaluatedvariable.h scrut/semantictype.h
common/main.o: scrut/operation.h scrut/rules.tab.h scrut/expr.h
common/main.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
common/main.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
common/main.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
common/main.o: tagger/inflectlexicon.h tagger/wordruleterms.h
common/main.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
common/main.o: prob/prob.h scrut/ruleset.h prob/output.h ../misc/xmloutput.h
common/main.o: prob/config.h prob/probdefines.h
common/message.o: common/message.h tagger/settings.h common/basics.h
common/message.o: /usr/include/stdio.h /usr/include/features.h
common/message.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
common/message.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
common/message.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
common/message.o: /usr/include/bits/typesizes.h /usr/include/libio.h
common/message.o: /usr/include/_G_config.h /usr/include/wchar.h
common/message.o: /usr/include/bits/stdio_lim.h
common/message.o: /usr/include/bits/sys_errlist.h common/ensure.h
common/message.o: /usr/include/assert.h /usr/include/limits.h
common/message.o: /usr/include/bits/posix1_lim.h
common/message.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
common/message.o: /usr/include/bits/posix2_lim.h tagger/sentence.h
common/message.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
common/message.o: /usr/include/xlocale.h /usr/include/ctype.h
common/message.o: /usr/include/endian.h /usr/include/bits/endian.h
common/message.o: /usr/include/bits/byteswap.h common/stringbuf.h
common/message.o: tagger/token.h tagger/word.h tagger/hashscatter.h
common/message.o: tagger/inflectrule.h tagger/tag.h tagger/bitmap.h
common/message.o: common/file.h tagger/feature.h tagger/tagdefines.h
common/message.o: tagger/wordtag.h
developers-tagger/developers-tagger.o: /usr/include/math.h
developers-tagger/developers-tagger.o: /usr/include/features.h
developers-tagger/developers-tagger.o: /usr/include/bits/predefs.h
developers-tagger/developers-tagger.o: /usr/include/sys/cdefs.h
developers-tagger/developers-tagger.o: /usr/include/bits/wordsize.h
developers-tagger/developers-tagger.o: /usr/include/gnu/stubs.h
developers-tagger/developers-tagger.o: /usr/include/gnu/stubs-64.h
developers-tagger/developers-tagger.o: /usr/include/bits/huge_val.h
developers-tagger/developers-tagger.o: /usr/include/bits/huge_valf.h
developers-tagger/developers-tagger.o: /usr/include/bits/huge_vall.h
developers-tagger/developers-tagger.o: /usr/include/bits/inf.h
developers-tagger/developers-tagger.o: /usr/include/bits/nan.h
developers-tagger/developers-tagger.o: /usr/include/bits/mathdef.h
developers-tagger/developers-tagger.o: /usr/include/bits/mathcalls.h
developers-tagger/developers-tagger.o: /usr/include/time.h
developers-tagger/developers-tagger.o: /usr/include/bits/types.h
developers-tagger/developers-tagger.o: /usr/include/bits/typesizes.h
developers-tagger/developers-tagger.o: developers-tagger/developers-tagger.h
developers-tagger/developers-tagger.o: tagger/tagger.h common/basics.h
developers-tagger/developers-tagger.o: /usr/include/stdio.h
developers-tagger/developers-tagger.o: /usr/include/libio.h
developers-tagger/developers-tagger.o: /usr/include/_G_config.h
developers-tagger/developers-tagger.o: /usr/include/wchar.h
developers-tagger/developers-tagger.o: /usr/include/bits/stdio_lim.h
developers-tagger/developers-tagger.o: /usr/include/bits/sys_errlist.h
developers-tagger/developers-tagger.o: common/ensure.h /usr/include/assert.h
developers-tagger/developers-tagger.o: tagger/lexicon.h tagger/hashtable.h
developers-tagger/developers-tagger.o: /usr/include/string.h
developers-tagger/developers-tagger.o: /usr/include/xlocale.h
developers-tagger/developers-tagger.o: /usr/include/stdlib.h
developers-tagger/developers-tagger.o: /usr/include/sys/types.h
developers-tagger/developers-tagger.o: /usr/include/endian.h
developers-tagger/developers-tagger.o: /usr/include/bits/endian.h
developers-tagger/developers-tagger.o: /usr/include/bits/byteswap.h
developers-tagger/developers-tagger.o: /usr/include/sys/select.h
developers-tagger/developers-tagger.o: /usr/include/bits/select.h
developers-tagger/developers-tagger.o: /usr/include/bits/sigset.h
developers-tagger/developers-tagger.o: /usr/include/bits/time.h
developers-tagger/developers-tagger.o: /usr/include/sys/sysmacros.h
developers-tagger/developers-tagger.o: /usr/include/bits/pthreadtypes.h
developers-tagger/developers-tagger.o: /usr/include/alloca.h
developers-tagger/developers-tagger.o: tagger/morflexicon.h tagger/word.h
developers-tagger/developers-tagger.o: tagger/hashscatter.h
developers-tagger/developers-tagger.o: tagger/inflectrule.h
developers-tagger/developers-tagger.o: /usr/include/limits.h
developers-tagger/developers-tagger.o: /usr/include/bits/posix1_lim.h
developers-tagger/developers-tagger.o: /usr/include/bits/local_lim.h
developers-tagger/developers-tagger.o: /usr/include/linux/limits.h
developers-tagger/developers-tagger.o: /usr/include/bits/posix2_lim.h
developers-tagger/developers-tagger.o: common/message.h tagger/tag.h
developers-tagger/developers-tagger.o: tagger/bitmap.h common/file.h
developers-tagger/developers-tagger.o: /usr/include/ctype.h tagger/feature.h
developers-tagger/developers-tagger.o: tagger/settings.h tagger/tagdefines.h
developers-tagger/developers-tagger.o: tagger/wordtag.h common/stringbuf.h
developers-tagger/developers-tagger.o: tagger/hasharray.h tagger/taglexicon.h
developers-tagger/developers-tagger.o: tagger/tagtrigram.h tagger/token.h
developers-tagger/developers-tagger.o: tagger/styleword.h
developers-tagger/developers-tagger.o: tagger/newwordlexicon.h
developers-tagger/developers-tagger.o: tagger/newword.h tagger/wordlexicon.h
developers-tagger/developers-tagger.o: tagger/inflectlexicon.h
developers-tagger/developers-tagger.o: tagger/wordruleterms.h
developers-tagger/developers-tagger.o: tagger/sentence.h tagger/wordtoken.h
developers-tagger/developers-tagger.o: tagger/letter.h common/timer.h
developers-tagger/developers-tagger.o: /usr/include/sys/time.h tagger/text.h
developers-tagger/developers-tagger.o: token/tokenizer.h common/FlexLexer.h
developers-tagger/developers-tagger.o: tagger/trigramgadget.h tagger/morf.h
developers-tagger/developers-tagger.o: developers-tagger/settings_manip.h
developers-tagger/developers-tagger.o: tagger/heap.h
developers-tagger/main.o: tagger/settings.h common/basics.h
developers-tagger/main.o: /usr/include/stdio.h /usr/include/features.h
developers-tagger/main.o: /usr/include/bits/predefs.h
developers-tagger/main.o: /usr/include/sys/cdefs.h
developers-tagger/main.o: /usr/include/bits/wordsize.h
developers-tagger/main.o: /usr/include/gnu/stubs.h
developers-tagger/main.o: /usr/include/gnu/stubs-64.h
developers-tagger/main.o: /usr/include/bits/types.h
developers-tagger/main.o: /usr/include/bits/typesizes.h /usr/include/libio.h
developers-tagger/main.o: /usr/include/_G_config.h /usr/include/wchar.h
developers-tagger/main.o: /usr/include/bits/stdio_lim.h
developers-tagger/main.o: /usr/include/bits/sys_errlist.h common/ensure.h
developers-tagger/main.o: /usr/include/assert.h /usr/include/limits.h
developers-tagger/main.o: /usr/include/bits/posix1_lim.h
developers-tagger/main.o: /usr/include/bits/local_lim.h
developers-tagger/main.o: /usr/include/linux/limits.h
developers-tagger/main.o: /usr/include/bits/posix2_lim.h common/file.h
developers-tagger/main.o: /usr/include/ctype.h /usr/include/endian.h
developers-tagger/main.o: /usr/include/bits/endian.h
developers-tagger/main.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
developers-tagger/main.o: common/message.h tagger/letter.h
developers-tagger/main.o: /usr/include/string.h
developers-tagger/main.o: developers-tagger/developers-tagger.h
developers-tagger/main.o: tagger/tagger.h tagger/lexicon.h tagger/hashtable.h
developers-tagger/main.o: /usr/include/stdlib.h /usr/include/sys/types.h
developers-tagger/main.o: /usr/include/time.h /usr/include/sys/select.h
developers-tagger/main.o: /usr/include/bits/select.h
developers-tagger/main.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
developers-tagger/main.o: /usr/include/sys/sysmacros.h
developers-tagger/main.o: /usr/include/bits/pthreadtypes.h
developers-tagger/main.o: /usr/include/alloca.h tagger/morflexicon.h
developers-tagger/main.o: tagger/word.h tagger/hashscatter.h
developers-tagger/main.o: tagger/inflectrule.h tagger/tag.h tagger/bitmap.h
developers-tagger/main.o: tagger/feature.h tagger/tagdefines.h
developers-tagger/main.o: tagger/wordtag.h common/stringbuf.h
developers-tagger/main.o: tagger/hasharray.h tagger/taglexicon.h
developers-tagger/main.o: tagger/tagtrigram.h tagger/token.h
developers-tagger/main.o: tagger/styleword.h tagger/newwordlexicon.h
developers-tagger/main.o: tagger/newword.h tagger/wordlexicon.h
developers-tagger/main.o: tagger/inflectlexicon.h tagger/wordruleterms.h
developers-tagger/main.o: tagger/sentence.h tagger/wordtoken.h common/timer.h
developers-tagger/main.o: /usr/include/sys/time.h tagger/text.h
developers-tagger/main.o: token/tokenizer.h common/FlexLexer.h
developers-tagger/main.o: tagger/trigramgadget.h
developers-tagger/main.o: developers-tagger/settings_manip.h
developers-tagger/settings_manip.o: developers-tagger/settings_manip.h
developers-tagger/settings_manip.o: common/message.h common/file.h
developers-tagger/settings_manip.o: /usr/include/ctype.h
developers-tagger/settings_manip.o: /usr/include/features.h
developers-tagger/settings_manip.o: /usr/include/bits/predefs.h
developers-tagger/settings_manip.o: /usr/include/sys/cdefs.h
developers-tagger/settings_manip.o: /usr/include/bits/wordsize.h
developers-tagger/settings_manip.o: /usr/include/gnu/stubs.h
developers-tagger/settings_manip.o: /usr/include/gnu/stubs-64.h
developers-tagger/settings_manip.o: /usr/include/bits/types.h
developers-tagger/settings_manip.o: /usr/include/bits/typesizes.h
developers-tagger/settings_manip.o: /usr/include/endian.h
developers-tagger/settings_manip.o: /usr/include/bits/endian.h
developers-tagger/settings_manip.o: /usr/include/bits/byteswap.h
developers-tagger/settings_manip.o: /usr/include/xlocale.h common/ensure.h
developers-tagger/settings_manip.o: /usr/include/assert.h tagger/settings.h
developers-tagger/settings_manip.o: common/basics.h /usr/include/stdio.h
developers-tagger/settings_manip.o: /usr/include/libio.h
developers-tagger/settings_manip.o: /usr/include/_G_config.h
developers-tagger/settings_manip.o: /usr/include/wchar.h
developers-tagger/settings_manip.o: /usr/include/bits/stdio_lim.h
developers-tagger/settings_manip.o: /usr/include/bits/sys_errlist.h
developers-tagger/settings_manip.o: /usr/include/limits.h
developers-tagger/settings_manip.o: /usr/include/bits/posix1_lim.h
developers-tagger/settings_manip.o: /usr/include/bits/local_lim.h
developers-tagger/settings_manip.o: /usr/include/linux/limits.h
developers-tagger/settings_manip.o: /usr/include/bits/posix2_lim.h
prob/annot.o: prob/annot.h tagger/sentence.h tagger/wordtoken.h
prob/annot.o: tagger/letter.h /usr/include/string.h /usr/include/features.h
prob/annot.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
prob/annot.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
prob/annot.o: /usr/include/gnu/stubs-64.h /usr/include/xlocale.h
prob/annot.o: /usr/include/ctype.h /usr/include/bits/types.h
prob/annot.o: /usr/include/bits/typesizes.h /usr/include/endian.h
prob/annot.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
prob/annot.o: common/basics.h /usr/include/stdio.h /usr/include/libio.h
prob/annot.o: /usr/include/_G_config.h /usr/include/wchar.h
prob/annot.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
prob/annot.o: common/ensure.h /usr/include/assert.h tagger/settings.h
prob/annot.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
prob/annot.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
prob/annot.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
prob/annot.o: tagger/token.h tagger/word.h tagger/hashscatter.h
prob/annot.o: tagger/inflectrule.h common/message.h tagger/tag.h
prob/annot.o: tagger/bitmap.h common/file.h tagger/feature.h
prob/annot.o: tagger/tagdefines.h tagger/wordtag.h
prob/annot.o: /usr/include/xercesc/util/PlatformUtils.hpp
prob/annot.o: /usr/include/xercesc/util/XercesDefs.hpp
prob/annot.o: /usr/include/xercesc/util/Xerces_autoconf_config.hpp
prob/annot.o: /usr/include/inttypes.h /usr/include/stdint.h
prob/annot.o: /usr/include/bits/wchar.h
prob/annot.o: /usr/include/xercesc/util/XercesVersion.hpp
prob/annot.o: /usr/include/xercesc/util/XMLException.hpp
prob/annot.o: /usr/include/xercesc/util/XMemory.hpp /usr/include/stdlib.h
prob/annot.o: /usr/include/sys/types.h /usr/include/time.h
prob/annot.o: /usr/include/sys/select.h /usr/include/bits/select.h
prob/annot.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
prob/annot.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
prob/annot.o: /usr/include/alloca.h
prob/annot.o: /usr/include/xercesc/util/XMLExceptMsgs.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMError.hpp
prob/annot.o: /usr/include/xercesc/util/XMLUni.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLErrorReporter.hpp
prob/annot.o: /usr/include/xercesc/util/PanicHandler.hpp
prob/annot.o: /usr/include/xercesc/util/XMLFileMgr.hpp
prob/annot.o: /usr/include/xercesc/util/XMLMutexMgr.hpp
prob/annot.o: /usr/include/xercesc/parsers/XercesDOMParser.hpp
prob/annot.o: /usr/include/xercesc/parsers/AbstractDOMParser.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMDocument.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMNode.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMDocumentRange.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMDocumentTraversal.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMNodeFilter.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMXPathEvaluator.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMXPathResult.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLDocumentHandler.hpp
prob/annot.o: /usr/include/xercesc/util/RefVectorOf.hpp
prob/annot.o: /usr/include/xercesc/util/BaseRefVectorOf.hpp
prob/annot.o: /usr/include/xercesc/util/ArrayIndexOutOfBoundsException.hpp
prob/annot.o: /usr/include/xercesc/util/XMLEnumerator.hpp
prob/annot.o: /usr/include/xercesc/framework/MemoryManager.hpp
prob/annot.o: /usr/include/xercesc/util/BaseRefVectorOf.c
prob/annot.o: /usr/include/xercesc/util/RefVectorOf.c
prob/annot.o: /usr/include/xercesc/framework/XMLAttr.hpp
prob/annot.o: /usr/include/xercesc/util/QName.hpp
prob/annot.o: /usr/include/xercesc/util/XMLString.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLBuffer.hpp
prob/annot.o: /usr/include/xercesc/util/XMLUniDefs.hpp
prob/annot.o: /usr/include/xercesc/internal/XSerializable.hpp
prob/annot.o: /usr/include/xercesc/internal/XSerializeEngine.hpp
prob/annot.o: /usr/include/xercesc/util/RefHashTableOf.hpp
prob/annot.o: /usr/include/xercesc/util/Hashers.hpp
prob/annot.o: /usr/include/xercesc/util/IllegalArgumentException.hpp
prob/annot.o: /usr/include/xercesc/util/NoSuchElementException.hpp
prob/annot.o: /usr/include/xercesc/util/RuntimeException.hpp
prob/annot.o: /usr/include/xercesc/util/RefHashTableOf.c
prob/annot.o: /usr/include/xercesc/util/Janitor.hpp
prob/annot.o: /usr/include/xercesc/util/Janitor.c
prob/annot.o: /usr/include/xercesc/util/NullPointerException.hpp
prob/annot.o: /usr/include/xercesc/util/ValueVectorOf.hpp
prob/annot.o: /usr/include/xercesc/util/ValueVectorOf.c
prob/annot.o: /usr/include/xercesc/internal/XSerializationException.hpp
prob/annot.o: /usr/include/xercesc/internal/XProtoType.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLAttDef.hpp
prob/annot.o: /usr/include/xercesc/validators/datatype/DatatypeValidator.hpp
prob/annot.o: /usr/include/xercesc/util/KVStringPair.hpp
prob/annot.o: /usr/include/xercesc/util/regx/RegularExpression.hpp
prob/annot.o: /usr/include/xercesc/util/RefArrayVectorOf.hpp
prob/annot.o: /usr/include/xercesc/util/RefArrayVectorOf.c
prob/annot.o: /usr/include/xercesc/util/regx/Op.hpp
prob/annot.o: /usr/include/xercesc/util/regx/TokenFactory.hpp
prob/annot.o: /usr/include/xercesc/util/regx/Token.hpp
prob/annot.o: /usr/include/xercesc/util/Mutexes.hpp
prob/annot.o: /usr/include/xercesc/util/regx/BMPattern.hpp
prob/annot.o: /usr/include/xercesc/util/regx/OpFactory.hpp
prob/annot.o: /usr/include/xercesc/util/regx/RegxUtil.hpp
prob/annot.o: /usr/include/xercesc/validators/schema/SchemaSymbols.hpp
prob/annot.o: /usr/include/xercesc/framework/psvi/XSSimpleTypeDefinition.hpp
prob/annot.o: /usr/include/xercesc/framework/psvi/XSTypeDefinition.hpp
prob/annot.o: /usr/include/xercesc/framework/psvi/XSObject.hpp
prob/annot.o: /usr/include/xercesc/framework/psvi/XSConstants.hpp
prob/annot.o: /usr/include/xercesc/framework/ValidationContext.hpp
prob/annot.o: /usr/include/xercesc/util/NameIdPool.hpp
prob/annot.o: /usr/include/xercesc/util/NameIdPool.c
prob/annot.o: /usr/include/xercesc/framework/XMLEntityHandler.hpp
prob/annot.o: /usr/include/xercesc/util/SecurityManager.hpp
prob/annot.o: /usr/include/xercesc/util/ValueStackOf.hpp
prob/annot.o: /usr/include/xercesc/util/EmptyStackException.hpp
prob/annot.o: /usr/include/xercesc/util/ValueStackOf.c
prob/annot.o: /usr/include/xercesc/validators/DTD/DocTypeHandler.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLNotationDecl.hpp
prob/annot.o: /usr/include/xercesc/validators/DTD/DTDAttDef.hpp
prob/annot.o: /usr/include/xercesc/validators/DTD/DTDElementDecl.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLElementDecl.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLAttDefList.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLContentModel.hpp
prob/annot.o: /usr/include/xercesc/validators/DTD/DTDEntityDecl.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLEntityDecl.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMDocumentType.hpp
prob/annot.o: /usr/include/xercesc/framework/XMLBufferMgr.hpp
prob/annot.o: /usr/include/xercesc/framework/psvi/PSVIHandler.hpp
prob/annot.o: /usr/include/xercesc/validators/common/Grammar.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMNamedNodeMap.hpp
prob/annot.o: /usr/include/xercesc/dom/DOMNodeList.hpp prob/domerror.h
prob/annot.o: /usr/include/xercesc/sax/HandlerBase.hpp
prob/annot.o: /usr/include/xercesc/sax/DocumentHandler.hpp
prob/annot.o: /usr/include/xercesc/sax/DTDHandler.hpp
prob/annot.o: /usr/include/xercesc/sax/EntityResolver.hpp
prob/annot.o: /usr/include/xercesc/sax/ErrorHandler.hpp
prob/annot.o: /usr/include/xercesc/sax/SAXParseException.hpp
prob/annot.o: /usr/include/xercesc/sax/SAXException.hpp
prob/client.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/client.o: tagger/tagdefines.h prob/probdefines.h
prob/output.o: ../misc/xmloutput.h prob/output.h prob/defines.h
prob/output.o: /usr/include/xercesc/util/PlatformUtils.hpp
prob/output.o: /usr/include/xercesc/util/XercesDefs.hpp
prob/output.o: /usr/include/xercesc/util/Xerces_autoconf_config.hpp
prob/output.o: /usr/include/inttypes.h /usr/include/features.h
prob/output.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
prob/output.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
prob/output.o: /usr/include/gnu/stubs-64.h /usr/include/stdint.h
prob/output.o: /usr/include/bits/wchar.h
prob/output.o: /usr/include/xercesc/util/XercesVersion.hpp
prob/output.o: /usr/include/xercesc/util/XMLException.hpp
prob/output.o: /usr/include/xercesc/util/XMemory.hpp /usr/include/stdlib.h
prob/output.o: /usr/include/sys/types.h /usr/include/bits/types.h
prob/output.o: /usr/include/bits/typesizes.h /usr/include/time.h
prob/output.o: /usr/include/endian.h /usr/include/bits/endian.h
prob/output.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
prob/output.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
prob/output.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
prob/output.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
prob/output.o: /usr/include/xercesc/util/XMLExceptMsgs.hpp
prob/output.o: /usr/include/xercesc/dom/DOMError.hpp
prob/output.o: /usr/include/xercesc/util/XMLUni.hpp
prob/output.o: /usr/include/xercesc/framework/XMLErrorReporter.hpp
prob/output.o: /usr/include/xercesc/util/PanicHandler.hpp
prob/output.o: /usr/include/xercesc/util/XMLFileMgr.hpp
prob/output.o: /usr/include/xercesc/util/XMLMutexMgr.hpp
prob/pipe_client.o: /usr/include/stdio.h /usr/include/features.h
prob/pipe_client.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
prob/pipe_client.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
prob/pipe_client.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
prob/pipe_client.o: /usr/include/bits/typesizes.h /usr/include/libio.h
prob/pipe_client.o: /usr/include/_G_config.h /usr/include/wchar.h
prob/pipe_client.o: /usr/include/bits/stdio_lim.h
prob/pipe_client.o: /usr/include/bits/sys_errlist.h prob/pipe_client.h
prob/probadmin.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/probadmin.o: tagger/tagdefines.h prob/probdefines.h tagger/taglexicon.h
prob/probadmin.o: tagger/bitmap.h common/basics.h /usr/include/stdio.h
prob/probadmin.o: /usr/include/features.h /usr/include/bits/predefs.h
prob/probadmin.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
prob/probadmin.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
prob/probadmin.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
prob/probadmin.o: /usr/include/libio.h /usr/include/_G_config.h
prob/probadmin.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
prob/probadmin.o: /usr/include/bits/sys_errlist.h common/ensure.h
prob/probadmin.o: /usr/include/assert.h common/file.h /usr/include/ctype.h
prob/probadmin.o: /usr/include/endian.h /usr/include/bits/endian.h
prob/probadmin.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
prob/probadmin.o: common/message.h tagger/feature.h tagger/hasharray.h
prob/probadmin.o: /usr/include/string.h tagger/tag.h tagger/hashscatter.h
prob/probadmin.o: tagger/settings.h /usr/include/limits.h
prob/probadmin.o: /usr/include/bits/posix1_lim.h
prob/probadmin.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
prob/probadmin.o: /usr/include/bits/posix2_lim.h tagger/tagtrigram.h
prob/probadmin.o: tagger/token.h tagger/word.h tagger/inflectrule.h
prob/probadmin.o: tagger/wordtag.h common/stringbuf.h prob/annot.h
prob/probadmin.o: prob/config.h prob/probdevelop.h prob/output.h
prob/probadmin.o: ../misc/xmloutput.h prob/report.h prob/probinfo.h
prob/probadmin.o: prob/probmatch.h scrut/matching.h tagger/wordtoken.h
prob/probadmin.o: tagger/letter.h scrut/element.h scrut/rule.h common/timer.h
prob/probadmin.o: /usr/include/sys/time.h /usr/include/time.h
prob/probadmin.o: /usr/include/bits/time.h /usr/include/sys/select.h
prob/probadmin.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
prob/probadmin.o: tagger/sentence.h /usr/include/stdlib.h
prob/probadmin.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
prob/probadmin.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
prob/probcheck.o: prob/probcheck.h scrut/scrutinizer.h scrut/matching.h
prob/probcheck.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
prob/probcheck.o: /usr/include/features.h /usr/include/bits/predefs.h
prob/probcheck.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
prob/probcheck.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
prob/probcheck.o: /usr/include/xlocale.h /usr/include/ctype.h
prob/probcheck.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
prob/probcheck.o: /usr/include/endian.h /usr/include/bits/endian.h
prob/probcheck.o: /usr/include/bits/byteswap.h common/basics.h
prob/probcheck.o: /usr/include/stdio.h /usr/include/libio.h
prob/probcheck.o: /usr/include/_G_config.h /usr/include/wchar.h
prob/probcheck.o: /usr/include/bits/stdio_lim.h
prob/probcheck.o: /usr/include/bits/sys_errlist.h common/ensure.h
prob/probcheck.o: /usr/include/assert.h tagger/settings.h
prob/probcheck.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
prob/probcheck.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
prob/probcheck.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
prob/probcheck.o: tagger/token.h tagger/word.h tagger/hashscatter.h
prob/probcheck.o: tagger/inflectrule.h common/message.h tagger/tag.h
prob/probcheck.o: tagger/bitmap.h common/file.h tagger/feature.h
prob/probcheck.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
prob/probcheck.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
prob/probcheck.o: /usr/include/time.h /usr/include/bits/time.h
prob/probcheck.o: /usr/include/sys/select.h /usr/include/bits/select.h
prob/probcheck.o: /usr/include/bits/sigset.h tagger/sentence.h
prob/probcheck.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
prob/probcheck.o: scrut/matchingset.h scrut/rules.h tagger/text.h
prob/probcheck.o: tagger/hashtable.h /usr/include/stdlib.h
prob/probcheck.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
prob/probcheck.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
prob/probcheck.o: scrut/evaluatedvariable.h scrut/semantictype.h
prob/probcheck.o: scrut/operation.h scrut/rules.tab.h scrut/expr.h
prob/probcheck.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
prob/probcheck.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
prob/probcheck.o: tagger/newwordlexicon.h tagger/newword.h
prob/probcheck.o: tagger/wordlexicon.h tagger/inflectlexicon.h
prob/probcheck.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
prob/probcheck.o: tagger/trigramgadget.h prob/prob.h prob/defines.h
prob/probcheck.o: prob/config.h prob/probdefines.h prob/annot.h prob/report.h
prob/probcheck.o: prob/probinfo.h prob/probmatch.h prob/probadmin.h
prob/probcheck.o: prob/probdevelop.h prob/output.h ../misc/xmloutput.h
prob/probdevelop.o: prob/probdevelop.h prob/output.h ../misc/xmloutput.h
prob/probdevelop.o: prob/probdefines.h
prob/probinfo.o: prob/probinfo.h scrut/scrutinizer.h scrut/matching.h
prob/probinfo.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
prob/probinfo.o: /usr/include/features.h /usr/include/bits/predefs.h
prob/probinfo.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
prob/probinfo.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
prob/probinfo.o: /usr/include/xlocale.h /usr/include/ctype.h
prob/probinfo.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
prob/probinfo.o: /usr/include/endian.h /usr/include/bits/endian.h
prob/probinfo.o: /usr/include/bits/byteswap.h common/basics.h
prob/probinfo.o: /usr/include/stdio.h /usr/include/libio.h
prob/probinfo.o: /usr/include/_G_config.h /usr/include/wchar.h
prob/probinfo.o: /usr/include/bits/stdio_lim.h
prob/probinfo.o: /usr/include/bits/sys_errlist.h common/ensure.h
prob/probinfo.o: /usr/include/assert.h tagger/settings.h
prob/probinfo.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
prob/probinfo.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
prob/probinfo.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
prob/probinfo.o: tagger/token.h tagger/word.h tagger/hashscatter.h
prob/probinfo.o: tagger/inflectrule.h common/message.h tagger/tag.h
prob/probinfo.o: tagger/bitmap.h common/file.h tagger/feature.h
prob/probinfo.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
prob/probinfo.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
prob/probinfo.o: /usr/include/time.h /usr/include/bits/time.h
prob/probinfo.o: /usr/include/sys/select.h /usr/include/bits/select.h
prob/probinfo.o: /usr/include/bits/sigset.h tagger/sentence.h
prob/probinfo.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
prob/probinfo.o: scrut/matchingset.h scrut/rules.h tagger/text.h
prob/probinfo.o: tagger/hashtable.h /usr/include/stdlib.h
prob/probinfo.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
prob/probinfo.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
prob/probinfo.o: scrut/evaluatedvariable.h scrut/semantictype.h
prob/probinfo.o: scrut/operation.h scrut/rules.tab.h scrut/expr.h
prob/probinfo.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
prob/probinfo.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
prob/probinfo.o: tagger/newwordlexicon.h tagger/newword.h
prob/probinfo.o: tagger/wordlexicon.h tagger/inflectlexicon.h
prob/probinfo.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
prob/probinfo.o: tagger/trigramgadget.h prob/prob.h
prob/probmatch.o: prob/probmatch.h prob/probdefines.h prob/probdevelop.h
prob/probmatch.o: prob/output.h ../misc/xmloutput.h tagger/tag.h
prob/probmatch.o: /usr/include/string.h /usr/include/features.h
prob/probmatch.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
prob/probmatch.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
prob/probmatch.o: /usr/include/gnu/stubs-64.h /usr/include/xlocale.h
prob/probmatch.o: common/basics.h /usr/include/stdio.h
prob/probmatch.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
prob/probmatch.o: /usr/include/libio.h /usr/include/_G_config.h
prob/probmatch.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
prob/probmatch.o: /usr/include/bits/sys_errlist.h common/ensure.h
prob/probmatch.o: /usr/include/assert.h tagger/bitmap.h common/file.h
prob/probmatch.o: /usr/include/ctype.h /usr/include/endian.h
prob/probmatch.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
prob/probmatch.o: common/message.h tagger/feature.h tagger/hashscatter.h
prob/probmatch.o: tagger/settings.h /usr/include/limits.h
prob/probmatch.o: /usr/include/bits/posix1_lim.h
prob/probmatch.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
prob/probmatch.o: /usr/include/bits/posix2_lim.h tagger/tagdefines.h
prob/report.o: prob/report.h prob/probinfo.h prob/probmatch.h
prob/report.o: prob/probdefines.h scrut/rule.h common/timer.h
prob/report.o: /usr/include/sys/time.h /usr/include/features.h
prob/report.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
prob/report.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
prob/report.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
prob/report.o: /usr/include/bits/typesizes.h /usr/include/time.h
prob/report.o: /usr/include/bits/time.h /usr/include/sys/select.h
prob/report.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
prob/report.o: tagger/sentence.h tagger/wordtoken.h tagger/letter.h
prob/report.o: /usr/include/string.h /usr/include/xlocale.h
prob/report.o: /usr/include/ctype.h /usr/include/endian.h
prob/report.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
prob/report.o: common/basics.h /usr/include/stdio.h /usr/include/libio.h
prob/report.o: /usr/include/_G_config.h /usr/include/wchar.h
prob/report.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
prob/report.o: common/ensure.h /usr/include/assert.h tagger/settings.h
prob/report.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
prob/report.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
prob/report.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
prob/report.o: tagger/token.h tagger/word.h tagger/hashscatter.h
prob/report.o: tagger/inflectrule.h common/message.h tagger/tag.h
prob/report.o: tagger/bitmap.h common/file.h tagger/feature.h
prob/report.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
prob/report.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
prob/report.o: prob/output.h ../misc/xmloutput.h scrut/scrutinizer.h
prob/report.o: scrut/matching.h scrut/matchingset.h scrut/rules.h
prob/report.o: tagger/text.h tagger/hashtable.h /usr/include/stdlib.h
prob/report.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
prob/report.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
prob/report.o: scrut/evaluatedvariable.h scrut/semantictype.h
prob/report.o: scrut/operation.h scrut/rules.tab.h scrut/expr.h
prob/report.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
prob/report.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
prob/report.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
prob/report.o: tagger/inflectlexicon.h tagger/wordruleterms.h
prob/report.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
prob/report.o: prob/prob.h scrut/ruleset.h prob/annot.h prob/probdevelop.h
prob/report.o: prob/probadmin.h prob/defines.h prob/config.h
prob/represent.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/represent.o: tagger/tagdefines.h prob/probdefines.h tagger/taglexicon.h
prob/represent.o: tagger/bitmap.h common/basics.h /usr/include/stdio.h
prob/represent.o: /usr/include/features.h /usr/include/bits/predefs.h
prob/represent.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
prob/represent.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
prob/represent.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
prob/represent.o: /usr/include/libio.h /usr/include/_G_config.h
prob/represent.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
prob/represent.o: /usr/include/bits/sys_errlist.h common/ensure.h
prob/represent.o: /usr/include/assert.h common/file.h /usr/include/ctype.h
prob/represent.o: /usr/include/endian.h /usr/include/bits/endian.h
prob/represent.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
prob/represent.o: common/message.h tagger/feature.h tagger/hasharray.h
prob/represent.o: /usr/include/string.h tagger/tag.h tagger/hashscatter.h
prob/represent.o: tagger/settings.h /usr/include/limits.h
prob/represent.o: /usr/include/bits/posix1_lim.h
prob/represent.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
prob/represent.o: /usr/include/bits/posix2_lim.h tagger/tagtrigram.h
prob/represent.o: tagger/token.h tagger/word.h tagger/inflectrule.h
prob/represent.o: tagger/wordtag.h common/stringbuf.h prob/annot.h
prob/represent.o: prob/config.h
prob/trigrams.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/trigrams.o: tagger/tagdefines.h prob/probdefines.h
scrut/element.o: scrut/rules.h /usr/include/stdio.h /usr/include/features.h
scrut/element.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/element.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/element.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
scrut/element.o: /usr/include/bits/typesizes.h /usr/include/libio.h
scrut/element.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/element.o: /usr/include/bits/stdio_lim.h
scrut/element.o: /usr/include/bits/sys_errlist.h common/stringbuf.h
scrut/element.o: /usr/include/string.h /usr/include/xlocale.h common/basics.h
scrut/element.o: common/ensure.h /usr/include/assert.h tagger/text.h
scrut/element.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/element.o: /usr/include/sys/types.h /usr/include/time.h
scrut/element.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/element.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
scrut/element.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
scrut/element.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
scrut/element.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/element.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/element.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/element.o: tagger/settings.h /usr/include/limits.h
scrut/element.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
scrut/element.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
scrut/element.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/element.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/element.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/element.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/element.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/element.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/element.o: tagger/tagtrigram.h scrut/semantictype.h scrut/operation.h
scrut/element.o: scrut/rules.tab.h scrut/expr.h scrut/rulesettings.h
scrut/element.o: scrut/ruleset.h
scrut/expr.o: scrut/expr.h scrut/evaluatedvariable.h scrut/matching.h
scrut/expr.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/expr.o: /usr/include/features.h /usr/include/bits/predefs.h
scrut/expr.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
scrut/expr.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
scrut/expr.o: /usr/include/xlocale.h /usr/include/ctype.h
scrut/expr.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
scrut/expr.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/expr.o: /usr/include/bits/byteswap.h common/basics.h
scrut/expr.o: /usr/include/stdio.h /usr/include/libio.h
scrut/expr.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/expr.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
scrut/expr.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/expr.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
scrut/expr.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
scrut/expr.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
scrut/expr.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/expr.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/expr.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/expr.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/expr.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/expr.o: /usr/include/time.h /usr/include/bits/time.h
scrut/expr.o: /usr/include/sys/select.h /usr/include/bits/select.h
scrut/expr.o: /usr/include/bits/sigset.h tagger/sentence.h
scrut/expr.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/expr.o: scrut/operation.h scrut/rules.tab.h scrut/rulesettings.h
scrut/expr.o: scrut/semantictype.h scrut/rules.h tagger/text.h
scrut/expr.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/expr.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
scrut/expr.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/expr.o: scrut/scrutinizer.h scrut/matchingset.h scrut/gramerror.h
scrut/expr.o: tagger/tagger.h tagger/lexicon.h tagger/morflexicon.h
scrut/expr.o: tagger/styleword.h tagger/newwordlexicon.h tagger/newword.h
scrut/expr.o: tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/expr.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
scrut/expr.o: tagger/trigramgadget.h prob/prob.h prob/memhandler.h
scrut/gramerror.o: scrut/expr.h scrut/evaluatedvariable.h scrut/matching.h
scrut/gramerror.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/gramerror.o: /usr/include/features.h /usr/include/bits/predefs.h
scrut/gramerror.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
scrut/gramerror.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
scrut/gramerror.o: /usr/include/xlocale.h /usr/include/ctype.h
scrut/gramerror.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
scrut/gramerror.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/gramerror.o: /usr/include/bits/byteswap.h common/basics.h
scrut/gramerror.o: /usr/include/stdio.h /usr/include/libio.h
scrut/gramerror.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/gramerror.o: /usr/include/bits/stdio_lim.h
scrut/gramerror.o: /usr/include/bits/sys_errlist.h common/ensure.h
scrut/gramerror.o: /usr/include/assert.h tagger/settings.h
scrut/gramerror.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
scrut/gramerror.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
scrut/gramerror.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
scrut/gramerror.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/gramerror.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/gramerror.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/gramerror.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/gramerror.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/gramerror.o: /usr/include/time.h /usr/include/bits/time.h
scrut/gramerror.o: /usr/include/sys/select.h /usr/include/bits/select.h
scrut/gramerror.o: /usr/include/bits/sigset.h tagger/sentence.h
scrut/gramerror.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/gramerror.o: scrut/operation.h scrut/rules.tab.h scrut/rulesettings.h
scrut/gramerror.o: scrut/semantictype.h scrut/gramerror.h scrut/scrutinizer.h
scrut/gramerror.o: scrut/matchingset.h scrut/rules.h tagger/text.h
scrut/gramerror.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/gramerror.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
scrut/gramerror.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/gramerror.o: tagger/tagger.h tagger/lexicon.h tagger/morflexicon.h
scrut/gramerror.o: tagger/styleword.h tagger/newwordlexicon.h
scrut/gramerror.o: tagger/newword.h tagger/wordlexicon.h
scrut/gramerror.o: tagger/inflectlexicon.h tagger/wordruleterms.h
scrut/gramerror.o: token/tokenizer.h common/FlexLexer.h
scrut/gramerror.o: tagger/trigramgadget.h prob/prob.h prob/output.h
scrut/gramerror.o: ../misc/xmloutput.h
scrut/identry.o: scrut/rules.h /usr/include/stdio.h /usr/include/features.h
scrut/identry.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/identry.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/identry.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
scrut/identry.o: /usr/include/bits/typesizes.h /usr/include/libio.h
scrut/identry.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/identry.o: /usr/include/bits/stdio_lim.h
scrut/identry.o: /usr/include/bits/sys_errlist.h common/stringbuf.h
scrut/identry.o: /usr/include/string.h /usr/include/xlocale.h common/basics.h
scrut/identry.o: common/ensure.h /usr/include/assert.h tagger/text.h
scrut/identry.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/identry.o: /usr/include/sys/types.h /usr/include/time.h
scrut/identry.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/identry.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
scrut/identry.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
scrut/identry.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
scrut/identry.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/identry.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/identry.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/identry.o: tagger/settings.h /usr/include/limits.h
scrut/identry.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
scrut/identry.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
scrut/identry.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/identry.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/identry.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/identry.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/identry.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/identry.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/identry.o: tagger/tagtrigram.h scrut/semantictype.h scrut/operation.h
scrut/identry.o: scrut/rules.tab.h scrut/expr.h scrut/rulesettings.h
scrut/identry.o: prob/memhandler.h
scrut/matching.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/matching.o: /usr/include/features.h /usr/include/bits/predefs.h
scrut/matching.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
scrut/matching.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
scrut/matching.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
scrut/matching.o: /usr/include/time.h /usr/include/bits/time.h
scrut/matching.o: /usr/include/sys/select.h /usr/include/bits/select.h
scrut/matching.o: /usr/include/bits/sigset.h tagger/sentence.h
scrut/matching.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/matching.o: /usr/include/xlocale.h /usr/include/ctype.h
scrut/matching.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/matching.o: /usr/include/bits/byteswap.h common/basics.h
scrut/matching.o: /usr/include/stdio.h /usr/include/libio.h
scrut/matching.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/matching.o: /usr/include/bits/stdio_lim.h
scrut/matching.o: /usr/include/bits/sys_errlist.h common/ensure.h
scrut/matching.o: /usr/include/assert.h tagger/settings.h
scrut/matching.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
scrut/matching.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
scrut/matching.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
scrut/matching.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/matching.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/matching.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/matching.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/matching.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/matching.o: scrut/rules.h tagger/text.h tagger/hashtable.h
scrut/matching.o: /usr/include/stdlib.h /usr/include/sys/types.h
scrut/matching.o: /usr/include/sys/sysmacros.h
scrut/matching.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/matching.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/matching.o: scrut/semantictype.h scrut/operation.h scrut/rules.tab.h
scrut/matching.o: scrut/expr.h scrut/rulesettings.h
scrut/matchingset.o: scrut/matchingset.h scrut/matching.h tagger/wordtoken.h
scrut/matchingset.o: tagger/letter.h /usr/include/string.h
scrut/matchingset.o: /usr/include/features.h /usr/include/bits/predefs.h
scrut/matchingset.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
scrut/matchingset.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
scrut/matchingset.o: /usr/include/xlocale.h /usr/include/ctype.h
scrut/matchingset.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
scrut/matchingset.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/matchingset.o: /usr/include/bits/byteswap.h common/basics.h
scrut/matchingset.o: /usr/include/stdio.h /usr/include/libio.h
scrut/matchingset.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/matchingset.o: /usr/include/bits/stdio_lim.h
scrut/matchingset.o: /usr/include/bits/sys_errlist.h common/ensure.h
scrut/matchingset.o: /usr/include/assert.h tagger/settings.h
scrut/matchingset.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
scrut/matchingset.o: /usr/include/bits/local_lim.h
scrut/matchingset.o: /usr/include/linux/limits.h
scrut/matchingset.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
scrut/matchingset.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/matchingset.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/matchingset.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/matchingset.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/matchingset.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/matchingset.o: /usr/include/time.h /usr/include/bits/time.h
scrut/matchingset.o: /usr/include/sys/select.h /usr/include/bits/select.h
scrut/matchingset.o: /usr/include/bits/sigset.h tagger/sentence.h
scrut/matchingset.o: tagger/taglexicon.h tagger/hasharray.h
scrut/matchingset.o: tagger/tagtrigram.h scrut/gramerror.h
scrut/matchingset.o: scrut/rulesettings.h
scrut/operation.o: scrut/expr.h scrut/evaluatedvariable.h scrut/matching.h
scrut/operation.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/operation.o: /usr/include/features.h /usr/include/bits/predefs.h
scrut/operation.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
scrut/operation.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
scrut/operation.o: /usr/include/xlocale.h /usr/include/ctype.h
scrut/operation.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
scrut/operation.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/operation.o: /usr/include/bits/byteswap.h common/basics.h
scrut/operation.o: /usr/include/stdio.h /usr/include/libio.h
scrut/operation.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/operation.o: /usr/include/bits/stdio_lim.h
scrut/operation.o: /usr/include/bits/sys_errlist.h common/ensure.h
scrut/operation.o: /usr/include/assert.h tagger/settings.h
scrut/operation.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
scrut/operation.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
scrut/operation.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
scrut/operation.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/operation.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/operation.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/operation.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/operation.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/operation.o: /usr/include/time.h /usr/include/bits/time.h
scrut/operation.o: /usr/include/sys/select.h /usr/include/bits/select.h
scrut/operation.o: /usr/include/bits/sigset.h tagger/sentence.h
scrut/operation.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/operation.o: scrut/operation.h scrut/rules.tab.h scrut/rulesettings.h
scrut/operation.o: scrut/semantictype.h
scrut/regexps.o: /usr/include/stdlib.h /usr/include/features.h
scrut/regexps.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/regexps.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/regexps.o: /usr/include/gnu/stubs-64.h /usr/include/sys/types.h
scrut/regexps.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
scrut/regexps.o: /usr/include/time.h /usr/include/endian.h
scrut/regexps.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
scrut/regexps.o: /usr/include/sys/select.h /usr/include/bits/select.h
scrut/regexps.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
scrut/regexps.o: /usr/include/sys/sysmacros.h
scrut/regexps.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/regexps.o: /usr/include/stdio.h /usr/include/libio.h
scrut/regexps.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/regexps.o: /usr/include/bits/stdio_lim.h
scrut/regexps.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
scrut/regexps.o: /usr/include/xlocale.h scrut/regexp.h
scrut/rule.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
scrut/rule.o: tagger/letter.h /usr/include/string.h /usr/include/features.h
scrut/rule.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/rule.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/rule.o: /usr/include/gnu/stubs-64.h /usr/include/xlocale.h
scrut/rule.o: /usr/include/ctype.h /usr/include/bits/types.h
scrut/rule.o: /usr/include/bits/typesizes.h /usr/include/endian.h
scrut/rule.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
scrut/rule.o: common/basics.h /usr/include/stdio.h /usr/include/libio.h
scrut/rule.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/rule.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
scrut/rule.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/rule.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
scrut/rule.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
scrut/rule.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
scrut/rule.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/rule.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/rule.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/rule.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/rule.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/rule.o: /usr/include/time.h /usr/include/bits/time.h
scrut/rule.o: /usr/include/sys/select.h /usr/include/bits/select.h
scrut/rule.o: /usr/include/bits/sigset.h tagger/sentence.h
scrut/rule.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/rule.o: scrut/matchingset.h scrut/rules.h tagger/text.h
scrut/rule.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/rule.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
scrut/rule.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/rule.o: scrut/evaluatedvariable.h scrut/semantictype.h
scrut/rule.o: scrut/operation.h scrut/rules.tab.h scrut/expr.h
scrut/rule.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
scrut/rule.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
scrut/rule.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
scrut/rule.o: tagger/inflectlexicon.h tagger/wordruleterms.h
scrut/rule.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
scrut/rule.o: prob/prob.h scrut/ruleset.h
scrut/rules.tab.o: /usr/include/stdio.h /usr/include/features.h
scrut/rules.tab.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/rules.tab.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/rules.tab.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
scrut/rules.tab.o: /usr/include/bits/typesizes.h /usr/include/libio.h
scrut/rules.tab.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/rules.tab.o: /usr/include/bits/stdio_lim.h
scrut/rules.tab.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
scrut/rules.tab.o: /usr/include/sys/types.h /usr/include/time.h
scrut/rules.tab.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/rules.tab.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
scrut/rules.tab.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
scrut/rules.tab.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
scrut/rules.tab.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/rules.tab.o: /usr/include/string.h /usr/include/xlocale.h
scrut/rules.tab.o: stava/stavaapi.h scrut/scrutinizer.h scrut/matching.h
scrut/rules.tab.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/rules.tab.o: common/basics.h common/ensure.h /usr/include/assert.h
scrut/rules.tab.o: tagger/settings.h /usr/include/limits.h
scrut/rules.tab.o: /usr/include/bits/posix1_lim.h
scrut/rules.tab.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
scrut/rules.tab.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
scrut/rules.tab.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/rules.tab.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/rules.tab.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/rules.tab.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/rules.tab.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/rules.tab.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/rules.tab.o: tagger/tagtrigram.h scrut/matchingset.h scrut/rules.h
scrut/rules.tab.o: tagger/text.h tagger/hashtable.h scrut/evaluatedvariable.h
scrut/rules.tab.o: scrut/semantictype.h scrut/operation.h scrut/rules.tab.h
scrut/rules.tab.o: scrut/expr.h scrut/rulesettings.h scrut/gramerror.h
scrut/rules.tab.o: tagger/tagger.h tagger/lexicon.h tagger/morflexicon.h
scrut/rules.tab.o: tagger/styleword.h tagger/newwordlexicon.h
scrut/rules.tab.o: tagger/newword.h tagger/wordlexicon.h
scrut/rules.tab.o: tagger/inflectlexicon.h tagger/wordruleterms.h
scrut/rules.tab.o: token/tokenizer.h common/FlexLexer.h
scrut/rules.tab.o: tagger/trigramgadget.h prob/prob.h scrut/ruleset.h
scrut/rules.tab.o: scrut/lex.yy.c /usr/include/errno.h
scrut/rules.tab.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
scrut/rules.tab.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
scrut/rules.tab.o: /usr/include/asm-generic/errno-base.h
scrut/rules.tab.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
scrut/rules.tab.o: /usr/include/bits/environments.h
scrut/rules.tab.o: /usr/include/bits/confname.h /usr/include/getopt.h
scrut/rules.tab.o: scrut/eval.cpp
scrut/ruleset.o: scrut/rules.h /usr/include/stdio.h /usr/include/features.h
scrut/ruleset.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/ruleset.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/ruleset.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
scrut/ruleset.o: /usr/include/bits/typesizes.h /usr/include/libio.h
scrut/ruleset.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/ruleset.o: /usr/include/bits/stdio_lim.h
scrut/ruleset.o: /usr/include/bits/sys_errlist.h common/stringbuf.h
scrut/ruleset.o: /usr/include/string.h /usr/include/xlocale.h common/basics.h
scrut/ruleset.o: common/ensure.h /usr/include/assert.h tagger/text.h
scrut/ruleset.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/ruleset.o: /usr/include/sys/types.h /usr/include/time.h
scrut/ruleset.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/ruleset.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
scrut/ruleset.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
scrut/ruleset.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
scrut/ruleset.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/ruleset.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/ruleset.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/ruleset.o: tagger/settings.h /usr/include/limits.h
scrut/ruleset.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
scrut/ruleset.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
scrut/ruleset.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/ruleset.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/ruleset.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/ruleset.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/ruleset.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/ruleset.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/ruleset.o: tagger/tagtrigram.h scrut/semantictype.h scrut/operation.h
scrut/ruleset.o: scrut/rules.tab.h scrut/expr.h scrut/rulesettings.h
scrut/ruleset.o: scrut/ruleset.h
scrut/rulesettings.o: scrut/rulesettings.h common/basics.h
scrut/rulesettings.o: /usr/include/stdio.h /usr/include/features.h
scrut/rulesettings.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/rulesettings.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/rulesettings.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
scrut/rulesettings.o: /usr/include/bits/typesizes.h /usr/include/libio.h
scrut/rulesettings.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/rulesettings.o: /usr/include/bits/stdio_lim.h
scrut/rulesettings.o: /usr/include/bits/sys_errlist.h common/ensure.h
scrut/rulesettings.o: /usr/include/assert.h
scrut/scrutinizer.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
scrut/scrutinizer.o: tagger/letter.h /usr/include/string.h
scrut/scrutinizer.o: /usr/include/features.h /usr/include/bits/predefs.h
scrut/scrutinizer.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
scrut/scrutinizer.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
scrut/scrutinizer.o: /usr/include/xlocale.h /usr/include/ctype.h
scrut/scrutinizer.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
scrut/scrutinizer.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/scrutinizer.o: /usr/include/bits/byteswap.h common/basics.h
scrut/scrutinizer.o: /usr/include/stdio.h /usr/include/libio.h
scrut/scrutinizer.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/scrutinizer.o: /usr/include/bits/stdio_lim.h
scrut/scrutinizer.o: /usr/include/bits/sys_errlist.h common/ensure.h
scrut/scrutinizer.o: /usr/include/assert.h tagger/settings.h
scrut/scrutinizer.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
scrut/scrutinizer.o: /usr/include/bits/local_lim.h
scrut/scrutinizer.o: /usr/include/linux/limits.h
scrut/scrutinizer.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
scrut/scrutinizer.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/scrutinizer.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/scrutinizer.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/scrutinizer.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/scrutinizer.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/scrutinizer.o: /usr/include/time.h /usr/include/bits/time.h
scrut/scrutinizer.o: /usr/include/sys/select.h /usr/include/bits/select.h
scrut/scrutinizer.o: /usr/include/bits/sigset.h tagger/sentence.h
scrut/scrutinizer.o: tagger/taglexicon.h tagger/hasharray.h
scrut/scrutinizer.o: tagger/tagtrigram.h scrut/matchingset.h scrut/rules.h
scrut/scrutinizer.o: tagger/text.h tagger/hashtable.h /usr/include/stdlib.h
scrut/scrutinizer.o: /usr/include/sys/types.h /usr/include/sys/sysmacros.h
scrut/scrutinizer.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/scrutinizer.o: scrut/evaluatedvariable.h scrut/semantictype.h
scrut/scrutinizer.o: scrut/operation.h scrut/rules.tab.h scrut/expr.h
scrut/scrutinizer.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
scrut/scrutinizer.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
scrut/scrutinizer.o: tagger/newwordlexicon.h tagger/newword.h
scrut/scrutinizer.o: tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/scrutinizer.o: tagger/wordruleterms.h token/tokenizer.h
scrut/scrutinizer.o: common/FlexLexer.h tagger/trigramgadget.h prob/prob.h
scrut/scrutinizer.o: scrut/ruleset.h ../misc/xmloutput.h stava/stavaapi.h
scrut/scrutinizer.o: prob/output.h prob/defines.h prob/report.h
scrut/scrutinizer.o: prob/probinfo.h prob/probmatch.h prob/probdefines.h
scrut/server_socket.o: /usr/include/arpa/inet.h /usr/include/features.h
scrut/server_socket.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/server_socket.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/server_socket.o: /usr/include/gnu/stubs-64.h /usr/include/netinet/in.h
scrut/server_socket.o: /usr/include/stdint.h /usr/include/bits/wchar.h
scrut/server_socket.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
scrut/server_socket.o: /usr/include/sys/types.h /usr/include/bits/types.h
scrut/server_socket.o: /usr/include/bits/typesizes.h /usr/include/time.h
scrut/server_socket.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/server_socket.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
scrut/server_socket.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
scrut/server_socket.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
scrut/server_socket.o: /usr/include/bits/pthreadtypes.h
scrut/server_socket.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
scrut/server_socket.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
scrut/server_socket.o: /usr/include/asm-generic/socket.h
scrut/server_socket.o: /usr/include/asm/sockios.h
scrut/server_socket.o: /usr/include/asm-generic/sockios.h
scrut/server_socket.o: /usr/include/bits/in.h /usr/include/netdb.h
scrut/server_socket.o: /usr/include/rpc/netdb.h /usr/include/bits/netdb.h
scrut/server_socket.o: /usr/include/strings.h /usr/include/xlocale.h
scrut/server_socket.o: scrut/server_socket.h scrut/socket.h
scrut/server_socket.o: /usr/include/stdio.h /usr/include/libio.h
scrut/server_socket.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/server_socket.o: /usr/include/bits/stdio_lim.h
scrut/server_socket.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
scrut/socket.o: /usr/include/netdb.h /usr/include/features.h
scrut/socket.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/socket.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/socket.o: /usr/include/gnu/stubs-64.h /usr/include/netinet/in.h
scrut/socket.o: /usr/include/stdint.h /usr/include/bits/wchar.h
scrut/socket.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
scrut/socket.o: /usr/include/sys/types.h /usr/include/bits/types.h
scrut/socket.o: /usr/include/bits/typesizes.h /usr/include/time.h
scrut/socket.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/socket.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
scrut/socket.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
scrut/socket.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
scrut/socket.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/uio.h
scrut/socket.o: /usr/include/bits/socket.h /usr/include/bits/sockaddr.h
scrut/socket.o: /usr/include/asm/socket.h /usr/include/asm-generic/socket.h
scrut/socket.o: /usr/include/asm/sockios.h /usr/include/asm-generic/sockios.h
scrut/socket.o: /usr/include/bits/in.h /usr/include/rpc/netdb.h
scrut/socket.o: /usr/include/bits/netdb.h /usr/include/strings.h
scrut/socket.o: /usr/include/xlocale.h /usr/include/unistd.h
scrut/socket.o: /usr/include/bits/posix_opt.h
scrut/socket.o: /usr/include/bits/environments.h /usr/include/bits/confname.h
scrut/socket.o: /usr/include/getopt.h scrut/socket.h /usr/include/arpa/inet.h
scrut/socket.o: /usr/include/stdio.h /usr/include/libio.h
scrut/socket.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/socket.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
scrut/socket.o: /usr/include/string.h
scrut/web.o: /usr/include/errno.h /usr/include/features.h
scrut/web.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/web.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/web.o: /usr/include/gnu/stubs-64.h /usr/include/bits/errno.h
scrut/web.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
scrut/web.o: /usr/include/asm-generic/errno.h
scrut/web.o: /usr/include/asm-generic/errno-base.h scrut/server_socket.h
scrut/web.o: scrut/socket.h /usr/include/arpa/inet.h
scrut/web.o: /usr/include/netinet/in.h /usr/include/stdint.h
scrut/web.o: /usr/include/bits/wchar.h /usr/include/sys/socket.h
scrut/web.o: /usr/include/sys/uio.h /usr/include/sys/types.h
scrut/web.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
scrut/web.o: /usr/include/time.h /usr/include/endian.h
scrut/web.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
scrut/web.o: /usr/include/sys/select.h /usr/include/bits/select.h
scrut/web.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
scrut/web.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
scrut/web.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
scrut/web.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
scrut/web.o: /usr/include/asm-generic/socket.h /usr/include/asm/sockios.h
scrut/web.o: /usr/include/asm-generic/sockios.h /usr/include/bits/in.h
scrut/web.o: /usr/include/stdio.h /usr/include/libio.h
scrut/web.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/web.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
scrut/web.o: /usr/include/string.h /usr/include/xlocale.h scrut/scrutinizer.h
scrut/web.o: scrut/matching.h tagger/wordtoken.h tagger/letter.h
scrut/web.o: /usr/include/ctype.h common/basics.h common/ensure.h
scrut/web.o: /usr/include/assert.h tagger/settings.h /usr/include/limits.h
scrut/web.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
scrut/web.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
scrut/web.o: common/stringbuf.h tagger/token.h tagger/word.h
scrut/web.o: tagger/hashscatter.h tagger/inflectrule.h common/message.h
scrut/web.o: tagger/tag.h tagger/bitmap.h common/file.h tagger/feature.h
scrut/web.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/web.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/web.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/web.o: tagger/tagtrigram.h scrut/matchingset.h scrut/rules.h
scrut/web.o: tagger/text.h tagger/hashtable.h /usr/include/stdlib.h
scrut/web.o: /usr/include/alloca.h scrut/evaluatedvariable.h
scrut/web.o: scrut/semantictype.h scrut/operation.h scrut/rules.tab.h
scrut/web.o: scrut/expr.h scrut/rulesettings.h scrut/gramerror.h
scrut/web.o: tagger/tagger.h tagger/lexicon.h tagger/morflexicon.h
scrut/web.o: tagger/styleword.h tagger/newwordlexicon.h tagger/newword.h
scrut/web.o: tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/web.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
scrut/web.o: tagger/trigramgadget.h prob/prob.h prob/output.h
scrut/web.o: ../misc/xmloutput.h
tagger/hashscatter.o: tagger/hashscatter.h tagger/letter.h
tagger/hashscatter.o: /usr/include/string.h /usr/include/features.h
tagger/hashscatter.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
tagger/hashscatter.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
tagger/hashscatter.o: /usr/include/gnu/stubs-64.h /usr/include/xlocale.h
tagger/hashscatter.o: /usr/include/ctype.h /usr/include/bits/types.h
tagger/hashscatter.o: /usr/include/bits/typesizes.h /usr/include/endian.h
tagger/hashscatter.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
tagger/hashscatter.o: common/basics.h /usr/include/stdio.h
tagger/hashscatter.o: /usr/include/libio.h /usr/include/_G_config.h
tagger/hashscatter.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
tagger/hashscatter.o: /usr/include/bits/sys_errlist.h common/ensure.h
tagger/hashscatter.o: /usr/include/assert.h
tagger/inflectlexicon.o: common/file.h /usr/include/ctype.h
tagger/inflectlexicon.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/inflectlexicon.o: /usr/include/sys/cdefs.h
tagger/inflectlexicon.o: /usr/include/bits/wordsize.h
tagger/inflectlexicon.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/inflectlexicon.o: /usr/include/bits/types.h
tagger/inflectlexicon.o: /usr/include/bits/typesizes.h /usr/include/endian.h
tagger/inflectlexicon.o: /usr/include/bits/endian.h
tagger/inflectlexicon.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
tagger/inflectlexicon.o: common/ensure.h /usr/include/assert.h
tagger/inflectlexicon.o: common/message.h tagger/inflectlexicon.h
tagger/inflectlexicon.o: common/basics.h /usr/include/stdio.h
tagger/inflectlexicon.o: /usr/include/libio.h /usr/include/_G_config.h
tagger/inflectlexicon.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
tagger/inflectlexicon.o: /usr/include/bits/sys_errlist.h tagger/inflectrule.h
tagger/inflectlexicon.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
tagger/inflectlexicon.o: /usr/include/bits/local_lim.h
tagger/inflectlexicon.o: /usr/include/linux/limits.h
tagger/inflectlexicon.o: /usr/include/bits/posix2_lim.h tagger/tag.h
tagger/inflectlexicon.o: /usr/include/string.h tagger/bitmap.h
tagger/inflectlexicon.o: tagger/feature.h tagger/hashscatter.h
tagger/inflectlexicon.o: tagger/settings.h tagger/tagdefines.h
tagger/inflectlexicon.o: tagger/letter.h tagger/taglexicon.h
tagger/inflectlexicon.o: tagger/hasharray.h tagger/tagtrigram.h
tagger/inflectlexicon.o: tagger/token.h tagger/word.h tagger/wordtag.h
tagger/inflectlexicon.o: common/stringbuf.h
tagger/inflectrule.o: tagger/letter.h /usr/include/string.h
tagger/inflectrule.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/inflectrule.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
tagger/inflectrule.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/inflectrule.o: /usr/include/xlocale.h /usr/include/ctype.h
tagger/inflectrule.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
tagger/inflectrule.o: /usr/include/endian.h /usr/include/bits/endian.h
tagger/inflectrule.o: /usr/include/bits/byteswap.h common/basics.h
tagger/inflectrule.o: /usr/include/stdio.h /usr/include/libio.h
tagger/inflectrule.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/inflectrule.o: /usr/include/bits/stdio_lim.h
tagger/inflectrule.o: /usr/include/bits/sys_errlist.h common/ensure.h
tagger/inflectrule.o: /usr/include/assert.h tagger/inflectrule.h
tagger/inflectrule.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
tagger/inflectrule.o: /usr/include/bits/local_lim.h
tagger/inflectrule.o: /usr/include/linux/limits.h
tagger/inflectrule.o: /usr/include/bits/posix2_lim.h tagger/tag.h
tagger/inflectrule.o: tagger/bitmap.h common/file.h common/message.h
tagger/inflectrule.o: tagger/feature.h tagger/hashscatter.h tagger/settings.h
tagger/inflectrule.o: tagger/tagdefines.h
tagger/letter.o: tagger/letter.h /usr/include/string.h
tagger/letter.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/letter.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
tagger/letter.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/letter.o: /usr/include/xlocale.h /usr/include/ctype.h
tagger/letter.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
tagger/letter.o: /usr/include/endian.h /usr/include/bits/endian.h
tagger/letter.o: /usr/include/bits/byteswap.h common/basics.h
tagger/letter.o: /usr/include/stdio.h /usr/include/libio.h
tagger/letter.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/letter.o: /usr/include/bits/stdio_lim.h
tagger/letter.o: /usr/include/bits/sys_errlist.h common/ensure.h
tagger/letter.o: /usr/include/assert.h
tagger/lexicon.o: /usr/include/math.h /usr/include/features.h
tagger/lexicon.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
tagger/lexicon.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
tagger/lexicon.o: /usr/include/gnu/stubs-64.h /usr/include/bits/huge_val.h
tagger/lexicon.o: /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h
tagger/lexicon.o: /usr/include/bits/inf.h /usr/include/bits/nan.h
tagger/lexicon.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
tagger/lexicon.o: common/file.h /usr/include/ctype.h
tagger/lexicon.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
tagger/lexicon.o: /usr/include/endian.h /usr/include/bits/endian.h
tagger/lexicon.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
tagger/lexicon.o: common/ensure.h /usr/include/assert.h common/message.h
tagger/lexicon.o: tagger/letter.h /usr/include/string.h common/basics.h
tagger/lexicon.o: /usr/include/stdio.h /usr/include/libio.h
tagger/lexicon.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/lexicon.o: /usr/include/bits/stdio_lim.h
tagger/lexicon.o: /usr/include/bits/sys_errlist.h tagger/morf.h
tagger/lexicon.o: tagger/sentence.h tagger/wordtoken.h tagger/settings.h
tagger/lexicon.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
tagger/lexicon.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
tagger/lexicon.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
tagger/lexicon.o: tagger/token.h tagger/word.h tagger/hashscatter.h
tagger/lexicon.o: tagger/inflectrule.h tagger/tag.h tagger/bitmap.h
tagger/lexicon.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
tagger/lexicon.o: tagger/lexicon.h tagger/hashtable.h /usr/include/stdlib.h
tagger/lexicon.o: /usr/include/sys/types.h /usr/include/time.h
tagger/lexicon.o: /usr/include/sys/select.h /usr/include/bits/select.h
tagger/lexicon.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
tagger/lexicon.o: /usr/include/sys/sysmacros.h
tagger/lexicon.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
tagger/lexicon.o: tagger/morflexicon.h tagger/hasharray.h tagger/taglexicon.h
tagger/lexicon.o: tagger/tagtrigram.h tagger/styleword.h
tagger/lexicon.o: tagger/newwordlexicon.h tagger/newword.h
tagger/lexicon.o: tagger/wordlexicon.h tagger/inflectlexicon.h
tagger/lexicon.o: tagger/wordruleterms.h tagger/tagger.h common/timer.h
tagger/lexicon.o: /usr/include/sys/time.h tagger/text.h token/tokenizer.h
tagger/lexicon.o: common/FlexLexer.h tagger/trigramgadget.h
tagger/main.o: common/file.h /usr/include/ctype.h /usr/include/features.h
tagger/main.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
tagger/main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
tagger/main.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
tagger/main.o: /usr/include/bits/typesizes.h /usr/include/endian.h
tagger/main.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
tagger/main.o: /usr/include/xlocale.h common/ensure.h /usr/include/assert.h
tagger/main.o: common/message.h tagger/letter.h /usr/include/string.h
tagger/main.o: common/basics.h /usr/include/stdio.h /usr/include/libio.h
tagger/main.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
tagger/main.o: tagger/settings.h /usr/include/limits.h
tagger/main.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
tagger/main.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
tagger/main.o: tagger/tagger.h tagger/lexicon.h tagger/hashtable.h
tagger/main.o: /usr/include/stdlib.h /usr/include/sys/types.h
tagger/main.o: /usr/include/time.h /usr/include/sys/select.h
tagger/main.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
tagger/main.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
tagger/main.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
tagger/main.o: tagger/morflexicon.h tagger/word.h tagger/hashscatter.h
tagger/main.o: tagger/inflectrule.h tagger/tag.h tagger/bitmap.h
tagger/main.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
tagger/main.o: common/stringbuf.h tagger/hasharray.h tagger/taglexicon.h
tagger/main.o: tagger/tagtrigram.h tagger/token.h tagger/styleword.h
tagger/main.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
tagger/main.o: tagger/inflectlexicon.h tagger/wordruleterms.h
tagger/main.o: tagger/sentence.h tagger/wordtoken.h common/timer.h
tagger/main.o: /usr/include/sys/time.h tagger/text.h token/tokenizer.h
tagger/main.o: common/FlexLexer.h tagger/trigramgadget.h
tagger/morflexicon.o: common/file.h /usr/include/ctype.h
tagger/morflexicon.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/morflexicon.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
tagger/morflexicon.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/morflexicon.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
tagger/morflexicon.o: /usr/include/endian.h /usr/include/bits/endian.h
tagger/morflexicon.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
tagger/morflexicon.o: common/ensure.h /usr/include/assert.h common/message.h
tagger/morflexicon.o: tagger/morflexicon.h tagger/word.h
tagger/morflexicon.o: /usr/include/string.h tagger/hashscatter.h
tagger/morflexicon.o: tagger/inflectrule.h common/basics.h
tagger/morflexicon.o: /usr/include/stdio.h /usr/include/libio.h
tagger/morflexicon.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/morflexicon.o: /usr/include/bits/stdio_lim.h
tagger/morflexicon.o: /usr/include/bits/sys_errlist.h /usr/include/limits.h
tagger/morflexicon.o: /usr/include/bits/posix1_lim.h
tagger/morflexicon.o: /usr/include/bits/local_lim.h
tagger/morflexicon.o: /usr/include/linux/limits.h
tagger/morflexicon.o: /usr/include/bits/posix2_lim.h tagger/tag.h
tagger/morflexicon.o: tagger/bitmap.h tagger/feature.h tagger/settings.h
tagger/morflexicon.o: tagger/tagdefines.h tagger/wordtag.h common/stringbuf.h
tagger/morflexicon.o: tagger/hasharray.h tagger/taglexicon.h
tagger/morflexicon.o: tagger/tagtrigram.h tagger/token.h tagger/styleword.h
tagger/newwordlexicon.o: tagger/newwordlexicon.h tagger/newword.h
tagger/newwordlexicon.o: tagger/word.h /usr/include/string.h
tagger/newwordlexicon.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/newwordlexicon.o: /usr/include/sys/cdefs.h
tagger/newwordlexicon.o: /usr/include/bits/wordsize.h
tagger/newwordlexicon.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/newwordlexicon.o: /usr/include/xlocale.h tagger/hashscatter.h
tagger/newwordlexicon.o: tagger/inflectrule.h common/basics.h
tagger/newwordlexicon.o: /usr/include/stdio.h /usr/include/bits/types.h
tagger/newwordlexicon.o: /usr/include/bits/typesizes.h /usr/include/libio.h
tagger/newwordlexicon.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/newwordlexicon.o: /usr/include/bits/stdio_lim.h
tagger/newwordlexicon.o: /usr/include/bits/sys_errlist.h common/ensure.h
tagger/newwordlexicon.o: /usr/include/assert.h /usr/include/limits.h
tagger/newwordlexicon.o: /usr/include/bits/posix1_lim.h
tagger/newwordlexicon.o: /usr/include/bits/local_lim.h
tagger/newwordlexicon.o: /usr/include/linux/limits.h
tagger/newwordlexicon.o: /usr/include/bits/posix2_lim.h common/message.h
tagger/newwordlexicon.o: tagger/tag.h tagger/bitmap.h common/file.h
tagger/newwordlexicon.o: /usr/include/ctype.h /usr/include/endian.h
tagger/newwordlexicon.o: /usr/include/bits/endian.h
tagger/newwordlexicon.o: /usr/include/bits/byteswap.h tagger/feature.h
tagger/newwordlexicon.o: tagger/settings.h tagger/tagdefines.h
tagger/newwordlexicon.o: tagger/wordtag.h common/stringbuf.h
tagger/newwordlexicon.o: tagger/hashtable.h /usr/include/stdlib.h
tagger/newwordlexicon.o: /usr/include/sys/types.h /usr/include/time.h
tagger/newwordlexicon.o: /usr/include/sys/select.h /usr/include/bits/select.h
tagger/newwordlexicon.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
tagger/newwordlexicon.o: /usr/include/sys/sysmacros.h
tagger/newwordlexicon.o: /usr/include/bits/pthreadtypes.h
tagger/newwordlexicon.o: /usr/include/alloca.h
tagger/sentence.o: tagger/letter.h /usr/include/string.h
tagger/sentence.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/sentence.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
tagger/sentence.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/sentence.o: /usr/include/xlocale.h /usr/include/ctype.h
tagger/sentence.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
tagger/sentence.o: /usr/include/endian.h /usr/include/bits/endian.h
tagger/sentence.o: /usr/include/bits/byteswap.h common/basics.h
tagger/sentence.o: /usr/include/stdio.h /usr/include/libio.h
tagger/sentence.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/sentence.o: /usr/include/bits/stdio_lim.h
tagger/sentence.o: /usr/include/bits/sys_errlist.h common/ensure.h
tagger/sentence.o: /usr/include/assert.h tagger/sentence.h tagger/wordtoken.h
tagger/sentence.o: tagger/settings.h /usr/include/limits.h
tagger/sentence.o: /usr/include/bits/posix1_lim.h
tagger/sentence.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
tagger/sentence.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
tagger/sentence.o: tagger/token.h tagger/word.h tagger/hashscatter.h
tagger/sentence.o: tagger/inflectrule.h common/message.h tagger/tag.h
tagger/sentence.o: tagger/bitmap.h common/file.h tagger/feature.h
tagger/sentence.o: tagger/tagdefines.h tagger/wordtag.h tagger/tagger.h
tagger/sentence.o: tagger/lexicon.h tagger/hashtable.h /usr/include/stdlib.h
tagger/sentence.o: /usr/include/sys/types.h /usr/include/time.h
tagger/sentence.o: /usr/include/sys/select.h /usr/include/bits/select.h
tagger/sentence.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
tagger/sentence.o: /usr/include/sys/sysmacros.h
tagger/sentence.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
tagger/sentence.o: tagger/morflexicon.h tagger/hasharray.h
tagger/sentence.o: tagger/taglexicon.h tagger/tagtrigram.h tagger/styleword.h
tagger/sentence.o: tagger/newwordlexicon.h tagger/newword.h
tagger/sentence.o: tagger/wordlexicon.h tagger/inflectlexicon.h
tagger/sentence.o: tagger/wordruleterms.h common/timer.h
tagger/sentence.o: /usr/include/sys/time.h tagger/text.h token/tokenizer.h
tagger/sentence.o: common/FlexLexer.h tagger/trigramgadget.h
tagger/settings.o: common/basics.h /usr/include/stdio.h
tagger/settings.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/settings.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
tagger/settings.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/settings.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
tagger/settings.o: /usr/include/libio.h /usr/include/_G_config.h
tagger/settings.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
tagger/settings.o: /usr/include/bits/sys_errlist.h common/ensure.h
tagger/settings.o: /usr/include/assert.h common/file.h /usr/include/ctype.h
tagger/settings.o: /usr/include/endian.h /usr/include/bits/endian.h
tagger/settings.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
tagger/settings.o: common/message.h tagger/settings.h /usr/include/limits.h
tagger/settings.o: /usr/include/bits/posix1_lim.h
tagger/settings.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
tagger/settings.o: /usr/include/bits/posix2_lim.h
tagger/tag.o: common/message.h tagger/tag.h /usr/include/string.h
tagger/tag.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/tag.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
tagger/tag.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/tag.o: /usr/include/xlocale.h common/basics.h /usr/include/stdio.h
tagger/tag.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
tagger/tag.o: /usr/include/libio.h /usr/include/_G_config.h
tagger/tag.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
tagger/tag.o: /usr/include/bits/sys_errlist.h common/ensure.h
tagger/tag.o: /usr/include/assert.h tagger/bitmap.h common/file.h
tagger/tag.o: /usr/include/ctype.h /usr/include/endian.h
tagger/tag.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
tagger/tag.o: tagger/feature.h tagger/hashscatter.h tagger/settings.h
tagger/tag.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
tagger/tag.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
tagger/tag.o: /usr/include/bits/posix2_lim.h tagger/tagdefines.h
tagger/tag.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
tagger/tag.o: tagger/token.h tagger/word.h tagger/inflectrule.h
tagger/tag.o: tagger/wordtag.h common/stringbuf.h
tagger/tagger.o: /usr/include/math.h /usr/include/features.h
tagger/tagger.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
tagger/tagger.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
tagger/tagger.o: /usr/include/gnu/stubs-64.h /usr/include/bits/huge_val.h
tagger/tagger.o: /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h
tagger/tagger.o: /usr/include/bits/inf.h /usr/include/bits/nan.h
tagger/tagger.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
tagger/tagger.o: common/file.h /usr/include/ctype.h /usr/include/bits/types.h
tagger/tagger.o: /usr/include/bits/typesizes.h /usr/include/endian.h
tagger/tagger.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
tagger/tagger.o: /usr/include/xlocale.h common/ensure.h /usr/include/assert.h
tagger/tagger.o: common/message.h tagger/letter.h /usr/include/string.h
tagger/tagger.o: common/basics.h /usr/include/stdio.h /usr/include/libio.h
tagger/tagger.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/tagger.o: /usr/include/bits/stdio_lim.h
tagger/tagger.o: /usr/include/bits/sys_errlist.h tagger/morf.h
tagger/tagger.o: tagger/sentence.h tagger/wordtoken.h tagger/settings.h
tagger/tagger.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
tagger/tagger.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
tagger/tagger.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
tagger/tagger.o: tagger/token.h tagger/word.h tagger/hashscatter.h
tagger/tagger.o: tagger/inflectrule.h tagger/tag.h tagger/bitmap.h
tagger/tagger.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
tagger/tagger.o: tagger/tagger.h tagger/lexicon.h tagger/hashtable.h
tagger/tagger.o: /usr/include/stdlib.h /usr/include/sys/types.h
tagger/tagger.o: /usr/include/time.h /usr/include/sys/select.h
tagger/tagger.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
tagger/tagger.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
tagger/tagger.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
tagger/tagger.o: tagger/morflexicon.h tagger/hasharray.h tagger/taglexicon.h
tagger/tagger.o: tagger/tagtrigram.h tagger/styleword.h
tagger/tagger.o: tagger/newwordlexicon.h tagger/newword.h
tagger/tagger.o: tagger/wordlexicon.h tagger/inflectlexicon.h
tagger/tagger.o: tagger/wordruleterms.h common/timer.h
tagger/tagger.o: /usr/include/sys/time.h tagger/text.h token/tokenizer.h
tagger/tagger.o: common/FlexLexer.h tagger/trigramgadget.h
tagger/taglexicon.o: /usr/include/math.h /usr/include/features.h
tagger/taglexicon.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
tagger/taglexicon.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
tagger/taglexicon.o: /usr/include/gnu/stubs-64.h /usr/include/bits/huge_val.h
tagger/taglexicon.o: /usr/include/bits/huge_valf.h
tagger/taglexicon.o: /usr/include/bits/huge_vall.h /usr/include/bits/inf.h
tagger/taglexicon.o: /usr/include/bits/nan.h /usr/include/bits/mathdef.h
tagger/taglexicon.o: /usr/include/bits/mathcalls.h /usr/include/string.h
tagger/taglexicon.o: /usr/include/xlocale.h /usr/include/stdlib.h
tagger/taglexicon.o: /usr/include/sys/types.h /usr/include/bits/types.h
tagger/taglexicon.o: /usr/include/bits/typesizes.h /usr/include/time.h
tagger/taglexicon.o: /usr/include/endian.h /usr/include/bits/endian.h
tagger/taglexicon.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
tagger/taglexicon.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
tagger/taglexicon.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
tagger/taglexicon.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
tagger/taglexicon.o: common/file.h /usr/include/ctype.h common/ensure.h
tagger/taglexicon.o: /usr/include/assert.h common/message.h tagger/letter.h
tagger/taglexicon.o: common/basics.h /usr/include/stdio.h
tagger/taglexicon.o: /usr/include/libio.h /usr/include/_G_config.h
tagger/taglexicon.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
tagger/taglexicon.o: /usr/include/bits/sys_errlist.h tagger/settings.h
tagger/taglexicon.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
tagger/taglexicon.o: /usr/include/bits/local_lim.h
tagger/taglexicon.o: /usr/include/linux/limits.h
tagger/taglexicon.o: /usr/include/bits/posix2_lim.h tagger/taglexicon.h
tagger/taglexicon.o: tagger/bitmap.h tagger/feature.h tagger/hasharray.h
tagger/taglexicon.o: tagger/tag.h tagger/hashscatter.h tagger/tagdefines.h
tagger/taglexicon.o: tagger/tagtrigram.h tagger/token.h tagger/word.h
tagger/taglexicon.o: tagger/inflectrule.h tagger/wordtag.h common/stringbuf.h
tagger/text.o: tagger/sentence.h tagger/wordtoken.h tagger/letter.h
tagger/text.o: /usr/include/string.h /usr/include/features.h
tagger/text.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
tagger/text.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
tagger/text.o: /usr/include/gnu/stubs-64.h /usr/include/xlocale.h
tagger/text.o: /usr/include/ctype.h /usr/include/bits/types.h
tagger/text.o: /usr/include/bits/typesizes.h /usr/include/endian.h
tagger/text.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
tagger/text.o: common/basics.h /usr/include/stdio.h /usr/include/libio.h
tagger/text.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/text.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
tagger/text.o: common/ensure.h /usr/include/assert.h tagger/settings.h
tagger/text.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
tagger/text.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
tagger/text.o: /usr/include/bits/posix2_lim.h common/stringbuf.h
tagger/text.o: tagger/token.h tagger/word.h tagger/hashscatter.h
tagger/text.o: tagger/inflectrule.h common/message.h tagger/tag.h
tagger/text.o: tagger/bitmap.h common/file.h tagger/feature.h
tagger/text.o: tagger/tagdefines.h tagger/wordtag.h tagger/text.h
tagger/word.o: /usr/include/math.h /usr/include/features.h
tagger/word.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
tagger/word.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
tagger/word.o: /usr/include/gnu/stubs-64.h /usr/include/bits/huge_val.h
tagger/word.o: /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h
tagger/word.o: /usr/include/bits/inf.h /usr/include/bits/nan.h
tagger/word.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
tagger/word.o: tagger/word.h /usr/include/string.h /usr/include/xlocale.h
tagger/word.o: tagger/hashscatter.h tagger/inflectrule.h common/basics.h
tagger/word.o: /usr/include/stdio.h /usr/include/bits/types.h
tagger/word.o: /usr/include/bits/typesizes.h /usr/include/libio.h
tagger/word.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/word.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
tagger/word.o: common/ensure.h /usr/include/assert.h /usr/include/limits.h
tagger/word.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
tagger/word.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
tagger/word.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
tagger/word.o: /usr/include/ctype.h /usr/include/endian.h
tagger/word.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
tagger/word.o: tagger/feature.h tagger/settings.h tagger/tagdefines.h
tagger/word.o: tagger/wordtag.h common/stringbuf.h tagger/newword.h
tagger/wordlexicon.o: common/file.h /usr/include/ctype.h
tagger/wordlexicon.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/wordlexicon.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
tagger/wordlexicon.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/wordlexicon.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
tagger/wordlexicon.o: /usr/include/endian.h /usr/include/bits/endian.h
tagger/wordlexicon.o: /usr/include/bits/byteswap.h /usr/include/xlocale.h
tagger/wordlexicon.o: common/ensure.h /usr/include/assert.h common/message.h
tagger/wordlexicon.o: tagger/letter.h /usr/include/string.h common/basics.h
tagger/wordlexicon.o: /usr/include/stdio.h /usr/include/libio.h
tagger/wordlexicon.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/wordlexicon.o: /usr/include/bits/stdio_lim.h
tagger/wordlexicon.o: /usr/include/bits/sys_errlist.h tagger/newwordlexicon.h
tagger/wordlexicon.o: tagger/newword.h tagger/word.h tagger/hashscatter.h
tagger/wordlexicon.o: tagger/inflectrule.h /usr/include/limits.h
tagger/wordlexicon.o: /usr/include/bits/posix1_lim.h
tagger/wordlexicon.o: /usr/include/bits/local_lim.h
tagger/wordlexicon.o: /usr/include/linux/limits.h
tagger/wordlexicon.o: /usr/include/bits/posix2_lim.h tagger/tag.h
tagger/wordlexicon.o: tagger/bitmap.h tagger/feature.h tagger/settings.h
tagger/wordlexicon.o: tagger/tagdefines.h tagger/wordtag.h common/stringbuf.h
tagger/wordlexicon.o: tagger/hashtable.h /usr/include/stdlib.h
tagger/wordlexicon.o: /usr/include/sys/types.h /usr/include/time.h
tagger/wordlexicon.o: /usr/include/sys/select.h /usr/include/bits/select.h
tagger/wordlexicon.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
tagger/wordlexicon.o: /usr/include/sys/sysmacros.h
tagger/wordlexicon.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
tagger/wordlexicon.o: tagger/wordlexicon.h tagger/hasharray.h
tagger/wordlexicon.o: tagger/inflectlexicon.h tagger/taglexicon.h
tagger/wordlexicon.o: tagger/tagtrigram.h tagger/token.h tagger/styleword.h
tagger/wordtag.o: tagger/word.h /usr/include/string.h /usr/include/features.h
tagger/wordtag.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
tagger/wordtag.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
tagger/wordtag.o: /usr/include/gnu/stubs-64.h /usr/include/xlocale.h
tagger/wordtag.o: tagger/hashscatter.h tagger/inflectrule.h common/basics.h
tagger/wordtag.o: /usr/include/stdio.h /usr/include/bits/types.h
tagger/wordtag.o: /usr/include/bits/typesizes.h /usr/include/libio.h
tagger/wordtag.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/wordtag.o: /usr/include/bits/stdio_lim.h
tagger/wordtag.o: /usr/include/bits/sys_errlist.h common/ensure.h
tagger/wordtag.o: /usr/include/assert.h /usr/include/limits.h
tagger/wordtag.o: /usr/include/bits/posix1_lim.h
tagger/wordtag.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
tagger/wordtag.o: /usr/include/bits/posix2_lim.h common/message.h
tagger/wordtag.o: tagger/tag.h tagger/bitmap.h common/file.h
tagger/wordtag.o: /usr/include/ctype.h /usr/include/endian.h
tagger/wordtag.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
tagger/wordtag.o: tagger/feature.h tagger/settings.h tagger/tagdefines.h
tagger/wordtag.o: tagger/wordtag.h common/stringbuf.h tagger/wordlexicon.h
tagger/wordtag.o: tagger/newword.h tagger/hasharray.h tagger/inflectlexicon.h
tagger/wordtag.o: tagger/taglexicon.h tagger/tagtrigram.h tagger/token.h
tagger/wordtag.o: tagger/styleword.h
tagger/wordtoken.o: tagger/letter.h /usr/include/string.h
tagger/wordtoken.o: /usr/include/features.h /usr/include/bits/predefs.h
tagger/wordtoken.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
tagger/wordtoken.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
tagger/wordtoken.o: /usr/include/xlocale.h /usr/include/ctype.h
tagger/wordtoken.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
tagger/wordtoken.o: /usr/include/endian.h /usr/include/bits/endian.h
tagger/wordtoken.o: /usr/include/bits/byteswap.h common/basics.h
tagger/wordtoken.o: /usr/include/stdio.h /usr/include/libio.h
tagger/wordtoken.o: /usr/include/_G_config.h /usr/include/wchar.h
tagger/wordtoken.o: /usr/include/bits/stdio_lim.h
tagger/wordtoken.o: /usr/include/bits/sys_errlist.h common/ensure.h
tagger/wordtoken.o: /usr/include/assert.h tagger/settings.h
tagger/wordtoken.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
tagger/wordtoken.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
tagger/wordtoken.o: /usr/include/bits/posix2_lim.h tagger/token.h
tagger/wordtoken.o: tagger/newword.h tagger/word.h tagger/hashscatter.h
tagger/wordtoken.o: tagger/inflectrule.h common/message.h tagger/tag.h
tagger/wordtoken.o: tagger/bitmap.h common/file.h tagger/feature.h
tagger/wordtoken.o: tagger/tagdefines.h tagger/wordtag.h common/stringbuf.h
tagger/wordtoken.o: tagger/wordtoken.h
token/tokenizer.o: common/FlexLexer.h tagger/token.h /usr/include/string.h
token/tokenizer.o: /usr/include/features.h /usr/include/bits/predefs.h
token/tokenizer.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
token/tokenizer.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
token/tokenizer.o: /usr/include/xlocale.h /usr/include/stdlib.h
token/tokenizer.o: /usr/include/sys/types.h /usr/include/bits/types.h
token/tokenizer.o: /usr/include/bits/typesizes.h /usr/include/time.h
token/tokenizer.o: /usr/include/endian.h /usr/include/bits/endian.h
token/tokenizer.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
token/tokenizer.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
token/tokenizer.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
token/tokenizer.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
cgigranska/wwwscrutinizer.o: /usr/include/stdio.h /usr/include/features.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/predefs.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/cdefs.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/wordsize.h
cgigranska/wwwscrutinizer.o: /usr/include/gnu/stubs.h
cgigranska/wwwscrutinizer.o: /usr/include/gnu/stubs-64.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/types.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/typesizes.h
cgigranska/wwwscrutinizer.o: /usr/include/libio.h /usr/include/_G_config.h
cgigranska/wwwscrutinizer.o: /usr/include/wchar.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/stdio_lim.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/sys_errlist.h
cgigranska/wwwscrutinizer.o: /usr/include/stdlib.h /usr/include/sys/types.h
cgigranska/wwwscrutinizer.o: /usr/include/time.h /usr/include/endian.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/endian.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/byteswap.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/select.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/select.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/sigset.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/time.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/sysmacros.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/pthreadtypes.h
cgigranska/wwwscrutinizer.o: /usr/include/alloca.h /usr/include/string.h
cgigranska/wwwscrutinizer.o: /usr/include/xlocale.h /usr/include/sys/timeb.h
cgigranska/wwwscrutinizer.o: /usr/include/unistd.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/posix_opt.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/environments.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/confname.h
cgigranska/wwwscrutinizer.o: /usr/include/getopt.h /usr/include/sys/wait.h
cgigranska/wwwscrutinizer.o: /usr/include/signal.h /usr/include/bits/signum.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/siginfo.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/sigaction.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/sigcontext.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/sigstack.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/ucontext.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/sigthread.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/resource.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/resource.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/waitflags.h
cgigranska/wwwscrutinizer.o: /usr/include/bits/waitstatus.h
cgigranska/wwwscrutinizer.o: /usr/include/errno.h /usr/include/bits/errno.h
cgigranska/wwwscrutinizer.o: /usr/include/linux/errno.h
cgigranska/wwwscrutinizer.o: /usr/include/asm/errno.h
cgigranska/wwwscrutinizer.o: /usr/include/asm-generic/errno.h
cgigranska/wwwscrutinizer.o: /usr/include/asm-generic/errno-base.h
cgigranska/wwwscrutinizer.o: /usr/include/ctype.h
scrut/lex.yy.o: /usr/include/stdio.h /usr/include/features.h
scrut/lex.yy.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
scrut/lex.yy.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
scrut/lex.yy.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
scrut/lex.yy.o: /usr/include/bits/typesizes.h /usr/include/libio.h
scrut/lex.yy.o: /usr/include/_G_config.h /usr/include/wchar.h
scrut/lex.yy.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
scrut/lex.yy.o: /usr/include/string.h /usr/include/xlocale.h
scrut/lex.yy.o: /usr/include/errno.h /usr/include/bits/errno.h
scrut/lex.yy.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
scrut/lex.yy.o: /usr/include/asm-generic/errno.h
scrut/lex.yy.o: /usr/include/asm-generic/errno-base.h /usr/include/stdlib.h
scrut/lex.yy.o: /usr/include/sys/types.h /usr/include/time.h
scrut/lex.yy.o: /usr/include/endian.h /usr/include/bits/endian.h
scrut/lex.yy.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
scrut/lex.yy.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
scrut/lex.yy.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
scrut/lex.yy.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
scrut/lex.yy.o: common/stringbuf.h common/basics.h common/ensure.h
scrut/lex.yy.o: /usr/include/assert.h /usr/include/unistd.h
scrut/lex.yy.o: /usr/include/bits/posix_opt.h
scrut/lex.yy.o: /usr/include/bits/environments.h /usr/include/bits/confname.h
scrut/lex.yy.o: /usr/include/getopt.h
stava/rattstava.o: /usr/include/stdio.h /usr/include/features.h
stava/rattstava.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
stava/rattstava.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
stava/rattstava.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
stava/rattstava.o: /usr/include/bits/typesizes.h /usr/include/libio.h
stava/rattstava.o: /usr/include/_G_config.h /usr/include/wchar.h
stava/rattstava.o: /usr/include/bits/stdio_lim.h
stava/rattstava.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
stava/rattstava.o: /usr/include/sys/types.h /usr/include/time.h
stava/rattstava.o: /usr/include/endian.h /usr/include/bits/endian.h
stava/rattstava.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
stava/rattstava.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
stava/rattstava.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
stava/rattstava.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
stava/rattstava.o: /usr/include/string.h /usr/include/xlocale.h stava/stava.h
stava/rattstava.o: stava/rattstava.h stava/suffix.h
stava/stavaapi.o: /usr/include/stdio.h /usr/include/features.h
stava/stavaapi.o: /usr/include/bits/predefs.h /usr/include/sys/cdefs.h
stava/stavaapi.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
stava/stavaapi.o: /usr/include/gnu/stubs-64.h /usr/include/bits/types.h
stava/stavaapi.o: /usr/include/bits/typesizes.h /usr/include/libio.h
stava/stavaapi.o: /usr/include/_G_config.h /usr/include/wchar.h
stava/stavaapi.o: /usr/include/bits/stdio_lim.h
stava/stavaapi.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
stava/stavaapi.o: /usr/include/sys/types.h /usr/include/time.h
stava/stavaapi.o: /usr/include/endian.h /usr/include/bits/endian.h
stava/stavaapi.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
stava/stavaapi.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
stava/stavaapi.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
stava/stavaapi.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
stava/stavaapi.o: /usr/include/string.h /usr/include/xlocale.h
stava/stavaapi.o: /usr/include/ctype.h stava/stava.h stava/rattstava.h
stava/stavaapi.o: stava/suffix.h stava/stavaconstants.h stava/stavaapi.h
stava/suffix.o: stava/suffix.h stava/stava.h /usr/include/stdio.h
stava/suffix.o: /usr/include/features.h /usr/include/bits/predefs.h
stava/suffix.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
stava/suffix.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
stava/suffix.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
stava/suffix.o: /usr/include/libio.h /usr/include/_G_config.h
stava/suffix.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
stava/suffix.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
stava/suffix.o: /usr/include/sys/types.h /usr/include/time.h
stava/suffix.o: /usr/include/endian.h /usr/include/bits/endian.h
stava/suffix.o: /usr/include/bits/byteswap.h /usr/include/sys/select.h
stava/suffix.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
stava/suffix.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
stava/suffix.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
stava/suffix.o: /usr/include/string.h /usr/include/xlocale.h
