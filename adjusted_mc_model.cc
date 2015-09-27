#include "adjusted_mc_model.h"

AdjustedMCModel::AdjustedMCModel(std::string p1, std::string p2,
                                 bool best_of_five, const ModelData &data,
                                 unsigned int num_matches, bool verbose)
    : MCModel(p1, p2, best_of_five, num_matches, verbose),
      iid_probs_(iid_probs),
      non_iid_strength_(non_iid_strength) {
  for (unsigned int i = 0; i < kNumMatches_; ++i) {
    matches_played_.push_back(PlayMatch());
  }
}

double AdjustedMCModel::ServeWinProbability(const Point &p) const {
  // Test stub. Simulate iid. player (p1) against non-iid. player (p2).
  if (p.server() == p1_) {
    return iid_probs_[p1_];
  }

  else {
    // Determine point type:
    if (p.is_break_point()) {
      if (verbose_) {
        std::cout << "Returning affected probability (break point)"
                  << std::endl;
        std::cout << "It is: " << iid_probs_[p2_] - non_iid_strength_
                  << std::endl;
      }
      return iid_probs_[p2_] - non_iid_strength_;
    } else {
      return iid_probs_[p2_];
    }
  }
}
