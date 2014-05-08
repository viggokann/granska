#! /bin/bash

echo "Setting environment variables for Granska"
echo ""
export TAGGER_LEXICON= /granska/lex
echo "TAGGER_LEXICON = "$TAGGER_LEXICON
export STAVA_LEXICON= /granska/stava/lib/
echo "STAVA_LEXICON = " $STAVA_LEXICON
export SCRUTINIZER_RULE_FILE= /granska/regler/regelsamling.ver8
echo "SCRUTINIZER_RULE_FILE = "$SCRUTINIZER_RULE_FILE
export SCRUTINIZER_TEST_TEXT= /granska/regler/regelsamling.ver8.testfil
echo "SCRUTINIZER_TEST_TEXT = "$SCRUTINIZER_TEST_TEXT
echo "Done"
