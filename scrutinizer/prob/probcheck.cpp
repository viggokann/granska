#ifdef PROBCHECK

#pragma warning(disable: 4786)
#include "probcheck.h"
#include "wordtoken.h"
#include "sentence.h"
#include "annot.h"
#include "config.h"
#include "report.h"
#include "probadmin.h"
#include "probdevelop.h"
#include "misc/xmloutput.h" //Oscar, used for fixXML

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>

#ifndef IMPROVE_TAGGER
#pragma warning (disable: 4102)  // unused 'goto' labels
#endif


namespace Prob
{
    int sentence_offset = -1;

    struct Triple
    {
        Triple() : count(0), end(0), length(0) {}
        Triple add(const Prob::matchset &set, int i)
        {
            Triple t(*this);
            t.end		 = set[i].end;
            t.length		+= set[i].end - set[i].begin + 1;
            t.index[t.count++]	 = i;
            return t;
        }
        bool operator<(const Triple &t) const
        {
            return length < t.length || 
                (length == t.length && count < t.count);
        }

        int index[MAX_BLOCK];
        int count;
        int end;
        int length;
    };
}

int
Prob::prob_check(const Scrutinizer          *scrutinizer,
                 const AbstractSentence     *sent)
{
    WordToken *first = sent->GetWordToken(0 + 2);

    current_sentence = sent;

    // check if already calculated
    if(sentence_offset == first->Offset())
        return last_prob_err;

#ifdef DEVELOPER_OUTPUT
    if(config().xml_output)
        out = &output();
#endif // DEVELOPER_OUTPUT

    // init tokens
    const char *the_words[MAX_SENTENCE_LENGTH];
    const Tag  *the_tags[MAX_SENTENCE_LENGTH];
#ifdef NO_SENTENCE_DELIMITERS
    Info info(scrutinizer, sent->GetWordTokensAt(0 + 2), the_words, the_tags);
    info.size   = sent->NWords();
#else // NO_SENTENCE_DELIMITERS
    Info info(scrutinizer, sent->GetWordTokensAt(0), the_words, the_tags);
    info.size   = sent->NTokens();
#endif // NO_SENTENCE_DELIMITERS

    sentence_offset = first->Offset();
    info.sent_offset = sentence_offset;

    Output *force = &output();

    force->push("s");
    force->attr("ref", sentence_offset);
    force->add("words", info.size);

    // output sentence in clear text    
    
      {
      std::string s;
      for(int i = 0; i < info.size; i++)
      {
      s += info.tokens[i]->RealString();
      if(i < info.size - 1 && info.tokens[i]->HasTrailingSpace()) // jsh, fixed trailing spaces
      s += " ";
      }
  
      force->add("text",  Misc::fixXML(s.c_str()));
      }
    
    //Oscar, fixes output of original text, but oh no it's messing up bad
    //force->add("text", Misc::fixXML(sent->getOriginalText()));

    if(sent->IsHeading())
        force->add("heading");
    if(sent->EndsParagraph())
        force->add("paragraph");

    // get tokens and words
    force->push("contents");
    int i;
    for(i = 0; i < info.size; i++)
        {
            info.words[i] = info.tokens[i]->RealString();
            info.tags[i]  = info.tokens[i]->SelectedTag();
            force->add("w", Misc::fixXML(info.words[i]));
            force->attr("no", i);
            if(info.tokens[i]->Offset())
                out->attr("ref", info.tokens[i]->Offset() - sentence_offset);
#ifdef DEVELOPER_OUTPUT
            out->attr("tag", info.tags[i]->String());
#else  // DEVELOPER_OUTPUT
            force->attr("tag", info.tags[i]->Index());
#endif // DEVELOPER_OUTPUT
            //if(info.tokens[i]->LemmaString()[0] != '"')
            force->attr("lemma", Misc::fixXML(info.tokens[i]->LemmaString()));
            //else
            //   force->attr("lemma", "'");
        }
    for(i = info.size; i < MAX_SENTENCE_LENGTH; i++)
        {
            info.words[i] = 0;
            info.tags[i]  = 0;
        }
    force->pop();  // force->push("contents");

    if(config().repr_file)
        {
            // get matchings and clauses
            matchset	matchings;
            bool	clause_begin[MAX_SENTENCE_LENGTH];
            bool	clause_end[MAX_SENTENCE_LENGTH];

#ifdef IMPROVE_TAGGER
            matchset set;

            out->push("matchings");
            get_matchings(info, matchings, clause_begin, clause_end);
            tagger_matchings(info, matchings, set);
            out->pop(); // push("matchings");

            try_improve_tagger(info, set, sent);
            last_prob_err = PROBCHECK_OK;
            goto end;
#endif // IMPROVE_TAGGER

#ifndef DONT_USE_RULES
            out->push("matchings");
            get_matchings(info, matchings, clause_begin, clause_end);
            additional_matchings(info, matchings);
            out->pop(); // push("matchings");
#endif // DONT_USE_RULES
            analyze_sentence(info, clause_begin, clause_end, matchings);
            analyze_sentence_deep(info, clause_begin, clause_end, matchings);

            // apply models and merge last_prob_err
            std::vector<int> errs[MAX_MODELS];
            {
                int tmp_res[MAX_MODELS];	    // jb: will be removed, use errs instead
                int j;
                out->push("models");
                for(j = 0; j < config().model_c; j++)
                    {
                        tmp_res[j] = PROBCHECK_OK;
                        out->push("model");
                        out->attr("name", model_name(config().model[j]));
                        info.model = j;
                        prob_check(info, matchings, clause_begin, clause_end, errs[j]);

                        out_errors(info, errs[j]);

                        if(!errs[j].empty())
                            tmp_res[j] = *errs[j].begin();

                        out->add("result", tmp_res[j]);
                        out->pop();  // push("model")
                    }
                out->pop();  // push("models")

                last_prob_err = decision(tmp_res);
            }


#ifndef NO_SENTENCE_DELIMITERS
            for(int idx = 0; idx < errs[0].size(); idx++)
                {
                    int &r = errs[0][idx];

                    //report_tags(info, errs[0][idx], errs[0][idx]);

                    // remove sentence delimiters
                    r -= 2;

                    // stay within trigram boundaries, otherwise the mark()
                    // function will try to mark tokens outside the sentence
                    // in prob_err@prob: mark(X[err - 1] X[err] X[err + 1])
                    if(r <= 0)
                        r = 1;
                    else if(r >= sent->NWords())
                        r = sent->NWords() - 2;
                }
#endif // NO_SENTENCE_DELIMITERS

            if(errs[0].empty())
                last_prob_err = PROBCHECK_OK;
            else
                last_prob_err = *errs[0].begin();

            if(config().multiple_detect)
                {
                    // get continuous intervals from error vector: e.g. (0 1 2) (5) (7 8) 
                    std::vector<std::pair<int, int> > intv;
                    get_intervals(errs[0], intv);
                    out->push("final_result");
                    for(unsigned int i = 0; i < intv.size(); i++)
                        {
                            report(info, intv[i].first, intv[i].second);
                            out->add("err");
                            out->attr("begin", intv[i].first);
                            out->attr("end", intv[i].second);
                            out_type(info.sent_offset, intv[i].first, intv[i].second);
                        }
                    out->pop();  // push("final_result");
                }
            else
                {
                    // report last_prob_err
                    out->add("final_result", last_prob_err);
                    report(info, last_prob_err, last_prob_err);
                    out_type(info.sent_offset, last_prob_err, last_prob_err);
                }
            sentence_count++;
        }  // if(config().repr_file)


#ifdef IMPROVE_TAGGER
 end:
#endif // IMPROVE_TAGGER

    force->pop();  // push("s")

    return last_prob_err;
}

