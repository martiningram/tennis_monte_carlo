#include "adjusted_mc_model.h"

AdjustedMCModel::AdjustedMCModel(std::string p1, std::string p2,
                                 bool best_of_five, const ModelData &data,
                                 unsigned int num_matches, bool verbose)
    : MCModel(p1, p2, best_of_five, num_matches, verbose), data_(data) {
  for (unsigned int i = 0; i < kNumMatches_; ++i) {
    matches_played_.push_back(PlayMatch());
  }
}

std::array<bool, 5> AdjustedMCModel::IdentifyPoint(const Point &p) const {
  std::array<bool, 5> result;

  result[0] = p.is_tiebreak();
  result[1] = p.is_break_point();
  result[2] = p.is_point_before_bp();
  result[3] = p.is_set_up();
  result[4] = p.is_set_down();

  if (verbose_) {
    std::cout << "The point at score " << p.score().player_points(p.server())
              << "-" << p.score().player_points(p.returner())
              << " was identified as: ";
    for (auto &el : result) {
      std::cout << el << " ";
    }
    std::cout << std::endl;
  }

  return result;
}

double AdjustedMCModel::ServeWinProbability(const Point &p) const {
  std::array<bool, 5> point_type = IdentifyPoint(p);
  double non_iid_prob = data_.ServeWinProbabilityNonIID(p.server(), point_type);
  if (verbose_) {
    std::cout << "The probability is: " << non_iid_prob << std::endl;
  }
  return non_iid_prob;
}
