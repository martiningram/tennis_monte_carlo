#include "importance_model_data.h"
#include <fstream>
#include <iostream>
#include <assert.h>
#include <sstream>
#include "exceptions.h"
#include "tools.h"

ImportanceModelData::ImportanceModelData(std::string p1, std::string p2,
                                         std::string match_title,
                                         ModelMap model_probs_p1,
                                         ModelMap model_probs_p2, double p1_iid,
                                         double p2_iid)
    : p1_(p1),
      p2_(p2),
      match_title_(match_title),
      p1_iid_(p1_iid),
      p2_iid_(p2_iid),
      model_probs_p1_(model_probs_p1),
      model_probs_p2_(model_probs_p2) {}

unsigned long long ImportanceModelData::total_missing_points_(0);
unsigned long long ImportanceModelData::total_predicted_points_(0);

std::string ImportanceModelData::p1() const { return p1_; }

std::string ImportanceModelData::p2() const { return p2_; }

std::string ImportanceModelData::match_title() const { return match_title_; }

double ImportanceModelData::ServeWinProbabilityNonIID(const Point &p) {
  // Untangle map one step at a time:
  // Flags:
  std::array<bool, 5> flags;
  flags[0] = p.is_tiebreak();
  flags[1] = p.is_break_point();
  flags[2] = p.is_point_before_bp();
  flags[3] = p.is_set_up();
  flags[4] = p.is_set_down();

  ModelMap &relevant_map =
      (p.server() == p1_) ? (model_probs_p1_) : (model_probs_p2_);

  auto match_it = relevant_map.find(flags);
  assert(match_it != relevant_map.end());

  auto &match_level = match_it->second;

  // Sets won:
  unsigned int sets_won_server = p.score().sets_won(p.server());
  unsigned int sets_won_returner = p.score().sets_won(p.returner());

  SetsWon sets_won(sets_won_server, sets_won_returner);

  auto sets_it = match_level.find(sets_won);
  assert(sets_it != match_level.end());

  auto &set_level = sets_it->second;

  // Set score:
  unsigned int games_won_server = p.score().player_games(p.server());
  unsigned int games_won_returner = p.score().player_games(p.returner());

  SetScore set_score(games_won_server, games_won_returner);

  auto games_it = set_level.find(set_score);

  if (games_it == set_level.end()) {
    // This situation apparently is very rare. Return i.i.d.:
    ++total_missing_points_;
    ++total_predicted_points_;
    return ServeWinProbabilityIID(p.server());
  }
  assert(games_it != set_level.end());

  auto &game_level = games_it->second;

  // Game score:
  unsigned int points_won_server = p.score().player_points(p.server());
  unsigned int points_won_returner = p.score().player_points(p.returner());

  // Modifications: Assume that any score greater than 6-6 in a tiebreak and 3-3
  // in a game is equivalent to 6-6 or 3-3 with the appropriate difference. This
  // is to be able to cope with unusual scenarios.
  if (p.is_tiebreak() && points_won_server > 6 && points_won_returner > 6) {
    unsigned int new_p_server = 6 + static_cast<int>(points_won_server) -
                                static_cast<int>(points_won_returner);
    unsigned int new_p_returner = 6;
    points_won_server = new_p_server;
    points_won_returner = new_p_returner;
  } else if (!p.is_tiebreak() && points_won_server > 3 &&
             points_won_returner > 3) {
    unsigned int new_p_server = 3 + static_cast<int>(points_won_server) -
                                static_cast<int>(points_won_returner);
    unsigned int new_p_returner = 3;
    points_won_server = new_p_server;
    points_won_returner = new_p_returner;
  }

  GameScore game_score(points_won_server, points_won_returner);

  auto points_it = game_level.find(game_score);
  if (points_it == game_level.end()) {
    // This point is so rare it did not occur in the database. Return an i.i.d.
    // probability.
    ++total_missing_points_;
    ++total_predicted_points_;
    return ServeWinProbabilityIID(p.server());
  }

  ++total_predicted_points_;

  double probability = points_it->second;

  return probability;
}