static void
Prob::prob_check(const Info           &in,
                 const matchset       &matchings,
                 bool                  clause_begin[],
                 bool                  clause_end[],
                 std::vector<int>     &ret_errs)

{
    int	 pos[MAX_SENTENCE_LENGTH];
    bool err[MAX_SENTENCE_LENGTH];

    Info info(in, 0, in.size - 1);    // change clause boundaries

    int n = prob_check(info, 1, info.size - 2, pos);
    {
        int i;
        for(i = 0; i < info.size; i++)
            err[i] = false;
        for(i = 0; i < n; i++)
            err[pos[i]] = true;
    }

    if(n)
        out->add("error_count", n);
    int last_prob_err = PROBCHECK_OK;

    // remove the probabilistic errors one by one
    int clause_from = 0;
    int clause_to   = -1;
    int begin = 0;
    int end = -1;
    std::set<int> all_errs;
    while(last_prob_err == PROBCHECK_OK)
        {
            // find contiguous block of probabilistic errors
            int i_start = end + 1;
            clause_to   = -1;
            begin	    = -1;
            end	    = -1;
            for(int i = i_start; i < info.size; i++)
                {
                    // passed clause boundary and no error found
                    if(begin == -1 && clause_begin[i])
                        clause_from = i;

                    // first error found
                    if(err[i] && begin == -1)
                        begin = i;

                    // clause boundary found after encountered error
                    if(begin != -1 && clause_begin[i + 1])
                        {
                            clause_to = i;
                            break;
                        }
	    
                    // the error found should be included or is too far away
                    if(err[i] && (end == -1 || end >= i - 3))
                        end = i;
                }
            if(begin == -1)
                break;

            if(clause_to == -1)
                clause_to = info.size - 1;
            if(end == -1)
                end = begin;

            if(begin == end &&
               (begin == clause_from || begin == clause_to))
                continue;   // phrase boundary, ignore

            // get matchings that affects found errors
            matchset set;
            relevant_matchings(info, matchings, begin, end, set);
            int tmp_begin = begin;
            int tmp_end   = end;
#if 0
            matching_distance_two(matchings, tags, clause_from, clause_to,
                                  begin, end, set);
#endif

            // find and remove the current error
            Info new_info(info, clause_from, clause_to);
            std::vector<int> errs(pos, pos + n);
            process_matchings(new_info, set, begin, end, errs);
            all_errs.insert(errs.begin(), errs.end());

            // output errors
            if(!errs.empty())
                {
                    out->push("report_errs");
                    for(unsigned int j = 0; j < errs.size(); j++)
                        out->add("err", errs[j]);
                    out->pop();  // push("errs");
                }

            begin = tmp_begin;
            end   = tmp_end;
        }
    
    //ret_errs.assign(all_errs.begin(), all_errs.end());    // jb: bad, MSVC does not support this!!!
    ret_errs.clear();
    ret_errs.reserve(all_errs.size());
    std::copy(all_errs.begin(), all_errs.end(), std::back_inserter(ret_errs));
}

static void output_matching(const char *name, int begin, int end)
{
    Prob::out->add("match", name);
    Prob::out->attr("begin", begin);
    Prob::out->attr("end", end);
}

