#include "iid_mc_model.h"

#include "point.h"

IIDMCModel::IIDMCModel(std::string p1, std::string p2, bool best_of_five,
                       std::map<std::string, double> serve_win_probs,
                       unsigned int num_matches)
    : MCModel(p1, p2, best_of_five, num_matches),
      serve_win_prob_(serve_win_probs) {}

virtual double IIDMCModel::ServeWinProbability(const Point &p) {
  double win_prob = serve_win_prob_[p.server()];

  return win_prob;
}
