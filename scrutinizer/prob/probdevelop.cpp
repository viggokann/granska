#pragma warning(disable: 4786)
#include "probdevelop.h"


namespace Prob
{
    int sentence_count = 0;
    const char *annot_file  = 0;
    const char *output_file = 0;
    const AbstractSentence *current_sentence = 0;

    Output *out = &no_output();

    int type[type_count];
    int type_c[type_count];
    int type_tot[type_count];
    int flag[type_count];

    const char *type_names[] =
    {
	"other",
	"sem/gram err",
	"verb spell",
	"compound",
	"spelling",
	"style",
	"missing word",
	"word order",
	"foreign word",
	"tok/sent err",
	"bad tag",
	"false alarm",
	"no type",
    };

    foundmap found_by_prob;
    foundmap found_by_scrut;
    std::vector<int> tags_in_false_alarms;
    rulemap rules_used[type_count + 1];
    rulemap rules_scrut;
    int rules_used_count[type_count + 1][MAX_BLOCK];
    std::set<std::string> rules_avail;
}
