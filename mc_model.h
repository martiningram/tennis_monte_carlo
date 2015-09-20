#ifndef MCMODEL_H
#define MCMODEL_H

class MCModel {
 protected:
  virtual double ServeWinProbability(const Point &p) const = 0;

  std::vector<Match> matches_played_;

  bool best_of_five_;

  std::string p1_, p2_;

  const unsigned int kNumMatches_;

  void PlayPoint(Point &p);
  ServiceGame PlayGame(std::string cur_server, Score &cur_score);
  Set PlaySet(std::string cur_server, std::string cur_returner,
              Score &cur_score);
  Tiebreak *PlayTiebreak(std::string cur_server, std::string cur_returner,
                         Score &cur_score);
  Match PlayMatch();

 public:
  MCModel(std::string p1, std::string p2, bool best_of_five,
          unsigned int num_matches = 1000);
};

#endif
