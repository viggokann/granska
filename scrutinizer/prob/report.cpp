#pragma warning(disable:4786)
#include "report.h"
#include "rule.h"
#include "output.h"
#include "taglexicon.h"
#include "scrutinizer.h"
#include "ruleset.h"
#include "annot.h"
#include "probdevelop.h"
#include "probadmin.h"
#include "config.h"
#include <map>
#include <string>
#include <set>
#include <sstream>
#include <algorithm>


typedef std::map<const Category *, int> granska_rule_types_t;
typedef std::set<const Rule *> granska_rules_t;
granska_rule_types_t granska_rule_types;
granska_rules_t granska_rules_used;


void Prob::report_granska_rules(const RuleSet *rs)
{
    for(int i = 0; i < rs->NRules(); i++)
    {
	const Rule *r = rs->GetRule(i);
	if(r->IsHelpRule())
	    continue;

        // insert rule category into list of used
	const Category *cat = r->FirstRuleTerm()->GetCategory();
	if(cat) 
	  granska_rule_types[cat] = 0;
    }
}

void Prob::report_granska_rule_use(const Rule *r)
{
    // insert rule into list of used
    granska_rules_used.insert(r);

    // update the rule category counter
    const Category *cat = r->FirstRuleTerm()->GetCategory();
    if(cat)
      granska_rule_types[cat]++;
}

void Prob::output_granska_rules()
{
    Output &out = output();

    out.push("rules");

    // first, we output the rule categories
    out.push("categories");
    granska_rule_types_t::const_iterator i = granska_rule_types.begin();
    for(; i != granska_rule_types.end(); ++i)
    {
	out.push("category");
	out.add("name", i->first->Name());
	const char *tmp;
	tmp = i->first->Info();
	if(tmp && strcmp(tmp, "") != 0)
	    out.add("info", i->first->Info());
	tmp = i->first->LinkURL();
	if(tmp && strcmp(tmp, "") != 0)
	    out.add("linkurl", i->first->LinkURL());
	tmp = i->first->LinkText();
	if(tmp && strcmp(tmp, "") != 0)
	    out.add("linktext", i->first->LinkText());
	out.add("count", i->second);
	out.pop();  // push("category");
    }
    out.pop();  // push("categories");

    // second, we output the rules used
    out.push("rules_used");
    granska_rules_t::const_iterator j = granska_rules_used.begin();
    for(; j != granska_rules_used.end(); ++j)
    {
	const RuleTerm *r = (*j)->FirstRuleTerm();
	out.push("rule");
	out.add("name", (*j)->Name());
	out.add("category", r->GetCategory()->Name());
	const char *tmp;
	tmp = r->GetLinkURL();
	if(tmp && strcmp(tmp, "") != 0)
	    out.add("linkurl", r->GetLinkURL());
	tmp = r->GetLinkText();
	if(tmp && strcmp(tmp, "") != 0)
	    out.add("linktext", r->GetLinkText());
	out.pop();  // push("rule");
    }
    out.pop();  // push("rules_used");

    out.pop();  // push("rules");
}

void Prob::output_tag_list(const Scrutinizer *s)
{
    const TagLexicon &l = s->Tags();
    Output &out = output();
    
    out.push("tags");
    for(int i = 0; i < l.Size(); i++)
    {
	out.add("tag");
	out.attr("no", i);
	out.attr("name", l[i].String());
    }

    out.pop();  // push("tags");
}

void
Prob::report(const Info &info, int begin, int end)
{
    if(begin != PROBCHECK_OK)
    {
	found_by_prob.insert(foundmap::value_type(info.sent_offset, std::make_pair(begin, end)));

	annot_iter it = annots.find(info.sent_offset);
	if(it == annots.end())
	{
	    std::cout << "WARNING: an unknown (not annotated) alarm "
		      << "occurred in sentence at offset "
		      << info.sent_offset << " in pos " << begin << "-" << end << std::endl;

	    not_annotated(begin, end, "found by probcheck");
	}
    }
}

void
Prob::report_tags(const Info &info, int begin, int end)
{
    annot_iter it = annots.find(info.sent_offset);
    if(it == annots.end() || it->second.value[0] == FALSE_ALARM)
    {
	int from = begin - 1 >= 0 ? begin - 1 : 0;
	int to   = end + 1 < info.size ? end + 1 : info.size - 1;

	std::cout << "sent: " << info.sent_offset
		  << " (begin: " << begin << ", end:" << end << "): ";
	for(int i = from; i <= to; i++)
	{
	    tags_in_false_alarms[info.tags[i]->Index()]++;
	    std::cout << info.tags[i]->String() << "\t ";
	}
	std::cout << std::endl;
    }
}

