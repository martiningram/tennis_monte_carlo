#include "model_data.h"
#include <fstream>
#include <iostream>
#include <assert.h>
#include <sstream>

ModelData::ModelData(std::string p1, std::string p2, std::string match_title,
                     std::map<std::array<bool, 5>, double> model_probs_p1,
                     std::map<std::array<bool, 5>, double> model_probs_p2,
                     double p1_iid, double p2_iid)
    : p1_(p1),
      p2_(p2),
      match_title_(match_title),
      model_probabilities_p1_(model_probs_p1),
      model_probabilities_p2_(model_probs_p2),
      p1_iid_(p1_iid),
      p2_iid_(p2_iid) {}

std::string ModelData::p1() const { return p1_; }

std::string ModelData::p2() const { return p2_; }

double ModelData::ServeWinProbabilityNonIID(
    std::string player, std::array<bool, 5> point_type) const {
  assert(player == p1_ || player == p2_);
  std::map<std::array<bool, 5>, double> const *relevant_map;

  if (player == p1_) {
    relevant_map = &model_probabilities_p1_;
  } else {
    relevant_map = &model_probabilities_p2_;
  }

  auto it = relevant_map->find(point_type);
  assert(it != relevant_map->end());
  return it->second;
}

double ModelData::ServeWinProbabilityIID(std::string player) const {
  assert(player == p1_ || player == p2_);

  if (player == p1_) {
    return p1_iid_;
  } else {
    return p2_iid_;
  }
}

std::vector<ModelData> ModelData::ImportFromFile(std::string csv_file) {
  std::ifstream i;
  i.open(csv_file);
  assert(i.good());

  std::string cur_line;

  struct PlayerMatchData {
    std::map<std::array<bool, 5>, double> non_iid;
    double iid;
    std::string opponent;
  };

  std::vector<ModelData> results;

  std::map<std::string, std::map<std::string, PlayerMatchData>> data;

  // need to skip first line:

  std::getline(i, cur_line);

  while (std::getline(i, cur_line)) {
    std::istringstream iss(cur_line);

    std::array<bool, 5> cur_line;
    std::string cur_player;
    std::string cur_opponent;
    std::string cur_match;

    std::string tiebreak;
    std::getline(iss, tiebreak, ',');
    cur_line[0] = (tiebreak == "TRUE");

    std::string breakpoint;
    std::getline(iss, breakpoint, ',');
    cur_line[1] = (breakpoint == "TRUE");

    std::string before_breakpoint;
    std::getline(iss, before_breakpoint, ',');
    cur_line[2] = (before_breakpoint == "TRUE");

    std::string set_up;
    std::getline(iss, set_up, ',');
    cur_line[3] = (set_up == "1");

    std::string set_down;
    std::getline(iss, set_down, ',');
    cur_line[4] = (set_down == "1");

    std::getline(iss, cur_player, ',');
    std::getline(iss, cur_opponent, ',');
    std::getline(iss, cur_match, ',');

    // Returning match:
    std::getline(iss, cur_match, ',');

    // Need to cut off the last name to have match name. Find colon:

    std::size_t last_char = cur_match.rfind(':');
    cur_match = cur_match.substr(0, last_char);

    double probability;
    double probability_iid;

    std::string p_non_iid;
    std::string p_iid;

    std::getline(iss, p_non_iid, ',');
    std::getline(iss, p_iid, ',');

    probability = std::stod(p_non_iid);
    probability_iid = std::stod(p_iid);

    data[cur_match][cur_player].non_iid[cur_line] = probability;
    data[cur_match][cur_player].iid = probability_iid;
    data[cur_match][cur_player].opponent = cur_opponent;
  }

  // Go through each match
  for (std::pair<const std::string, std::map<std::string, PlayerMatchData>> &p :
       data) {
    // There should be exactly two results each time
    assert(p.second.size() == 2);

    // Construct the ModelData object
    auto map_it = p.second.begin();
    std::string p1 = map_it->first;
    ++map_it;
    std::string p2 = map_it->first;

    const PlayerMatchData &p1_data = p.second[p1];
    const PlayerMatchData &p2_data = p.second[p2];

    ModelData cur_data(p1, p2, p.first, p1_data.non_iid, p2_data.non_iid,
                       p1_data.iid, p2_data.iid);

    // Uncomment to print (and check):

    /*std::cout << p1 << " vs. " << p2 << std::endl;*/

    // for (auto pr : p1_data.non_iid) {
    // for (auto val : pr.first) {
    // std::cout << val << " ";
    //}
    // std::cout << ", " << pr.second << std::endl;
    /*}*/

    results.emplace_back(cur_data);
  }
  return results;
}
