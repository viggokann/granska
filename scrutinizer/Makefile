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
XERCES = /misc/tcs/language/granska/src/xerces-c-src1_6_0
INCL = -I$(XERCES)/include
FLAGS = $(CFLAGS) -I$(CD) -I$(TD) -I$(SD) -I$(PD) -I$(STD) \
  -I$(TOD) -I.. $(INCL)
LIB_PATH = -L$(XERCES)/lib
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
WEB_LIBS = -lsocket -lnsl

ALLWWW_OBJ = $(WWW_OBJ) $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ) $(TAGGER_OBJ) $(PROBCHECK_OBJ)
ALL_OBJ = $(MAIN_OBJ) $(SCRUT_OBJ) $(COMMON_OBJ) $(STAVA_OBJ) $(TOK_OBJ)\
  $(TAGGER_OBJ) $(PROBCHECK_OBJ) $(WEB_OBJ)
ALL_FLAGS = $(FLAGS) $(COMMON_FLAGS) $(STAVA_FLAGS) $(TOK_FLAGS)\
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

TARGETS = scrutinizer
LIBS = -lxerces-c1_6_0 $(WEB_LIBS)
WWWLIBS = -lxerces-c1_6_0

all: $(TARGETS)

developers-tagg: $(DEVELOPERS_TAGG_OBJ)
	$(CPPC) $(FLAGS) -o $@ $^ $(LIB_PATH) $(LIBS) \

scrutinizer: $(ALL_OBJ)
	$(CPPC) $(ALL_FLAGS) -o scrutinizer $(ALL_OBJ) $(LIB_PATH) $(LIBS)

wwwscrutinizer: $(ALLWWW_OBJ)
	$(CPPC) $(ALL_FLAGS) -Bstatic -o wwwscrutinizer $(ALLWWW_OBJ) $(WWWLIBS)
	strip wwwscrutinizer

tagg: $(TAGG_OBJ)
	$(CPPC) $(FLAGS) -o $@ $^ $(LIB_PATH) $(LIBS)

%.o: %.cpp
	$(CPPC) $(ALL_FLAGS) -Idevelopers-tagger -c $*.cpp -o $*.o

%.o: %.c
	$(CC) $(ALL_FLAGS) -c $*.c -o $*.o

%.do: %.cpp
	$(CPPC) $(ALL_FLAGS) $(DEVELOPERS_TAGGER_FLAGS) -c $< -o $@

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

