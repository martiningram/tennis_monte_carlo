#ifndef MATCH_STATS_H
#define MATCH_STATS_H

#include <vector>
#include <string>
#include <functional>
#include <map>

#include "match.h"

namespace MatchStats {

struct SummaryStats {
  const std::map<std::string, double> p_win;
  const double average_length;
  const double average_num_sets;

  SummaryStats(const std::map<std::string, double> p_win_,
               const double average_length_, const double average_num_sets_)
      : p_win(p_win_),
        average_length(average_length_),
        average_num_sets(average_num_sets_) {}
};

double MatchWinProb(std::string player, const std::vector<Match> &matches);

double FindAverageOf(const std::vector<Match> &matches,
                     std::function<unsigned int(const Match &m)> f);

double AverageMatchLength(const std::vector<Match> &matches);

double AverageNumberOfSets(const std::vector<Match> &matches);

double PlayerServedFirst(std::string player, const std::vector<Match> &matches);

SummaryStats Summarise(const std::vector<Match> &matches);

std::map<std::string, std::map<unsigned int, double>> NumberBreakPointsHist(
    const std::vector<Match> &matches);
}

#endif
