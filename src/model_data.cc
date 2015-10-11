#include <fstream>
#include <iostream>
#include <assert.h>
#include <sstream>
#include <string>

#include "csv_file.h"
#include "model_data.h"
#include "tools.h"

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

std::string ModelData::match_title() const { return match_title_; }

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

  // Make sure header conforms to what I think it conforms to: (to avoid parsing
  // errors like before...)

  std::vector<std::string> expected_order{
      "\"tiebreak\"",        "\"breakpoint\"", "\"before_breakpoint\"",
      "\"set_up\"",          "\"set_down\"",   "\"point_importance\"",
      "\"serving\"",         "\"returning\"",  "\"serving_match\"",
      "\"returning_match\"", "\"p_iid\"",      "\"p_non_iid\""};

  std::getline(i, cur_line);

  {
    std::istringstream first_line(cur_line);
    for (std::string cur_element : expected_order) {
      std::string cur_header;
      std::getline(first_line, cur_header, ',');
      assert(cur_header == cur_element);
    }
  }

  while (std::getline(i, cur_line)) {
    std::istringstream iss(cur_line);

    std::array<bool, 5> cur_vals;
    std::string cur_player;
    std::string cur_opponent;
    std::string cur_match;

    std::string tiebreak;
    std::getline(iss, tiebreak, ',');
    cur_vals[0] = (tiebreak == "TRUE");

    std::string breakpoint;
    std::getline(iss, breakpoint, ',');
    cur_vals[1] = (breakpoint == "TRUE");

    std::string before_breakpoint;
    std::getline(iss, before_breakpoint, ',');
    cur_vals[2] = (before_breakpoint == "TRUE");

    std::string set_up;
    std::getline(iss, set_up, ',');
    cur_vals[3] = (set_up == "1");

    std::string set_down;
    std::getline(iss, set_down, ',');
    cur_vals[4] = (set_down == "1");

    // Skipping importance
    std::string importance;
    std::getline(iss, importance, ',');
    assert(importance == std::string("0.050849018051122"));

    std::getline(iss, cur_player, ',');
    Tools::Trim(cur_player, '"');
    std::getline(iss, cur_opponent, ',');
    Tools::Trim(cur_opponent, '"');
    std::getline(iss, cur_match, ',');

    // Returning match:
    std::getline(iss, cur_match, ',');
    Tools::Trim(cur_match, '"');

    // TODO: Add an assert for the headers!!
    // Need to cut off the last name to have match name. Find colon:

    std::size_t last_char = cur_match.rfind(':');
    cur_match = cur_match.substr(0, last_char);

    double probability;
    double probability_iid;

    std::string p_iid;
    std::string p_non_iid;

    std::getline(iss, p_iid, ',');
    std::getline(iss, p_non_iid, ',');

    probability = std::stod(p_non_iid);
    probability_iid = std::stod(p_iid);

    data[cur_match][cur_player].non_iid[cur_vals] = probability;
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

std::map<std::array<bool, 5>, double> ModelData::model_probs_p1() const {
  return model_probabilities_p1_;
}

std::map<std::array<bool, 5>, double> ModelData::model_probs_p2() const {
  return model_probabilities_p2_;
}

void ModelData::ExportToFile(const std::vector<ModelData> &m,
                             std::string csv_file) {
  std::ofstream o;

  o.open(csv_file);

  std::vector<std::string> headers{
      "\"tiebreak\"",  "\"breakpoint\"",    "\"before_breakpoint\"",
      "\"set_up\"",    "\"set_down\"",      "\"serving\"",
      "\"returning\"", "\"serving_match\"", "\"returning_match\"",
      "\"p_iid\"",     "\"p_noniid\""};

  std::string final_header = headers.back();
  headers.pop_back();

  for (std::string &h : headers) {
    o << h << ",";
  }

  o << final_header << std::endl;

  for (const ModelData &data : m) {
    std::map<std::array<bool, 5>, double> m_p1(data.model_probs_p1());
    std::map<std::array<bool, 5>, double> m_p2(data.model_probs_p2());

    auto both_maps = std::vector<decltype(m_p1)>{m_p1, m_p2};

    for (unsigned int i = 0; i < 2; ++i) {
      std::string cur_server = (i == 0) ? (data.p1()) : (data.p2());
      std::string cur_returner = (i == 0) ? (data.p2()) : (data.p1());
      auto &cur_map = both_maps[i];

      for (auto p : cur_map) {
        std::array<bool, 5> b = p.first;
        double p_noniid = p.second;

        auto translate = [](bool v) { return (v ? "TRUE" : "FALSE"); };
        o << translate(b[0]) << "," << translate(b[1]) << "," << translate(b[2])
          << ",";
        o << b[3] << "," << b[4] << ",";

        o << cur_server << "," << cur_returner << ",";

        // Make serving and returning match:

        std::string basis = data.match_title();

        std::string serving_match = basis + std::string(":") + cur_server;
        std::string returning_match = basis + std::string(":") + cur_returner;

        o << serving_match << "," << returning_match << ","
          << data.ServeWinProbabilityIID(cur_server) << "," << p_noniid
          << std::endl;
      }
    }
  }
  o.close();
}
