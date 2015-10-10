#ifndef MATCH_STATS_H
#define MATCH_STATS_H

#include <vector>
#include <string>
#include <functional>

#include "match.h"

namespace MatchStats {
double MatchWinProb(std::string player, const std::vector<Match> &matches);

double FindAverageOf(const std::vector<Match> &matches,
                     std::function<unsigned int(const Match &m)> f);

double AverageMatchLength(const std::vector<Match> &matches);

double AverageNumberOfSets(const std::vector<Match> &matches);
}

#endif