static void
Prob::get_matchings(const Info     &info,
                    matchset       &ret,
                    bool            clbegin[],
                    bool            clend[])
{

    int i;
    for(i = 0; i < MAX_SENTENCE_LENGTH; i++)
        clbegin[i] = clend[i] = false;

    const Tag *mad = info.scrutinizer->Tags().Find("mad");
    const Tag *pad = info.scrutinizer->Tags().Find("pad");
    for(i = 0; i < MAX_SENTENCE_LENGTH; i++)
        {
            if(info.tags[i] == mad || info.tags[i] == pad)
                {
                    clbegin[i + 1] = true;
                    clend[i] = true;
                }
        }
    // insert a clause end after sentence end
    clend[info.size] = true;

#ifdef ONLY_ADDITIONAL_RULES
    return;
#endif
    
    const MatchingSet &m = info.scrutinizer->GetMatchingSet();
    for(int j = 0; j < m.NMatchings(); j++)
        if(const Matching *p = m.GetMatchings() + j)
            {
                if(p->GetRuleTerm()->IsHelp())
                    continue;

                const char *name = p->GetRule()->Name();
                std::string s(name);
                if(s.length() < 5 ||
                   s.substr(s.length() - 5) != "recog")
                    continue;

                int begin = p->Start();
                int end   = p->Start() + p->NTokens() - 1;

#ifdef NO_SENTENCE_DELIMITERS
                // compensate for delimiters preceding the sentence
                begin = (begin - 2 < 0 ? 0 : begin - 2);
                end   = (end - 2 < 0 ? 0 : end - 2);
#endif // NO_SENTENCE_DELIMITERS

                output_matching(name, begin, end);

#ifdef DEVELOPER_OUTPUT
                rules_avail.insert(name);
#endif // DEVELOPER_OUTPUT

                if(strcmp(name, "clbegin@clrecog") == 0)
                    {
                        clbegin[begin] = true;
                        continue;
                    }
                else if(strcmp(name, "clend@clrecog") == 0)
                    {
                        clend[end] = true;  // begin is equal to end
                        continue;
                    }

                Tag *t = get_tag(info, p);

                Match match(begin, end, name, t);
                ret.push_back(match);
            }

    // handle Vaux NP V construction
    for(unsigned int k = 0; k < ret.size(); k++)
        {
            if(strcmp(ret[k].name, "vb_chain1_vb@vbrecog") != 0)
                continue;

            for(unsigned int l = k + 1; l < ret.size(); l++)
                if(strcmp(ret[l].name, "vb_chain1_nn@vbrecog") == 0 &&
                   ret[k].begin == ret[l].begin &&
                   ret[k].end == ret[l].end)
                    {
                        // fill vb part with both tags
                        Match &vb = ret[k];
                        Match &nn = ret[l];

                        vb.tag_c = 2;
                        vb.tag[1] = (nn.tag_c ? nn.tag[0] : 0);

                        // don't use nn part alone
                        nn.tag_c = 0;
                        break;
                    }
        }
}

static void
Prob::additional_matchings(const Info     &info,
                           matchset       &set)
{
    const Tag *mid = info.l->Find("mid");
    const Tag *mad = info.l->Find("mad");
    const Tag *pad = info.l->Find("pad");
    const Tag *in  = info.l->Find("in");
    for(int i = 0; i < info.size; i++)
        {
            const char *name = info.tags[i]->String();

            if(info.tags[i] == mid)			// midremove
                {
                    Match m(i, i, "midremove@additional", 0, REMOVE);
                    set.push_back(m);
                    output_matching(m.name, m.begin, m.end);
                }
            else if(info.tags[i] == pad)			// padremove
                {
                    Match m(i, i, "padremove@additional", 0, REMOVE);
                    set.push_back(m);
                    output_matching(m.name, m.begin, m.end);
                }
            else if(info.tags[i] == mad)			// mad2kn
                {
                    Match m(i, i, "mad2kn@additional", info.l->Find("kn"), REPLACE);
                    set.push_back(m);
                    output_matching(m.name, m.begin, m.end);
                }
            else if(name[0] == 'r' &&
                    name[1] == 'o')			// roremove
                {
                    Match m(i, i, "roremove@additional", 0, REMOVE);
                    set.push_back(m);
                    output_matching(m.name, m.begin, m.end);
                }
            else if(name[0] == 'r' &&
                    name[1] == 'g')			// rgremove
                {
                    Match m(i, i, "rgremove@additional", 0, REMOVE);
                    set.push_back(m);
                    output_matching(m.name, m.begin, m.end);
                }
            else if(info.tags[i] == in)			// inremove
                {
                    Match m(i, i, "inremove@additional", 0, REMOVE);
                    set.push_back(m);
                    output_matching(m.name, m.begin, m.end);
                }

#ifdef ONLY_ADDITIONAL_RULES
            else if(name[0] == 'a' &&
                    name[1] == 'b')			// abremove
                {
                    Match m(i, i, "abremove@additional", 0, REMOVE);
                    set.push_back(m);
                    output_matching(m.name, m.begin, m.end);
                }
#endif // ONLY_ADDITIONAL_RULES

#ifndef NO_SENTENCE_DELIMITERS
            else if(i == info.size - 2 && strncmp(name, "sen", 3) == 0)
                { 					// sen2mad
                    Match m(i, i, "sen2mad@additional", info.l->Find("mad"), REPLACE);
                    set.push_back(m);
                    output_matching(m.name, m.begin, m.end);
                    // sen2kn
                    Match n(i, i, "sen2kn@additional", info.l->Find("kn"), REPLACE);
                    set.push_back(n);
                    output_matching(n.name, n.begin, n.end);
                }
#endif // NO_SENTENCE_DELIMITERS
        }
}

static void
Prob::relevant_matchings(const Info         &info,
                         const matchset     &matchings,
                         int                 from,
                         int                 to,
                         matchset           &ret)
{
    out->push("relevant_matchings");

    matchset::const_iterator i = matchings.begin();
    for(; i != matchings.end(); i++)
        if(info.clause_from <= i->begin && info.clause_to >= i->end &&
           i->begin - PROXIMITY <= to && i->end + PROXIMITY >= from)
            {
                output_matching(i->name, i->begin, i->end);

                Match m(*i);
                m.type = m.applicable(from, to, info.tags[i->begin], false);
                if(m.type == REPLACE)
                    out->attr("action", "replace");
                else if(m.type == REMOVE)
                    out->attr("action", "remove");

                if(m.type != NOT_APPLICABLE)
                    ret.push_back(m);
            }
    out->pop();  // push("matchings_used");
}

