#! /bin/bash

echo "Setting environment variables for Granska"
echo ""
export GRANSKA_HOME=$1/granska/
echo "GRANSKA_HOME = "$GRANSKA_HOME
export TAGGER_LEXICON=$1/granska/lex
export DEVELOPERS_TAGGER_LEXICON=~/Skola/Exjobb/granska/lex
echo "TAGGER_LEXICON = "$TAGGER_LEXICON
export STAVA_LEXICON=$1/granska/stava/lib/
echo "STAVA_LEXICON = " $STAVA_LEXICON
export SCRUTINIZER_RULE_FILE=$1/granska/rulesets/wille/regelsamling.ver8
echo "SCRUTINIZER_RULE_FILE = "$SCRUTINIZER_RULE_FILE
export SCRUTINIZER_TEST_TEXT=$1/granska/rulesets/wille/regelsamling.ver8.testfil
export DEVELOPERS_TAGGER_OPT_TEXT=$1/granska/rulesets/wille/regelsamling.ver8.testfil
echo "SCRUTINIZER_TEST_TEXT = "$SCRUTINIZER_TEST_TEXT
echo "Done"
