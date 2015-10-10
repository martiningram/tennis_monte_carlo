#include "iid_mc_model.h"
#include "adjusted_mc_model.h"
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "model_data.h"
#include "importance_model_data.h"
#include "importance_mc_model.h"
#include <sstream>
#include <assert.h>

void run_model() {
  std::vector<ModelData> m =
      ModelData::ImportFromFile("wimbledon_iid_2014.csv");

  std::ofstream o;

  o.open("wimbledon_prediction.csv");

  o << "Player 1"
    << ","
    << "Player 2"
    << ","
    << "Match title"
    << ","
    << "p_win_iid"
    << ","
    << "p_win_dynamic"
    << ","
    << "p1_spw_iid"
    << ","
    << "p2_spw_iid"
    << ","
    << "average_sets"
    << ","
    << "average_sets_iid"
    << ","
    << "average_games"
    << ","
    << "average_games_iid" << std::endl;

  for (const ModelData &cur_match : m) {
    bool bo5 = true;

    const unsigned int kSimulations = 1E4;

    AdjustedMCModel adj(cur_match.p1(), cur_match.p2(), bo5, cur_match,
                        kSimulations);

    std::cout << cur_match.p1() << " has the following probs: ";

    for (auto entry : cur_match.model_probs_p1()) {
      for (auto b : entry.first) {
        std::cout << b << " ";
      }

      std::cout << entry.second << std::endl;
    }

    std::cout << cur_match.p2() << " has the following probs: ";

    for (auto entry : cur_match.model_probs_p2()) {
      for (auto b : entry.first) {
        std::cout << b << " ";
      }

      std::cout << entry.second << std::endl;
    }

    std::map<std::string, double> iid_probs;

    iid_probs[cur_match.p1()] =
        cur_match.ServeWinProbabilityIID(cur_match.p1());
    iid_probs[cur_match.p2()] =
        cur_match.ServeWinProbabilityIID(cur_match.p2());

    IIDMCModel iid_model(cur_match.p1(), cur_match.p2(), bo5, iid_probs,
                         kSimulations);

    const std::vector<Match> &matches = adj.matches();
    const std::vector<Match> &iid_matches = iid_model.matches();

    unsigned int i = std::count_if(
        matches.begin(), matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int iid_i = std::count_if(
        iid_matches.begin(), iid_matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int p1_served_first = std::count_if(matches.begin(), matches.end(),
                                                 [cur_match](const Match &m) {
      return m.server_at_start() == cur_match.p1();
    });

    unsigned int set_sum = 0;
    unsigned int set_sum_iid = 0;

    unsigned int game_sum = 0;
    unsigned int game_sum_iid = 0;

    for (unsigned int i = 0; i < kSimulations; ++i) {
      set_sum += matches[i].sets().size();
      set_sum_iid += iid_matches[i].sets().size();
      game_sum += matches[i].total_games();
      game_sum_iid += iid_matches[i].total_games();
    }

    double average_sets = set_sum / static_cast<double>(kSimulations);
    double average_sets_iid = set_sum_iid / static_cast<double>(kSimulations);

    double average_games = game_sum / static_cast<double>(kSimulations);
    double average_games_iid = game_sum_iid / static_cast<double>(kSimulations);

    double iid_win_prob = (iid_i / static_cast<double>(kSimulations));
    double non_iid_win_prob = (i / static_cast<double>(kSimulations));

    std::cout << "Match of " << cur_match.p1() << " against " << cur_match.p2()
              << std::endl;
    std::cout << "IID probability: " << iid_win_prob * 100 << "%" << std::endl;
    std::cout << "Non-IID probability: " << non_iid_win_prob * 100 << "%"
              << std::endl;
    std::cout << "IID sets: " << average_sets_iid
              << "; non-IID: " << average_sets << std::endl;
    std::cout << "IID games: " << average_games_iid
              << "; non-IID: " << average_games << std::endl;
    std::cout << cur_match.p1() << " served first "
              << p1_served_first / static_cast<double>(kSimulations) * 100
              << "% of the time." << std::endl;

    o << cur_match.p1() << "," << cur_match.p2() << ","
      << cur_match.match_title() << "," << iid_win_prob << ","
      << non_iid_win_prob << ","
      << cur_match.ServeWinProbabilityIID(cur_match.p1()) << ","
      << cur_match.ServeWinProbabilityIID(cur_match.p2()) << "," << average_sets
      << "," << average_sets_iid << "," << average_games << ","
      << average_games_iid << std::endl;
  }
  o.close();
}

void run_iid_prediction() {
  std::ifstream i;
  i.open("usopen_iid_2014.csv");
  assert(i.good());

  std::string tournament_name = "us open";

  std::vector<std::string> expected_order{
      "\"tiebreak\"", "\"breakpoint\"", "\"before_breakpoint\"", "\"set_up\"",
      "\"set_down\"", "\"serving\"",    "\"returning\"",         "\"p_iid\""};

  std::string cur_line;
  std::getline(i, cur_line);

  {
    std::istringstream first_line(cur_line);
    for (std::string cur_element : expected_order) {
      std::string cur_header;
      std::getline(first_line, cur_header, ',');
      assert(cur_header == cur_element);
    }
  }

  std::map<std::pair<std::string, std::string>, double> iid_probs;

  while (std::getline(i, cur_line)) {
    std::istringstream iss(cur_line);

    std::string cur_token;
    std::string serving;
    std::string returning;
    std::string p_iid;

    std::getline(iss, cur_token, ',');  // TB
    std::getline(iss, cur_token, ',');  // BP
    std::getline(iss, cur_token, ',');  // BBP
    std::getline(iss, cur_token, ',');  // SU
    std::getline(iss, cur_token, ',');  // SD
    std::getline(iss, serving, ',');    // serving
    std::getline(iss, returning, ',');  // returning
    std::getline(iss, p_iid, ',');      // p_iid

    iid_probs[std::make_pair(serving, returning)] = std::stod(p_iid);
  }

  std::map<std::pair<std::string, std::string>, std::pair<double, double>>
      match_iid;

  for (auto &elt : iid_probs) {
    if (match_iid.find(std::make_pair(elt.first.second, elt.first.first)) !=
        match_iid.end()) {
      continue;
    }
    std::pair<std::string, std::string> cur_match = elt.first;
    double p1_iid = elt.second;
    double p2_iid =
        iid_probs[std::make_pair(elt.first.second, elt.first.first)];
    match_iid[cur_match] = std::make_pair(p1_iid, p2_iid);
  }

  std::ofstream o;
  o.open("usopen_predictions_iid.csv");

  // Header:
  o << "Player 1"
    << ","
    << "Player 2"
    << ","
    << "p1_iid_spw"
    << ","
    << "p2_iid_spw"
    << ","
    << "p1_win_prob" << std::endl;

  const unsigned int kSimulations = 1E4;

  // Run Monte Carlo
  for (auto &p : match_iid) {
    std::map<std::string, double> iid_probs;

    iid_probs[p.first.first] = p.second.first;
    iid_probs[p.first.second] = p.second.second;

    IIDMCModel iid_model(p.first.first, p.first.second, true, iid_probs,
                         kSimulations);

    unsigned int win_count = 0;

    for (const Match &m : iid_model.matches()) {
      if (m.winner() == p.first.first) {
        win_count++;
      }
    }
    double win_prob_p1 = static_cast<double>(win_count) / kSimulations;
    std::cout << "Given the serve percentages of " << p.second.first << " and "
              << p.second.second << ", " << p.first.first << " has "
              << win_prob_p1 * 100 << "\% chance of winning against "
              << p.first.second << std::endl;
    o << p.first.first << "," << p.first.second << "," << p.second.first << ","
      << p.second.second << "," << win_prob_p1 << std::endl;
  }
  o.close();
}

void test_importance_model() {
  std::vector<ImportanceModelData> m =
      ImportanceModelData::ImportFromFile("atp_points_predicted_imp.csv");

  std::vector<ModelData> basic_matches =
      ModelData::ImportFromFile("atp_points_predicted.csv");

  std::ofstream o;

  o.open("iid_vs_non_iid_imp.csv");

  o << "Player 1"
    << ","
    << "Player 2"
    << ","
    << "Match title"
    << ","
    << "p_win_iid"
    << ","
    << "p_win_dynamic"
    << ","
    << "p_win_dynamic_basic"
    << ","
    << "p1_spw_iid"
    << ","
    << "p2_spw_iid"
    << ","
    << "average_sets"
    << ","
    << "average_sets_iid"
    << ","
    << "average_games"
    << ","
    << "average_games_iid" << std::endl;

  for (ImportanceModelData &cur_match : m) {
    bool bo5 = true;

    const unsigned int kSimulations = 1E4;

    ImportanceMCModel adj(cur_match.p1(), cur_match.p2(), bo5, cur_match,
                          kSimulations);

    // Find corresponding ModelData
    ModelData basic_m =
        *std::find_if(basic_matches.begin(), basic_matches.end(),
                      [cur_match](const ModelData &d) {
          return cur_match.p1() == d.p1() && cur_match.p2() == d.p2() &&
                 cur_match.match_title() == d.match_title();
        });

    AdjustedMCModel basic_adj(cur_match.p1(), cur_match.p2(), bo5, basic_m,
                              kSimulations);

    std::map<std::string, double> iid_probs;

    iid_probs[cur_match.p1()] =
        cur_match.ServeWinProbabilityIID(cur_match.p1());
    iid_probs[cur_match.p2()] =
        cur_match.ServeWinProbabilityIID(cur_match.p2());

    IIDMCModel iid_model(cur_match.p1(), cur_match.p2(), bo5, iid_probs,
                         kSimulations);

    const std::vector<Match> &matches = adj.matches();
    const std::vector<Match> &iid_matches = iid_model.matches();
    const std::vector<Match> &basic_matches = basic_adj.matches();

    unsigned int i = std::count_if(
        matches.begin(), matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int iid_i = std::count_if(
        iid_matches.begin(), iid_matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int basic_i = std::count_if(
        basic_matches.begin(), basic_matches.end(),
        [cur_match](const Match &m) { return m.winner() == cur_match.p1(); });

    unsigned int p1_served_first = std::count_if(matches.begin(), matches.end(),
                                                 [cur_match](const Match &m) {
      return m.server_at_start() == cur_match.p1();
    });

    unsigned int set_sum = 0;
    unsigned int set_sum_iid = 0;

    unsigned int game_sum = 0;
    unsigned int game_sum_iid = 0;

    for (unsigned int i = 0; i < kSimulations; ++i) {
      set_sum += matches[i].sets().size();
      set_sum_iid += iid_matches[i].sets().size();
      game_sum += matches[i].total_games();
      game_sum_iid += iid_matches[i].total_games();
    }

    double average_sets = set_sum / static_cast<double>(kSimulations);
    double average_sets_iid = set_sum_iid / static_cast<double>(kSimulations);

    double average_games = game_sum / static_cast<double>(kSimulations);
    double average_games_iid = game_sum_iid / static_cast<double>(kSimulations);

    double iid_win_prob = (iid_i / static_cast<double>(kSimulations));
    double non_iid_win_prob = (i / static_cast<double>(kSimulations));
    double basic_win_prob = (basic_i / static_cast<double>(kSimulations));

    std::cout << "Match of " << cur_match.p1() << " against " << cur_match.p2()
              << std::endl;
    std::cout << "IID probability: " << iid_win_prob * 100 << "%" << std::endl;
    std::cout << "Non-IID probability: " << non_iid_win_prob * 100 << "%"
              << std::endl;
    std::cout << "Basic non-iid: " << basic_win_prob * 100 << "%" << std::endl;
    std::cout << "IID sets: " << average_sets_iid
              << "; non-IID: " << average_sets << std::endl;
    std::cout << "IID games: " << average_games_iid
              << "; non-IID: " << average_games << std::endl;
    std::cout << cur_match.p1() << " served first "
              << p1_served_first / static_cast<double>(kSimulations) * 100
              << "% of the time." << std::endl;

    std::cout << ImportanceModelData::total_missing_points_ << " out of "
              << ImportanceModelData::total_predicted_points_ << std::endl;

    // Reset:
    ImportanceModelData::total_missing_points_ = 0;
    ImportanceModelData::total_predicted_points_ = 0;

    o << cur_match.p1() << "," << cur_match.p2() << ","
      << cur_match.match_title() << "," << iid_win_prob << ","
      << non_iid_win_prob << "," << basic_win_prob << ","
      << cur_match.ServeWinProbabilityIID(cur_match.p1()) << ","
      << cur_match.ServeWinProbabilityIID(cur_match.p2()) << "," << average_sets
      << "," << average_sets_iid << "," << average_games << ","
      << average_games_iid << std::endl;
  }
  o.close();
}

void test_importance_version() {
  std::vector<ImportanceModelData> m = ImportanceModelData::ImportFromFile(
      "/vol/bitbucket/mdi14/atp_points_predicted_imp.csv");
}

void TestMatchLengthConvergence() {
  std::vector<unsigned int> num_simulations{
      10,                            100,   200,
      500,                           1000,  2000,
      5000,                          10000, static_cast<unsigned int>(5E4),
      static_cast<unsigned int>(1E5)};

  std::ofstream o;

  o.open("convergence.csv");

  std::map<std::string, double> cur_spws;

  const unsigned int kNumSimulationsEach = 10;

  cur_spws["Player 1"] = 0.5;
  cur_spws["Player 2"] = 0.5;

  o << "No. of simulations"
    << ","
    << "Average Length" << std::endl;

  for (unsigned int simulation_no : num_simulations) {
    for (unsigned int i = 0; i < kNumSimulationsEach; ++i) {
      IIDMCModel cur_simulations("Player 1", "Player 2", false, cur_spws,
                                 simulation_no);

      unsigned long long total_games_played = 0;

      for (const Match &m : cur_simulations.matches()) {
        total_games_played += m.total_games();
      }

      double average_num =
          total_games_played / static_cast<double>(simulation_no);

      std::cout << simulation_no << " " << average_num << std::endl;
      o << simulation_no << "," << average_num << std::endl;
    }
  }
  o.close();
}

void ExportIIDLengthGrid() {
  unsigned int grid_size = 100;

  std::ofstream o;

  o.open("length_grid_1e3.csv");

  double grid_min = 0.01;
  double grid_max = 0.99;

  double step_size = (grid_max - grid_min) / grid_size;

  const unsigned int kNumSimulations = 1000;

  std::string p1 = "Player 1";
  std::string p2 = "Player 2";

  bool bo5 = false;

  for (unsigned int i = 0; i < grid_size; ++i) {
    double cur_spw_1 = i * step_size + grid_min;

    for (unsigned int j = 0; j < grid_size; ++j) {
      double cur_spw_2 = j * step_size + grid_min;
      std::map<std::string, double> cur_spws;

      cur_spws[p1] = cur_spw_1;
      cur_spws[p2] = cur_spw_2;

      IIDMCModel cur_simulations(p1, p2, bo5, cur_spws, kNumSimulations);

      // Find average number of games:
      unsigned long long total_games_played = 0;

      for (const Match &m : cur_simulations.matches()) {
        total_games_played += m.total_games();
      }

      double average_num =
          total_games_played / static_cast<double>(kNumSimulations);

      std::cout << cur_spw_1 << " " << cur_spw_2 << " " << average_num
                << std::endl;

      o << cur_spw_1 << "," << cur_spw_2 << "," << average_num << std::endl;
    }
  }
  o.close();
}

void verbose_test_run() {
  std::vector<ModelData> m = ModelData::ImportFromFile(
      "atp_points_predicted_player_no_tournament_name.csv");

  ModelData::ExportToFile(m, "atp_points_predicted_player_export_test.csv");

  ModelData test = m[5];

  bool bo5 = true;

  const unsigned int kSimulations = 1;

  AdjustedMCModel adj(test.p1(), test.p2(), bo5, test, kSimulations, true);

  std::map<std::string, double> iid_probs;

  iid_probs[test.p1()] = test.ServeWinProbabilityIID(test.p1());
  iid_probs[test.p2()] = test.ServeWinProbabilityIID(test.p2());

  IIDMCModel iid_model(test.p1(), test.p2(), bo5, iid_probs, kSimulations,
                       false);
}

void verbose_test_run_iid() {
  std::map<std::string, double> cur_spws;

  cur_spws["Player 1"] = 0.01;
  cur_spws["Player 2"] = 0.01;

  IIDMCModel iid_model("Player 1", "Player 2", false, cur_spws, 1, true);
}

void verbose_test_run_importance() {
  std::vector<ImportanceModelData> m =
      ImportanceModelData::ImportFromFile("atp_points_predicted_imp.csv");

  ImportanceModelData test =
      *std::find_if(m.begin(), m.end(), [](const ImportanceModelData &m) {
        return m.p1() == "Adrian Mannarino" && m.p2() == "Fabio Fognini";
      });

  bool bo5 = true;

  const unsigned int kSimulations = 1;

  ImportanceMCModel adj(test.p1(), test.p2(), bo5, test, kSimulations, true);

  std::cout << ImportanceModelData::total_missing_points_ << " out of "
            << ImportanceModelData::total_predicted_points_ << std::endl;

  std::map<std::string, double> iid_probs;

  iid_probs[test.p1()] = test.ServeWinProbabilityIID(test.p1());
  iid_probs[test.p2()] = test.ServeWinProbabilityIID(test.p2());

  IIDMCModel iid_model(test.p1(), test.p2(), bo5, iid_probs, kSimulations);
}

int main() { run_iid_prediction(); }