static void
Prob::matching_distance_two(const Info         &info,
                            const matchset     &matchings,
                            int                &from,
                            int                &to,
                            matchset           &ret)
{
    out->push("matchings_used");
    out->add("comment", "distance two");

    bool begins[MAX_SENTENCE_LENGTH];
    bool ends[MAX_SENTENCE_LENGTH];

    for(int i = 0; i < MAX_SENTENCE_LENGTH; i++)
        begins[i] = ends[i] = false;

    matchset::const_iterator it = ret.begin();
    for(; it != ret.end(); ++it)
        {
            begins[it->begin]   = true;
            ends[it->end]	    = true;
        }
    
    it = matchings.begin();
    for(; it != matchings.end(); ++it)
        {
            if(it->begin < info.clause_from ||
               it->end > info.clause_to)
                continue;

            // not applicable, continue
            if(!begins[it->end + 1] &&
               (it->begin - 1 < 0 || !ends[it->begin - 1]))
                continue;

            // check if already present
            matchset::const_iterator iter = ret.begin();
            for(; iter != ret.end(); ++iter)
                if(iter->begin == it->begin &&
                   iter->end == it->end &&
                   strcmp(iter->name, it->name) == 0)
                    break;	// found
    
            // not already present in matchings to be used, add
            if(iter == ret.end())
                {
                    Match m(*it);
                    m.type = m.applicable(from, to, info.tags[it->begin], true);
    
                    if(m.type == NOT_APPLICABLE)
                        continue;

                    ret.push_back(m);
                    output_matching(it->name, it->begin, it->end);
                    from = (it->begin < from ? it->begin : from);
                    to   = (it->end > from ? it->end : to);
                }
        }
    out->pop();  // push("matchings_used");
}


#ifdef IMPROVE_TAGGER
static void
Prob::tagger_matchings(const Info     &info,
                       matchset       &ret)
{
    out->push("matchings_used");
    out->add("comment", "improve tagger");

    matchset::const_iterator i = matchings.begin();
    for(; i != matchings.end(); i++)
        if(0 <= i->begin && info.size - 1 >= i->end)
            {
                output_matching(i->name, i->begin, i->end);

                Match m(*i);
                m.type = m.tagger_applicable(info.tags[i->begin], false);
                if(m.type == REPLACE)
                    out->attr("action", "replace");
                else if(m.type == REMOVE)
                    out->attr("action", "remove");

                if(m.type != NOT_APPLICABLE)
                    ret.push_back(m);
            }
    out->pop();  // push("matchings_used");
}
#endif // IMPROVE_TAGGER

static void
build_covers(const Prob::matchset          &set,
             int                            from,
             int                            to,
             std::vector<Prob::Triple>     &cover)
{
    using namespace Prob;

    // create matching beginnings
    int	win_size = to - from + WINDOW_SIZE + 2*(PROXIMITY - 1);
    if(win_size > MAX_BLOCK)
        win_size = MAX_BLOCK;

    typedef int	     array_t[MAX_MATCHINGS];
    array_t	    *begin = new array_t[win_size];
    int		    *begin_c = new int[win_size];

#ifdef IMPROVE_TAGGER

    int i;
    int pos;
    // find matching beginnings in window pos 0...n-1
    for(pos = 0; pos < win_size; pos++)
        {
            begin_c[pos] = 0;
            for(i = 0; i < set.size(); i++)
                if(set[i].begin == from + pos)
                    begin[pos][begin_c[pos]++] = i;
        }

    cover.resize(1);
    for(pos = 0; pos < win_size; pos++)
        if(begin_c[pos] != 0)
            {
                cover[0] = cover[0].add(set, begin[pos][0]);
                pos = cover[0].end;
            }
    if(cover[0].count == 0)
        cover.clear();

#else // IMPROVE_TAGGER

    cover.reserve(32);

    unsigned int i;
    int pos;
    // find matching beginnings in window pos 1...n-1
    for(pos = 1; pos < win_size; pos++)
        {
            begin_c[pos] = 0;
            for(i = 0; i < set.size(); i++)
                if(set[i].begin == from + pos - PROXIMITY)
                    begin[pos][begin_c[pos]++] = i;
        }

    // find matchings covering window pos 0
    begin_c[0] = 0;
    for(i = 0; i < set.size(); i++)
        if (set[i].begin <= from - PROXIMITY &&
            set[i].end >= from - PROXIMITY)
            cover.push_back(Triple().add(set, i));

    // create coverings
    for(pos = 1; pos < win_size; pos++)
        {
            // for each, add matchings beginning at the end of c
            int sz = cover.size();
            for(int i = 0; i < sz; i++)
                {
                    if(cover[i].end > to + PROXIMITY ||
                       cover[i].end >= from + pos - PROXIMITY)
                        continue;
                    // jb: note: do not use Triple &t=cover[i] here
                    // since it will change when cover resizes!
                    for(int k = 0; k < begin_c[pos]; k++)
                        cover.push_back(cover[i].add(set, begin[pos][k]));
                }
            // add one for the empty set
            for(int k = 0; k < begin_c[pos]; k++)
                cover.push_back(Triple().add(set, begin[pos][k]));
        }
    std::sort(cover.begin(), cover.end());

#endif // IMPROVE_TAGGER

    out->push("matchings_used");
    out->add("tokens");
    out->attr("begin", from);
    out->attr("end", to);
    for(i = 0; i < cover.size(); i++)
        {
            out->push("comb");
            Triple &t = cover[i];
            for(int j = 0; j < t.count; j++)
                {
                    const Match &m = set[t.index[j]];
                    out->add("match");
                    out->attr("begin", m.begin);
                    out->attr("end", m.end);
                }
            out->pop();  // push("comb");
        }
    out->pop();  // push("matchings_used");
    delete [] begin;
    delete [] begin_c;
}