void
Prob::report(const AbstractSentence     *s,
                const Rule                 *r,
                int                         size,
                const int                   begin[],
                const int                   end[])
{
    int sent_offset = s->GetWordToken(0 + 2)->Offset();
    if(!r->Name() || size == 0)
    {
    	//std::cout << "--> no name or zero size, offset: "
	//	  << sent_offset << std::endl;
	return;
    }
    int offset = begin[0] - 2;

    // is the matching relevant
    std::string rname = r->Name();
    std::string::size_type pos = rname.find('@');
    if(pos == std::string::npos)
    {
    	//std::cout << "--> could not find category symbol '@', offset: "
	//	  << sent_offset << std::endl;
	return;
    }

    std::string rule = rname.substr(0, pos);
    std::string cat = rname.substr(pos + 1);
    if(cat.compare("prob") == 0)
    {
	annot_iter it = annots.find(sent_offset);
	if(it == annots.end())
	{
    	    //std::cout << "--> sentence not found, offset: "
	    //		<< sent_offset << std::endl;
	    return;
	}
	const Annot &a = it->second;

	bool window = (rule == "prob_err");
	int j;
	for(j = 0; j < a.count; j++)
	    if(a.value[j] == offset ||
	       a.value[j] == offset - window ||
	       a.value[j] == offset + window)
		break;
	if(a.count == j)
	{
    	    //std::cout << "--> position " << offset
	    //	        << " not found, offset: "
	    //          << sent_offset << std::endl;
	    return;	    
	}

	std::pair<rulemap::iterator, bool> p = 
	    rules_scrut.insert(rulemap::value_type(rule, 1));
	if(!p.second)
	    p.first->second++;
    }
    else  // found by the original scrutinizer
    {	
	// ignore prob rules (they always end with 'recog')
	if(cat.length() >= 5 && cat.substr(cat.length() - 5) == "recog")
	    return;

	annot_iter it = annots.find(sent_offset);
	if(it == annots.end())
	{
    	    //std::cout << "WARNING: scrut: sentence not annotated, offset: "
	    //	      << sent_offset << ", rule = " 
	    //	      << rname << std::endl;
	    std::ostringstream o;
	    o << "found by scrutinizer: " << rname;
	    not_annotated(begin[0] - 2, end[0] - 2, o.str(), s);
	}
	found_by_scrut.insert(
	    foundmap::value_type(sent_offset,
				 std::make_pair(offset, offset)));
    }
    report_granska_rule_use(r);

    //std::cout << "--> added " << rname << " at pos "
    //	      << offset << ", offset: "
    //	      << sent_offset << std::endl;
}

void
Prob::out_type(int offset, int begin, int end)
{
    annot_iter it = annots.find(offset);
    if(it == annots.end())
    {
	if(begin != PROBCHECK_OK)
	    out->add("sent_comm", "falsk, inte annoterad");

	return;
    }
    const Annot &a = it->second;

    if(begin == PROBCHECK_OK)
    {
	if(a.value[0] == PROBCHECK_OK ||
	   a.value[0] == FALSE_ALARM)
	    out->add("sent_comm", "ok");
	else
	{
	    std::ostringstream s;
	    s << "missad detekt " << a.value[0];
	    out->add("sent_comm", s.str().c_str());
	}
    }
    else // suspicious word has been found
    {
	if(a.value[0] == PROBCHECK_OK)
	    out->add("sent_comm", "inte annoterad");
	else if(a.value[0] == FALSE_ALARM)
	    out->add("sent_comm", "falsk");
	else
	{
	    int j;
	    for(j = 0; j < a.count; j++)
	    {
    		int v = a.value[j];
		if((begin >= v - 1 &&	    // detect
		    begin <= v + 1) ||
		   (end >= v - 1 &&
		    end <= v + 1) ||
		   (begin <= v - 1 &&
		    end >= v + 1))
		{
		    out->add("sent_comm", "detekt");
		    break;
		}
	    }

	    if(j == a.count)
	    {
		std::ostringstream s;
		s << "falsk, missad detekt " << a.value[0];
		out->add("sent_comm", s.str().c_str());
	    }
	}
    }
    if(a.type[0] & LOOK_AT)
	out->add("sent_comm", "lookat");
}

