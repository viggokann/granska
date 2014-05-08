#! /bin/bash

echo "Setting environment variables for Granska"
echo ""
export TAGGER_LEXICON=/home/wilhelm/Skola/Exjobb/granska/lex
echo "TAGGER_LEXICON = "$TAGGER_LEXICON
export STAVA_LEXICON=/home/wilhelm/Skola/Exjobb/granska/stava/lib/
echo "STAVA_LEXICON = " $STAVA_LEXICON
export SCRUTINIZER_RULE_FILE=/home/wilhelm/Skola/Exjobb/granska/regler/regelsamling.ver8
echo "SCRUTINIZER_RULE_FILE = "$SCRUTINIZER_RULE_FILE
export SCRUTINIZER_TEST_TEXT=/home/wilhelm/Skola/Exjobb/granska/test.txt
echo "SCRUTINIZER_TEST_TEXT = "$SCRUTINIZER_TEST_TEXT
echo "Done"
