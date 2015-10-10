#include "iid_mc_model.h"

#include "point.h"

IIDMCModel::IIDMCModel(std::string p1, std::string p2, bool best_of_five,
                       std::map<std::string, double> serve_win_probs,
                       unsigned int num_matches, bool verbose)
    : MCModel(p1, p2, best_of_five, num_matches, verbose),
      serve_win_prob_(serve_win_probs) {
  for (unsigned int i = 0; i < kNumMatches_; ++i) {
    matches_played_.push_back(PlayMatch());
  }
}

double IIDMCModel::ServeWinProbability(const Point &p) const {
  double win_prob = (serve_win_prob_.find(p.server()))->second;

  return win_prob;
}
