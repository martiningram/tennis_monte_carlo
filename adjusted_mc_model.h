#ifndef ADJUSTED_MC_MODEL_H
#define ADJUSTED_MC_MODEL_H

#include <map>

#include "mc_model.h"
#include "model_data.h"

class AdjustedMCModel : public MCModel {
  double ServeWinProbability(const Point &p) const;

  ModelData data_;

  std::array<bool, 5> IdentifyPoint(const Point &p) const;

 public:
  AdjustedMCModel(std::string p1, std::string p2, bool best_of_five,
                  const ModelData &data, unsigned int num_matches = 1000,
                  bool verbose = false);
};

#endif
