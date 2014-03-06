#ifndef PROB_DEVELOP_H
#define PROB_DEVELOP_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include "output.h"
#include "probdefines.h"


class AbstractSentence;

namespace Prob
{
    extern Output *out;

    const unsigned int type_count = 11;
    extern int type[type_count];
    extern int type_c[type_count];
    extern int type_tot[type_count];
    extern int flag[type_count];

    extern const char *type_names[];

    typedef std::map<std::string, int> rulemap;
    typedef std::multimap<int, std::pair<int, int> > foundmap;
    extern foundmap found_by_prob;
    extern foundmap found_by_scrut;
    extern std::vector<int> tags_in_false_alarms;
    extern rulemap rules_used[type_count + 1];
    extern rulemap rules_scrut;
    extern int rules_used_count[type_count + 1][MAX_BLOCK];
    extern std::set<std::string> rules_avail;
}


#endif /* PROB_DEVELOP_H */