static void
Prob::process_matchings(const Info           &info,
                        const matchset       &set,
                        int                   from,
                        int                   to,
                        std::vector<int>     &errs)
{
    std::vector<Triple> cover;
    build_covers(set, from, to, cover);
    
    out->push("modify");

    out->add("tokens");
    out->attr("begin", from);
    out->attr("end", to);

    out->add("clause");
    out->attr("begin", info.clause_from);
    out->attr("end", info.clause_to);

    for(unsigned int i = 0; i < cover.size(); i++)
        {
            // build matching list
            const Match *m[MAX_BLOCK];
            for(int j = 0; j < cover[i].count; j++)
                {
                    int k = cover[i].index[j];
#if 0
                    out->add("rule", set[k].name);
                    out->attr("begin", set[k].begin);
                    out->attr("end", set[k].end);

                    if(set[k].type == REPLACE)
                        {
                            out->push("replace");
                            for(int l = 0; l < set[k].tag_c; l++)
                                out->add("tag", set[k].tag[l]->String());
                            out->pop();  // push("replace");
                        }
                    else if(set[k].type == REMOVE)
                        out->add("remove");
#endif

                    m[j] = &set[k];
                }

            // apply replacements and eliminations
            const char	*new_words[MAX_SENTENCE_LENGTH];
            const Tag	*new_tags[MAX_SENTENCE_LENGTH];
            int		 remap[MAX_SENTENCE_LENGTH];

            int	begin = from;
            int	end = to;
            int	new_to = apply(info, m, cover[i].count,
                               new_words, new_tags, remap, begin, end);

            // check only erroneous part
            int   pos[MAX_SENTENCE_LENGTH];
            Info  new_info(info, info.clause_from, new_to, new_tags);
            int   n = prob_check(new_info, begin, end, pos);
            if(n == 0)
                {
                    report_rules(m, cover[i].count, info.sent_offset, from, to);
                    errs.clear();
                    goto end;
                }

            // output errors
            {
                out->push("remain_errs");
                for(int j = 0; j < n; j++)
                    {
                        out->add("err");
                        out->attr("old", pos[j]);
                        out->attr("new", remap[pos[j]]);
                    }
                out->pop();  // push("errs");
            }

            if(n < (int)errs.size())
                {
                    errs.clear();
                    for(int j = 0; j < n; j++)
                        errs.push_back(remap[pos[j]]);
                }
        }

 end:
    out->pop();  // push("modify");
}

#ifdef IMPROVE_TAGGER
static int
Prob::try_improve_tagger(const Info                 &info,
                         matchset                   &set,
                         const AbstractSentence     *sent)
{
    std::vector<Triple> cover;
    build_covers(set, 0, size - 1, cover);
    
    for(int i = 0; i < cover.size(); i++)
        {
            // build matching list
            const Match *m[MAX_BLOCK];

            for(int j = 0; j < cover[i].count; j++)
                {
                    int k = cover[i].index[j];
                    m[j] = &set[k];
                }

            // apply replacements and eliminations
            const char	*new_words[MAX_SENTENCE_LENGTH];
            const Tag	*new_tags[MAX_SENTENCE_LENGTH];
            int		 remap[MAX_SENTENCE_LENGTH];

            int	begin = 0;
            int	end = info.size - 1;
            Info    new_info(info, 0, size - 1);
            int	new_to = apply(new_info, m, cover[i].count,
                               new_words, new_tags, remap,
                               begin, end, true);

            // tag the sentence again
            int k;
            {
                DynamicSentence s(sent);

                bool used[MAX_SENTENCE_LENGTH];
                for(k = 0; k < size; k++)
                    used[k] = false;
                for(k = 0; k < new_to; k++)
                    used[remap[k]] = true;
                for(k = size - 1; k >= 0; k--)
                    if(!used[k])
                        s.Delete(k + 2);

                int dummy = 0;
                s.TagMe();

                for(k = 0; k < new_to; k++)
                    new_tags[k] = s.GetWordToken(k + 2)->SelectedTag();
            }


            bool found = false;
            for(k = 0; k < new_to; k++)
                if(new_tags[k] != tags[remap[k]])
                    {
                        found = true;
                        std::cout << "     %% " << remap[k] << ": ny tagg '"
                                  << new_words[k] << " <" << new_tags[k]->String()
                                  << ">' skiljer sig från gammal tagg '"
                                  << words[remap[k]] << " <"
                                  << tags[remap[k]]->String()
                                  << ">'" << std::endl;
                    }
            if(!found)	// all tags the same
                continue;
            for(k = 0; k < info.size; k++)
                {
                    // find index for remapped index
                    int map;
                    for(map = 0; map < new_to; map++)
                        if(remap[map] == k)
                            break;
                    if(map == new_to)	// not remapped
                        map = k;
                    else if(new_tags[map] != tags[k])
                        {
                            std::cout << "      ->" << k << " " << new_words[map]
                                      << " <" << new_tags[map]->String()
                                      << ">, gammal tagg var <"
                                      << tags[k]->String() << ">"
                                      << std::endl;
                            continue;
                        }
                    std::cout << "        " << k << " " << words[k]
                              << " <" << tags[k]->String()
                              << ">" << std::endl;
                }
        }
    return 0;
}
#endif // IMPROVE_TAGGER

