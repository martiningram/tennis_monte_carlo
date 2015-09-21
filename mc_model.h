#ifndef MC_MODEL_H
#define MC_MODEL_H

#include <vector>
#include "match.h"
#include "set.h"
#include "service_game.h"
#include <random>

class Point;
class Tiebreak;

class MCModel {
 protected:
  virtual double ServeWinProbability(const Point &p) const = 0;

  std::vector<Match> matches_played_;

  std::default_random_engine generator_;

  std::string p1_, p2_;

  bool best_of_five_;

  const unsigned int kNumMatches_;

  bool verbose_;

  void PlayPoint(Point &p);
  ServiceGame PlayGame(std::string cur_server, std::string cur_returner,
                       Score &cur_score);
  Set PlaySet(std::string cur_server, std::string cur_returner,
              Score &cur_score);
  Tiebreak *PlayTiebreak(std::string cur_server, std::string cur_returner,
                         Score &cur_score);
  Match PlayMatch();

 public:
  MCModel(std::string p1, std::string p2, bool best_of_five,
          unsigned int num_matches = 1000, bool verbose = false);

  const std::vector<Match> &matches() const { return matches_played_; }
};

#endif
