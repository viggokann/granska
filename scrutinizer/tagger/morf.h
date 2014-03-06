/* morf.hh
 * author: Johan Carlberger
 * last change: 981030
 * comments: MIN_PREFIX_LENGTH determines how many letters must precede a suffix.
 *           used in tagger and freqstatmorf.cc.
 *           the longer prefix the faster tagger and smaller lexicon.
 *           an experiment 980917 gave the following results:
 *           MIN_PREFIX_LENGTH  #morfs  #correct tags
 *           0                  62995   13632
 *           1                  55933   13634
 *           2                  47128   13634
 *           3                  38393   13619
 *
 *           MIN_PREFIX_LENGTH=2 was chosen for obvious reasons
 */

#ifndef _morf_hh
#define _morf_hh

const uint MIN_PREFIX_LENGTH = 2;
const int MAX_LAST_CHARS = 5;
const int MIN_LAST_CHARS = 1;


#endif
