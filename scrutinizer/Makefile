###############################################
#
#  makefile -- fil för att bygga projektet
#
#  skriv först make depend för att bygga
#    beroenden (tillkommer nederst i makefilen)
#  skriv sedan make som vanligt
#


# Define where the lexicon files, rule files, Stava lexicon files, etc. are located.
# These folders will be used if no command line option is given and no environment variable is set.

# if no STAVA_LEXICON in environment, use:
#STAVALEXICONFOLDER = /opt/stava/GPLstava_extra/lib/
STAVALEXICONFOLDER = ${CURDIR}/../stava/lib/

# if no SCRUTINIZER_RULE_FILE in environment, use:
#DEFAULTRULEFILE = /opt/servergranska/cgigranska/default-swedish-rules
DEFAULTRULEFILE = ${CURDIR}/../rulesets/svesveTng/therules

# if no TAGGER_LEXICON in environment, use:
#TAGGERLEXICONFOLDER = /opt/servergranska/lexikon/
TAGGERLEXICONFOLDER = ${CURDIR}/../suc/

# when "use example sentences" is chosen, use this file:
EXAMPLESENTENCESFILE = ${CURDIR}/exempel_litet.txt

DEFAULTFILES = -DSTAVALEXICONFOLDER=\"$(STAVALEXICONFOLDER)\" -DDEFAULTRULEFILE=\"$(DEFAULTRULEFILE)\" -DTAGGERLEXICONFOLDER=\"$(TAGGERLEXICONFOLDER)\" -DEXAMPLESENTENCESFILE=\"$(EXAMPLESENTENCESFILE)\"

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
#CC   = gcc -std=gnu89# c-kompilator
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
#XERCES = /usr/local/include/xercesc

INCL = -I/usr/include/xercesc

FLAGS = $(CFLAGS) -I$(CD) -I$(TD) -I$(SD) -I$(PD) -I$(STD) \
  -I$(TOD) -I.. $(INCL)
#LIB_PATH_RT = -R$(XERCES)/lib #set runtime-path at compile time

STAVA_OBJ = $(STD)/stavaapi.o $(STD)/suffix.o $(STD)/rattstava.o
STAVA_FLAGS = -DAPI

TOK_OBJ = $(TOD)/tokenizer.o

MAIN_OBJ = $(CD)/main.o
COMMON_OBJ = $(CD)/file.o $(CD)/message.o $(CD)/timer.o $(CD)/utf2latin1.o
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
  $(SD)/rulesettings.o $(SD)/scrutinizer.o $(SD)/re.o
SCRUT_FLAGS = $(DEFAULTFILES) -DMESSAGES -DTIMER -DDEVELOPER_OUTPUT #Developer_output use for parser version of granska

PROBCHECK_OBJ = $(PD)/annot.o $(PD)/output.o\
  $(PD)/probcheck.o $(PD)/trans.o $(PD)/report.o $(PD)/probdevelop.o $(PD)/probinfo.o $(PD)/probmatch.o \
  $(PD)/represent.o $(PD)/probadmin.o \
  ../misc/xmloutput.o
PROBCHECK_FLAGS = -DPROBCHECK -DNO_SENTENCE_DELIMITERS  
  #-DTRANSITIVITY -DNO_XML_SUPPORT -DNO_ANNOT_FROM_FILE 

WEB_OBJ = $(SD)/web.o $(SD)/socket.o $(SD)/server_socket.o
WEB_FLAGS = $(DEFAULTFILES) -DWEB_SCRUTINIZER

API_OBJ = granskaapi.o
API = $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ)\
  $(TAGGER_OBJ) $(PROBCHECK_OBJ) $(API_OBJ)

ALLWWW_OBJ = $(WWW_OBJ) $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ) $(TAGGER_OBJ) $(PROBCHECK_OBJ)
ALL_OBJ = $(MAIN_OBJ) $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ)\
  $(TAGGER_OBJ) $(PROBCHECK_OBJ) $(WEB_OBJ) 
ALL_FLAGS = -fPIC $(FLAGS) $(COMMON_FLAGS) $(STAVA_FLAGS) $(TOK_FLAGS)\
  $(DEVELOPERS_TAGGER_FLAGS) $(TAGGER_FLAGS) $(SCRUT_FLAGS)\
  $(PROBCHECK_FLAGS) $(WEB_FLAGS) # comment this row out when compiling wwwscrutinizer

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

# TARGETS = depend scrut/rules.tab.h scrutinizer wwwscrutinizer library testlibrary
TARGETS = scrut/rules.tab.h scrutinizer wwwscrutinizer library testlibrary
# TARGETS = scrutinizer
# WWWLIBS = -lxerces-c

all: $(TARGETS)

developers-tagg: $(DEVELOPERS_TAGG_OBJ)
	$(CPPC) $(FLAGS) -o $@ $^ $(LIBS) \
#	$(CPPC) $(FLAGS) -o $@ $^ $(LIB_PATH) $(LIBS) \

library: $(API)
	$(CPPC) -shared $(ALL_FLAGS) -o libgranska.so $(API) $(LIBS)

testlibrary: test.o XMLHandler.o library
	g++ -g -c test.cpp XMLHandler.cpp
	g++ test.o XMLHandler.o -L. -lgranska $(LIBS) -o testlibrary

scrutinizer: $(ALL_OBJ)
	$(CPPC) $(ALL_FLAGS) -o scrutinizer $(ALL_OBJ) $(LIBS)
#	$(CPPC) $(ALL_FLAGS) -o scrutinizer $(ALL_OBJ) $(LIB_PATH) $(LIBS)

wwwscrutinizer: $(ALLWWW_OBJ)
	$(CPPC) $(ALL_FLAGS) -Bstatic -o wwwscrutinizer $(ALLWWW_OBJ) $(WWWLIBS)
	strip wwwscrutinizer

tagg: $(TAGG_OBJ)
	$(CPPC) $(FLAGS) -o $@ $^ $(LIBS)
#	$(CPPC) $(FLAGS) -o $@ $^ $(LIB_PATH) $(LIBS)

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
#	rm scrutinizer testlibrary wwwscrutinizer libgranska.so
	rm -f testlibrary libgranska.so

realclean: clean 	
	rm -f $(TARGETS)
	rm -f tagg
	rm -f scrutinizer.normal scrutinizer.parse

depend:
	makedepend -- $(ALL_FLAGS) -- */*.cpp */*.c

scrutinizer: $(SD)/rules.tab.h
wwwscrutinizer: $(SD)/rules.tab.h
cgigranska/wwwmain.o: $(SD)/rules.tab.h
common/main.o: $(SD)/rules.tab.h
prob/probcheck.o: $(SD)/rules.tab.h
prob/probinfo.o: $(SD)/rules.tab.h
prob/report.o: $(SD)/rules.tab.h
scrut/element.o: $(SD)/rules.tab.h
scrut/expr.o: $(SD)/rules.tab.h
scrut/gramerror.o: $(SD)/rules.tab.h
scrut/identry.o: $(SD)/rules.tab.h
scrut/matching.o: $(SD)/rules.tab.h
scrut/operation.o: $(SD)/rules.tab.h
scrut/rule.o: $(SD)/rules.tab.h
scrut/ruleset.o: $(SD)/rules.tab.h
scrut/scrutinizer.o: $(SD)/rules.tab.h
scrut/web.o: $(SD)/rules.tab.h
scrut/rules.tab.o: $(SD)/rules.tab.h

# DO NOT DELETE

