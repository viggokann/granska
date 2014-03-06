#ifndef REPORT_H
#define REPORT_H

#include "probinfo.h"
#include "probmatch.h"


class Rule;
class Scrutinizer;
class RuleSet;

namespace Prob
{
    enum annot_flag_t { F_NORMAL, F_NOSTYLE };

    void report_granska_rules(const RuleSet *rs);
    void report_granska_rule_use(const Rule *r);

    void output_granska_rules();
    void output_tag_list(const Scrutinizer *s);

    void report(const Info &info, int begin, int end);
    void report_tags(const Info &info, int begin, int end);
    void report_rules(const Match *[], int, int, int, int);
    void out_type(int sentence, int begin, int end);
    void report_final();
}


#endif /* REPORT_H */

