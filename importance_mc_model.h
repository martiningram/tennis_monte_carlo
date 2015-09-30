#ifndef IMPORTANCE_MC_MODEL_H
#define IMPORTANCE_MC_MODEL_H

#include <map>
#include "mc_model.h"
#include "importance_model_data.h"

class ImportanceMCModel : public MCModel {
  double ServeWinProbability(const Point &p) const;

  mutable ImportanceModelData data_;

 public:
  ImportanceMCModel(std::string p1, std::string p2, bool best_of_five,
                    const ImportanceModelData &data,
                    unsigned int num_matches = 1000, bool verbose = false);
};

#endif