void type_index(int                   t,
                std::vector<int>     &index,
                bool                 &ignore)
{
    using namespace Prob;	    // F_NORMAL, type, flag, type_count

    ignore = false;

    for(unsigned int i = 1; i < type_count; i++)
	if(flag[i] == F_NORMAL)
	{
	    if((t & type[i]) == type[i])
	    {
		index.push_back(i);
		if((i >= 8 && i <= 10) ||   // ignore FOREIGN, ERR_TOK_SENT, BAD_TAG
		   (t & STYLE))
		    ignore = true;
	    }
	}
	else if(flag[i] == F_NOSTYLE && !(t & STYLE))
	{
	    if((t & type[i]) == type[i])
	    {
		index.push_back(i);
		if((i >= 8 && i <= 10) ||   // ignore FOREIGN, ERR_TOK_SENT, BAD_TAG
		   (t & STYLE))
		    ignore = true;
	    }
	}

    // if none found, vector is empty
}

void
Prob::report_rules(const Match     *m[],
                      int              size,
                      int              offset,
                      int              from,
                      int              to)
{
    std::vector<int> types;

    out->push("rule_use");
    bool dummy;
    bool found = false;
    annot_iter it = annots.find(offset);
    if(it == annots.end())
    {
	types.push_back(type_count);	    // false ALARM in last slot
	out->add("type", "False alarm averted (not annotated)");
    }
    else if(it->second.value[0] == FALSE_ALARM)
    {
	types.push_back(type_count);	    // false ALARM in last slot
	out->add("type", "False alarm averted");
    }
    else
    {
	const Annot &a = it->second;

        out->add("type", "Annotations");
	out->attr("count", a.count);

	out->push("annots");
	for(int j = 0; j < a.count; j++)
	    if(a.value[j] >= from &&
	       a.value[j] <= to)
	    {
		unsigned int k;
		for(k = 0; k < type_count; k++)
		    if(type[k] == a.type[j])
			break;
		if(k == type_count)
		    k = type_count + 1;
		out->add("annot", type_names[k]);
		out->attr("pos", a.value[j]);
		{
		    std::ostringstream s;
		    s << std::hex << a.type[j] << std::dec;
		    out->attr("hex", s.str().c_str());
		}
	        type_index(a.type[j], types, dummy);
		found = true;
	    }
	out->pop();  // push("annots");
    }

    out->push("types");
    out->attr("count", (int)types.size());
    for(unsigned int n = 0; n < types.size(); n++)
    {
	out->add("type", type_names[types[n]]);
	out->attr("index", types[n]);
    }
    out->pop();  // push("types");

    std::sort(types.begin(), types.end());
    std::unique(types.begin(), types.end());

    out->push("resolves");
    for(unsigned int j = 0; j < types.size(); j++)
    {
	int ind = types[j];
	for(int l = 0; l < size; l++)
	{
	    const char *name = m[l]->name;
	    std::pair<rulemap::iterator, bool> pr = 
		rules_used[ind].insert(rulemap::value_type(name, 1));
	    if(!pr.second)
		pr.first->second++;
	    out->add("resolve", type_names[ind]);
	    out->attr("by", name);
	    out->attr("index", ind);
	}
        rules_used_count[ind][size]++;
    }
    out->pop();  // push("resolves");

    if(types.size() == 0 && it != annots.end() && it->second.count == 0)
	std::cout << "NOTE: sentence at offset " << offset
		  << " has no type" << std::endl;

    out->pop();  // push_back("rule_use");
}


static int
find_annot(int offset, int pos)
{
    Prob::annot_iter it = Prob::annots.find(offset);
    if(it == Prob::annots.end())
	return -1;

    for(int i = 0; i < it->second.count; i++)
	if(it->second.value[i] == pos ||
	   it->second.value[i] == pos - 1 ||
	   it->second.value[i] == pos + 1)
	   return i;

    return -1;
}