static int
Prob::prob_check(const Info     &info,
                 int             begin,
                 int             end,
                 int             pos[])
{
    double	min = 1e100;
    double      first = 1e100;
    int		min_index = 0;
    uchar	index[5];
    int		count = 0;
    double	thresh = config().thresh[info.model];
    pos[0] = -1;

    const bool print = false;

    out->push("probcheck");
    out->attr("begin", begin);
    out->attr("end", end);
    out->attr("clause_from", info.clause_from);
    out->attr("clause_to", info.clause_to);

    int i;
    if(end < begin)
        {
            std::ostringstream fs;
            fs << "slutord " << end << " är mindre än startord " << begin;
            out->add("note", fs.str().c_str());
            goto end;
        }

    for(i = begin; i <= end; i++)
        {
            index[0] = (i - 2 >= 0 && info.tags[i - 2] ?	    // jb: CHANGE BACK TO i - 2 >= 0!!!
                        info.tags[i - 2]->Index() : 255);
            index[1] = info.tags[i - 1]->Index();
            index[2] = info.tags[i]->Index();
            index[3] = info.tags[i + 1]->Index();
            index[4] = (i + 2 < info.size && info.tags[i + 2] ?
                        info.tags[i + 2]->Index() : 255);

            double p = analyze(*info.l, index, info.model, print);
            if(p < min)
                {
                    min = p;
                    min_index = i;
                }
            if(p < thresh)
                {
                    if(count == 0)
                        first = p;
                    pos[count++] = i;
                }
        }

    if(min < thresh)
        {
            out->push("result");
            out->attr("stat", "susp");
            out->add("ttt_freq", first);
            out->add("thresh", thresh);
            out->add("pos", pos[0]);
            out->pop();  // push("result");
        }
    else
        {
            out->push("result");
            out->attr("stat", "ok");
            out->add("ttt_freq", min);
            out->add("thresh", thresh);
            out->add("pos", min_index);
            out->pop();  // push("result");
        }

 end:
    out->pop();  // push("probcheck");

    return count;
}

static int
copy_range(const char	   *words[], 
           const Tag	   *tags[], 
           int		    from,
           int		    to,
           const char 	   *words_out[],
           const Tag 	   *tags_out[],
           int		    remap[],
           int	    	    dest,
           bool		    silent)
{
    for(int i = 0; from + i <= to; i++)
        {
            int l = from + i;
            words_out[dest + i]	= words[l];
            tags_out[dest + i]	= tags[l];
            remap[dest + i]		= l;

            if(!silent && tags[l])	    // check since we copy null tags too
                {
                    Prob::out->add("w", words[l]);
                    Prob::out->attr("old", l);
                    Prob::out->attr("new", dest + i);
                    Prob::out->attr("tag", tags[l]->String());
                }
        }
    return to - from + 1;
}

static int
Prob::apply(const Info            &info,
            const Prob::Match     *m[],
            int                    m_size,
            const char            *words_out[],
            const Tag             *tags_out[],
            int                    remap[],
            int                   &begin,
            int                   &end,
            bool                   silent)
{
    int index = info.clause_from;

    if(config().clause_delimits_context)
        {
            if(index > 0)
                tags_out[index - 1] = 0;
        }
    else
        {
            // copy tags from 0 to clause_from
            copy_range(info.words, info.tags, 0, info.clause_from - 1,
                       words_out, tags_out, remap, 0, /*true*/silent);
        }

    // copy tags up to first matching in set
    index += copy_range(info.words, info.tags, info.clause_from, m[0]->begin - 1,
                        words_out, tags_out, remap, index, silent);

    begin = (index < begin ? index : begin);

    int i;
    for(i = 0; i < m_size; i++)
        {
            if(m[i]->type == REPLACE)
                {
                    // find representative word within matching
                    for(int k = 0; k < m[i]->tag_c; k++)
                        {
                            const char *tag = 0;
                            int j;
                            for(j = m[i]->begin; j <= m[i]->end; j++)
                                if(info.tags[j]->String() == m[i]->tag[k]->String())
                                    break;
                            if(j != m[i]->end + 1)
                                {
                                    tag = info.tags[j]->String();
                                    words_out[index] = info.words[j];
                                }
                            else
                                {
                                    tag = m[i]->tag[k]->String();
                                    words_out[index] = "";
                                }
                            if(words_out[index] == info.words[j])
                                out->add("w", info.words[j]);
                            else
                                out->add("w");
                            out->attr("begin", m[i]->begin);
                            out->attr("end", m[i]->end);
                            out->attr("tag", tag);
                            out->attr("action", "replace");

                            tags_out[index] = m[i]->tag[k];
                            remap[index]    = (m[i]->begin + m[i]->end) / 2;  // heuristic
                            index++;
                        }
                }
            else if(m[i]->type == REMOVE)
                {
                    out->add("removed");
                    out->attr("begin", m[i]->begin);
                    out->attr("end", m[i]->end);
                }
            if(i < m_size - 1)	// copy tags between two matchings
                index +=
                    copy_range(info.words, info.tags, m[i]->end + 1, m[i + 1]->begin - 1,
                               words_out, tags_out, remap, index, silent);
            else
                {
                    // last matching, copy to end of clause
                    int last = index;
                    index += copy_range(info.words, info.tags, m[i]->end + 1, info.clause_to,
                                        words_out, tags_out, remap, index, silent);
                    end = (m[i]->end > end ? last - 1 :
                           end + (index - info.clause_to - 1));

                    if(config().clause_delimits_context)
                        {
                            tags_out[index] = 0;
                        }
                    else
                        {
                            // also, copy remaining tokens
                            copy_range(info.words, info.tags, info.clause_to + 1, info.size - 1 + 2,
                                       words_out, tags_out, remap, index, /*true*/silent);
                        }
                }
        }

    // if all errs are eliminated, check something anyway
    if(end < begin)
        begin = end;

    // avoid trigram boundaries
    if(begin < info.clause_from + 1)
        begin = info.clause_from + 1;
    else if(begin > index - 2)
        begin = index - 2;

    if(end < info.clause_from + 1)
        end = info.clause_from + 1;
    else if(end > index - 2)
        end = index - 2;

    // this is actually necessary since begin/end may have moved
    if(begin < info.clause_from + 1 || begin > index - 2 ||
       end < info.clause_from + 1 || end > index - 2)
        {
            begin = 10;  // will not be not checked
            end = 0;
        }

    return index - 1;
}

