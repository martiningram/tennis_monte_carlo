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

MatchStats::SummaryStats MatchStats::Summarise(
    const std::vector<Match> &matches) {
  auto players = matches.front().players();

  std::map<std::string, double> spws;

  spws[players.first] = MatchWinProb(players.first, matches);
  spws[players.second] = 1 - spws[players.first];

  double average_length = AverageMatchLength(matches);
  double average_num_sets = AverageNumberOfSets(matches);

  return MatchStats::SummaryStats(spws, average_length, average_num_sets);
}

std::map<std::string, std::map<unsigned int, double>>
MatchStats::NumberBreakPointsHist(const std::vector<Match> &matches) {
  std::map<std::string, std::map<unsigned int, double>> result;

  unsigned int num_matches = matches.size();

  for (const Match &m : matches) {
    std::pair<std::string, std::string> players = m.players();

    for (auto p : std::vector<std::string>{players.first, players.second}) {
      unsigned int bps = 0;
      unsigned int won = 0;

      std::vector<Point> points = m.GetServicePoints(p);

      for (auto &point : points) {
        if (point.is_break_point()) {
          ++bps;
          if (point.server_won()) {
            ++won;
          }
        }
      }
      auto it = result[p].find(won);

      if (it == result[p].end()) {
        result[p][won] = 1 / static_cast<double>(num_matches);
      } else {
        (it->second) += 1 / static_cast<double>(num_matches);
      }
    }
  }
  return result;
}