cgigranska/wwwmain.o: cgigranska/wwwscrutinizer.h scrut/scrutinizer.h
cgigranska/wwwmain.o: scrut/matching.h tagger/wordtoken.h tagger/letter.h
cgigranska/wwwmain.o: /usr/include/string.h /usr/include/ctype.h
cgigranska/wwwmain.o: /usr/include/features.h /usr/include/stdc-predef.h
cgigranska/wwwmain.o: /usr/include/endian.h common/basics.h
cgigranska/wwwmain.o: /usr/include/stdio.h common/ensure.h
cgigranska/wwwmain.o: /usr/include/assert.h tagger/settings.h
cgigranska/wwwmain.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
cgigranska/wwwmain.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
cgigranska/wwwmain.o: common/message.h tagger/tag.h tagger/bitmap.h
cgigranska/wwwmain.o: common/file.h tagger/feature.h tagger/tagdefines.h
cgigranska/wwwmain.o: tagger/wordtag.h scrut/element.h scrut/rule.h
cgigranska/wwwmain.o: common/timer.h tagger/sentence.h tagger/taglexicon.h
cgigranska/wwwmain.o: tagger/hasharray.h tagger/tagtrigram.h
cgigranska/wwwmain.o: scrut/matchingset.h scrut/rules.h tagger/text.h
cgigranska/wwwmain.o: tagger/hashtable.h /usr/include/stdlib.h
cgigranska/wwwmain.o: /usr/include/alloca.h scrut/evaluatedvariable.h
cgigranska/wwwmain.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
cgigranska/wwwmain.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
cgigranska/wwwmain.o: tagger/lexicon.h tagger/morflexicon.h
cgigranska/wwwmain.o: tagger/styleword.h tagger/newwordlexicon.h
cgigranska/wwwmain.o: tagger/newword.h tagger/wordlexicon.h
cgigranska/wwwmain.o: tagger/inflectlexicon.h tagger/wordruleterms.h
cgigranska/wwwmain.o: token/tokenizer.h common/FlexLexer.h
cgigranska/wwwmain.o: tagger/trigramgadget.h prob/prob.h scrut/ruleset.h
cgigranska/wwwmain.o: /usr/include/unistd.h
common/file.o: /usr/include/ctype.h /usr/include/features.h
common/file.o: /usr/include/stdc-predef.h /usr/include/endian.h
common/file.o: /usr/include/string.h /usr/include/stdio.h common/file.h
common/file.o: common/ensure.h /usr/include/assert.h common/message.h
common/main.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
common/main.o: tagger/letter.h /usr/include/string.h /usr/include/ctype.h
common/main.o: /usr/include/features.h /usr/include/stdc-predef.h
common/main.o: /usr/include/endian.h common/basics.h /usr/include/stdio.h
common/main.o: common/ensure.h /usr/include/assert.h tagger/settings.h
common/main.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
common/main.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
common/main.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
common/main.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
common/main.o: scrut/element.h scrut/rule.h common/timer.h tagger/sentence.h
common/main.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
common/main.o: scrut/matchingset.h scrut/rules.h tagger/text.h
common/main.o: tagger/hashtable.h /usr/include/stdlib.h /usr/include/alloca.h
common/main.o: scrut/evaluatedvariable.h scrut/semantictype.h
common/main.o: scrut/operation.h scrut/expr.h scrut/rulesettings.h
common/main.o: scrut/gramerror.h tagger/tagger.h tagger/lexicon.h
common/main.o: tagger/morflexicon.h tagger/styleword.h
common/main.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
common/main.o: tagger/inflectlexicon.h tagger/wordruleterms.h
common/main.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
common/main.o: prob/prob.h scrut/ruleset.h prob/output.h ../misc/xmloutput.h
common/main.o: prob/config.h prob/probdefines.h
common/message.o: common/message.h tagger/settings.h common/basics.h
common/message.o: /usr/include/stdio.h common/ensure.h /usr/include/assert.h
common/message.o: /usr/include/features.h /usr/include/stdc-predef.h
common/message.o: /usr/include/limits.h tagger/sentence.h tagger/wordtoken.h
common/message.o: tagger/letter.h /usr/include/string.h /usr/include/ctype.h
common/message.o: /usr/include/endian.h common/stringbuf.h tagger/token.h
common/message.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
common/message.o: tagger/tag.h tagger/bitmap.h common/file.h tagger/feature.h
common/message.o: tagger/tagdefines.h tagger/wordtag.h
developers-tagger/developers-tagger.o: /usr/include/math.h
developers-tagger/developers-tagger.o: /usr/include/time.h
developers-tagger/developers-tagger.o: /usr/include/features.h
developers-tagger/developers-tagger.o: /usr/include/stdc-predef.h
developers-tagger/developers-tagger.o: developers-tagger/developers-tagger.h
developers-tagger/developers-tagger.o: tagger/tagger.h common/basics.h
developers-tagger/developers-tagger.o: /usr/include/stdio.h common/ensure.h
developers-tagger/developers-tagger.o: /usr/include/assert.h tagger/lexicon.h
developers-tagger/developers-tagger.o: tagger/hashtable.h
developers-tagger/developers-tagger.o: /usr/include/string.h
developers-tagger/developers-tagger.o: /usr/include/stdlib.h
developers-tagger/developers-tagger.o: /usr/include/alloca.h
developers-tagger/developers-tagger.o: tagger/morflexicon.h tagger/word.h
developers-tagger/developers-tagger.o: tagger/hashscatter.h
developers-tagger/developers-tagger.o: tagger/inflectrule.h
developers-tagger/developers-tagger.o: /usr/include/limits.h common/message.h
developers-tagger/developers-tagger.o: tagger/tag.h tagger/bitmap.h
developers-tagger/developers-tagger.o: common/file.h /usr/include/ctype.h
developers-tagger/developers-tagger.o: /usr/include/endian.h tagger/feature.h
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
developers-tagger/developers-tagger.o: tagger/text.h token/tokenizer.h
developers-tagger/developers-tagger.o: common/FlexLexer.h
developers-tagger/developers-tagger.o: tagger/trigramgadget.h tagger/morf.h
developers-tagger/developers-tagger.o: developers-tagger/settings_manip.h
developers-tagger/developers-tagger.o: tagger/heap.h
developers-tagger/main.o: tagger/settings.h common/basics.h
developers-tagger/main.o: /usr/include/stdio.h common/ensure.h
developers-tagger/main.o: /usr/include/assert.h /usr/include/features.h
developers-tagger/main.o: /usr/include/stdc-predef.h /usr/include/limits.h
developers-tagger/main.o: common/file.h /usr/include/ctype.h
developers-tagger/main.o: /usr/include/endian.h common/message.h
developers-tagger/main.o: tagger/letter.h /usr/include/string.h
developers-tagger/main.o: developers-tagger/developers-tagger.h
developers-tagger/main.o: tagger/tagger.h tagger/lexicon.h tagger/hashtable.h
developers-tagger/main.o: /usr/include/stdlib.h /usr/include/alloca.h
developers-tagger/main.o: tagger/morflexicon.h tagger/word.h
developers-tagger/main.o: tagger/hashscatter.h tagger/inflectrule.h
developers-tagger/main.o: tagger/tag.h tagger/bitmap.h tagger/feature.h
developers-tagger/main.o: tagger/tagdefines.h tagger/wordtag.h
developers-tagger/main.o: common/stringbuf.h tagger/hasharray.h
developers-tagger/main.o: tagger/taglexicon.h tagger/tagtrigram.h
developers-tagger/main.o: tagger/token.h tagger/styleword.h
developers-tagger/main.o: tagger/newwordlexicon.h tagger/newword.h
developers-tagger/main.o: tagger/wordlexicon.h tagger/inflectlexicon.h
developers-tagger/main.o: tagger/wordruleterms.h tagger/sentence.h
developers-tagger/main.o: tagger/wordtoken.h common/timer.h tagger/text.h
developers-tagger/main.o: token/tokenizer.h common/FlexLexer.h
developers-tagger/main.o: tagger/trigramgadget.h
developers-tagger/main.o: developers-tagger/settings_manip.h
developers-tagger/settings_manip.o: developers-tagger/settings_manip.h
developers-tagger/settings_manip.o: common/message.h common/file.h
developers-tagger/settings_manip.o: /usr/include/ctype.h
developers-tagger/settings_manip.o: /usr/include/features.h
developers-tagger/settings_manip.o: /usr/include/stdc-predef.h
developers-tagger/settings_manip.o: /usr/include/endian.h common/ensure.h
developers-tagger/settings_manip.o: /usr/include/assert.h tagger/settings.h
developers-tagger/settings_manip.o: common/basics.h /usr/include/stdio.h
developers-tagger/settings_manip.o: /usr/include/limits.h
prob/annot.o: prob/annot.h tagger/sentence.h tagger/wordtoken.h
prob/annot.o: tagger/letter.h /usr/include/string.h /usr/include/ctype.h
prob/annot.o: /usr/include/features.h /usr/include/stdc-predef.h
prob/annot.o: /usr/include/endian.h common/basics.h /usr/include/stdio.h
prob/annot.o: common/ensure.h /usr/include/assert.h tagger/settings.h
prob/annot.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
prob/annot.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
prob/annot.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
prob/annot.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
prob/annot.o: /usr/include/xercesc/util/PlatformUtils.hpp
prob/annot.o: /usr/include/xercesc/util/XercesDefs.hpp
prob/annot.o: /usr/include/xercesc/util/Xerces_autoconf_config.hpp
prob/annot.o: /usr/include/stdint.h /usr/include/inttypes.h
prob/annot.o: /usr/include/xercesc/util/XercesVersion.hpp
prob/annot.o: /usr/include/xercesc/util/XMLException.hpp
prob/annot.o: /usr/include/xercesc/util/XMemory.hpp /usr/include/stdlib.h
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
prob/annot.o: /usr/include/xercesc/util/PlatformUtils.hpp
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
prob/annot.o: /usr/include/xercesc/sax/SAXParseException.hpp
prob/annot.o: /usr/include/xercesc/sax/SAXException.hpp
prob/annot.o: /usr/include/xercesc/sax/ErrorHandler.hpp
prob/client.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/client.o: tagger/tagdefines.h prob/probdefines.h
prob/output.o: ../misc/xmloutput.h prob/output.h prob/defines.h
prob/output.o: /usr/include/xercesc/util/PlatformUtils.hpp
prob/output.o: /usr/include/xercesc/util/XercesDefs.hpp
prob/output.o: /usr/include/xercesc/util/Xerces_autoconf_config.hpp
prob/output.o: /usr/include/stdint.h /usr/include/inttypes.h
prob/output.o: /usr/include/features.h /usr/include/stdc-predef.h
prob/output.o: /usr/include/xercesc/util/XercesVersion.hpp
prob/output.o: /usr/include/xercesc/util/XMLException.hpp
prob/output.o: /usr/include/xercesc/util/XMemory.hpp /usr/include/stdlib.h
prob/output.o: /usr/include/alloca.h
prob/output.o: /usr/include/xercesc/util/XMLExceptMsgs.hpp
prob/output.o: /usr/include/xercesc/dom/DOMError.hpp
prob/output.o: /usr/include/xercesc/util/XMLUni.hpp
prob/output.o: /usr/include/xercesc/framework/XMLErrorReporter.hpp
prob/output.o: /usr/include/xercesc/util/PanicHandler.hpp
prob/output.o: /usr/include/xercesc/util/XMLFileMgr.hpp
prob/output.o: /usr/include/xercesc/util/XMLMutexMgr.hpp
prob/pipe_client.o: /usr/include/stdio.h prob/pipe_client.h
prob/probadmin.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/probadmin.o: tagger/tagdefines.h prob/probdefines.h tagger/taglexicon.h
prob/probadmin.o: tagger/bitmap.h common/basics.h /usr/include/stdio.h
prob/probadmin.o: common/ensure.h /usr/include/assert.h
prob/probadmin.o: /usr/include/features.h /usr/include/stdc-predef.h
prob/probadmin.o: common/file.h /usr/include/ctype.h /usr/include/endian.h
prob/probadmin.o: common/message.h tagger/feature.h tagger/hasharray.h
prob/probadmin.o: /usr/include/string.h tagger/tag.h tagger/hashscatter.h
prob/probadmin.o: tagger/settings.h /usr/include/limits.h tagger/tagtrigram.h
prob/probadmin.o: tagger/token.h tagger/word.h tagger/inflectrule.h
prob/probadmin.o: tagger/wordtag.h common/stringbuf.h prob/annot.h
prob/probadmin.o: prob/config.h prob/probdevelop.h prob/output.h
prob/probadmin.o: ../misc/xmloutput.h prob/report.h prob/probinfo.h
prob/probadmin.o: prob/probmatch.h scrut/matching.h tagger/wordtoken.h
prob/probadmin.o: tagger/letter.h scrut/element.h scrut/rule.h common/timer.h
prob/probadmin.o: tagger/sentence.h
prob/probcheck.o: prob/probcheck.h scrut/scrutinizer.h scrut/matching.h
prob/probcheck.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
prob/probcheck.o: /usr/include/ctype.h /usr/include/features.h
prob/probcheck.o: /usr/include/stdc-predef.h /usr/include/endian.h
prob/probcheck.o: common/basics.h /usr/include/stdio.h common/ensure.h
prob/probcheck.o: /usr/include/assert.h tagger/settings.h
prob/probcheck.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
prob/probcheck.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
prob/probcheck.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
prob/probcheck.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
prob/probcheck.o: scrut/element.h scrut/rule.h common/timer.h
prob/probcheck.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
prob/probcheck.o: tagger/tagtrigram.h scrut/matchingset.h scrut/rules.h
prob/probcheck.o: tagger/text.h tagger/hashtable.h /usr/include/stdlib.h
prob/probcheck.o: /usr/include/alloca.h scrut/evaluatedvariable.h
prob/probcheck.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
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
prob/probinfo.o: /usr/include/ctype.h /usr/include/features.h
prob/probinfo.o: /usr/include/stdc-predef.h /usr/include/endian.h
prob/probinfo.o: common/basics.h /usr/include/stdio.h common/ensure.h
prob/probinfo.o: /usr/include/assert.h tagger/settings.h
prob/probinfo.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
prob/probinfo.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
prob/probinfo.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
prob/probinfo.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
prob/probinfo.o: scrut/element.h scrut/rule.h common/timer.h
prob/probinfo.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
prob/probinfo.o: tagger/tagtrigram.h scrut/matchingset.h scrut/rules.h
prob/probinfo.o: tagger/text.h tagger/hashtable.h /usr/include/stdlib.h
prob/probinfo.o: /usr/include/alloca.h scrut/evaluatedvariable.h
prob/probinfo.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
prob/probinfo.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
prob/probinfo.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
prob/probinfo.o: tagger/newwordlexicon.h tagger/newword.h
prob/probinfo.o: tagger/wordlexicon.h tagger/inflectlexicon.h
prob/probinfo.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
prob/probinfo.o: tagger/trigramgadget.h prob/prob.h
prob/probmatch.o: prob/probmatch.h prob/probdefines.h prob/probdevelop.h
prob/probmatch.o: prob/output.h ../misc/xmloutput.h tagger/tag.h
prob/probmatch.o: /usr/include/string.h common/basics.h /usr/include/stdio.h
prob/probmatch.o: common/ensure.h /usr/include/assert.h
prob/probmatch.o: /usr/include/features.h /usr/include/stdc-predef.h
prob/probmatch.o: tagger/bitmap.h common/file.h /usr/include/ctype.h
prob/probmatch.o: /usr/include/endian.h common/message.h tagger/feature.h
prob/probmatch.o: tagger/hashscatter.h tagger/settings.h
prob/probmatch.o: /usr/include/limits.h tagger/tagdefines.h
prob/report.o: prob/report.h prob/probinfo.h prob/probmatch.h
prob/report.o: prob/probdefines.h scrut/rule.h common/timer.h
prob/report.o: tagger/sentence.h tagger/wordtoken.h tagger/letter.h
prob/report.o: /usr/include/string.h /usr/include/ctype.h
prob/report.o: /usr/include/features.h /usr/include/stdc-predef.h
prob/report.o: /usr/include/endian.h common/basics.h /usr/include/stdio.h
prob/report.o: common/ensure.h /usr/include/assert.h tagger/settings.h
prob/report.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
prob/report.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
prob/report.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
prob/report.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
prob/report.o: scrut/element.h tagger/taglexicon.h tagger/hasharray.h
prob/report.o: tagger/tagtrigram.h prob/output.h ../misc/xmloutput.h
prob/report.o: scrut/scrutinizer.h scrut/matching.h scrut/matchingset.h
prob/report.o: scrut/rules.h tagger/text.h tagger/hashtable.h
prob/report.o: /usr/include/stdlib.h /usr/include/alloca.h
prob/report.o: scrut/evaluatedvariable.h scrut/semantictype.h
prob/report.o: scrut/operation.h scrut/expr.h scrut/rulesettings.h
prob/report.o: scrut/gramerror.h tagger/tagger.h tagger/lexicon.h
prob/report.o: tagger/morflexicon.h tagger/styleword.h
prob/report.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
prob/report.o: tagger/inflectlexicon.h tagger/wordruleterms.h
prob/report.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
prob/report.o: prob/prob.h scrut/ruleset.h prob/annot.h prob/probdevelop.h
prob/report.o: prob/probadmin.h prob/defines.h prob/config.h
prob/represent.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/represent.o: tagger/tagdefines.h prob/probdefines.h tagger/taglexicon.h
prob/represent.o: tagger/bitmap.h common/basics.h /usr/include/stdio.h
prob/represent.o: common/ensure.h /usr/include/assert.h
prob/represent.o: /usr/include/features.h /usr/include/stdc-predef.h
prob/represent.o: common/file.h /usr/include/ctype.h /usr/include/endian.h
prob/represent.o: common/message.h tagger/feature.h tagger/hasharray.h
prob/represent.o: /usr/include/string.h tagger/tag.h tagger/hashscatter.h
prob/represent.o: tagger/settings.h /usr/include/limits.h tagger/tagtrigram.h
prob/represent.o: tagger/token.h tagger/word.h tagger/inflectrule.h
prob/represent.o: tagger/wordtag.h common/stringbuf.h prob/annot.h
prob/represent.o: prob/config.h
prob/trigrams.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/trigrams.o: tagger/tagdefines.h prob/probdefines.h
scrut/element.o: scrut/rules.h /usr/include/stdio.h common/stringbuf.h
scrut/element.o: /usr/include/string.h common/basics.h common/ensure.h
scrut/element.o: /usr/include/assert.h /usr/include/features.h
scrut/element.o: /usr/include/stdc-predef.h tagger/text.h tagger/hashtable.h
scrut/element.o: /usr/include/stdlib.h /usr/include/alloca.h
scrut/element.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/element.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/element.o: /usr/include/endian.h tagger/settings.h
scrut/element.o: /usr/include/limits.h tagger/token.h tagger/word.h
scrut/element.o: tagger/hashscatter.h tagger/inflectrule.h common/message.h
scrut/element.o: tagger/tag.h tagger/bitmap.h common/file.h tagger/feature.h
scrut/element.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/element.o: scrut/rule.h common/timer.h tagger/sentence.h
scrut/element.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/element.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
scrut/element.o: scrut/rulesettings.h scrut/ruleset.h
scrut/expr.o: scrut/expr.h scrut/evaluatedvariable.h scrut/matching.h
scrut/expr.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/expr.o: /usr/include/ctype.h /usr/include/features.h
scrut/expr.o: /usr/include/stdc-predef.h /usr/include/endian.h
scrut/expr.o: common/basics.h /usr/include/stdio.h common/ensure.h
scrut/expr.o: /usr/include/assert.h tagger/settings.h /usr/include/limits.h
scrut/expr.o: common/stringbuf.h tagger/token.h tagger/word.h
scrut/expr.o: tagger/hashscatter.h tagger/inflectrule.h common/message.h
scrut/expr.o: tagger/tag.h tagger/bitmap.h common/file.h tagger/feature.h
scrut/expr.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/expr.o: scrut/rule.h common/timer.h tagger/sentence.h
scrut/expr.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/expr.o: scrut/operation.h scrut/rulesettings.h scrut/semantictype.h
scrut/expr.o: scrut/rules.h tagger/text.h tagger/hashtable.h
scrut/expr.o: /usr/include/stdlib.h /usr/include/alloca.h scrut/scrutinizer.h
scrut/expr.o: scrut/matchingset.h scrut/gramerror.h tagger/tagger.h
scrut/expr.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
scrut/expr.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
scrut/expr.o: tagger/inflectlexicon.h tagger/wordruleterms.h
scrut/expr.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
scrut/expr.o: prob/prob.h prob/memhandler.h
scrut/gramerror.o: scrut/expr.h scrut/evaluatedvariable.h scrut/matching.h
scrut/gramerror.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/gramerror.o: /usr/include/ctype.h /usr/include/features.h
scrut/gramerror.o: /usr/include/stdc-predef.h /usr/include/endian.h
scrut/gramerror.o: common/basics.h /usr/include/stdio.h common/ensure.h
scrut/gramerror.o: /usr/include/assert.h tagger/settings.h
scrut/gramerror.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
scrut/gramerror.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/gramerror.o: common/message.h tagger/tag.h tagger/bitmap.h
scrut/gramerror.o: common/file.h tagger/feature.h tagger/tagdefines.h
scrut/gramerror.o: tagger/wordtag.h scrut/element.h scrut/rule.h
scrut/gramerror.o: common/timer.h tagger/sentence.h tagger/taglexicon.h
scrut/gramerror.o: tagger/hasharray.h tagger/tagtrigram.h scrut/operation.h
scrut/gramerror.o: scrut/rulesettings.h scrut/semantictype.h
scrut/gramerror.o: scrut/gramerror.h scrut/scrutinizer.h scrut/matchingset.h
scrut/gramerror.o: scrut/rules.h tagger/text.h tagger/hashtable.h
scrut/gramerror.o: /usr/include/stdlib.h /usr/include/alloca.h
scrut/gramerror.o: tagger/tagger.h tagger/lexicon.h tagger/morflexicon.h
scrut/gramerror.o: tagger/styleword.h tagger/newwordlexicon.h
scrut/gramerror.o: tagger/newword.h tagger/wordlexicon.h
scrut/gramerror.o: tagger/inflectlexicon.h tagger/wordruleterms.h
scrut/gramerror.o: token/tokenizer.h common/FlexLexer.h
scrut/gramerror.o: tagger/trigramgadget.h prob/prob.h prob/output.h
scrut/gramerror.o: ../misc/xmloutput.h
scrut/identry.o: scrut/rules.h /usr/include/stdio.h common/stringbuf.h
scrut/identry.o: /usr/include/string.h common/basics.h common/ensure.h
scrut/identry.o: /usr/include/assert.h /usr/include/features.h
scrut/identry.o: /usr/include/stdc-predef.h tagger/text.h tagger/hashtable.h
scrut/identry.o: /usr/include/stdlib.h /usr/include/alloca.h
scrut/identry.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/identry.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/identry.o: /usr/include/endian.h tagger/settings.h
scrut/identry.o: /usr/include/limits.h tagger/token.h tagger/word.h
scrut/identry.o: tagger/hashscatter.h tagger/inflectrule.h common/message.h
scrut/identry.o: tagger/tag.h tagger/bitmap.h common/file.h tagger/feature.h
scrut/identry.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/identry.o: scrut/rule.h common/timer.h tagger/sentence.h
scrut/identry.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/identry.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
scrut/identry.o: scrut/rulesettings.h prob/memhandler.h
scrut/matching.o: scrut/rule.h common/timer.h tagger/sentence.h
scrut/matching.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/matching.o: /usr/include/ctype.h /usr/include/features.h
scrut/matching.o: /usr/include/stdc-predef.h /usr/include/endian.h
scrut/matching.o: common/basics.h /usr/include/stdio.h common/ensure.h
scrut/matching.o: /usr/include/assert.h tagger/settings.h
scrut/matching.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
scrut/matching.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/matching.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
scrut/matching.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
scrut/matching.o: scrut/element.h tagger/taglexicon.h tagger/hasharray.h
scrut/matching.o: tagger/tagtrigram.h scrut/rules.h tagger/text.h
scrut/matching.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/matching.o: /usr/include/alloca.h scrut/evaluatedvariable.h
scrut/matching.o: scrut/matching.h scrut/semantictype.h scrut/operation.h
scrut/matching.o: scrut/expr.h scrut/rulesettings.h
scrut/matchingset.o: scrut/matchingset.h scrut/matching.h tagger/wordtoken.h
scrut/matchingset.o: tagger/letter.h /usr/include/string.h
scrut/matchingset.o: /usr/include/ctype.h /usr/include/features.h
scrut/matchingset.o: /usr/include/stdc-predef.h /usr/include/endian.h
scrut/matchingset.o: common/basics.h /usr/include/stdio.h common/ensure.h
scrut/matchingset.o: /usr/include/assert.h tagger/settings.h
scrut/matchingset.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
scrut/matchingset.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/matchingset.o: common/message.h tagger/tag.h tagger/bitmap.h
scrut/matchingset.o: common/file.h tagger/feature.h tagger/tagdefines.h
scrut/matchingset.o: tagger/wordtag.h scrut/element.h scrut/rule.h
scrut/matchingset.o: common/timer.h tagger/sentence.h tagger/taglexicon.h
scrut/matchingset.o: tagger/hasharray.h tagger/tagtrigram.h scrut/gramerror.h
scrut/matchingset.o: scrut/rulesettings.h
scrut/operation.o: scrut/expr.h scrut/evaluatedvariable.h scrut/matching.h
scrut/operation.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/operation.o: /usr/include/ctype.h /usr/include/features.h
scrut/operation.o: /usr/include/stdc-predef.h /usr/include/endian.h
scrut/operation.o: common/basics.h /usr/include/stdio.h common/ensure.h
scrut/operation.o: /usr/include/assert.h tagger/settings.h
scrut/operation.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
scrut/operation.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/operation.o: common/message.h tagger/tag.h tagger/bitmap.h
scrut/operation.o: common/file.h tagger/feature.h tagger/tagdefines.h
scrut/operation.o: tagger/wordtag.h scrut/element.h scrut/rule.h
scrut/operation.o: common/timer.h tagger/sentence.h tagger/taglexicon.h
scrut/operation.o: tagger/hasharray.h tagger/tagtrigram.h scrut/operation.h
scrut/operation.o: scrut/rulesettings.h scrut/semantictype.h
scrut/regexps.o: /usr/include/stdlib.h /usr/include/alloca.h
scrut/regexps.o: /usr/include/features.h /usr/include/stdc-predef.h
scrut/regexps.o: /usr/include/stdio.h /usr/include/string.h scrut/regexp.h
scrut/rule.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
scrut/rule.o: tagger/letter.h /usr/include/string.h /usr/include/ctype.h
scrut/rule.o: /usr/include/features.h /usr/include/stdc-predef.h
scrut/rule.o: /usr/include/endian.h common/basics.h /usr/include/stdio.h
scrut/rule.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/rule.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
scrut/rule.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/rule.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
scrut/rule.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
scrut/rule.o: scrut/element.h scrut/rule.h common/timer.h tagger/sentence.h
scrut/rule.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/rule.o: scrut/matchingset.h scrut/rules.h tagger/text.h
scrut/rule.o: tagger/hashtable.h /usr/include/stdlib.h /usr/include/alloca.h
scrut/rule.o: scrut/evaluatedvariable.h scrut/semantictype.h
scrut/rule.o: scrut/operation.h scrut/expr.h scrut/rulesettings.h
scrut/rule.o: scrut/gramerror.h tagger/tagger.h tagger/lexicon.h
scrut/rule.o: tagger/morflexicon.h tagger/styleword.h tagger/newwordlexicon.h
scrut/rule.o: tagger/newword.h tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/rule.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
scrut/rule.o: tagger/trigramgadget.h prob/prob.h scrut/ruleset.h
scrut/ruleset.o: scrut/rules.h /usr/include/stdio.h common/stringbuf.h
scrut/ruleset.o: /usr/include/string.h common/basics.h common/ensure.h
scrut/ruleset.o: /usr/include/assert.h /usr/include/features.h
scrut/ruleset.o: /usr/include/stdc-predef.h tagger/text.h tagger/hashtable.h
scrut/ruleset.o: /usr/include/stdlib.h /usr/include/alloca.h
scrut/ruleset.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/ruleset.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/ruleset.o: /usr/include/endian.h tagger/settings.h
scrut/ruleset.o: /usr/include/limits.h tagger/token.h tagger/word.h
scrut/ruleset.o: tagger/hashscatter.h tagger/inflectrule.h common/message.h
scrut/ruleset.o: tagger/tag.h tagger/bitmap.h common/file.h tagger/feature.h
scrut/ruleset.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/ruleset.o: scrut/rule.h common/timer.h tagger/sentence.h
scrut/ruleset.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/ruleset.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
scrut/ruleset.o: scrut/rulesettings.h scrut/ruleset.h
scrut/rulesettings.o: scrut/rulesettings.h common/basics.h
scrut/rulesettings.o: /usr/include/stdio.h common/ensure.h
scrut/rulesettings.o: /usr/include/assert.h /usr/include/features.h
scrut/rulesettings.o: /usr/include/stdc-predef.h
scrut/scrutinizer.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
scrut/scrutinizer.o: tagger/letter.h /usr/include/string.h
scrut/scrutinizer.o: /usr/include/ctype.h /usr/include/features.h
scrut/scrutinizer.o: /usr/include/stdc-predef.h /usr/include/endian.h
scrut/scrutinizer.o: common/basics.h /usr/include/stdio.h common/ensure.h
scrut/scrutinizer.o: /usr/include/assert.h tagger/settings.h
scrut/scrutinizer.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
scrut/scrutinizer.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/scrutinizer.o: common/message.h tagger/tag.h tagger/bitmap.h
scrut/scrutinizer.o: common/file.h tagger/feature.h tagger/tagdefines.h
scrut/scrutinizer.o: tagger/wordtag.h scrut/element.h scrut/rule.h
scrut/scrutinizer.o: common/timer.h tagger/sentence.h tagger/taglexicon.h
scrut/scrutinizer.o: tagger/hasharray.h tagger/tagtrigram.h
scrut/scrutinizer.o: scrut/matchingset.h scrut/rules.h tagger/text.h
scrut/scrutinizer.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/scrutinizer.o: /usr/include/alloca.h scrut/evaluatedvariable.h
scrut/scrutinizer.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
scrut/scrutinizer.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
scrut/scrutinizer.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
scrut/scrutinizer.o: tagger/newwordlexicon.h tagger/newword.h
scrut/scrutinizer.o: tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/scrutinizer.o: tagger/wordruleterms.h token/tokenizer.h
scrut/scrutinizer.o: common/FlexLexer.h tagger/trigramgadget.h prob/prob.h
scrut/scrutinizer.o: scrut/ruleset.h ../misc/xmloutput.h stava/libstava.h
scrut/scrutinizer.o: common/utf2latin1.h prob/output.h prob/defines.h
scrut/scrutinizer.o: prob/report.h prob/probinfo.h prob/probmatch.h
scrut/scrutinizer.o: prob/probdefines.h
scrut/server_socket.o: /usr/include/arpa/inet.h /usr/include/features.h
scrut/server_socket.o: /usr/include/stdc-predef.h /usr/include/netinet/in.h
scrut/server_socket.o: /usr/include/endian.h /usr/include/netdb.h
scrut/server_socket.o: /usr/include/rpc/netdb.h /usr/include/strings.h
scrut/server_socket.o: scrut/server_socket.h scrut/socket.h
scrut/server_socket.o: /usr/include/stdio.h /usr/include/string.h
scrut/socket.o: /usr/include/netdb.h /usr/include/features.h
scrut/socket.o: /usr/include/stdc-predef.h /usr/include/netinet/in.h
scrut/socket.o: /usr/include/endian.h /usr/include/rpc/netdb.h
scrut/socket.o: /usr/include/unistd.h scrut/socket.h /usr/include/arpa/inet.h
scrut/socket.o: /usr/include/stdio.h /usr/include/string.h
scrut/web.o: /usr/include/errno.h /usr/include/features.h
scrut/web.o: /usr/include/stdc-predef.h scrut/server_socket.h scrut/socket.h
scrut/web.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
scrut/web.o: /usr/include/endian.h /usr/include/stdio.h /usr/include/string.h
scrut/web.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
scrut/web.o: tagger/letter.h /usr/include/ctype.h common/basics.h
scrut/web.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/web.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
scrut/web.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/web.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
scrut/web.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
scrut/web.o: scrut/element.h scrut/rule.h common/timer.h tagger/sentence.h
scrut/web.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/web.o: scrut/matchingset.h scrut/rules.h tagger/text.h
scrut/web.o: tagger/hashtable.h /usr/include/stdlib.h /usr/include/alloca.h
scrut/web.o: scrut/evaluatedvariable.h scrut/semantictype.h scrut/operation.h
scrut/web.o: scrut/expr.h scrut/rulesettings.h scrut/gramerror.h
scrut/web.o: tagger/tagger.h tagger/lexicon.h tagger/morflexicon.h
scrut/web.o: tagger/styleword.h tagger/newwordlexicon.h tagger/newword.h
scrut/web.o: tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/web.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
scrut/web.o: tagger/trigramgadget.h prob/prob.h prob/output.h
scrut/web.o: ../misc/xmloutput.h /usr/include/unistd.h common/utf2latin1.h
tagger/hashscatter.o: tagger/hashscatter.h tagger/letter.h
tagger/hashscatter.o: /usr/include/string.h /usr/include/ctype.h
tagger/hashscatter.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/hashscatter.o: /usr/include/endian.h common/basics.h
tagger/hashscatter.o: /usr/include/stdio.h common/ensure.h
tagger/hashscatter.o: /usr/include/assert.h
tagger/inflectlexicon.o: common/file.h /usr/include/ctype.h
tagger/inflectlexicon.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/inflectlexicon.o: /usr/include/endian.h common/ensure.h
tagger/inflectlexicon.o: /usr/include/assert.h common/message.h
tagger/inflectlexicon.o: tagger/inflectlexicon.h common/basics.h
tagger/inflectlexicon.o: /usr/include/stdio.h tagger/inflectrule.h
tagger/inflectlexicon.o: /usr/include/limits.h tagger/tag.h
tagger/inflectlexicon.o: /usr/include/string.h tagger/bitmap.h
tagger/inflectlexicon.o: tagger/feature.h tagger/hashscatter.h
tagger/inflectlexicon.o: tagger/settings.h tagger/tagdefines.h
tagger/inflectlexicon.o: tagger/letter.h tagger/taglexicon.h
tagger/inflectlexicon.o: tagger/hasharray.h tagger/tagtrigram.h
tagger/inflectlexicon.o: tagger/token.h tagger/word.h tagger/wordtag.h
tagger/inflectlexicon.o: common/stringbuf.h
tagger/inflectrule.o: tagger/letter.h /usr/include/string.h
tagger/inflectrule.o: /usr/include/ctype.h /usr/include/features.h
tagger/inflectrule.o: /usr/include/stdc-predef.h /usr/include/endian.h
tagger/inflectrule.o: common/basics.h /usr/include/stdio.h common/ensure.h
tagger/inflectrule.o: /usr/include/assert.h tagger/inflectrule.h
tagger/inflectrule.o: /usr/include/limits.h tagger/tag.h tagger/bitmap.h
tagger/inflectrule.o: common/file.h common/message.h tagger/feature.h
tagger/inflectrule.o: tagger/hashscatter.h tagger/settings.h
tagger/inflectrule.o: tagger/tagdefines.h
tagger/letter.o: tagger/letter.h /usr/include/string.h /usr/include/ctype.h
tagger/letter.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/letter.o: /usr/include/endian.h common/basics.h /usr/include/stdio.h
tagger/letter.o: common/ensure.h /usr/include/assert.h
tagger/lexicon.o: /usr/include/math.h common/file.h /usr/include/ctype.h
tagger/lexicon.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/lexicon.o: /usr/include/endian.h common/ensure.h /usr/include/assert.h
tagger/lexicon.o: common/message.h tagger/letter.h /usr/include/string.h
tagger/lexicon.o: common/basics.h /usr/include/stdio.h tagger/morf.h
tagger/lexicon.o: tagger/sentence.h tagger/wordtoken.h tagger/settings.h
tagger/lexicon.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
tagger/lexicon.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
tagger/lexicon.o: tagger/tag.h tagger/bitmap.h tagger/feature.h
tagger/lexicon.o: tagger/tagdefines.h tagger/wordtag.h tagger/lexicon.h
tagger/lexicon.o: tagger/hashtable.h /usr/include/stdlib.h
tagger/lexicon.o: /usr/include/alloca.h tagger/morflexicon.h
tagger/lexicon.o: tagger/hasharray.h tagger/taglexicon.h tagger/tagtrigram.h
tagger/lexicon.o: tagger/styleword.h tagger/newwordlexicon.h tagger/newword.h
tagger/lexicon.o: tagger/wordlexicon.h tagger/inflectlexicon.h
tagger/lexicon.o: tagger/wordruleterms.h tagger/tagger.h common/timer.h
tagger/lexicon.o: tagger/text.h token/tokenizer.h common/FlexLexer.h
tagger/lexicon.o: tagger/trigramgadget.h
tagger/main.o: common/file.h /usr/include/ctype.h /usr/include/features.h
tagger/main.o: /usr/include/stdc-predef.h /usr/include/endian.h
tagger/main.o: common/ensure.h /usr/include/assert.h common/message.h
tagger/main.o: tagger/letter.h /usr/include/string.h common/basics.h
tagger/main.o: /usr/include/stdio.h tagger/settings.h /usr/include/limits.h
tagger/main.o: tagger/tagger.h tagger/lexicon.h tagger/hashtable.h
tagger/main.o: /usr/include/stdlib.h /usr/include/alloca.h
tagger/main.o: tagger/morflexicon.h tagger/word.h tagger/hashscatter.h
tagger/main.o: tagger/inflectrule.h tagger/tag.h tagger/bitmap.h
tagger/main.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
tagger/main.o: common/stringbuf.h tagger/hasharray.h tagger/taglexicon.h
tagger/main.o: tagger/tagtrigram.h tagger/token.h tagger/styleword.h
tagger/main.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
tagger/main.o: tagger/inflectlexicon.h tagger/wordruleterms.h
tagger/main.o: tagger/sentence.h tagger/wordtoken.h common/timer.h
tagger/main.o: tagger/text.h token/tokenizer.h common/FlexLexer.h
tagger/main.o: tagger/trigramgadget.h
tagger/morflexicon.o: common/file.h /usr/include/ctype.h
tagger/morflexicon.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/morflexicon.o: /usr/include/endian.h common/ensure.h
tagger/morflexicon.o: /usr/include/assert.h common/message.h
tagger/morflexicon.o: tagger/morflexicon.h tagger/word.h
tagger/morflexicon.o: /usr/include/string.h tagger/hashscatter.h
tagger/morflexicon.o: tagger/inflectrule.h common/basics.h
tagger/morflexicon.o: /usr/include/stdio.h /usr/include/limits.h tagger/tag.h
tagger/morflexicon.o: tagger/bitmap.h tagger/feature.h tagger/settings.h
tagger/morflexicon.o: tagger/tagdefines.h tagger/wordtag.h common/stringbuf.h
tagger/morflexicon.o: tagger/hasharray.h tagger/taglexicon.h
tagger/morflexicon.o: tagger/tagtrigram.h tagger/token.h tagger/styleword.h
tagger/newwordlexicon.o: tagger/newwordlexicon.h tagger/newword.h
tagger/newwordlexicon.o: tagger/word.h /usr/include/string.h
tagger/newwordlexicon.o: tagger/hashscatter.h tagger/inflectrule.h
tagger/newwordlexicon.o: common/basics.h /usr/include/stdio.h common/ensure.h
tagger/newwordlexicon.o: /usr/include/assert.h /usr/include/features.h
tagger/newwordlexicon.o: /usr/include/stdc-predef.h /usr/include/limits.h
tagger/newwordlexicon.o: common/message.h tagger/tag.h tagger/bitmap.h
tagger/newwordlexicon.o: common/file.h /usr/include/ctype.h
tagger/newwordlexicon.o: /usr/include/endian.h tagger/feature.h
tagger/newwordlexicon.o: tagger/settings.h tagger/tagdefines.h
tagger/newwordlexicon.o: tagger/wordtag.h common/stringbuf.h
tagger/newwordlexicon.o: tagger/hashtable.h /usr/include/stdlib.h
tagger/newwordlexicon.o: /usr/include/alloca.h
tagger/sentence.o: tagger/letter.h /usr/include/string.h /usr/include/ctype.h
tagger/sentence.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/sentence.o: /usr/include/endian.h common/basics.h /usr/include/stdio.h
tagger/sentence.o: common/ensure.h /usr/include/assert.h tagger/sentence.h
tagger/sentence.o: tagger/wordtoken.h tagger/settings.h /usr/include/limits.h
tagger/sentence.o: common/stringbuf.h tagger/token.h tagger/word.h
tagger/sentence.o: tagger/hashscatter.h tagger/inflectrule.h common/message.h
tagger/sentence.o: tagger/tag.h tagger/bitmap.h common/file.h
tagger/sentence.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
tagger/sentence.o: tagger/tagger.h tagger/lexicon.h tagger/hashtable.h
tagger/sentence.o: /usr/include/stdlib.h /usr/include/alloca.h
tagger/sentence.o: tagger/morflexicon.h tagger/hasharray.h
tagger/sentence.o: tagger/taglexicon.h tagger/tagtrigram.h tagger/styleword.h
tagger/sentence.o: tagger/newwordlexicon.h tagger/newword.h
tagger/sentence.o: tagger/wordlexicon.h tagger/inflectlexicon.h
tagger/sentence.o: tagger/wordruleterms.h common/timer.h tagger/text.h
tagger/sentence.o: token/tokenizer.h common/FlexLexer.h
tagger/sentence.o: tagger/trigramgadget.h
tagger/settings.o: common/basics.h /usr/include/stdio.h common/ensure.h
tagger/settings.o: /usr/include/assert.h /usr/include/features.h
tagger/settings.o: /usr/include/stdc-predef.h common/file.h
tagger/settings.o: /usr/include/ctype.h /usr/include/endian.h
tagger/settings.o: common/message.h tagger/settings.h /usr/include/limits.h
tagger/tag.o: common/message.h tagger/tag.h /usr/include/string.h
tagger/tag.o: common/basics.h /usr/include/stdio.h common/ensure.h
tagger/tag.o: /usr/include/assert.h /usr/include/features.h
tagger/tag.o: /usr/include/stdc-predef.h tagger/bitmap.h common/file.h
tagger/tag.o: /usr/include/ctype.h /usr/include/endian.h tagger/feature.h
tagger/tag.o: tagger/hashscatter.h tagger/settings.h /usr/include/limits.h
tagger/tag.o: tagger/tagdefines.h tagger/taglexicon.h tagger/hasharray.h
tagger/tag.o: tagger/tagtrigram.h tagger/token.h tagger/word.h
tagger/tag.o: tagger/inflectrule.h tagger/wordtag.h common/stringbuf.h
tagger/tagger.o: /usr/include/math.h common/file.h /usr/include/ctype.h
tagger/tagger.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/tagger.o: /usr/include/endian.h common/ensure.h /usr/include/assert.h
tagger/tagger.o: common/message.h tagger/letter.h /usr/include/string.h
tagger/tagger.o: common/basics.h /usr/include/stdio.h tagger/morf.h
tagger/tagger.o: tagger/sentence.h tagger/wordtoken.h tagger/settings.h
tagger/tagger.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
tagger/tagger.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
tagger/tagger.o: tagger/tag.h tagger/bitmap.h tagger/feature.h
tagger/tagger.o: tagger/tagdefines.h tagger/wordtag.h tagger/tagger.h
tagger/tagger.o: tagger/lexicon.h tagger/hashtable.h /usr/include/stdlib.h
tagger/tagger.o: /usr/include/alloca.h tagger/morflexicon.h
tagger/tagger.o: tagger/hasharray.h tagger/taglexicon.h tagger/tagtrigram.h
tagger/tagger.o: tagger/styleword.h tagger/newwordlexicon.h tagger/newword.h
tagger/tagger.o: tagger/wordlexicon.h tagger/inflectlexicon.h
tagger/tagger.o: tagger/wordruleterms.h common/timer.h tagger/text.h
tagger/tagger.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
tagger/taglexicon.o: /usr/include/math.h /usr/include/string.h
tagger/taglexicon.o: /usr/include/stdlib.h /usr/include/alloca.h
tagger/taglexicon.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/taglexicon.o: common/file.h /usr/include/ctype.h /usr/include/endian.h
tagger/taglexicon.o: common/ensure.h /usr/include/assert.h common/message.h
tagger/taglexicon.o: tagger/letter.h common/basics.h /usr/include/stdio.h
tagger/taglexicon.o: tagger/settings.h /usr/include/limits.h
tagger/taglexicon.o: tagger/taglexicon.h tagger/bitmap.h tagger/feature.h
tagger/taglexicon.o: tagger/hasharray.h tagger/tag.h tagger/hashscatter.h
tagger/taglexicon.o: tagger/tagdefines.h tagger/tagtrigram.h tagger/token.h
tagger/taglexicon.o: tagger/word.h tagger/inflectrule.h tagger/wordtag.h
tagger/taglexicon.o: common/stringbuf.h
tagger/text.o: tagger/sentence.h tagger/wordtoken.h tagger/letter.h
tagger/text.o: /usr/include/string.h /usr/include/ctype.h
tagger/text.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/text.o: /usr/include/endian.h common/basics.h /usr/include/stdio.h
tagger/text.o: common/ensure.h /usr/include/assert.h tagger/settings.h
tagger/text.o: /usr/include/limits.h common/stringbuf.h tagger/token.h
tagger/text.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
tagger/text.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
tagger/text.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
tagger/text.o: tagger/text.h
tagger/word.o: /usr/include/math.h tagger/word.h /usr/include/string.h
tagger/word.o: tagger/hashscatter.h tagger/inflectrule.h common/basics.h
tagger/word.o: /usr/include/stdio.h common/ensure.h /usr/include/assert.h
tagger/word.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/word.o: /usr/include/limits.h common/message.h tagger/tag.h
tagger/word.o: tagger/bitmap.h common/file.h /usr/include/ctype.h
tagger/word.o: /usr/include/endian.h tagger/feature.h tagger/settings.h
tagger/word.o: tagger/tagdefines.h tagger/wordtag.h common/stringbuf.h
tagger/word.o: tagger/newword.h
tagger/wordlexicon.o: common/file.h /usr/include/ctype.h
tagger/wordlexicon.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/wordlexicon.o: /usr/include/endian.h common/ensure.h
tagger/wordlexicon.o: /usr/include/assert.h common/message.h tagger/letter.h
tagger/wordlexicon.o: /usr/include/string.h common/basics.h
tagger/wordlexicon.o: /usr/include/stdio.h tagger/newwordlexicon.h
tagger/wordlexicon.o: tagger/newword.h tagger/word.h tagger/hashscatter.h
tagger/wordlexicon.o: tagger/inflectrule.h /usr/include/limits.h tagger/tag.h
tagger/wordlexicon.o: tagger/bitmap.h tagger/feature.h tagger/settings.h
tagger/wordlexicon.o: tagger/tagdefines.h tagger/wordtag.h common/stringbuf.h
tagger/wordlexicon.o: tagger/hashtable.h /usr/include/stdlib.h
tagger/wordlexicon.o: /usr/include/alloca.h tagger/wordlexicon.h
tagger/wordlexicon.o: tagger/hasharray.h tagger/inflectlexicon.h
tagger/wordlexicon.o: tagger/taglexicon.h tagger/tagtrigram.h tagger/token.h
tagger/wordlexicon.o: tagger/styleword.h
tagger/wordtag.o: tagger/word.h /usr/include/string.h tagger/hashscatter.h
tagger/wordtag.o: tagger/inflectrule.h common/basics.h /usr/include/stdio.h
tagger/wordtag.o: common/ensure.h /usr/include/assert.h
tagger/wordtag.o: /usr/include/features.h /usr/include/stdc-predef.h
tagger/wordtag.o: /usr/include/limits.h common/message.h tagger/tag.h
tagger/wordtag.o: tagger/bitmap.h common/file.h /usr/include/ctype.h
tagger/wordtag.o: /usr/include/endian.h tagger/feature.h tagger/settings.h
tagger/wordtag.o: tagger/tagdefines.h tagger/wordtag.h common/stringbuf.h
tagger/wordtag.o: tagger/wordlexicon.h tagger/newword.h tagger/hasharray.h
tagger/wordtag.o: tagger/inflectlexicon.h tagger/taglexicon.h
tagger/wordtag.o: tagger/tagtrigram.h tagger/token.h tagger/styleword.h
tagger/wordtoken.o: tagger/letter.h /usr/include/string.h
tagger/wordtoken.o: /usr/include/ctype.h /usr/include/features.h
tagger/wordtoken.o: /usr/include/stdc-predef.h /usr/include/endian.h
tagger/wordtoken.o: common/basics.h /usr/include/stdio.h common/ensure.h
tagger/wordtoken.o: /usr/include/assert.h tagger/settings.h
tagger/wordtoken.o: /usr/include/limits.h tagger/token.h tagger/newword.h
tagger/wordtoken.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
tagger/wordtoken.o: common/message.h tagger/tag.h tagger/bitmap.h
tagger/wordtoken.o: common/file.h tagger/feature.h tagger/tagdefines.h
tagger/wordtoken.o: tagger/wordtag.h common/stringbuf.h tagger/wordtoken.h
token/tokenizer.o: common/FlexLexer.h tagger/token.h /usr/include/string.h
token/tokenizer.o: /usr/include/stdlib.h /usr/include/alloca.h
token/tokenizer.o: /usr/include/features.h /usr/include/stdc-predef.h
cgigranska/wwwscrutinizer.o: /usr/include/stdio.h /usr/include/stdlib.h
cgigranska/wwwscrutinizer.o: /usr/include/alloca.h /usr/include/features.h
cgigranska/wwwscrutinizer.o: /usr/include/stdc-predef.h /usr/include/string.h
cgigranska/wwwscrutinizer.o: /usr/include/time.h /usr/include/unistd.h
cgigranska/wwwscrutinizer.o: /usr/include/errno.h /usr/include/ctype.h
cgigranska/wwwscrutinizer.o: /usr/include/endian.h common/utf2latin1.h
common/utf2latin1.o: /usr/include/stdlib.h /usr/include/alloca.h
common/utf2latin1.o: /usr/include/features.h /usr/include/stdc-predef.h
common/utf2latin1.o: /usr/include/string.h common/utf2latin1.h
stava/rattstava.o: /usr/include/stdio.h /usr/include/stdlib.h
stava/rattstava.o: /usr/include/alloca.h /usr/include/features.h
stava/rattstava.o: /usr/include/stdc-predef.h /usr/include/string.h
stava/rattstava.o: stava/stava.h stava/rattstava.h stava/suffix.h
stava/stavaapi.o: /usr/include/stdio.h /usr/include/stdlib.h
stava/stavaapi.o: /usr/include/alloca.h /usr/include/features.h
stava/stavaapi.o: /usr/include/stdc-predef.h /usr/include/string.h
stava/stavaapi.o: /usr/include/ctype.h /usr/include/endian.h
stava/stavaapi.o: /usr/include/locale.h stava/stava.h stava/rattstava.h
stava/stavaapi.o: stava/suffix.h stava/libstava.h stava/stavaconstants.h
stava/suffix.o: stava/suffix.h stava/stava.h /usr/include/stdio.h
stava/suffix.o: /usr/include/stdlib.h /usr/include/alloca.h
stava/suffix.o: /usr/include/features.h /usr/include/stdc-predef.h
stava/suffix.o: /usr/include/string.h
stava/testaapi.o: /usr/include/stdio.h /usr/include/stdlib.h
stava/testaapi.o: /usr/include/alloca.h /usr/include/features.h
stava/testaapi.o: /usr/include/stdc-predef.h /usr/include/string.h
stava/testaapi.o: stava/libstava.h
