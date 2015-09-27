#ifndef ADJUSTED_MC_MODEL_H
#define ADJUSTED_MC_MODEL_H

#include "mc_model.h"
#include "model_data.h"
#include <map>

class AdjustedMCModel : public MCModel {
  double ServeWinProbability(const Point &p) const;

  mutable std::map<std::string, double> iid_probs_;

  double non_iid_strength_;

 public:
  AdjustedMCModel(std::string p1, std::string p2, bool best_of_five,
                  const ModelData &data, unsigned int num_matches = 1000,
                  bool verbose = false);
};

#endif
