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

    std::cout << cur_match << std::endl;

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
  for (const std::pair<std::string, std::map<std::string, PlayerMatchData>> &p :
       data) {
    std::cout << p.second.size() << std::endl;
    // There should be exactly two results each time
    assert(p.second.size() == 2);
  }
}
