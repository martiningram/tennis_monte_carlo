#ifndef ADJUSTED_MC_MODEL_H
#define ADJUSTED_MC_MODEL_H

#include "mc_model.h"
#include <map>

// Currently just a test stub. Will later be fed with data from BCMGM.
class AdjustedMCModel : public MCModel {
  double ServeWinProbability(const Point &p) const;

  mutable std::map<std::string, double> iid_probs_;

  double non_iid_strength_;

 public:
  AdjustedMCModel(std::string p1, std::string p2, bool best_of_five,
                  std::map<std::string, double> iid_probs,
                  double non_iid_strength, unsigned int num_matches = 1000,
                  bool verbose = false);
};

#endif
