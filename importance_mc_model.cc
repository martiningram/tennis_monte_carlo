#include "importance_mc_model.h"

ImportanceMCModel::ImportanceMCModel(std::string p1, std::string p2,
                                     bool best_of_five,
                                     const ImportanceModelData &data,
                                     unsigned int num_matches, bool verbose)
    : MCModel(p1, p2, best_of_five, num_matches, verbose), data_(data) {
  for (unsigned int i = 0; i < kNumMatches_; ++i) {
    matches_played_.push_back(PlayMatch());
  }
}

double ImportanceMCModel::ServeWinProbability(const Point &p) const {
  double non_iid_prob = data_.ServeWinProbabilityNonIID(p);
  if (verbose_) {
    std::cout << "The probability is: " << non_iid_prob << std::endl;
  }
  return non_iid_prob;
}