static int
inc_type_counts(int      type[],
                int      type_c[],
                int      flag[],
                int      types_count,
                int      offset,
                int      no,
                bool     silent = false)
{
    using namespace Prob;

    annot_iter it = Prob::annots.find(offset);
    int t = it->second.type[no];
    //t = t & ~LOOK_AT;

    if(t == Prob::NO_TYPE)	// all detects need not have a type
    {
	type_c[0]++;
	if(!silent)
	    std::cout << "NOTE: sentence at offset " << it->first
		      << "[" << no << "]" << " has no type"
		      << std::endl;
	return true;
    }
    if(t == Prob::ERR_TYPE)
	return -1;

    std::vector<int> indexes;
    bool ignore = false;
    type_index(t, indexes, ignore);

    if(indexes.size() == 0)
    {
	type_c[0]++;	    // unknown type
	if(!silent)
	    std::cout << "NOTE: unknown type: " << std::hex
		      << t << std::dec << std::endl;
    }
    else
    {
	//if(!silent)
	//    std::cout << "new error" << std::endl;
	for(unsigned int i = 0; i < indexes.size(); i++)
	{
	    type_c[indexes[i]]++;
	    //if(!silent)
	    //	std::cout << "type detected: " << type_names[indexes[i]] << std::endl;
	}
    }

    return !ignore;
}

