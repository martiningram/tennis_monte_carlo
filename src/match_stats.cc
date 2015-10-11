#include "match_stats.h"
#include <numeric>

double MatchStats::FindAverageOf(
    const std::vector<Match> &matches,
    std::function<unsigned int(const Match &m)> f) {
  long long sum =
      std::accumulate(matches.begin(), matches.end(), 0,
                      [f](unsigned int i, const Match &m) { return i + f(m); });
  return static_cast<double>(sum) / matches.size();
}

double MatchStats::MatchWinProb(std::string player,
                                const std::vector<Match> &matches) {
  auto quantity =
      [player](const Match &m) { return (m.winner() == player) ? 1 : 0; };
  return FindAverageOf(matches, quantity);
}

double MatchStats::AverageMatchLength(const std::vector<Match> &matches) {
  auto quantity = [](const Match &m) { return m.total_games(); };
  return FindAverageOf(matches, quantity);
}

double MatchStats::AverageNumberOfSets(const std::vector<Match> &matches) {
  auto quantity = [](const Match &m) { return m.sets().size(); };
  return FindAverageOf(matches, quantity);
}

double MatchStats::PlayerServedFirst(std::string player,
                                     const std::vector<Match> &matches) {
  auto quantity = [player](const Match &m) {
    return (m.server_at_start() == player) ? 1 : 0;
  };
  return FindAverageOf(matches, quantity);
}