double ImportanceModelData::ServeWinProbabilityIID(const std::string &player) {
  assert(player == p1_ || player == p2_);
  if (player == p1_) {
    return p1_iid_;
  } else {
    return p2_iid_;
  }
}

std::vector<ImportanceModelData> ImportanceModelData::ImportFromFile(
    std::string csv_file) {
  std::ifstream i;
  i.open(csv_file);
  assert(i.good());

  std::string cur_line;

  std::cout << "Beginning import." << std::endl;

  struct PlayerMatchData {
    std::map<Flags,
             std::map<SetsWon, std::map<SetScore, std::map<GameScore, double>>>>
        non_iid;
    double iid;
  };

  std::vector<ImportanceModelData> results;

  using MatchTitle = std::string;
  using PlayerName = std::string;

  std::map<MatchTitle, std::map<PlayerName, PlayerMatchData>> data;

  // need to skip first line:

  std::getline(i, cur_line);

  while (std::getline(i, cur_line)) {
    std::istringstream iss(cur_line);

    Flags cur_flags;

    std::string tiebreak = Tools::ReadString(iss);
    cur_flags[0] = (tiebreak == "TRUE");

    std::string breakpoint = Tools::ReadString(iss);
    cur_flags[1] = (breakpoint == "TRUE");

    std::string before_breakpoint = Tools::ReadString(iss);
    cur_flags[2] = (before_breakpoint == "TRUE");

    std::string set_up = Tools::ReadString(iss);
    cur_flags[3] = (set_up == "1");

    std::string set_down = Tools::ReadString(iss);
    cur_flags[4] = (set_down == "1");

    int serve_points_won = Tools::ReadInt(iss);
    int return_points_won = Tools::ReadInt(iss);
    GameScore game_score(serve_points_won, return_points_won);

    int serve_games_won = Tools::ReadInt(iss);
    int return_games_won = Tools::ReadInt(iss);
    SetScore set_score(serve_games_won, return_games_won);

    int serve_sets_won = Tools::ReadInt(iss);
    int return_sets_won = Tools::ReadInt(iss);
    SetsWon sets_won(serve_sets_won, return_sets_won);

    try {
      Tools::ReadDouble(iss);  // importance -- not used at the moment
    } catch (NoDataException e) {
      continue;
    }

    std::string cur_player = Tools::ReadString(iss);
    Tools::Trim(cur_player, '"');
    std::string cur_opponent = Tools::ReadString(iss);
    Tools::Trim(cur_opponent, '"');

    std::string cur_match;
    std::getline(iss, cur_match, ',');

    // Returning match:
    std::getline(iss, cur_match, ',');
    Tools::Trim(cur_match, '"');

    // Need to cut off the last name to have match name. Find colon:
    std::size_t last_char = cur_match.rfind(':');
    cur_match = cur_match.substr(0, last_char);

    std::string tournament_name;
    std::getline(iss, tournament_name, ',');

    double probability, probability_iid;

    try {
      probability = Tools::ReadDouble(iss);
      probability_iid = Tools::ReadDouble(iss);
    } catch (NoDataException e) {
      continue;
    }

    data[cur_match][cur_player]
        .non_iid[cur_flags][sets_won][set_score][game_score] = probability;
    data[cur_match][cur_player].iid = probability_iid;
  }

  // Go through each match
  for (std::pair<const std::string, std::map<std::string, PlayerMatchData>> &p :
       data) {
    // There should be exactly two results each time
    assert(p.second.size() == 2);

    // Construct the ImportanceModelData object
    auto map_it = p.second.begin();
    std::string p1 = map_it->first;
    ++map_it;
    std::string p2 = map_it->first;

    const PlayerMatchData &p1_data = p.second[p1];
    const PlayerMatchData &p2_data = p.second[p2];

    ImportanceModelData cur_data(p1, p2, p.first, p1_data.non_iid,
                                 p2_data.non_iid, p1_data.iid, p2_data.iid);

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

  std::cout << "Finished import." << std::endl;
  return results;
}