cgigranska/wwwmain.o: /usr/include/sys/types.h
cgigranska/wwwmain.o: /usr/include/sys/feature_tests.h
cgigranska/wwwmain.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
cgigranska/wwwmain.o: /usr/include/sys/machtypes.h
cgigranska/wwwmain.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
cgigranska/wwwmain.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
cgigranska/wwwmain.o: /usr/include/sys/time.h /usr/include/sys/wait.h
cgigranska/wwwmain.o: /usr/include/sys/resource.h /usr/include/sys/siginfo.h
cgigranska/wwwmain.o: /usr/include/sys/machsig.h /usr/include/sys/procset.h
cgigranska/wwwmain.o: /usr/include/sys/signal.h
cgigranska/wwwmain.o: /usr/include/sys/iso/signal_iso.h
cgigranska/wwwmain.o: /usr/include/sys/unistd.h cgigranska/wwwscrutinizer.h
cgigranska/wwwmain.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
cgigranska/wwwmain.o: tagger/letter.h /usr/include/string.h
cgigranska/wwwmain.o: /usr/include/iso/string_iso.h /usr/include/ctype.h
cgigranska/wwwmain.o: /usr/include/iso/ctype_iso.h
cgigranska/wwwmain.o: /usr/include/iso/ctype_c99.h common/basics.h
cgigranska/wwwmain.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
cgigranska/wwwmain.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
cgigranska/wwwmain.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
cgigranska/wwwmain.o: common/ensure.h /usr/include/assert.h tagger/settings.h
cgigranska/wwwmain.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
cgigranska/wwwmain.o: /usr/include/float.h common/stringbuf.h tagger/token.h
cgigranska/wwwmain.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
cgigranska/wwwmain.o: common/message.h tagger/tag.h tagger/bitmap.h
cgigranska/wwwmain.o: common/file.h tagger/feature.h tagger/tagdefines.h
cgigranska/wwwmain.o: tagger/wordtag.h scrut/element.h scrut/rule.h
cgigranska/wwwmain.o: common/timer.h tagger/sentence.h tagger/taglexicon.h
cgigranska/wwwmain.o: tagger/hasharray.h tagger/tagtrigram.h
cgigranska/wwwmain.o: scrut/matchingset.h scrut/rules.h tagger/text.h
cgigranska/wwwmain.o: tagger/hashtable.h /usr/include/stdlib.h
cgigranska/wwwmain.o: /usr/include/iso/stdlib_iso.h
cgigranska/wwwmain.o: /usr/include/iso/stdlib_c99.h scrut/evaluatedvariable.h
cgigranska/wwwmain.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
cgigranska/wwwmain.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
cgigranska/wwwmain.o: tagger/lexicon.h tagger/morflexicon.h
cgigranska/wwwmain.o: tagger/styleword.h tagger/newwordlexicon.h
cgigranska/wwwmain.o: tagger/newword.h tagger/wordlexicon.h
cgigranska/wwwmain.o: tagger/inflectlexicon.h tagger/wordruleterms.h
cgigranska/wwwmain.o: token/tokenizer.h common/FlexLexer.h
cgigranska/wwwmain.o: tagger/trigramgadget.h scrut/ruleset.h
common/file.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
common/file.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
common/file.o: /usr/include/sys/isa_defs.h /usr/include/iso/ctype_c99.h
common/file.o: /usr/include/string.h /usr/include/iso/string_iso.h
common/file.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
common/file.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
common/file.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
common/file.o: common/file.h common/ensure.h /usr/include/assert.h
common/file.o: common/message.h
common/main.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
common/main.o: tagger/letter.h /usr/include/string.h
common/main.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
common/main.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
common/main.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
common/main.o: /usr/include/iso/ctype_c99.h common/basics.h
common/main.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
common/main.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
common/main.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
common/main.o: common/ensure.h /usr/include/assert.h tagger/settings.h
common/main.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
common/main.o: /usr/include/float.h common/stringbuf.h tagger/token.h
common/main.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
common/main.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
common/main.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
common/main.o: scrut/element.h scrut/rule.h common/timer.h
common/main.o: /usr/include/sys/time.h /usr/include/sys/types.h
common/main.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
common/main.o: /usr/include/sys/select.h /usr/include/sys/time_impl.h
common/main.o: /usr/include/sys/time.h tagger/sentence.h tagger/taglexicon.h
common/main.o: tagger/hasharray.h tagger/tagtrigram.h scrut/matchingset.h
common/main.o: scrut/rules.h tagger/text.h tagger/hashtable.h
common/main.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
common/main.o: /usr/include/iso/stdlib_c99.h scrut/evaluatedvariable.h
common/main.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
common/main.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
common/main.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
common/main.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
common/main.o: tagger/inflectlexicon.h tagger/wordruleterms.h
common/main.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
common/main.o: scrut/ruleset.h prob/prob.h prob/output.h ../misc/xmloutput.h
common/main.o: prob/config.h prob/probdefines.h
common/message.o: common/message.h tagger/settings.h common/basics.h
common/message.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
common/message.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
common/message.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
common/message.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
common/message.o: /usr/include/iso/stdio_c99.h common/ensure.h
common/message.o: /usr/include/assert.h /usr/include/limits.h
common/message.o: /usr/include/iso/limits_iso.h /usr/include/float.h
common/message.o: tagger/sentence.h tagger/wordtoken.h tagger/letter.h
common/message.o: /usr/include/string.h /usr/include/iso/string_iso.h
common/message.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
common/message.o: /usr/include/iso/ctype_c99.h common/stringbuf.h
common/message.o: tagger/token.h tagger/word.h tagger/hashscatter.h
common/message.o: tagger/inflectrule.h tagger/tag.h tagger/bitmap.h
common/message.o: common/file.h tagger/feature.h tagger/tagdefines.h
common/message.o: tagger/wordtag.h
developers-tagger/developers-tagger.o: /usr/include/math.h
developers-tagger/developers-tagger.o: /usr/include/iso/math_iso.h
developers-tagger/developers-tagger.o: /usr/include/sys/feature_tests.h
developers-tagger/developers-tagger.o: /usr/include/sys/ccompile.h
developers-tagger/developers-tagger.o: /usr/include/sys/isa_defs.h
developers-tagger/developers-tagger.o: /usr/include/iso/math_c99.h
developers-tagger/developers-tagger.o: /usr/include/sys/time.h
developers-tagger/developers-tagger.o: developers-tagger/developers-tagger.h
developers-tagger/developers-tagger.o: tagger/tagger.h common/basics.h
developers-tagger/developers-tagger.o: /usr/include/stdio.h
developers-tagger/developers-tagger.o: /usr/include/iso/stdio_iso.h
developers-tagger/developers-tagger.o: /usr/include/sys/va_list.h
developers-tagger/developers-tagger.o: /usr/include/stdio_tag.h
developers-tagger/developers-tagger.o: /usr/include/stdio_impl.h
developers-tagger/developers-tagger.o: /usr/include/iso/stdio_c99.h
developers-tagger/developers-tagger.o: common/ensure.h /usr/include/assert.h
developers-tagger/developers-tagger.o: tagger/lexicon.h tagger/hashtable.h
developers-tagger/developers-tagger.o: /usr/include/string.h
developers-tagger/developers-tagger.o: /usr/include/iso/string_iso.h
developers-tagger/developers-tagger.o: /usr/include/stdlib.h
developers-tagger/developers-tagger.o: /usr/include/iso/stdlib_iso.h
developers-tagger/developers-tagger.o: /usr/include/iso/stdlib_c99.h
developers-tagger/developers-tagger.o: tagger/morflexicon.h tagger/word.h
developers-tagger/developers-tagger.o: tagger/hashscatter.h
developers-tagger/developers-tagger.o: tagger/inflectrule.h
developers-tagger/developers-tagger.o: /usr/include/limits.h
developers-tagger/developers-tagger.o: /usr/include/iso/limits_iso.h
developers-tagger/developers-tagger.o: common/message.h tagger/tag.h
developers-tagger/developers-tagger.o: tagger/bitmap.h common/file.h
developers-tagger/developers-tagger.o: /usr/include/ctype.h
developers-tagger/developers-tagger.o: /usr/include/iso/ctype_iso.h
developers-tagger/developers-tagger.o: /usr/include/iso/ctype_c99.h
developers-tagger/developers-tagger.o: tagger/feature.h tagger/settings.h
developers-tagger/developers-tagger.o: /usr/include/float.h
developers-tagger/developers-tagger.o: tagger/tagdefines.h tagger/wordtag.h
developers-tagger/developers-tagger.o: common/stringbuf.h tagger/hasharray.h
developers-tagger/developers-tagger.o: tagger/taglexicon.h
developers-tagger/developers-tagger.o: tagger/tagtrigram.h tagger/token.h
developers-tagger/developers-tagger.o: tagger/styleword.h
developers-tagger/developers-tagger.o: tagger/newwordlexicon.h
developers-tagger/developers-tagger.o: tagger/newword.h tagger/wordlexicon.h
developers-tagger/developers-tagger.o: tagger/inflectlexicon.h
developers-tagger/developers-tagger.o: tagger/wordruleterms.h
developers-tagger/developers-tagger.o: tagger/sentence.h tagger/wordtoken.h
developers-tagger/developers-tagger.o: tagger/letter.h common/timer.h
developers-tagger/developers-tagger.o: /usr/include/sys/time.h
developers-tagger/developers-tagger.o: /usr/include/sys/types.h
developers-tagger/developers-tagger.o: /usr/include/sys/machtypes.h
developers-tagger/developers-tagger.o: /usr/include/sys/int_types.h
developers-tagger/developers-tagger.o: /usr/include/sys/select.h
developers-tagger/developers-tagger.o: /usr/include/sys/time_impl.h
developers-tagger/developers-tagger.o: tagger/text.h token/tokenizer.h
developers-tagger/developers-tagger.o: common/FlexLexer.h
developers-tagger/developers-tagger.o: tagger/trigramgadget.h tagger/morf.h
developers-tagger/developers-tagger.o: developers-tagger/settings_manip.h
developers-tagger/developers-tagger.o: tagger/heap.h
developers-tagger/main.o: tagger/settings.h common/basics.h
developers-tagger/main.o: /usr/include/stdio.h
developers-tagger/main.o: /usr/include/sys/feature_tests.h
developers-tagger/main.o: /usr/include/sys/ccompile.h
developers-tagger/main.o: /usr/include/sys/isa_defs.h
developers-tagger/main.o: /usr/include/iso/stdio_iso.h
developers-tagger/main.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
developers-tagger/main.o: /usr/include/stdio_impl.h
developers-tagger/main.o: /usr/include/iso/stdio_c99.h common/ensure.h
developers-tagger/main.o: /usr/include/assert.h /usr/include/limits.h
developers-tagger/main.o: /usr/include/iso/limits_iso.h /usr/include/float.h
developers-tagger/main.o: common/file.h /usr/include/ctype.h
developers-tagger/main.o: /usr/include/iso/ctype_iso.h
developers-tagger/main.o: /usr/include/iso/ctype_c99.h common/message.h
developers-tagger/main.o: tagger/letter.h /usr/include/string.h
developers-tagger/main.o: /usr/include/iso/string_iso.h
developers-tagger/main.o: developers-tagger/developers-tagger.h
developers-tagger/main.o: tagger/tagger.h tagger/lexicon.h tagger/hashtable.h
developers-tagger/main.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
developers-tagger/main.o: /usr/include/iso/stdlib_c99.h tagger/morflexicon.h
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
developers-tagger/main.o: /usr/include/sys/time.h /usr/include/sys/types.h
developers-tagger/main.o: /usr/include/sys/machtypes.h
developers-tagger/main.o: /usr/include/sys/int_types.h
developers-tagger/main.o: /usr/include/sys/select.h
developers-tagger/main.o: /usr/include/sys/time_impl.h
developers-tagger/main.o: /usr/include/sys/time.h tagger/text.h
developers-tagger/main.o: token/tokenizer.h common/FlexLexer.h
developers-tagger/main.o: tagger/trigramgadget.h
developers-tagger/main.o: developers-tagger/settings_manip.h
developers-tagger/settings_manip.o: developers-tagger/settings_manip.h
developers-tagger/settings_manip.o: common/message.h common/file.h
developers-tagger/settings_manip.o: /usr/include/ctype.h
developers-tagger/settings_manip.o: /usr/include/iso/ctype_iso.h
developers-tagger/settings_manip.o: /usr/include/sys/feature_tests.h
developers-tagger/settings_manip.o: /usr/include/sys/ccompile.h
developers-tagger/settings_manip.o: /usr/include/sys/isa_defs.h
developers-tagger/settings_manip.o: /usr/include/iso/ctype_c99.h
developers-tagger/settings_manip.o: common/ensure.h /usr/include/assert.h
developers-tagger/settings_manip.o: tagger/settings.h common/basics.h
developers-tagger/settings_manip.o: /usr/include/stdio.h
developers-tagger/settings_manip.o: /usr/include/iso/stdio_iso.h
developers-tagger/settings_manip.o: /usr/include/sys/va_list.h
developers-tagger/settings_manip.o: /usr/include/stdio_tag.h
developers-tagger/settings_manip.o: /usr/include/stdio_impl.h
developers-tagger/settings_manip.o: /usr/include/iso/stdio_c99.h
developers-tagger/settings_manip.o: /usr/include/limits.h
developers-tagger/settings_manip.o: /usr/include/iso/limits_iso.h
developers-tagger/settings_manip.o: /usr/include/float.h
prob/annot.o: prob/annot.h tagger/sentence.h tagger/wordtoken.h
prob/annot.o: tagger/letter.h /usr/include/string.h
prob/annot.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
prob/annot.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
prob/annot.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
prob/annot.o: /usr/include/iso/ctype_c99.h common/basics.h
prob/annot.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
prob/annot.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
prob/annot.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
prob/annot.o: common/ensure.h /usr/include/assert.h tagger/settings.h
prob/annot.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
prob/annot.o: /usr/include/float.h common/stringbuf.h tagger/token.h
prob/annot.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
prob/annot.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
prob/annot.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/PlatformUtils.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XercesDefs.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/AutoSense.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLException.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLExceptMsgs.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLUni.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLErrorReporter.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLMsgLoader.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/parsers/DOMParser.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Document.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_DocumentType.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Node.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOMString.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_DOMImplementation.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Element.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_DocumentFragment.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Comment.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_CharacterData.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_CDATASection.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Text.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_ProcessingInstruction.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Attr.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Entity.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_EntityReference.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_NodeList.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Notation.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_NodeIterator.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_NodeFilter.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Node.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_TreeWalker.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_XMLDecl.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_Range.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLDocumentHandler.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/RefVectorOf.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/ArrayIndexOutOfBoundsException.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLEnumerator.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/RefVectorOf.c
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLAttr.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLString.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/QName.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLUniDefs.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLAttDef.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLEntityHandler.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/ValueStackOf.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/EmptyStackException.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/ValueVectorOf.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/ValueVectorOf.c
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/ValueStackOf.c
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/validators/DTD/DocTypeHandler.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLNotationDecl.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/validators/DTD/DTDAttDef.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/validators/DTD/DTDElementDecl.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/RefHashTableOf.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/KeyValuePair.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/KeyValuePair.c
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/HashBase.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/IllegalArgumentException.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/NoSuchElementException.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/RuntimeException.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/HashXMLCh.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/RefHashTableOf.c
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/NullPointerException.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLElementDecl.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLAttDefList.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLContentModel.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/validators/DTD/DTDEntityDecl.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLEntityDecl.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/dom/DOM_NamedNodeMap.hpp
prob/annot.o: prob/domerror.h
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/sax/SAXParseException.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/sax/SAXException.hpp
prob/annot.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/sax/ErrorHandler.hpp
prob/client.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/client.o: tagger/tagdefines.h prob/probdefines.h
prob/output.o: ../misc/xmloutput.h prob/output.h prob/defines.h
prob/output.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/PlatformUtils.hpp
prob/output.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XercesDefs.hpp
prob/output.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/AutoSense.hpp
prob/output.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLException.hpp
prob/output.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLExceptMsgs.hpp
prob/output.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLUni.hpp
prob/output.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/framework/XMLErrorReporter.hpp
prob/output.o: /misc/tcs/language/granska/src/xerces-c-src1_6_0/include/util/XMLMsgLoader.hpp
prob/pipe_client.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
prob/pipe_client.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
prob/pipe_client.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
prob/pipe_client.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
prob/pipe_client.o: /usr/include/iso/stdio_c99.h prob/pipe_client.h
prob/probadmin.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/probadmin.o: tagger/tagdefines.h prob/probdefines.h tagger/taglexicon.h
prob/probadmin.o: tagger/bitmap.h common/basics.h /usr/include/stdio.h
prob/probadmin.o: /usr/include/sys/feature_tests.h
prob/probadmin.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
prob/probadmin.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
prob/probadmin.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
prob/probadmin.o: /usr/include/iso/stdio_c99.h common/ensure.h
prob/probadmin.o: /usr/include/assert.h common/file.h /usr/include/ctype.h
prob/probadmin.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
prob/probadmin.o: common/message.h tagger/feature.h tagger/hasharray.h
prob/probadmin.o: tagger/tag.h /usr/include/string.h
prob/probadmin.o: /usr/include/iso/string_iso.h tagger/hashscatter.h
prob/probadmin.o: tagger/settings.h /usr/include/limits.h
prob/probadmin.o: /usr/include/iso/limits_iso.h /usr/include/float.h
prob/probadmin.o: tagger/tagtrigram.h tagger/token.h tagger/word.h
prob/probadmin.o: tagger/inflectrule.h tagger/wordtag.h common/stringbuf.h
prob/probadmin.o: prob/annot.h prob/config.h prob/probdevelop.h prob/output.h
prob/probadmin.o: ../misc/xmloutput.h prob/report.h prob/probinfo.h
prob/probadmin.o: prob/probmatch.h scrut/matching.h tagger/wordtoken.h
prob/probadmin.o: tagger/letter.h scrut/element.h scrut/rule.h common/timer.h
prob/probadmin.o: /usr/include/sys/time.h /usr/include/sys/types.h
prob/probadmin.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
prob/probadmin.o: /usr/include/sys/select.h /usr/include/sys/time_impl.h
prob/probadmin.o: /usr/include/sys/time.h tagger/sentence.h
prob/probcheck.o: prob/probcheck.h scrut/scrutinizer.h scrut/matching.h
prob/probcheck.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
prob/probcheck.o: /usr/include/iso/string_iso.h
prob/probcheck.o: /usr/include/sys/feature_tests.h
prob/probcheck.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
prob/probcheck.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
prob/probcheck.o: /usr/include/iso/ctype_c99.h common/basics.h
prob/probcheck.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
prob/probcheck.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
prob/probcheck.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
prob/probcheck.o: common/ensure.h /usr/include/assert.h tagger/settings.h
prob/probcheck.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
prob/probcheck.o: /usr/include/float.h common/stringbuf.h tagger/token.h
prob/probcheck.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
prob/probcheck.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
prob/probcheck.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
prob/probcheck.o: scrut/element.h scrut/rule.h common/timer.h
prob/probcheck.o: /usr/include/sys/time.h /usr/include/sys/types.h
prob/probcheck.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
prob/probcheck.o: /usr/include/sys/select.h /usr/include/sys/time_impl.h
prob/probcheck.o: /usr/include/sys/time.h tagger/sentence.h
prob/probcheck.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
prob/probcheck.o: scrut/matchingset.h scrut/rules.h tagger/text.h
prob/probcheck.o: tagger/hashtable.h /usr/include/stdlib.h
prob/probcheck.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
prob/probcheck.o: scrut/evaluatedvariable.h scrut/semantictype.h
prob/probcheck.o: scrut/operation.h scrut/expr.h scrut/rulesettings.h
prob/probcheck.o: scrut/gramerror.h tagger/tagger.h tagger/lexicon.h
prob/probcheck.o: tagger/morflexicon.h tagger/styleword.h
prob/probcheck.o: tagger/newwordlexicon.h tagger/newword.h
prob/probcheck.o: tagger/wordlexicon.h tagger/inflectlexicon.h
prob/probcheck.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
prob/probcheck.o: tagger/trigramgadget.h prob/defines.h prob/config.h
prob/probcheck.o: prob/probdefines.h prob/annot.h prob/report.h
prob/probcheck.o: prob/probinfo.h prob/probmatch.h prob/probadmin.h
prob/probcheck.o: prob/prob.h prob/probdevelop.h prob/output.h
prob/probcheck.o: ../misc/xmloutput.h
prob/probdevelop.o: prob/probdevelop.h prob/output.h ../misc/xmloutput.h
prob/probdevelop.o: prob/probdefines.h
prob/probinfo.o: prob/probinfo.h scrut/scrutinizer.h scrut/matching.h
prob/probinfo.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
prob/probinfo.o: /usr/include/iso/string_iso.h
prob/probinfo.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
prob/probinfo.o: /usr/include/sys/isa_defs.h /usr/include/ctype.h
prob/probinfo.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
prob/probinfo.o: common/basics.h /usr/include/stdio.h
prob/probinfo.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
prob/probinfo.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
prob/probinfo.o: /usr/include/iso/stdio_c99.h common/ensure.h
prob/probinfo.o: /usr/include/assert.h tagger/settings.h
prob/probinfo.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
prob/probinfo.o: /usr/include/float.h common/stringbuf.h tagger/token.h
prob/probinfo.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
prob/probinfo.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
prob/probinfo.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
prob/probinfo.o: scrut/element.h scrut/rule.h common/timer.h
prob/probinfo.o: /usr/include/sys/time.h /usr/include/sys/types.h
prob/probinfo.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
prob/probinfo.o: /usr/include/sys/select.h /usr/include/sys/time_impl.h
prob/probinfo.o: /usr/include/sys/time.h tagger/sentence.h
prob/probinfo.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
prob/probinfo.o: scrut/matchingset.h scrut/rules.h tagger/text.h
prob/probinfo.o: tagger/hashtable.h /usr/include/stdlib.h
prob/probinfo.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
prob/probinfo.o: scrut/evaluatedvariable.h scrut/semantictype.h
prob/probinfo.o: scrut/operation.h scrut/expr.h scrut/rulesettings.h
prob/probinfo.o: scrut/gramerror.h tagger/tagger.h tagger/lexicon.h
prob/probinfo.o: tagger/morflexicon.h tagger/styleword.h
prob/probinfo.o: tagger/newwordlexicon.h tagger/newword.h
prob/probinfo.o: tagger/wordlexicon.h tagger/inflectlexicon.h
prob/probinfo.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
prob/probinfo.o: tagger/trigramgadget.h
prob/probmatch.o: prob/probmatch.h prob/probdefines.h prob/probdevelop.h
prob/probmatch.o: prob/output.h ../misc/xmloutput.h tagger/tag.h
prob/probmatch.o: /usr/include/string.h /usr/include/iso/string_iso.h
prob/probmatch.o: /usr/include/sys/feature_tests.h
prob/probmatch.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
prob/probmatch.o: common/basics.h /usr/include/stdio.h
prob/probmatch.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
prob/probmatch.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
prob/probmatch.o: /usr/include/iso/stdio_c99.h common/ensure.h
prob/probmatch.o: /usr/include/assert.h tagger/bitmap.h common/file.h
prob/probmatch.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
prob/probmatch.o: /usr/include/iso/ctype_c99.h common/message.h
prob/probmatch.o: tagger/feature.h tagger/hashscatter.h tagger/settings.h
prob/probmatch.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
prob/probmatch.o: /usr/include/float.h tagger/tagdefines.h
prob/report.o: prob/report.h prob/probinfo.h prob/probmatch.h
prob/report.o: prob/probdefines.h scrut/rule.h common/timer.h
prob/report.o: /usr/include/sys/time.h /usr/include/sys/feature_tests.h
prob/report.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
prob/report.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
prob/report.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
prob/report.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
prob/report.o: tagger/sentence.h tagger/wordtoken.h tagger/letter.h
prob/report.o: /usr/include/string.h /usr/include/iso/string_iso.h
prob/report.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
prob/report.o: /usr/include/iso/ctype_c99.h common/basics.h
prob/report.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
prob/report.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
prob/report.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
prob/report.o: common/ensure.h /usr/include/assert.h tagger/settings.h
prob/report.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
prob/report.o: /usr/include/float.h common/stringbuf.h tagger/token.h
prob/report.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
prob/report.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
prob/report.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
prob/report.o: scrut/element.h tagger/taglexicon.h tagger/hasharray.h
prob/report.o: tagger/tagtrigram.h prob/output.h ../misc/xmloutput.h
prob/report.o: scrut/scrutinizer.h scrut/matching.h scrut/matchingset.h
prob/report.o: scrut/rules.h tagger/text.h tagger/hashtable.h
prob/report.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
prob/report.o: /usr/include/iso/stdlib_c99.h scrut/evaluatedvariable.h
prob/report.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
prob/report.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
prob/report.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
prob/report.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
prob/report.o: tagger/inflectlexicon.h tagger/wordruleterms.h
prob/report.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
prob/report.o: scrut/ruleset.h prob/annot.h prob/probdevelop.h
prob/report.o: prob/probadmin.h prob/prob.h prob/defines.h prob/config.h
prob/represent.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/represent.o: tagger/tagdefines.h prob/probdefines.h tagger/taglexicon.h
prob/represent.o: tagger/bitmap.h common/basics.h /usr/include/stdio.h
prob/represent.o: /usr/include/sys/feature_tests.h
prob/represent.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
prob/represent.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
prob/represent.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
prob/represent.o: /usr/include/iso/stdio_c99.h common/ensure.h
prob/represent.o: /usr/include/assert.h common/file.h /usr/include/ctype.h
prob/represent.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
prob/represent.o: common/message.h tagger/feature.h tagger/hasharray.h
prob/represent.o: tagger/tag.h /usr/include/string.h
prob/represent.o: /usr/include/iso/string_iso.h tagger/hashscatter.h
prob/represent.o: tagger/settings.h /usr/include/limits.h
prob/represent.o: /usr/include/iso/limits_iso.h /usr/include/float.h
prob/represent.o: tagger/tagtrigram.h tagger/token.h tagger/word.h
prob/represent.o: tagger/inflectrule.h tagger/wordtag.h common/stringbuf.h
prob/represent.o: prob/annot.h prob/config.h
prob/trigrams.o: prob/probadmin.h prob/prob.h prob/defines.h
prob/trigrams.o: tagger/tagdefines.h prob/probdefines.h
scrut/element.o: scrut/rules.h /usr/include/stdio.h
scrut/element.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
scrut/element.o: /usr/include/sys/isa_defs.h /usr/include/iso/stdio_iso.h
scrut/element.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/element.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/element.o: common/stringbuf.h /usr/include/string.h
scrut/element.o: /usr/include/iso/string_iso.h common/basics.h
scrut/element.o: common/ensure.h /usr/include/assert.h tagger/text.h
scrut/element.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/element.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
scrut/element.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/element.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/element.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
scrut/element.o: tagger/settings.h /usr/include/limits.h
scrut/element.o: /usr/include/iso/limits_iso.h /usr/include/float.h
scrut/element.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/element.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/element.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/element.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/element.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/element.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/element.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/element.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/element.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/element.o: tagger/tagtrigram.h scrut/semantictype.h scrut/operation.h
scrut/element.o: scrut/expr.h scrut/rulesettings.h scrut/ruleset.h
scrut/expr.o: scrut/expr.h scrut/evaluatedvariable.h scrut/matching.h
scrut/expr.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/expr.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
scrut/expr.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
scrut/expr.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
scrut/expr.o: /usr/include/iso/ctype_c99.h common/basics.h
scrut/expr.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
scrut/expr.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/expr.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/expr.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/expr.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
scrut/expr.o: /usr/include/float.h common/stringbuf.h tagger/token.h
scrut/expr.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/expr.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
scrut/expr.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
scrut/expr.o: scrut/element.h scrut/rule.h common/timer.h
scrut/expr.o: /usr/include/sys/time.h /usr/include/sys/types.h
scrut/expr.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
scrut/expr.o: /usr/include/sys/select.h /usr/include/sys/time_impl.h
scrut/expr.o: /usr/include/sys/time.h tagger/sentence.h tagger/taglexicon.h
scrut/expr.o: tagger/hasharray.h tagger/tagtrigram.h scrut/operation.h
scrut/expr.o: scrut/rulesettings.h scrut/semantictype.h scrut/rules.h
scrut/expr.o: tagger/text.h tagger/hashtable.h /usr/include/stdlib.h
scrut/expr.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
scrut/expr.o: scrut/scrutinizer.h scrut/matchingset.h scrut/gramerror.h
scrut/expr.o: tagger/tagger.h tagger/lexicon.h tagger/morflexicon.h
scrut/expr.o: tagger/styleword.h tagger/newwordlexicon.h tagger/newword.h
scrut/expr.o: tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/expr.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
scrut/expr.o: tagger/trigramgadget.h prob/memhandler.h
scrut/gramerror.o: scrut/expr.h scrut/evaluatedvariable.h scrut/matching.h
scrut/gramerror.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/gramerror.o: /usr/include/iso/string_iso.h
scrut/gramerror.o: /usr/include/sys/feature_tests.h
scrut/gramerror.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
scrut/gramerror.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
scrut/gramerror.o: /usr/include/iso/ctype_c99.h common/basics.h
scrut/gramerror.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
scrut/gramerror.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/gramerror.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/gramerror.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/gramerror.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
scrut/gramerror.o: /usr/include/float.h common/stringbuf.h tagger/token.h
scrut/gramerror.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/gramerror.o: common/message.h tagger/tag.h tagger/bitmap.h
scrut/gramerror.o: common/file.h tagger/feature.h tagger/tagdefines.h
scrut/gramerror.o: tagger/wordtag.h scrut/element.h scrut/rule.h
scrut/gramerror.o: common/timer.h /usr/include/sys/time.h
scrut/gramerror.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/gramerror.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/gramerror.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/gramerror.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/gramerror.o: tagger/tagtrigram.h scrut/operation.h scrut/rulesettings.h
scrut/gramerror.o: scrut/semantictype.h scrut/gramerror.h scrut/scrutinizer.h
scrut/gramerror.o: scrut/matchingset.h scrut/rules.h tagger/text.h
scrut/gramerror.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/gramerror.o: /usr/include/iso/stdlib_iso.h
scrut/gramerror.o: /usr/include/iso/stdlib_c99.h tagger/tagger.h
scrut/gramerror.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
scrut/gramerror.o: tagger/newwordlexicon.h tagger/newword.h
scrut/gramerror.o: tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/gramerror.o: tagger/wordruleterms.h token/tokenizer.h
scrut/gramerror.o: common/FlexLexer.h tagger/trigramgadget.h prob/prob.h
scrut/gramerror.o: prob/output.h ../misc/xmloutput.h
scrut/identry.o: scrut/rules.h /usr/include/stdio.h
scrut/identry.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
scrut/identry.o: /usr/include/sys/isa_defs.h /usr/include/iso/stdio_iso.h
scrut/identry.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/identry.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/identry.o: common/stringbuf.h /usr/include/string.h
scrut/identry.o: /usr/include/iso/string_iso.h common/basics.h
scrut/identry.o: common/ensure.h /usr/include/assert.h tagger/text.h
scrut/identry.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/identry.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
scrut/identry.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/identry.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/identry.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
scrut/identry.o: tagger/settings.h /usr/include/limits.h
scrut/identry.o: /usr/include/iso/limits_iso.h /usr/include/float.h
scrut/identry.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/identry.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/identry.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/identry.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/identry.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/identry.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/identry.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/identry.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/identry.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/identry.o: tagger/tagtrigram.h scrut/semantictype.h scrut/operation.h
scrut/identry.o: scrut/expr.h scrut/rulesettings.h prob/memhandler.h
scrut/matching.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/matching.o: /usr/include/sys/feature_tests.h
scrut/matching.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
scrut/matching.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/matching.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/matching.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/matching.o: tagger/sentence.h tagger/wordtoken.h tagger/letter.h
scrut/matching.o: /usr/include/string.h /usr/include/iso/string_iso.h
scrut/matching.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
scrut/matching.o: /usr/include/iso/ctype_c99.h common/basics.h
scrut/matching.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
scrut/matching.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/matching.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/matching.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/matching.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
scrut/matching.o: /usr/include/float.h common/stringbuf.h tagger/token.h
scrut/matching.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/matching.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
scrut/matching.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
scrut/matching.o: scrut/element.h tagger/taglexicon.h tagger/hasharray.h
scrut/matching.o: tagger/tagtrigram.h scrut/rules.h tagger/text.h
scrut/matching.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/matching.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
scrut/matching.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/matching.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
scrut/matching.o: scrut/rulesettings.h
scrut/matchingset.o: scrut/matchingset.h scrut/matching.h tagger/wordtoken.h
scrut/matchingset.o: tagger/letter.h /usr/include/string.h
scrut/matchingset.o: /usr/include/iso/string_iso.h
scrut/matchingset.o: /usr/include/sys/feature_tests.h
scrut/matchingset.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
scrut/matchingset.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
scrut/matchingset.o: /usr/include/iso/ctype_c99.h common/basics.h
scrut/matchingset.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
scrut/matchingset.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/matchingset.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/matchingset.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/matchingset.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
scrut/matchingset.o: /usr/include/float.h common/stringbuf.h tagger/token.h
scrut/matchingset.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/matchingset.o: common/message.h tagger/tag.h tagger/bitmap.h
scrut/matchingset.o: common/file.h tagger/feature.h tagger/tagdefines.h
scrut/matchingset.o: tagger/wordtag.h scrut/element.h scrut/rule.h
scrut/matchingset.o: common/timer.h /usr/include/sys/time.h
scrut/matchingset.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/matchingset.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/matchingset.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/matchingset.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/matchingset.o: tagger/tagtrigram.h scrut/gramerror.h
scrut/matchingset.o: scrut/rulesettings.h
scrut/operation.o: scrut/expr.h scrut/evaluatedvariable.h scrut/matching.h
scrut/operation.o: tagger/wordtoken.h tagger/letter.h /usr/include/string.h
scrut/operation.o: /usr/include/iso/string_iso.h
scrut/operation.o: /usr/include/sys/feature_tests.h
scrut/operation.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
scrut/operation.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
scrut/operation.o: /usr/include/iso/ctype_c99.h common/basics.h
scrut/operation.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
scrut/operation.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/operation.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/operation.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/operation.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
scrut/operation.o: /usr/include/float.h common/stringbuf.h tagger/token.h
scrut/operation.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/operation.o: common/message.h tagger/tag.h tagger/bitmap.h
scrut/operation.o: common/file.h tagger/feature.h tagger/tagdefines.h
scrut/operation.o: tagger/wordtag.h scrut/element.h scrut/rule.h
scrut/operation.o: common/timer.h /usr/include/sys/time.h
scrut/operation.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/operation.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/operation.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/operation.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/operation.o: tagger/tagtrigram.h scrut/operation.h scrut/rulesettings.h
scrut/operation.o: scrut/semantictype.h
scrut/regexps.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
scrut/regexps.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
scrut/regexps.o: /usr/include/sys/isa_defs.h /usr/include/iso/stdlib_c99.h
scrut/regexps.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
scrut/regexps.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/regexps.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/regexps.o: /usr/include/string.h /usr/include/iso/string_iso.h
scrut/regexps.o: scrut/regexp.h
scrut/rule.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
scrut/rule.o: tagger/letter.h /usr/include/string.h
scrut/rule.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
scrut/rule.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
scrut/rule.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
scrut/rule.o: /usr/include/iso/ctype_c99.h common/basics.h
scrut/rule.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
scrut/rule.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/rule.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/rule.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/rule.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
scrut/rule.o: /usr/include/float.h common/stringbuf.h tagger/token.h
scrut/rule.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/rule.o: common/message.h tagger/tag.h tagger/bitmap.h common/file.h
scrut/rule.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
scrut/rule.o: scrut/element.h scrut/rule.h common/timer.h
scrut/rule.o: /usr/include/sys/time.h /usr/include/sys/types.h
scrut/rule.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
scrut/rule.o: /usr/include/sys/select.h /usr/include/sys/time_impl.h
scrut/rule.o: /usr/include/sys/time.h tagger/sentence.h tagger/taglexicon.h
scrut/rule.o: tagger/hasharray.h tagger/tagtrigram.h scrut/matchingset.h
scrut/rule.o: scrut/rules.h tagger/text.h tagger/hashtable.h
scrut/rule.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
scrut/rule.o: /usr/include/iso/stdlib_c99.h scrut/evaluatedvariable.h
scrut/rule.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
scrut/rule.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
scrut/rule.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
scrut/rule.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
scrut/rule.o: tagger/inflectlexicon.h tagger/wordruleterms.h
scrut/rule.o: token/tokenizer.h common/FlexLexer.h tagger/trigramgadget.h
scrut/rule.o: scrut/ruleset.h
scrut/ruleset.o: scrut/rules.h /usr/include/stdio.h
scrut/ruleset.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
scrut/ruleset.o: /usr/include/sys/isa_defs.h /usr/include/iso/stdio_iso.h
scrut/ruleset.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/ruleset.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/ruleset.o: common/stringbuf.h /usr/include/string.h
scrut/ruleset.o: /usr/include/iso/string_iso.h common/basics.h
scrut/ruleset.o: common/ensure.h /usr/include/assert.h tagger/text.h
scrut/ruleset.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/ruleset.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
scrut/ruleset.o: scrut/evaluatedvariable.h scrut/matching.h
scrut/ruleset.o: tagger/wordtoken.h tagger/letter.h /usr/include/ctype.h
scrut/ruleset.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
scrut/ruleset.o: tagger/settings.h /usr/include/limits.h
scrut/ruleset.o: /usr/include/iso/limits_iso.h /usr/include/float.h
scrut/ruleset.o: tagger/token.h tagger/word.h tagger/hashscatter.h
scrut/ruleset.o: tagger/inflectrule.h common/message.h tagger/tag.h
scrut/ruleset.o: tagger/bitmap.h common/file.h tagger/feature.h
scrut/ruleset.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/ruleset.o: scrut/rule.h common/timer.h /usr/include/sys/time.h
scrut/ruleset.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/ruleset.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/ruleset.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/ruleset.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/ruleset.o: tagger/tagtrigram.h scrut/semantictype.h scrut/operation.h
scrut/ruleset.o: scrut/expr.h scrut/rulesettings.h scrut/ruleset.h
scrut/rulesettings.o: scrut/rulesettings.h common/basics.h
scrut/rulesettings.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
scrut/rulesettings.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
scrut/rulesettings.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
scrut/rulesettings.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
scrut/rulesettings.o: /usr/include/iso/stdio_c99.h common/ensure.h
scrut/rulesettings.o: /usr/include/assert.h
scrut/scrutinizer.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
scrut/scrutinizer.o: tagger/letter.h /usr/include/string.h
scrut/scrutinizer.o: /usr/include/iso/string_iso.h
scrut/scrutinizer.o: /usr/include/sys/feature_tests.h
scrut/scrutinizer.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
scrut/scrutinizer.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
scrut/scrutinizer.o: /usr/include/iso/ctype_c99.h common/basics.h
scrut/scrutinizer.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
scrut/scrutinizer.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
scrut/scrutinizer.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
scrut/scrutinizer.o: common/ensure.h /usr/include/assert.h tagger/settings.h
scrut/scrutinizer.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
scrut/scrutinizer.o: /usr/include/float.h common/stringbuf.h tagger/token.h
scrut/scrutinizer.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
scrut/scrutinizer.o: common/message.h tagger/tag.h tagger/bitmap.h
scrut/scrutinizer.o: common/file.h tagger/feature.h tagger/tagdefines.h
scrut/scrutinizer.o: tagger/wordtag.h scrut/element.h scrut/rule.h
scrut/scrutinizer.o: common/timer.h /usr/include/sys/time.h
scrut/scrutinizer.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/scrutinizer.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/scrutinizer.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/scrutinizer.o: tagger/sentence.h tagger/taglexicon.h tagger/hasharray.h
scrut/scrutinizer.o: tagger/tagtrigram.h scrut/matchingset.h scrut/rules.h
scrut/scrutinizer.o: tagger/text.h tagger/hashtable.h /usr/include/stdlib.h
scrut/scrutinizer.o: /usr/include/iso/stdlib_iso.h
scrut/scrutinizer.o: /usr/include/iso/stdlib_c99.h scrut/evaluatedvariable.h
scrut/scrutinizer.o: scrut/semantictype.h scrut/operation.h scrut/expr.h
scrut/scrutinizer.o: scrut/rulesettings.h scrut/gramerror.h tagger/tagger.h
scrut/scrutinizer.o: tagger/lexicon.h tagger/morflexicon.h tagger/styleword.h
scrut/scrutinizer.o: tagger/newwordlexicon.h tagger/newword.h
scrut/scrutinizer.o: tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/scrutinizer.o: tagger/wordruleterms.h token/tokenizer.h
scrut/scrutinizer.o: common/FlexLexer.h tagger/trigramgadget.h
scrut/scrutinizer.o: scrut/ruleset.h stava/stavaapi.h prob/prob.h
scrut/scrutinizer.o: prob/output.h ../misc/xmloutput.h prob/defines.h
scrut/scrutinizer.o: prob/report.h prob/probinfo.h prob/probmatch.h
scrut/scrutinizer.o: prob/probdefines.h
scrut/server_socket.o: /usr/include/arpa/inet.h
scrut/server_socket.o: /usr/include/sys/feature_tests.h
scrut/server_socket.o: /usr/include/sys/ccompile.h
scrut/server_socket.o: /usr/include/sys/isa_defs.h /usr/include/sys/socket.h
scrut/server_socket.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/server_socket.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/server_socket.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/server_socket.o: /usr/include/sys/time.h /usr/include/sys/uio.h
scrut/server_socket.o: /usr/include/sys/socket_impl.h /usr/include/sys/un.h
scrut/server_socket.o: /usr/include/net/if_dl.h /usr/include/sys/netconfig.h
scrut/server_socket.o: /usr/include/netinet/in.h /usr/include/sys/stream.h
scrut/server_socket.o: /usr/include/sys/vnode.h /usr/include/sys/t_lock.h
scrut/server_socket.o: /usr/include/sys/machlock.h /usr/include/sys/param.h
scrut/server_socket.o: /usr/include/sys/unistd.h /usr/include/sys/mutex.h
scrut/server_socket.o: /usr/include/sys/rwlock.h /usr/include/sys/semaphore.h
scrut/server_socket.o: /usr/include/sys/condvar.h /usr/include/sys/rwstlock.h
scrut/server_socket.o: /usr/include/sys/ksynch.h /usr/include/sys/cred.h
scrut/server_socket.o: /usr/include/sys/resource.h /usr/include/vm/seg_enum.h
scrut/server_socket.o: /usr/include/sys/kstat.h /usr/include/sys/kmem.h
scrut/server_socket.o: /usr/include/sys/vmem.h /usr/include/sys/poll.h
scrut/server_socket.o: /usr/include/sys/strmdep.h /usr/include/sys/model.h
scrut/server_socket.o: /usr/include/sys/strft.h /usr/include/sys/byteorder.h
scrut/server_socket.o: /usr/include/netdb.h /usr/include/strings.h
scrut/server_socket.o: /usr/include/string.h /usr/include/iso/string_iso.h
scrut/server_socket.o: scrut/server_socket.h scrut/socket.h
scrut/socket.o: /usr/include/netdb.h /usr/include/sys/types.h
scrut/socket.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
scrut/socket.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
scrut/socket.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/socket.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/socket.o: /usr/include/sys/time.h /usr/include/netinet/in.h
scrut/socket.o: /usr/include/sys/socket_impl.h /usr/include/sys/un.h
scrut/socket.o: /usr/include/net/if_dl.h /usr/include/sys/stream.h
scrut/socket.o: /usr/include/sys/vnode.h /usr/include/sys/t_lock.h
scrut/socket.o: /usr/include/sys/machlock.h /usr/include/sys/param.h
scrut/socket.o: /usr/include/sys/unistd.h /usr/include/sys/mutex.h
scrut/socket.o: /usr/include/sys/rwlock.h /usr/include/sys/semaphore.h
scrut/socket.o: /usr/include/sys/condvar.h /usr/include/sys/rwstlock.h
scrut/socket.o: /usr/include/sys/ksynch.h /usr/include/sys/cred.h
scrut/socket.o: /usr/include/sys/uio.h /usr/include/sys/resource.h
scrut/socket.o: /usr/include/vm/seg_enum.h /usr/include/sys/kstat.h
scrut/socket.o: /usr/include/sys/kmem.h /usr/include/sys/vmem.h
scrut/socket.o: /usr/include/sys/poll.h /usr/include/sys/strmdep.h
scrut/socket.o: /usr/include/sys/model.h /usr/include/sys/strft.h
scrut/socket.o: /usr/include/sys/byteorder.h /usr/include/sys/socket.h
scrut/socket.o: /usr/include/sys/netconfig.h /usr/include/strings.h
scrut/socket.o: /usr/include/string.h /usr/include/iso/string_iso.h
scrut/socket.o: /usr/include/unistd.h scrut/socket.h /usr/include/arpa/inet.h
scrut/web.o: /usr/include/errno.h /usr/include/sys/errno.h
scrut/web.o: scrut/server_socket.h scrut/socket.h /usr/include/arpa/inet.h
scrut/web.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
scrut/web.o: /usr/include/sys/isa_defs.h /usr/include/sys/socket.h
scrut/web.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
scrut/web.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
scrut/web.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
scrut/web.o: /usr/include/sys/time.h /usr/include/sys/uio.h
scrut/web.o: /usr/include/sys/socket_impl.h /usr/include/sys/un.h
scrut/web.o: /usr/include/net/if_dl.h /usr/include/sys/netconfig.h
scrut/web.o: /usr/include/netinet/in.h /usr/include/sys/stream.h
scrut/web.o: /usr/include/sys/vnode.h /usr/include/sys/t_lock.h
scrut/web.o: /usr/include/sys/machlock.h /usr/include/sys/param.h
scrut/web.o: /usr/include/sys/unistd.h /usr/include/sys/mutex.h
scrut/web.o: /usr/include/sys/rwlock.h /usr/include/sys/semaphore.h
scrut/web.o: /usr/include/sys/condvar.h /usr/include/sys/rwstlock.h
scrut/web.o: /usr/include/sys/ksynch.h /usr/include/sys/cred.h
scrut/web.o: /usr/include/sys/resource.h /usr/include/vm/seg_enum.h
scrut/web.o: /usr/include/sys/kstat.h /usr/include/sys/kmem.h
scrut/web.o: /usr/include/sys/vmem.h /usr/include/sys/poll.h
scrut/web.o: /usr/include/sys/strmdep.h /usr/include/sys/model.h
scrut/web.o: /usr/include/sys/strft.h /usr/include/sys/byteorder.h
scrut/web.o: scrut/scrutinizer.h scrut/matching.h tagger/wordtoken.h
scrut/web.o: tagger/letter.h /usr/include/string.h
scrut/web.o: /usr/include/iso/string_iso.h /usr/include/ctype.h
scrut/web.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
scrut/web.o: common/basics.h /usr/include/stdio.h
scrut/web.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
scrut/web.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
scrut/web.o: /usr/include/iso/stdio_c99.h common/ensure.h
scrut/web.o: /usr/include/assert.h tagger/settings.h /usr/include/limits.h
scrut/web.o: /usr/include/iso/limits_iso.h /usr/include/float.h
scrut/web.o: common/stringbuf.h tagger/token.h tagger/word.h
scrut/web.o: tagger/hashscatter.h tagger/inflectrule.h common/message.h
scrut/web.o: tagger/tag.h tagger/bitmap.h common/file.h tagger/feature.h
scrut/web.o: tagger/tagdefines.h tagger/wordtag.h scrut/element.h
scrut/web.o: scrut/rule.h common/timer.h tagger/sentence.h
scrut/web.o: tagger/taglexicon.h tagger/hasharray.h tagger/tagtrigram.h
scrut/web.o: scrut/matchingset.h scrut/rules.h tagger/text.h
scrut/web.o: tagger/hashtable.h /usr/include/stdlib.h
scrut/web.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
scrut/web.o: scrut/evaluatedvariable.h scrut/semantictype.h scrut/operation.h
scrut/web.o: scrut/expr.h scrut/rulesettings.h scrut/gramerror.h
scrut/web.o: tagger/tagger.h tagger/lexicon.h tagger/morflexicon.h
scrut/web.o: tagger/styleword.h tagger/newwordlexicon.h tagger/newword.h
scrut/web.o: tagger/wordlexicon.h tagger/inflectlexicon.h
scrut/web.o: tagger/wordruleterms.h token/tokenizer.h common/FlexLexer.h
scrut/web.o: tagger/trigramgadget.h prob/output.h ../misc/xmloutput.h
scrut/web.o: prob/prob.h
tagger/hashscatter.o: tagger/hashscatter.h tagger/letter.h
tagger/hashscatter.o: /usr/include/string.h /usr/include/iso/string_iso.h
tagger/hashscatter.o: /usr/include/sys/feature_tests.h
tagger/hashscatter.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/hashscatter.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
tagger/hashscatter.o: /usr/include/iso/ctype_c99.h common/basics.h
tagger/hashscatter.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
tagger/hashscatter.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
tagger/hashscatter.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
tagger/hashscatter.o: common/ensure.h /usr/include/assert.h
tagger/inflectlexicon.o: common/file.h /usr/include/ctype.h
tagger/inflectlexicon.o: /usr/include/iso/ctype_iso.h
tagger/inflectlexicon.o: /usr/include/sys/feature_tests.h
tagger/inflectlexicon.o: /usr/include/sys/ccompile.h
tagger/inflectlexicon.o: /usr/include/sys/isa_defs.h
tagger/inflectlexicon.o: /usr/include/iso/ctype_c99.h common/ensure.h
tagger/inflectlexicon.o: /usr/include/assert.h common/message.h
tagger/inflectlexicon.o: tagger/inflectlexicon.h common/basics.h
tagger/inflectlexicon.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
tagger/inflectlexicon.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
tagger/inflectlexicon.o: /usr/include/stdio_impl.h
tagger/inflectlexicon.o: /usr/include/iso/stdio_c99.h tagger/inflectrule.h
tagger/inflectlexicon.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
tagger/inflectlexicon.o: tagger/tag.h /usr/include/string.h
tagger/inflectlexicon.o: /usr/include/iso/string_iso.h tagger/bitmap.h
tagger/inflectlexicon.o: tagger/feature.h tagger/hashscatter.h
tagger/inflectlexicon.o: tagger/settings.h /usr/include/float.h
tagger/inflectlexicon.o: tagger/tagdefines.h tagger/letter.h
tagger/inflectlexicon.o: tagger/taglexicon.h tagger/hasharray.h
tagger/inflectlexicon.o: tagger/tagtrigram.h tagger/token.h tagger/word.h
tagger/inflectlexicon.o: tagger/wordtag.h common/stringbuf.h
tagger/inflectrule.o: tagger/letter.h /usr/include/string.h
tagger/inflectrule.o: /usr/include/iso/string_iso.h
tagger/inflectrule.o: /usr/include/sys/feature_tests.h
tagger/inflectrule.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/inflectrule.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
tagger/inflectrule.o: /usr/include/iso/ctype_c99.h common/basics.h
tagger/inflectrule.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
tagger/inflectrule.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
tagger/inflectrule.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
tagger/inflectrule.o: common/ensure.h /usr/include/assert.h
tagger/inflectrule.o: tagger/inflectrule.h /usr/include/limits.h
tagger/inflectrule.o: /usr/include/iso/limits_iso.h tagger/tag.h
tagger/inflectrule.o: tagger/bitmap.h common/file.h common/message.h
tagger/inflectrule.o: tagger/feature.h tagger/hashscatter.h tagger/settings.h
tagger/inflectrule.o: /usr/include/float.h tagger/tagdefines.h
tagger/letter.o: tagger/letter.h /usr/include/string.h
tagger/letter.o: /usr/include/iso/string_iso.h
tagger/letter.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
tagger/letter.o: /usr/include/sys/isa_defs.h /usr/include/ctype.h
tagger/letter.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
tagger/letter.o: common/basics.h /usr/include/stdio.h
tagger/letter.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/letter.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/letter.o: /usr/include/iso/stdio_c99.h common/ensure.h
tagger/letter.o: /usr/include/assert.h
tagger/lexicon.o: /usr/include/float.h /usr/include/sys/feature_tests.h
tagger/lexicon.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/lexicon.o: /usr/include/math.h /usr/include/iso/math_iso.h
tagger/lexicon.o: /usr/include/iso/math_c99.h common/file.h
tagger/lexicon.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
tagger/lexicon.o: /usr/include/iso/ctype_c99.h common/ensure.h
tagger/lexicon.o: /usr/include/assert.h common/message.h tagger/letter.h
tagger/lexicon.o: /usr/include/string.h /usr/include/iso/string_iso.h
tagger/lexicon.o: common/basics.h /usr/include/stdio.h
tagger/lexicon.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/lexicon.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/lexicon.o: /usr/include/iso/stdio_c99.h tagger/morf.h
tagger/lexicon.o: tagger/sentence.h tagger/wordtoken.h tagger/settings.h
tagger/lexicon.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
tagger/lexicon.o: common/stringbuf.h tagger/token.h tagger/word.h
tagger/lexicon.o: tagger/hashscatter.h tagger/inflectrule.h tagger/tag.h
tagger/lexicon.o: tagger/bitmap.h tagger/feature.h tagger/tagdefines.h
tagger/lexicon.o: tagger/wordtag.h tagger/lexicon.h tagger/hashtable.h
tagger/lexicon.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
tagger/lexicon.o: /usr/include/iso/stdlib_c99.h tagger/morflexicon.h
tagger/lexicon.o: tagger/hasharray.h tagger/taglexicon.h tagger/tagtrigram.h
tagger/lexicon.o: tagger/styleword.h tagger/newwordlexicon.h tagger/newword.h
tagger/lexicon.o: tagger/wordlexicon.h tagger/inflectlexicon.h
tagger/lexicon.o: tagger/wordruleterms.h tagger/tagger.h common/timer.h
tagger/lexicon.o: /usr/include/sys/time.h /usr/include/sys/types.h
tagger/lexicon.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
tagger/lexicon.o: /usr/include/sys/select.h /usr/include/sys/time_impl.h
tagger/lexicon.o: /usr/include/sys/time.h tagger/text.h token/tokenizer.h
tagger/lexicon.o: common/FlexLexer.h tagger/trigramgadget.h
tagger/main.o: common/file.h /usr/include/ctype.h
tagger/main.o: /usr/include/iso/ctype_iso.h /usr/include/sys/feature_tests.h
tagger/main.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/main.o: /usr/include/iso/ctype_c99.h common/ensure.h
tagger/main.o: /usr/include/assert.h common/message.h tagger/letter.h
tagger/main.o: /usr/include/string.h /usr/include/iso/string_iso.h
tagger/main.o: common/basics.h /usr/include/stdio.h
tagger/main.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/main.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/main.o: /usr/include/iso/stdio_c99.h tagger/settings.h
tagger/main.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
tagger/main.o: /usr/include/float.h tagger/tagger.h tagger/lexicon.h
tagger/main.o: tagger/hashtable.h /usr/include/stdlib.h
tagger/main.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
tagger/main.o: tagger/morflexicon.h tagger/word.h tagger/hashscatter.h
tagger/main.o: tagger/inflectrule.h tagger/tag.h tagger/bitmap.h
tagger/main.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
tagger/main.o: common/stringbuf.h tagger/hasharray.h tagger/taglexicon.h
tagger/main.o: tagger/tagtrigram.h tagger/token.h tagger/styleword.h
tagger/main.o: tagger/newwordlexicon.h tagger/newword.h tagger/wordlexicon.h
tagger/main.o: tagger/inflectlexicon.h tagger/wordruleterms.h
tagger/main.o: tagger/sentence.h tagger/wordtoken.h common/timer.h
tagger/main.o: /usr/include/sys/time.h /usr/include/sys/types.h
tagger/main.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
tagger/main.o: /usr/include/sys/select.h /usr/include/sys/time_impl.h
tagger/main.o: /usr/include/sys/time.h tagger/text.h token/tokenizer.h
tagger/main.o: common/FlexLexer.h tagger/trigramgadget.h
tagger/morflexicon.o: common/file.h /usr/include/ctype.h
tagger/morflexicon.o: /usr/include/iso/ctype_iso.h
tagger/morflexicon.o: /usr/include/sys/feature_tests.h
tagger/morflexicon.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/morflexicon.o: /usr/include/iso/ctype_c99.h common/ensure.h
tagger/morflexicon.o: /usr/include/assert.h common/message.h
tagger/morflexicon.o: tagger/morflexicon.h tagger/word.h
tagger/morflexicon.o: /usr/include/string.h /usr/include/iso/string_iso.h
tagger/morflexicon.o: tagger/hashscatter.h tagger/inflectrule.h
tagger/morflexicon.o: common/basics.h /usr/include/stdio.h
tagger/morflexicon.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/morflexicon.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/morflexicon.o: /usr/include/iso/stdio_c99.h /usr/include/limits.h
tagger/morflexicon.o: /usr/include/iso/limits_iso.h tagger/tag.h
tagger/morflexicon.o: tagger/bitmap.h tagger/feature.h tagger/settings.h
tagger/morflexicon.o: /usr/include/float.h tagger/tagdefines.h
tagger/morflexicon.o: tagger/wordtag.h common/stringbuf.h tagger/hasharray.h
tagger/morflexicon.o: tagger/taglexicon.h tagger/tagtrigram.h tagger/token.h
tagger/morflexicon.o: tagger/styleword.h
tagger/newwordlexicon.o: tagger/newwordlexicon.h tagger/newword.h
tagger/newwordlexicon.o: tagger/word.h /usr/include/string.h
tagger/newwordlexicon.o: /usr/include/iso/string_iso.h
tagger/newwordlexicon.o: /usr/include/sys/feature_tests.h
tagger/newwordlexicon.o: /usr/include/sys/ccompile.h
tagger/newwordlexicon.o: /usr/include/sys/isa_defs.h tagger/hashscatter.h
tagger/newwordlexicon.o: tagger/inflectrule.h common/basics.h
tagger/newwordlexicon.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
tagger/newwordlexicon.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
tagger/newwordlexicon.o: /usr/include/stdio_impl.h
tagger/newwordlexicon.o: /usr/include/iso/stdio_c99.h common/ensure.h
tagger/newwordlexicon.o: /usr/include/assert.h /usr/include/limits.h
tagger/newwordlexicon.o: /usr/include/iso/limits_iso.h common/message.h
tagger/newwordlexicon.o: tagger/tag.h tagger/bitmap.h common/file.h
tagger/newwordlexicon.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
tagger/newwordlexicon.o: /usr/include/iso/ctype_c99.h tagger/feature.h
tagger/newwordlexicon.o: tagger/settings.h /usr/include/float.h
tagger/newwordlexicon.o: tagger/tagdefines.h tagger/wordtag.h
tagger/newwordlexicon.o: common/stringbuf.h tagger/hashtable.h
tagger/newwordlexicon.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
tagger/newwordlexicon.o: /usr/include/iso/stdlib_c99.h
tagger/sentence.o: tagger/letter.h /usr/include/string.h
tagger/sentence.o: /usr/include/iso/string_iso.h
tagger/sentence.o: /usr/include/sys/feature_tests.h
tagger/sentence.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/sentence.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
tagger/sentence.o: /usr/include/iso/ctype_c99.h common/basics.h
tagger/sentence.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
tagger/sentence.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
tagger/sentence.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
tagger/sentence.o: common/ensure.h /usr/include/assert.h tagger/sentence.h
tagger/sentence.o: tagger/wordtoken.h tagger/settings.h /usr/include/limits.h
tagger/sentence.o: /usr/include/iso/limits_iso.h /usr/include/float.h
tagger/sentence.o: common/stringbuf.h tagger/token.h tagger/word.h
tagger/sentence.o: tagger/hashscatter.h tagger/inflectrule.h common/message.h
tagger/sentence.o: tagger/tag.h tagger/bitmap.h common/file.h
tagger/sentence.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
tagger/sentence.o: tagger/tagger.h tagger/lexicon.h tagger/hashtable.h
tagger/sentence.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
tagger/sentence.o: /usr/include/iso/stdlib_c99.h tagger/morflexicon.h
tagger/sentence.o: tagger/hasharray.h tagger/taglexicon.h tagger/tagtrigram.h
tagger/sentence.o: tagger/styleword.h tagger/newwordlexicon.h
tagger/sentence.o: tagger/newword.h tagger/wordlexicon.h
tagger/sentence.o: tagger/inflectlexicon.h tagger/wordruleterms.h
tagger/sentence.o: common/timer.h /usr/include/sys/time.h
tagger/sentence.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h
tagger/sentence.o: /usr/include/sys/int_types.h /usr/include/sys/select.h
tagger/sentence.o: /usr/include/sys/time_impl.h /usr/include/sys/time.h
tagger/sentence.o: tagger/text.h token/tokenizer.h common/FlexLexer.h
tagger/sentence.o: tagger/trigramgadget.h
tagger/settings.o: common/basics.h /usr/include/stdio.h
tagger/settings.o: /usr/include/sys/feature_tests.h
tagger/settings.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/settings.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/settings.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/settings.o: /usr/include/iso/stdio_c99.h common/ensure.h
tagger/settings.o: /usr/include/assert.h common/file.h /usr/include/ctype.h
tagger/settings.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
tagger/settings.o: common/message.h tagger/settings.h /usr/include/limits.h
tagger/settings.o: /usr/include/iso/limits_iso.h /usr/include/float.h
tagger/tag.o: common/message.h tagger/tag.h /usr/include/string.h
tagger/tag.o: /usr/include/iso/string_iso.h /usr/include/sys/feature_tests.h
tagger/tag.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/tag.o: common/basics.h /usr/include/stdio.h
tagger/tag.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/tag.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/tag.o: /usr/include/iso/stdio_c99.h common/ensure.h
tagger/tag.o: /usr/include/assert.h tagger/bitmap.h common/file.h
tagger/tag.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
tagger/tag.o: /usr/include/iso/ctype_c99.h tagger/feature.h
tagger/tag.o: tagger/hashscatter.h tagger/settings.h /usr/include/limits.h
tagger/tag.o: /usr/include/iso/limits_iso.h /usr/include/float.h
tagger/tag.o: tagger/tagdefines.h tagger/taglexicon.h tagger/hasharray.h
tagger/tag.o: tagger/tagtrigram.h tagger/token.h tagger/word.h
tagger/tag.o: tagger/inflectrule.h tagger/wordtag.h common/stringbuf.h
tagger/tagger.o: /usr/include/float.h /usr/include/sys/feature_tests.h
tagger/tagger.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/tagger.o: /usr/include/math.h /usr/include/iso/math_iso.h
tagger/tagger.o: /usr/include/iso/math_c99.h common/file.h
tagger/tagger.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
tagger/tagger.o: /usr/include/iso/ctype_c99.h common/ensure.h
tagger/tagger.o: /usr/include/assert.h common/message.h tagger/letter.h
tagger/tagger.o: /usr/include/string.h /usr/include/iso/string_iso.h
tagger/tagger.o: common/basics.h /usr/include/stdio.h
tagger/tagger.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/tagger.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/tagger.o: /usr/include/iso/stdio_c99.h tagger/morf.h tagger/sentence.h
tagger/tagger.o: tagger/wordtoken.h tagger/settings.h /usr/include/limits.h
tagger/tagger.o: /usr/include/iso/limits_iso.h common/stringbuf.h
tagger/tagger.o: tagger/token.h tagger/word.h tagger/hashscatter.h
tagger/tagger.o: tagger/inflectrule.h tagger/tag.h tagger/bitmap.h
tagger/tagger.o: tagger/feature.h tagger/tagdefines.h tagger/wordtag.h
tagger/tagger.o: tagger/tagger.h tagger/lexicon.h tagger/hashtable.h
tagger/tagger.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
tagger/tagger.o: /usr/include/iso/stdlib_c99.h tagger/morflexicon.h
tagger/tagger.o: tagger/hasharray.h tagger/taglexicon.h tagger/tagtrigram.h
tagger/tagger.o: tagger/styleword.h tagger/newwordlexicon.h tagger/newword.h
tagger/tagger.o: tagger/wordlexicon.h tagger/inflectlexicon.h
tagger/tagger.o: tagger/wordruleterms.h common/timer.h
tagger/tagger.o: /usr/include/sys/time.h /usr/include/sys/types.h
tagger/tagger.o: /usr/include/sys/machtypes.h /usr/include/sys/int_types.h
tagger/tagger.o: /usr/include/sys/select.h /usr/include/sys/time_impl.h
tagger/tagger.o: /usr/include/sys/time.h tagger/text.h token/tokenizer.h
tagger/tagger.o: common/FlexLexer.h tagger/trigramgadget.h
tagger/taglexicon.o: /usr/include/math.h /usr/include/iso/math_iso.h
tagger/taglexicon.o: /usr/include/sys/feature_tests.h
tagger/taglexicon.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/taglexicon.o: /usr/include/iso/math_c99.h /usr/include/string.h
tagger/taglexicon.o: /usr/include/iso/string_iso.h /usr/include/stdlib.h
tagger/taglexicon.o: /usr/include/iso/stdlib_iso.h
tagger/taglexicon.o: /usr/include/iso/stdlib_c99.h common/file.h
tagger/taglexicon.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
tagger/taglexicon.o: /usr/include/iso/ctype_c99.h common/ensure.h
tagger/taglexicon.o: /usr/include/assert.h common/message.h tagger/letter.h
tagger/taglexicon.o: common/basics.h /usr/include/stdio.h
tagger/taglexicon.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/taglexicon.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/taglexicon.o: /usr/include/iso/stdio_c99.h tagger/settings.h
tagger/taglexicon.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
tagger/taglexicon.o: /usr/include/float.h tagger/taglexicon.h tagger/bitmap.h
tagger/taglexicon.o: tagger/feature.h tagger/hasharray.h tagger/tag.h
tagger/taglexicon.o: tagger/hashscatter.h tagger/tagdefines.h
tagger/taglexicon.o: tagger/tagtrigram.h tagger/token.h tagger/word.h
tagger/taglexicon.o: tagger/inflectrule.h tagger/wordtag.h common/stringbuf.h
tagger/text.o: tagger/sentence.h tagger/wordtoken.h tagger/letter.h
tagger/text.o: /usr/include/string.h /usr/include/iso/string_iso.h
tagger/text.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
tagger/text.o: /usr/include/sys/isa_defs.h /usr/include/ctype.h
tagger/text.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
tagger/text.o: common/basics.h /usr/include/stdio.h
tagger/text.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/text.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/text.o: /usr/include/iso/stdio_c99.h common/ensure.h
tagger/text.o: /usr/include/assert.h tagger/settings.h /usr/include/limits.h
tagger/text.o: /usr/include/iso/limits_iso.h /usr/include/float.h
tagger/text.o: common/stringbuf.h tagger/token.h tagger/word.h
tagger/text.o: tagger/hashscatter.h tagger/inflectrule.h common/message.h
tagger/text.o: tagger/tag.h tagger/bitmap.h common/file.h tagger/feature.h
tagger/text.o: tagger/tagdefines.h tagger/wordtag.h tagger/text.h
tagger/word.o: /usr/include/math.h /usr/include/iso/math_iso.h
tagger/word.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
tagger/word.o: /usr/include/sys/isa_defs.h /usr/include/iso/math_c99.h
tagger/word.o: tagger/word.h /usr/include/string.h
tagger/word.o: /usr/include/iso/string_iso.h tagger/hashscatter.h
tagger/word.o: tagger/inflectrule.h common/basics.h /usr/include/stdio.h
tagger/word.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/word.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/word.o: /usr/include/iso/stdio_c99.h common/ensure.h
tagger/word.o: /usr/include/assert.h /usr/include/limits.h
tagger/word.o: /usr/include/iso/limits_iso.h common/message.h tagger/tag.h
tagger/word.o: tagger/bitmap.h common/file.h /usr/include/ctype.h
tagger/word.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
tagger/word.o: tagger/feature.h tagger/settings.h /usr/include/float.h
tagger/word.o: tagger/tagdefines.h tagger/wordtag.h common/stringbuf.h
tagger/word.o: tagger/newword.h
tagger/wordlexicon.o: common/file.h /usr/include/ctype.h
tagger/wordlexicon.o: /usr/include/iso/ctype_iso.h
tagger/wordlexicon.o: /usr/include/sys/feature_tests.h
tagger/wordlexicon.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/wordlexicon.o: /usr/include/iso/ctype_c99.h common/ensure.h
tagger/wordlexicon.o: /usr/include/assert.h common/message.h tagger/letter.h
tagger/wordlexicon.o: /usr/include/string.h /usr/include/iso/string_iso.h
tagger/wordlexicon.o: common/basics.h /usr/include/stdio.h
tagger/wordlexicon.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
tagger/wordlexicon.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
tagger/wordlexicon.o: /usr/include/iso/stdio_c99.h tagger/newwordlexicon.h
tagger/wordlexicon.o: tagger/newword.h tagger/word.h tagger/hashscatter.h
tagger/wordlexicon.o: tagger/inflectrule.h /usr/include/limits.h
tagger/wordlexicon.o: /usr/include/iso/limits_iso.h tagger/tag.h
tagger/wordlexicon.o: tagger/bitmap.h tagger/feature.h tagger/settings.h
tagger/wordlexicon.o: /usr/include/float.h tagger/tagdefines.h
tagger/wordlexicon.o: tagger/wordtag.h common/stringbuf.h tagger/hashtable.h
tagger/wordlexicon.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
tagger/wordlexicon.o: /usr/include/iso/stdlib_c99.h tagger/wordlexicon.h
tagger/wordlexicon.o: tagger/hasharray.h tagger/inflectlexicon.h
tagger/wordlexicon.o: tagger/taglexicon.h tagger/tagtrigram.h tagger/token.h
tagger/wordlexicon.o: tagger/styleword.h
tagger/wordtag.o: tagger/word.h /usr/include/string.h
tagger/wordtag.o: /usr/include/iso/string_iso.h
tagger/wordtag.o: /usr/include/sys/feature_tests.h
tagger/wordtag.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/wordtag.o: tagger/hashscatter.h tagger/inflectrule.h common/basics.h
tagger/wordtag.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
tagger/wordtag.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
tagger/wordtag.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
tagger/wordtag.o: common/ensure.h /usr/include/assert.h /usr/include/limits.h
tagger/wordtag.o: /usr/include/iso/limits_iso.h common/message.h tagger/tag.h
tagger/wordtag.o: tagger/bitmap.h common/file.h /usr/include/ctype.h
tagger/wordtag.o: /usr/include/iso/ctype_iso.h /usr/include/iso/ctype_c99.h
tagger/wordtag.o: tagger/feature.h tagger/settings.h /usr/include/float.h
tagger/wordtag.o: tagger/tagdefines.h tagger/wordtag.h common/stringbuf.h
tagger/wordtag.o: tagger/wordlexicon.h tagger/newword.h tagger/hasharray.h
tagger/wordtag.o: tagger/inflectlexicon.h tagger/taglexicon.h
tagger/wordtag.o: tagger/tagtrigram.h tagger/token.h tagger/styleword.h
tagger/wordtoken.o: tagger/letter.h /usr/include/string.h
tagger/wordtoken.o: /usr/include/iso/string_iso.h
tagger/wordtoken.o: /usr/include/sys/feature_tests.h
tagger/wordtoken.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
tagger/wordtoken.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
tagger/wordtoken.o: /usr/include/iso/ctype_c99.h common/basics.h
tagger/wordtoken.o: /usr/include/stdio.h /usr/include/iso/stdio_iso.h
tagger/wordtoken.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
tagger/wordtoken.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
tagger/wordtoken.o: common/ensure.h /usr/include/assert.h tagger/settings.h
tagger/wordtoken.o: /usr/include/limits.h /usr/include/iso/limits_iso.h
tagger/wordtoken.o: /usr/include/float.h tagger/token.h tagger/newword.h
tagger/wordtoken.o: tagger/word.h tagger/hashscatter.h tagger/inflectrule.h
tagger/wordtoken.o: common/message.h tagger/tag.h tagger/bitmap.h
tagger/wordtoken.o: common/file.h tagger/feature.h tagger/tagdefines.h
tagger/wordtoken.o: tagger/wordtag.h common/stringbuf.h tagger/wordtoken.h
token/tokenizer.o: common/FlexLexer.h /usr/include/stdlib.h
token/tokenizer.o: /usr/include/iso/stdlib_iso.h
token/tokenizer.o: /usr/include/sys/feature_tests.h
token/tokenizer.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
token/tokenizer.o: /usr/include/iso/stdlib_c99.h
cgigranska/wwwscrutinizer.o: /usr/include/stdio.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/feature_tests.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/ccompile.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/isa_defs.h
cgigranska/wwwscrutinizer.o: /usr/include/iso/stdio_iso.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/va_list.h
cgigranska/wwwscrutinizer.o: /usr/include/stdio_tag.h
cgigranska/wwwscrutinizer.o: /usr/include/stdio_impl.h
cgigranska/wwwscrutinizer.o: /usr/include/iso/stdio_c99.h
cgigranska/wwwscrutinizer.o: /usr/include/stdlib.h
cgigranska/wwwscrutinizer.o: /usr/include/iso/stdlib_iso.h
cgigranska/wwwscrutinizer.o: /usr/include/iso/stdlib_c99.h
cgigranska/wwwscrutinizer.o: /usr/include/string.h
cgigranska/wwwscrutinizer.o: /usr/include/iso/string_iso.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/time.h /usr/include/sys/timeb.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/types.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/machtypes.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/int_types.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/select.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/time_impl.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/time.h /usr/include/unistd.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/unistd.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/wait.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/resource.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/siginfo.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/machsig.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/procset.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/signal.h
cgigranska/wwwscrutinizer.o: /usr/include/sys/iso/signal_iso.h
cgigranska/wwwscrutinizer.o: /usr/include/errno.h /usr/include/sys/errno.h
cgigranska/wwwscrutinizer.o: /usr/include/ctype.h
cgigranska/wwwscrutinizer.o: /usr/include/iso/ctype_iso.h
cgigranska/wwwscrutinizer.o: /usr/include/iso/ctype_c99.h
stava/rattstava.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
stava/rattstava.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
stava/rattstava.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
stava/rattstava.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
stava/rattstava.o: /usr/include/iso/stdio_c99.h /usr/include/stdlib.h
stava/rattstava.o: /usr/include/iso/stdlib_iso.h
stava/rattstava.o: /usr/include/iso/stdlib_c99.h /usr/include/string.h
stava/rattstava.o: /usr/include/iso/string_iso.h stava/stava.h
stava/rattstava.o: stava/rattstava.h stava/suffix.h
stava/stavaapi.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
stava/stavaapi.o: /usr/include/sys/ccompile.h /usr/include/sys/isa_defs.h
stava/stavaapi.o: /usr/include/iso/stdio_iso.h /usr/include/sys/va_list.h
stava/stavaapi.o: /usr/include/stdio_tag.h /usr/include/stdio_impl.h
stava/stavaapi.o: /usr/include/iso/stdio_c99.h /usr/include/stdlib.h
stava/stavaapi.o: /usr/include/iso/stdlib_iso.h /usr/include/iso/stdlib_c99.h
stava/stavaapi.o: /usr/include/string.h /usr/include/iso/string_iso.h
stava/stavaapi.o: /usr/include/ctype.h /usr/include/iso/ctype_iso.h
stava/stavaapi.o: /usr/include/iso/ctype_c99.h stava/stava.h
stava/stavaapi.o: stava/rattstava.h stava/suffix.h stava/stavaconstants.h
stava/stavaapi.o: stava/stavaapi.h
stava/suffix.o: stava/suffix.h stava/stava.h /usr/include/stdio.h
stava/suffix.o: /usr/include/sys/feature_tests.h /usr/include/sys/ccompile.h
stava/suffix.o: /usr/include/sys/isa_defs.h /usr/include/iso/stdio_iso.h
stava/suffix.o: /usr/include/sys/va_list.h /usr/include/stdio_tag.h
stava/suffix.o: /usr/include/stdio_impl.h /usr/include/iso/stdio_c99.h
stava/suffix.o: /usr/include/stdlib.h /usr/include/iso/stdlib_iso.h
stava/suffix.o: /usr/include/iso/stdlib_c99.h /usr/include/string.h
stava/suffix.o: /usr/include/iso/string_iso.h