static bool 
analysis(const char *name, const char *&ret)
{
    const char *categ = strchr(name, '@');
    categ++;	// step past '@'
    
    if(strcmp(categ, "additional") == 0)
        return false;
    else if(strcmp(categ, "pprecog") == 0)
        ret = "PP";
    else if(strcmp(categ, "jjrecog") == 0)
        ret = "AP";
    else if(strcmp(categ, "abrecog") == 0)
        ret = "ADVP";
    else if(strcmp(name, "np_relclause@nprecog") == 0)
        ret = "NPrel";
    else if(strcmp(name, "np_predattr@nprecog") == 0)
        ret = "NPpred";
    else if(strcmp(name, "np_min@nprecog") == 0)
        ret = "NPmin";
    else if(strcmp(name, "np_hp@nprecog") == 0)
        ret = "NPhp";
    else if(strcmp(name, "np_comp@nprecog") == 0)
        ret = "NPcomp";
    else if(strcmp(name, "np_app@nprecog") == 0)
        ret = "NPapp";
    else if(strcmp(name, "np_pm@nprecog") == 0)
        ret = "NPpm";
    else if(strcmp(name, "np_jj@nprecog") == 0)
        ret = "NPjj";
    else if(strcmp(categ, "nprecog") == 0 &&
            strncmp(name, "np_inf", 6) == 0)
        ret = "NPinf";
    else if(strcmp(categ, "nprecog") == 0)
        ret = "NP***";
    else if(strcmp(categ, "vbrecog") == 0 &&
            strncmp(name, "vb_inf", 6) == 0)
        ret = "VBinf";
    else if(strcmp(categ, "vbrecog") == 0 &&
            strncmp(name, "vb_chain1_vb", 12) == 0)
        ret = "VBaux_NP_VB";
    else if(strcmp(categ, "vbrecog") == 0 &&
            strncmp(name, "vb_chain1_nn", 12) == 0)
        return false; // don't use vb_aux NP vb constructions
    else if(strcmp(categ, "vbrecog") == 0)
        ret = "VB";
    else
        {
            std::string s = name;

            // all other recog rules
            if(s.length() >= 5 &&
               s.substr(s.length() - 5) == "recog")    
                ret = name;
            else
                {
                    // it's a scrut rule like 'sär@särskrivn'
                    // do not include
                    return false;
                }
        }
    return true;
}

void
Prob::analyze_sentence(const Info         &info,
                       bool                clause_begin[],
                       bool                clause_end[],
                       const matchset     &set)
{
    std::vector<const char *> constit;	// constituent list
    std::vector<int> beg;
    std::vector<int> end;
    std::vector<Match *> match;
    std::vector<Match> m(set);

    std::sort(m.begin(), m.end());

    for(int i = 0; i < info.size; i++)
        {
            if(clause_begin[i])
                {
                    constit.push_back("CL");
                    beg.push_back(i);
                    end.push_back(i);
                    match.push_back(0);
                }
            matchset::reverse_iterator it = m.rbegin();
            for(; it != m.rend(); it++)
                {
                    if(it->begin == i)
                        {
                            const char *name  = it->name;
                            const char *an;
                            if(!analysis(name, an))
                                continue;
		
                            i = it->end;
                            constit.push_back(an);
                            beg.push_back(it->begin);
                            end.push_back(it->end);
                            match.push_back(&*it);
                            break;
                        }
                }

            // no matching found, output the tag itself
            if(it == m.rend())
                {
                    constit.push_back(info.tags[i]->String());
                    beg.push_back(i);
                    end.push_back(i);
                    match.push_back(0);
                }

        }

    out->push("analysis");
    out->push("clause");
    unsigned int j;
    for(j = 0; j < constit.size(); j++)
        {
            if(strcmp(constit[j], "CL") == 0)
                {
                    if(j != 0)
                        {
                            out->pop();  // push("clause");
                            out->push("clause");
                        }
                }
            else
                {
                    // words contained in phrase
                    {
                        std::string s;
                        for(int k = beg[j]; k <= end[j]; k++)
                            {
                                s += info.words[k];
                                if(k < end[j])
                                    s += " ";
                            }
                        out->add("ph", Misc::fixXML(s.c_str()));
                    }
                    out->attr("type", constit[j]);
                    if(beg[j] == end[j])
                        out->attr("pos", beg[j]);
                    else
                        {
                            out->attr("begin", beg[j]);
                            out->attr("end", end[j]);
                        }
                    if(match[j] &&		    // add tag list
                       match[j]->tag_c != 0 &&
                       (beg[j] != end[j] ||	    // don't show singleton repr,
                        match[j]->tag_c > 1))	    // exception: vb_aux NP vb
                        {
                            std::string s;
                            for(int k = 0; k < match[j]->tag_c; k++)
                                {
                                    const Tag *t = match[j]->tag[k];
                                    if(t)
                                        s += match[j]->tag[k]->String();
                                    if(k < match[j]->tag_c - 1)
                                        s += ", ";
                                }
                            out->attr("repr", s.c_str());
                        }
                }
        }
    out->pop();  // push("clause");
    out->pop();  // push("analysis");
}


static std::string
remove_nl(std::string s)
{
    std::string::size_type pos = s.find('\n');
    if(pos == std::string::npos)
        return s;

    std::string r(s);
    r.replace(pos, 1, " ");
    return r;
}