void
Prob::report_final()
{
    std::vector<char> stat;
    std::vector<char> det;
    unsigned int i;

    stat.resize(1000);
    for(i = 0; i < 1000; i++)
	stat[i] = '.';
    det.reserve(100);
   
    int detect_known	= 0;
    int detect		= 0;
    int false_alarm	= 0;
    int unknown		= 0;

    int count = 0;
    foundmap::iterator it = found_by_prob.begin();;
    for(; it != found_by_prob.end(); ++it, count++)
    {
	// get reported result
	std::pair<int, int> rep = it->second;

	// get annotation
	annot_iter itr = annots.find(it->first);
	if(itr == annots.end())
	{
	    std::cout << "WARNING: an unknown (not annotated) alarm "
		      << "occurred in sentence at offset "
		      << it->first << " in pos " << rep.first
		      << "-" << rep.second << std::endl;
	    unknown++;
	    continue;
	}
	const Annot &a = itr->second;

	if(rep.first != PROBCHECK_OK &&
	   a.value[0] == FALSE_ALARM)
	    false_alarm++;
	else if(rep.first != PROBCHECK_OK &&
		(a.value[0] != PROBCHECK_OK ||
		 a.count > 1))
	{
	    int j;
	    bool found = false;
	    for(j = 0; j < a.count; j++)
	    {
		int v = a.value[j];
		if ((rep.first >= v - 1 &&	    // detect
		     rep.first <= v + 1) ||
		    (rep.second >= v - 1 &&
		     rep.second <= v + 1) ||
		    (rep.first <= v - 1 &&
		     rep.second >= v + 1))
		{
		    int val = inc_type_counts(type, type_c, flag,
					      type_count, it->first, j);
		    if(val == 1)
			detect++;
		    else if(val == 0)    // style
		    {}
		    else    // val == -1, error type
		    {}

		    found = true;
		}
		else if (rep.first == v - 2 ||	    // close to detect
		         rep.second == v + 2 ||
			 rep.first == v + 2 ||
		         rep.second == v - 2)
		{
		    std::cout << "NOTE: an alarm occurred "
			      << "in sentence at offset "
			      << it->first << " in pos " 
			      << rep.first << "-" << rep.second
			      << ", expected pos " 
			      << a.value[0] << std::endl;
		}
	    }

	    if(!found)
		false_alarm++;
	}
    }

    annot_iter ai = annots.begin();
    for(; ai != annots.end(); ++ai)
	if(ai->second.value[0] >= 0)
	{
	    if(inc_type_counts(type, type_tot, flag, type_count,
			       ai->first, 0, true) == 1)
		detect_known++;
		// += a.count; // fair, we detect one per row only!
	}


    out->push("results");

    out->push("summary");
    int style = type_c[5];
    out->add("method", model_name());
    out->add("error_thresh", config().thresh[0]);
    out->add("errors_known", detect_known);
    out->add("detected_errors", detect);
    out->add("style_errors", style);
    out->add("false_alarms", false_alarm);
    out->add("total_alarms", false_alarm + detect + style);
    if(unknown)
	out->add("not_annotated", unknown);
    out->add("coverage", double(detect) / detect_known * 100);
    out->add("precision", double(detect) / (detect + false_alarm) * 100);
    out->pop();  // push("summary");

    out->push("alarm_details");
    out->push("spelling_errors");
    out->add("total", type_c[4]);
    out->attr("cov", double(type_c[4]) / type_tot[4] * 100);
    out->add("sem_gram_err", type_c[1]);
    out->attr("cov", double(type_c[1]) / type_tot[1] * 100);
    out->add("verb_err", type_c[2]);
    out->attr("cov", double(type_c[2]) / type_tot[2] * 100);
    out->add("compound_err", type_c[3]);
    out->attr("cov", double(type_c[3]) / type_tot[3] * 100);
    int tmp = type_c[4] - type_c[1] - type_c[2] - type_c[3];
    out->add("other", tmp);
    out->attr("cov", double(tmp) / (type_tot[4] - type_tot[1] -
			           type_tot[2] - type_tot[3]) * 100);
    out->pop();  // push("spelling_errors");

    out->push("word_err");
    out->add("total", type_c[6] + type_c[7]);
    out->attr("cov", double(type_c[6] + type_c[7]) /
			   (type_tot[6] + type_tot[7]) * 100);
    out->add("missing", type_c[6]);
    out->attr("cov", double(type_c[6]) / type_tot[6] * 100);
    out->add("order", type_c[7]);
    out->attr("cov", double(type_c[7]) / type_tot[7] * 100);
    out->pop();  // push("word_err");

    out->push("misc");
    tmp = type_c[5] + type_c[8] + type_c[0] + type_c[9] + type_c[10];
    out->add("total", tmp);
    out->attr("cov", double(tmp) / (type_tot[5] + type_tot[8] + type_tot[0] +
		   	           type_tot[9] + type_tot[10]) * 100);
    out->add("style", type_c[5]);
    out->attr("cov", double(type_c[5]) / type_tot[5] * 100);
    out->add("foreign", type_c[8]);
    out->attr("cov", double(type_c[8]) / type_tot[8] * 100);
    out->add("tok_sent", type_c[9]);
    out->attr("cov", double(type_c[9]) / type_tot[9] * 100);
    out->add("bad_tag", type_c[10]);
    out->attr("cov", double(type_c[10]) / type_tot[10] * 100);
    out->add("other", type_c[0]);
    out->attr("cov", double(type_c[0]) /
		   	   (type_tot[0] ? type_tot[0] : 1) * 100);
    out->pop();  // push("misc");

    out->pop();  // push("alarm_details");
    
    out->push("rules");
    out->push("scrut_rules");
    rulemap::const_iterator iter = rules_scrut.begin();
    for(; iter != rules_scrut.end(); ++iter)
    {
	out->add("rule", iter->second);
	out->attr("name", iter->first.c_str());
    }
    out->pop();  // push("scrut_rules");

    out->push("rules_used_per_category");
    for(i = 0; i < type_count + 1; i++)
    {
	if(rules_used[i].begin() == rules_used[i].end())
	    continue;

	out->push("category");
	out->attr("name", type_names[i]);
	for(int j = 1; j <= 3; j++)
	{
	    out->add("usage", rules_used_count[i][j]);
	    out->attr("count", j);
	}
	iter = rules_used[i].begin();
	for(; iter != rules_used[i].end(); ++iter)
	{
	    out->add("rule", iter->first.c_str());
	    out->attr("count", iter->second);
	    rules_avail.erase(iter->first);
	}
	out->pop();  // push("category");
    }
    out->pop();  // push("rules_used_per_category");

    // print unused rules
    out->push("rules_not_used");
    {
	std::set<std::string>::const_iterator it = rules_avail.begin();
	for(; it != rules_avail.end(); ++it)
	    out->add("rule", it->c_str());
    }
    out->pop();  // push("rules_not_used");
    out->pop();  // push("rules");

    out->pop();  // push("results");

#if 0
    // show tags involved in false alarms
    {
	std::map<std::string, int> m;
	std::cout << "Tags involved in false alarms:" << std::endl;
	int j;
	for(j = 0; j < 149; j++)
	    m[tag_name[j]] = j;

	std::map<std::string, int>::iterator it = m.begin();
	for(; it != m.end(); ++it)
	{
	    j = it->second;
	    std::cout << tag_name[j] << "(" << j << "): "
		      << tags_in_false_alarms[j] << ", "
		      << tag_freq[j] << ", "
		      << (double)tags_in_false_alarms[j] / tag_freq[j] * 10000
		      << std::endl;
	}
    }
#endif

    std::ostringstream ostr;
    ostr  << model_name() << "-"
	  << config().thresh[0] << "-"
	  << config().g_no << "-" 
	  << config().g_coeff << "-"
	  << config().h_no << "-"
	  << config().h_coeff << ","
	  << detect << ","
	  << detect_known << ","
	  << false_alarm << ","
	  << not_annot.size() << ","
	  << type_c[5] << "," 		// style
	  << type_tot[5];
    std::cout << "rec:  "
	      << (double)100*(detect + type_c[5]) / (detect_known + type_tot[5])
	      << std::endl;
    std::cout << "prec: "
	      << (double)100*(detect + type_c[5]) /
	         (detect + type_c[5] + false_alarm + not_annot.size()) 
	      << std::endl;
    std::cout << "#out " << ostr.str();
#if 0
    std::cout << " : ";
    for(int j = 0; j < count; j++)
	std::cout << stat[j];
#endif
    
    std::cout << std::endl;




#ifdef USE_AS_CLIENT
    config().output[0] = detect;
    config().output[1] = detect_known;
    config().output[2] = false_alarm;
    config().output[3] = not_annot.size();
    config().output[4] = type_c[5];
    config().output[5] = type_tot[5];
#else
    #if 0
	std::ofstream s("c:\\johnny\\other\\prob_out\\data.txt");
	s << detect << " "
	  << detect_known << " "
	  << false_alarm << " "
	  << not_annot.size() << " "
	  << type_c[5] << " "
	  << type_tot[5];
    #endif
#endif

#if 0
    // determine intersection between probcheck and original scrut
    {
	typedef foundmap::const_iterator it_type;

	std::vector<int> p;
	std::vector<int> s;

	it_type mi;
	for(mi = found_by_prob.begin(); mi != found_by_prob.end(); ++mi)
	    p.push_back(mi->first);
	for(mi = found_by_scrut.begin(); mi != found_by_scrut.end(); ++mi)
	    s.push_back(mi->first);

	unsigned int i;
#if 0
	// really verbose
	for(i = 0; i < p.size(); i++)
	{
	    if(!((i + 1) % 20))
		std::cout << std::endl;
	    std::cout << p[i] << "("
		      << found_by_prob[p[i]] << "), ";
	}
        std::cout << std::endl;

	    // really verbose
	for(i = 0; i < s.size(); i++)
	{
	    if(!((i + 1) % 20))
		std::cout << std::endl;
	    std::cout << s[i] << "("
		      << found_by_scrut[s[i]] << "), ";
	}
        std::cout << std::endl;
#endif

	int overlap = 0;
	int additional = 0;	// in same sentence but different place
	std::vector<int> res;
	std::set_intersection(p.begin(), p.end(),
			      s.begin(), s.end(),
			      std::back_inserter(res));

	std::cout << "Found by prob:       " << p.size() << std::endl;
	std::cout << "Found by scrut:      " << s.size() << std::endl;
	std::cout << "Found by both:       ";

	// assess the overlap
	int typec[type_count];
	for(i = 0; i < type_count; i++)
	    typec[i] = 0;

	int false_alarms = 0;
	for(i = 0; i < res.size(); i++)
	{
	    int fp = found_by_prob[res[i]];
	    int fs = found_by_scrut[res[i]];
	    if(fp == fs || fp == fs - 1 || fp == fs + 1)
	    {
		overlap++;
		if(!((overlap + 1) % 20))
		    std::cout << std::endl;
		std::cout << res[i] << "("
    			  << fp << "), ";
		int index = find_annot(res[i], fp);
		if(index != -1)
		{
		    if(annots[res[i]].value[index] == FALSE_ALARM)
			false_alarms++;
		    else
			inc_type_counts(type, typec, flag, type_count,
		    			res[i], index);
		}
	    }
	    else if(fp == fs + 2 || fp == fs - 2)
		std::cout << "NOTE: prob pos=" << fp << ", scrut pos=" << fs 
			  << " in sentence at offset " << res[i]
			  << std::endl;
	    else
		additional++;
	}
	std::cout << std::endl;
	std::cout << "Found by both:       " << overlap << std::endl;

	res.clear();
	std::set_difference(p.begin(), p.end(),
			    s.begin(), s.end(),
			    std::back_inserter(res));
	std::cout << "Found only by prob:  " << res.size() + additional 
		  << " (incl. " << additional << " in same sent, diff pos)"
		  << std::endl;

	res.clear();
	std::set_difference(s.begin(), s.end(),
			    p.begin(), p.end(),
			    std::back_inserter(res));
	std::cout << "Found only by scrut: " << res.size() + additional 
		  << " (incl. " << additional << " in same sent, diff pos)" 
		  << std::endl;

	// output overlap statistics
	for(i = 0; i < type_count; i++)
	    std::cout << type_names[i] << ": "
		      << typec[i] << std::endl;
	std::cout << "false alarms: " << false_alarms << std::endl;
    }
#endif // #if 0
}
