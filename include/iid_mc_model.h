#ifndef IID_MC_MODEL_H
#define IID_MC_MODEL_H

#include <string>
#include <map>
#include "mc_model.h"

class Point;

class IIDMCModel : public MCModel {
  virtual double ServeWinProbability(const Point &p) const;

  std::map<std::string, double> serve_win_prob_;

 public:
  IIDMCModel(std::string p1, std::string p2, bool best_of_five,
             std::map<std::string, double> serve_win_probs,
             unsigned int num_matches = 1000, bool verbose = false);
};

#endif
