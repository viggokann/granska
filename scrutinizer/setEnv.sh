#! /bin/bash

echo "Setting environment variables for Granska"
echo ""
export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH = $LD_LIBRARY_PATH"
export GRANSKA_HOME=$PWD/..
echo "GRANSKA_HOME = $GRANSKA_HOME"
export TAGGER_LEXICON=$GRANSKA_HOME/lex
export DEVELOPERS_TAGGER_LEXICON=$GRANSKA_HOME/lex
echo "TAGGER_LEXICON = $TAGGER_LEXICON"
export STAVA_LEXICON=$GRANSKA_HOME/stava/lib/
echo "STAVA_LEXICON = $STAVA_LEXICON"
export SCRUTINIZER_RULE_FILE=$GRANSKA_HOME/rulesets/wille/regelsamling.ver8
echo "SCRUTINIZER_RULE_FILE = $SCRUTINIZER_RULE_FILE"
export SCRUTINIZER_TEST_TEXT=$GRANSKA_HOME/rulesets/wille/regelsamling.ver8.testfil
export DEVELOPERS_TAGGER_OPT_TEXT=$GRANSKA_HOME/rulesets/wille/regelsamling.ver8.testfil
echo "SCRUTINIZER_TEST_TEXT = $SCRUTINIZER_TEST_TEXT"
echo "Done"
