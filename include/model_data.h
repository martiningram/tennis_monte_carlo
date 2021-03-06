#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#include <vector>
#include <string>
#include <array>
#include <map>

class ModelData {
  std::string p1_;
  std::string p2_;

  std::string match_title_;

  std::map<std::array<bool, 5>, double> model_probabilities_p1_;
  std::map<std::array<bool, 5>, double> model_probabilities_p2_;

  double p1_iid_;
  double p2_iid_;

 public:
  ModelData(std::string p1, std::string p2, std::string match_title,
            std::map<std::array<bool, 5>, double> model_probs_p1,
            std::map<std::array<bool, 5>, double> model_probs_p2,
            double p1_iid_, double p2_iid);

  double ServeWinProbabilityNonIID(std::string player,
                                   std::array<bool, 5> point_type) const;

  double ServeWinProbabilityIID(std::string player) const;

  std::string p1() const;
  std::string p2() const;
  std::string match_title() const;
  std::map<std::array<bool, 5>, double> model_probs_p1() const;
  std::map<std::array<bool, 5>, double> model_probs_p2() const;

  static std::vector<ModelData> ImportFromFile(std::string csv_file);
  static void ExportToFile(const std::vector<ModelData> &,
                           std::string csv_file);
};

#endif
