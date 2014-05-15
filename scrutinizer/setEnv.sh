#! /bin/bash

echo "Setting environment variables for Granska"
echo ""
export GRANSKA_HOME=~/Skola/Exjobb/granska/
echo "GRANSKA_HOME = "$GRANSKA_HOME
export TAGGER_LEXICON=~/Skola/Exjobb/granska/lex
echo "TAGGER_LEXICON = "$TAGGER_LEXICON
export STAVA_LEXICON=~/Skola/Exjobb/granska/stava/lib/
echo "STAVA_LEXICON = " $STAVA_LEXICON
export SCRUTINIZER_RULE_FILE=~/Skola/Exjobb/granska/rulesets/wille/regelsamling.ver8
echo "SCRUTINIZER_RULE_FILE = "$SCRUTINIZER_RULE_FILE
export SCRUTINIZER_TEST_TEXT=~/Skola/Exjobb/granska/rulesets/wille/regelsamling.ver8.testfil
echo "SCRUTINIZER_TEST_TEXT = "$SCRUTINIZER_TEST_TEXT
echo "Done"
