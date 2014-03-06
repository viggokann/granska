#ifndef PROBCHECK_H
#define PROBCHECK_H

#pragma warning (disable: 4786)
#include "scrutinizer.h"
#include <vector>
#include <string>

#include "defines.h"
#include "config.h"


namespace Prob
{
    static int last_prob_err	= 0;

    class Info;
    class Match;
    typedef std::vector<Match> matchset;

    static void
    prob_check(const Info         &info,
               const matchset     &matchings,
               bool                clause_begin[],
               bool                clause_end[],
	       std::vector<int>	  &errs);
    static int
    prob_check(const Info     &info,
               int             begin,
               int             end,
               int             pos[]);


    static void
    get_matchings(const Info     &info,
                  matchset       &ret,
                  bool            clause_begin[],
                  bool            clause_end[]);
    static void 
    additional_matchings(const Info     &info,
                         matchset       &ret);
    static void 
    relevant_matchings(const Info         &info,
                       const matchset     &matchings,
                       int                 begin,
                       int                 end,
                       matchset           &ret);
    static void
    matching_distance_two(const Info         &info,
                          const matchset     &matchings,
                          int                &begin,
                          int                &end,
                          matchset           &ret);
    static void
    process_matchings(const Info         &info,
                      const matchset     &set,
                      int                 begin,
                      int                 end,
		      std::vector<int>   &errs);


#ifdef TRY_IMPROVE_TAGGER
    static int
    try_improve_tagger(const Info                 &info,
                       matchset                   &set,
                       const AbstractSentence     *sent);

    static void
    tagger_matchings(const Info         &info,
                     const matchset     &matchings,
                     matchset           &ret);
#endif

    static void
    analyze_sentence(const Info         &info,
                     bool                clause_begin[],
                     bool                clause_end[],
                     const matchset     &set);

    static void
    analyze_sentence_deep(const Info         &info,
                          bool                clause_begin[],
                          bool                clause_end[],
                          const matchset     &set);

    static int
    apply(const Info      &info,
          const Match     *m[],
          int              m_size,
          const char      *words_out[],
          const Tag       *tags_out[],
          int              remap[],
          int             &begin,
          int             &end,
          bool             silent = false);

    static void
    out_errors(const Info &info, const std::vector<int> &v);
}


#endif /* PROBCHECK_H */