static bool 
analysis_deep(const char *name, const char *&ret)
{
    const char *categ = strchr(name, '@');
    categ++;	// step past '@'
    
    if(strcmp(categ, "additional") == 0)
        return false;
    else if(strcmp(categ, "pprecog") == 0)
        ret = "PP";
    else if(strcmp(categ, "jjrecog") == 0)
        ret = "APMIN";
    else if(strcmp(categ, "abrecog") == 0)
        ret = "ADVP";
    else if(strcmp(name, "np_relclause@nprecog") == 0)
        ret = "NP";
    else if(strcmp(name, "np_predattr@nprecog") == 0)
        ret = "NP";
    else if(strcmp(name, "np_min@nprecog") == 0)
        ret = "NP";
    else if(strcmp(name, "np_hp@nprecog") == 0)
        ret = "NP";
    else if(strcmp(name, "np_comp@nprecog") == 0)
        ret = "NP";
    else if(strcmp(name, "np_app@nprecog") == 0)
        ret = "NP";
    else if(strcmp(name, "np_pm@nprecog") == 0)
        ret = "NP";
    else if(strcmp(name, "np_jj@nprecog") == 0)
        ret = "NP";
    else if(strcmp(categ, "nprecog") == 0 &&
            strncmp(name, "np_inf", 6) == 0)
        ret = "INFP";
    else if(strcmp(categ, "nprecog") == 0)
        ret = "NP";
    else if(strcmp(categ, "vbrecog") == 0 &&
            strncmp(name, "vb_inf", 6) == 0)
        ret = "VC";
    else if(strcmp(categ, "vbrecog") == 0 &&
            strncmp(name, "vb_chain1_vb", 12) == 0)
        ret = "VC";
    else if(strcmp(categ, "vbrecog") == 0 &&
            strncmp(name, "vb_chain1_nn", 12) == 0)
        return false; // don't use vb_aux NP vb constructions
    else if(strcmp(categ, "vbrecog") == 0)
        ret = "VC";
    else
        return false; // it's another recog rule or scrut rule like 'sär@särskrivn'

    return true;
}

void
Prob::analyze_sentence_deep(const Info         &info,
                            bool                clause_begin[],
                            bool                clause_end[],
                            const matchset     &set)
{
    if(!config().bio)
        return;

    out->push("bio");

    std::vector<const char *> constit;	// constituent list
    std::vector<int> beg;
    std::vector<int> end;
    std::vector<Match *> match;
    std::vector<Match> m(set);

    std::sort(m.begin(), m.end());

    static struct Local
    {
        bool inside(int i, int beg, int end) { return i >= beg && i <= end; }
    } local;

    matchset::reverse_iterator it = m.rbegin();
    //out->push("found");
    for(; it != m.rend(); it++)
        {
            //out->add("rule");
            //out->attr("name", it->name);
            //out->attr("beg", it->begin);
            //out->attr("end", it->end);

            // must not overlap previously found match
            unsigned int j;
            for(j = 0; j < beg.size(); j++)
                if((local.inside(it->begin, beg[j], end[j]) &&
                    !local.inside(it->end, beg[j], end[j])) ||
                   (!local.inside(it->begin, beg[j], end[j]) &&
                    local.inside(it->end, beg[j], end[j])))
                    break;
            if(j < beg.size())
                {
                    //out->attr("status", "reject");
                    continue;
                }

            //out->attr("status", "accept");

            const char *name = it->name;
            const char *an;
            if(!analysis_deep(name, an))
                continue;
	
            constit.push_back(an);
            beg.push_back(it->begin);
            end.push_back(it->end);
            match.push_back(&*it);
        }
    //out->pop(); //push("found");

    // word, tag, phrase, clause
    enum { BIO_WORD = 0, BIO_TAG, BIO_PHRASE, BIO_CLAUSE, BIO_COUNT };
    std::vector<std::vector<std::string> > data(info.size, std::vector<std::string>(BIO_COUNT));

    const char *outside = "0";
    unsigned int j;
    for(j = 0; j < data.size(); j++)
        //for(j = data.size() - 1; j >= 0 && j < data.size(); j--)
        {
            data[j][BIO_WORD]   = info.words[j];
            data[j][BIO_TAG]    = info.tags[j]->String();
            data[j][BIO_PHRASE] = outside;
            data[j][BIO_CLAUSE] = clause_begin[j] ? "CLB" : "CLI";
            for(unsigned int i = 0; i < constit.size(); i++)
                {
                    if(!(beg[i] <= (int)j && end[i] >= (int)j))
                        continue;

                    std::string type  = constit[i];
                    if(!match[i]/* ||
                                   (type != "NP" && type != "VC" && type != "PP" &&
                                   type != "ADVP" && type != "ADJP")*/)
                        continue;

                    //j = end[i];
                    //j = beg[i];
                    if(data[j][BIO_PHRASE] == outside)
                        data[j][BIO_PHRASE] = "";
                    if(beg[i] == (int)j)
                        data[j][BIO_PHRASE] = type + "B|" + data[j][BIO_PHRASE];
                    else
                        data[j][BIO_PHRASE] = type + "I|" + data[j][BIO_PHRASE];
                }
        }
    data[0][BIO_CLAUSE] = "CLB";

    for(j = 0; j < data.size(); j++)
        {
            std::ostringstream os;
            for(unsigned int i = 0; i < data[j].size(); i++)
                {
                    if(i != 0)
                        os << "\t";
                    if(i != BIO_PHRASE)
                        os << data[j][i];
                    else
                        {
                            if(data[j][i] == outside)
                                os << data[j][i];
                            else
                                os << data[j][i].substr(0, data[j][i].length() - 1);  // remove trailing | (pipe)
                        }
                }
            //os << std::endl;
            out->add("row", Misc::fixXML(remove_nl(os.str()).c_str()));
        }
    out->add("row", "");
    out->pop();  // push("bio");
}

int Prob::last_prob_error()
{
    return last_prob_err;
}

void Prob::out_errors(const Info &info, const std::vector<int> &v)
{
#ifdef DEVELOPER_OUTPUT
    // output errors
    out->push("errors_found");
    for(int i = 0; i < info.size; i++)
        {
            unsigned int k;
            for(k = 0; k < v.size(); k++)
                if(v[k] == i)
                    break;
            if(k == v.size())  // no exact match found
                for(k = 0; k < v.size(); k++)
                    if(v[k] == i + 1 ||
                       v[k] == i - 1)
                        break;
            std::ostringstream os;
            os << info.words[i] << "\t" 
               << info.tags[i]->String() << "\t" 
               << (k == v.size() ? "ok" : (v[k] == i ? "err_center" : "err"));
            out->add("row_err", Misc::fixXML(os.str().c_str()));
        }
    out->add("row_err", "");
    out->pop();  // push("errors_found");
#endif // DEVELOPER_OUTPUT
}


#endif // ifdef PROBCHECK (topmost)


