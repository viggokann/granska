#ifndef REPRESENT_H
#define REPRESENT_H


class TagLexicon;

namespace Prob
{
    float   analyze(const TagLexicon	&l,
		    uchar		 index[5],
		    int			 model,
		    bool		 debug = false);
}


#endif // REPRESENT_H
