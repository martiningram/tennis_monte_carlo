#ifndef IMPORTANCE_MODEL_DATA_H
#define IMPORTANCE_MODEL_DATA_H

#include <vector>
#include <string>
#include <array>
#include <map>

#include "point.h"

// Consider moving these into a separate file if any conflicts:
using SetsWon = std::pair<unsigned int, unsigned int>;
using SetScore = std::pair<unsigned int, unsigned int>;
using GameScore = std::pair<unsigned int, unsigned int>;
using Flags = std::array<bool, 5>;
using ModelMap = std::map<
    Flags, std::map<SetsWon, std::map<SetScore, std::map<GameScore, double>>>>;

class ImportanceModelData {
  std::string p1_, p2_;

  std::string match_title_;

  double p1_iid_, p2_iid_;

  std::map<Flags,
           std::map<SetsWon, std::map<SetScore, std::map<GameScore, double>>>>
      model_probs_p1_, model_probs_p2_;

 public:
  static std::vector<ImportanceModelData> ImportFromFile(std::string csv_file);

  ImportanceModelData(std::string p1, std::string p2, std::string match_title,
                      ModelMap model_probs_p1, ModelMap model_probs_p2,
                      double p1_iid, double p2_iid);

  static unsigned long long total_missing_points_;
  static unsigned long long total_predicted_points_;

  std::string p1() const;
  std::string p2() const;
  std::string match_title() const;

  double ServeWinProbabilityNonIID(const Point &p);
  double ServeWinProbabilityIID(const std::string &player);
};

#endif
