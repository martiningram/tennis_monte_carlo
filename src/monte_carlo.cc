#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <functional>

#include "iid_mc_model.h"
#include "adjusted_mc_model.h"
#include "model_data.h"
#include "importance_model_data.h"
#include "importance_mc_model.h"
#include "match_stats.h"
#include "tools.h"
#include "csv_writer.h"
#include "csv_file.h"

std::pair<std::map<std::string, double>, int> LookupSPW(
    std::string tournament_name, std::string year, std::string p1,
    std::string p2) {
  std::map<std::string, double> result;

  if (tournament_name == std::string("French Open")) {
    return std::make_pair(result, -1);
  }

  auto criterion =
      [tournament_name, p1, p2](const std::map<std::string, std::string> &m) {
    return ((m.find("event_name"))->second).find(tournament_name) !=
               std::string::npos &&
           ((Tools::ToLower((m.find("playerA"))->second) ==
                 Tools::ToLower(p1) &&
             Tools::ToLower((m.find("playerB"))->second) ==
                 Tools::ToLower(p2)) ||
            (Tools::ToLower((m.find("playerA"))->second) ==
                 Tools::ToLower(p2) &&
             Tools::ToLower((m.find("playerB"))->second) ==
                 Tools::ToLower(p1)));
  };

  std::string filename;

  if (year == "2015") {
    filename = "python/matches_2015_loop.csv";
  } else if (year == "2014") {
    filename = "python/matches_2014_atp.csv";
  } else {
    // return empty map
    return std::make_pair(result, -1);
  }

  std::cout << "Searching for " << p1 << " vs. " << p2 << " in " << filename
            << " at event " << tournament_name << std::endl;

  auto matches_found = CSVFile::FindInFile(filename, criterion);

  if (matches_found.size() != 1) {
    std::cout << "No matches found in local database." << std::endl;
    // return empty map
    return std::make_pair(result, -1);
  }

  std::map<std::string, std::string> row = matches_found.front();

  // Calculate spw:
  double fsp_1 = std::stod(row["a_first_serve_played"]) /
                 std::stod(row["a_first_serve_total"]);
  double fsw_1 = std::stod(row["a_first_serve_won"]) /
                 std::stod(row["a_first_serve_played"]);
  double ssw_1 = std::stod(row["a_second_serve_won"]) /
                 std::stod(row["a_second_serve_played"]);

  double spw_1 = fsp_1 * fsw_1 + (1 - fsp_1) * ssw_1;

  double fsp_2 = std::stod(row["b_first_serve_played"]) /
                 std::stod(row["b_first_serve_total"]);
  double fsw_2 = std::stod(row["b_first_serve_won"]) /
                 std::stod(row["b_first_serve_played"]);
  double ssw_2 = std::stod(row["b_second_serve_won"]) /
                 std::stod(row["b_second_serve_played"]);

  double spw_2 = fsp_2 * fsw_2 + (1 - fsp_2) * ssw_2;

  result[Tools::ToLower(row["playerA"])] = spw_1;
  result[Tools::ToLower(row["playerB"])] = spw_2;

  std::string actual_score = row["score"];

  int actual_games = 0;

  if (actual_score.find("RET") != std::string::npos ||
      actual_score.find("W/O") != std::string::npos) {
    return std::make_pair(result, actual_games);
  }

  auto sets = Tools::Split(actual_score, ' ');
  for (auto set : sets) {
    Tools::Trim(set, ';');
    set = set.substr(0, set.find("("));
    auto elts = Tools::Split(set, '-');
    actual_games += std::stoi(elts[0]) + std::stoi(elts[1]);
  }

  return std::make_pair(result, actual_games);
}

void run_model(std::string import_file, std::string output_name,
               std::string tournament, std::string year, bool bo5,
               bool recalculate = false) {
  if (Tools::FileExists(output_name) && !recalculate) {
    std::cout << "Already calculated " << import_file
              << ". Use recalculate option if desired." << std::endl;
    return;
  }

  std::vector<ModelData> m = ModelData::ImportFromFile(import_file);
  const unsigned int kSimulations = 1E4;

  CSVWriter w(output_name);

  for (const ModelData &cur_match : m) {
    // Run the adjusted model:
    AdjustedMCModel adj(cur_match.p1(), cur_match.p2(), bo5, cur_match,
                        kSimulations);

    // Run the iid model:
    std::map<std::string, double> iid_probs;

    iid_probs[cur_match.p1()] =
        cur_match.ServeWinProbabilityIID(cur_match.p1());
    iid_probs[cur_match.p2()] =
        cur_match.ServeWinProbabilityIID(cur_match.p2());

    IIDMCModel iid_model(cur_match.p1(), cur_match.p2(), bo5, iid_probs,
                         kSimulations);

    // Run the actual result:
    std::vector<Match> actual_matches;
    std::pair<std::map<std::string, double>, int> lookup_result =
        LookupSPW(tournament, year, cur_match.p1(), cur_match.p2());

    auto actual_spw = lookup_result.first;
    int actual_games = lookup_result.second;

    if (actual_spw.size() > 0) {
      std::cout << "Found actual result!" << std::endl;
      IIDMCModel actual_result(Tools::ToLower(cur_match.p1()),
                               Tools::ToLower(cur_match.p2()), bo5, actual_spw,
                               kSimulations);
      actual_matches = actual_result.matches();
    }

    // Find the matches:
    const std::vector<Match> &matches = adj.matches();
    const std::vector<Match> &iid_matches = iid_model.matches();

    // Calculate quantities of interest:
    double p1_served_first =
        MatchStats::PlayerServedFirst(cur_match.p1(), matches);
    double average_sets = MatchStats::AverageNumberOfSets(matches);
    double average_games = MatchStats::AverageMatchLength(matches);
    double non_iid_win_prob = MatchStats::MatchWinProb(cur_match.p1(), matches);

    double average_sets_iid = MatchStats::AverageNumberOfSets(iid_matches);
    double average_games_iid = MatchStats::AverageMatchLength(iid_matches);
    double iid_win_prob = MatchStats::MatchWinProb(cur_match.p1(), iid_matches);

    double actual_length_estimate =
        (actual_matches.size() > 0)
            ? MatchStats::AverageMatchLength(actual_matches)
            : -1;

    // Write to CSV:
    std::map<std::string, std::string> csv_results;

    csv_results["average_sets_dynamic"] = Tools::ToString(average_sets);
    csv_results["average_games_dynamic"] = Tools::ToString(average_games);
    csv_results["p_win_noniid"] = Tools::ToString(non_iid_win_prob);

    csv_results["average_sets_iid"] = Tools::ToString(average_sets_iid);
    csv_results["average_games_iid"] = Tools::ToString(average_games_iid);
    csv_results["p_win_iid"] = Tools::ToString(iid_win_prob);

    csv_results["actual_games_iid"] = Tools::ToString(actual_length_estimate);
    csv_results["actual_games"] =
        Tools::ToString(static_cast<unsigned int>(actual_games));

    csv_results["Player 1"] = cur_match.p1();
    csv_results["Player 2"] = cur_match.p2();
    csv_results["Match title"] = cur_match.match_title();

    csv_results["spw_iid_p1"] =
        Tools::ToString(cur_match.ServeWinProbabilityIID(cur_match.p1()));
    csv_results["spw_iid_p2"] =
        Tools::ToString(cur_match.ServeWinProbabilityIID(cur_match.p2()));

    w.WriteLine(csv_results);

    // Output them:
    std::cout << "Match of " << cur_match.p1() << " against " << cur_match.p2()
              << std::endl;
    std::cout << "IID probability: " << iid_win_prob * 100 << "%" << std::endl;
    std::cout << "Non-IID probability: " << non_iid_win_prob * 100 << "%"
              << std::endl;
    std::cout << "IID sets: " << average_sets_iid
              << "; non-IID: " << average_sets << std::endl;
    std::cout << "IID games: " << average_games_iid
              << "; non-IID: " << average_games << std::endl;
    std::cout << cur_match.p1() << " served first " << p1_served_first * 100
              << "% of the time." << std::endl;
    std::cout << "Actual length estimate is: " << actual_length_estimate
              << std::endl;
  }
}

/*void CheckIIDFromStats() {*/
// std::string tournament = "US Open";
// bool bo5 = true;
// const unsigned int kNumSimulations = 1E4;

// CSVWriter w(tournament + std::string(" with iid length prediction.csv"));

// for (auto row : matches_found) {
// std::string p1 = row["playerA"];
// std::string p2 = row["playerB"];

//// Calculate spw:
// double fsp_1 = std::stod(row["a_first_serve_played"]) /
// std::stod(row["a_first_serve_total"]);
// double fsw_1 = std::stod(row["a_first_serve_won"]) /
// std::stod(row["a_first_serve_played"]);
// double ssw_1 = std::stod(row["a_second_serve_won"]) /
// std::stod(row["a_second_serve_played"]);

// double spw_1 = fsp_1 * fsw_1 + (1 - fsp_1) * ssw_1;

// double fsp_2 = std::stod(row["b_first_serve_played"]) /
// std::stod(row["b_first_serve_total"]);
// double fsw_2 = std::stod(row["b_first_serve_won"]) /
// std::stod(row["b_first_serve_played"]);
// double ssw_2 = std::stod(row["b_second_serve_won"]) /
// std::stod(row["b_second_serve_played"]);

// double spw_2 = fsp_2 * fsw_2 + (1 - fsp_2) * ssw_2;

// std::map<std::string, double> spws;

// spws[p1] = spw_1;
// spws[p2] = spw_2;

// std::string actual_score = row["score"];

// if (actual_score.find("RET") != std::string::npos ||
// actual_score.find("W/O") != std::string::npos) {
// continue;
//}

// std::cout << p1 << " " << p2 << " " << spw_1 << " " << spw_2 << std::endl;
// std::cout << "Simulating..." << std::endl;
// IIDMCModel iid_model(p1, p2, bo5, spws, kNumSimulations);
// std::cout << "Finished simulating." << std::endl;
// MatchStats::SummaryStats s = MatchStats::Summarise(iid_model.matches());

// unsigned int actual_games = 0;

// auto sets = Tools::Split(actual_score, ' ');
// for (auto set : sets) {
// Tools::Trim(set, ';');
// set = set.substr(0, set.find("("));
// auto elts = Tools::Split(set, '-');
// actual_games += std::stoi(elts[0]) + std::stoi(elts[1]);
//}

// row["games"] = Tools::ToString(actual_games);
// row["simulated_games"] = Tools::ToString(s.average_length);

// w.WriteLine(row);

// std::cout << "Simulated: " << s.average_length
//<< "; actual: " << actual_games << " in: " << actual_score
//<< std::endl;
//}
//}

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
              << win_prob_p1 * 100 << "% chance of winning against "
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
  std::vector<unsigned int> num_simulations{10,   100,  200,  500,
                                            1000, 2000, 5000, 10000};

  std::ofstream o;

  o.open("convergence_bo5.csv");

  std::map<std::string, double> cur_spws;

  const unsigned int kNumSimulationsEach = 10;

  cur_spws["Player 1"] = 0.01;
  cur_spws["Player 2"] = 0.01;

  o << "No. of simulations"
    << ","
    << "Average Length" << std::endl;

  for (unsigned int simulation_no : num_simulations) {
    for (unsigned int i = 0; i < kNumSimulationsEach; ++i) {
      IIDMCModel cur_simulations("Player 1", "Player 2", true, cur_spws,
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

void ExportBPGrid() {
  unsigned int grid_size = 100;

  const unsigned int kMaxBPNo = 30;

  std::ofstream o;

  o.open("bp_grid_1e4_bo3.csv");

  double grid_min = 0.01;
  double grid_max = 0.99;

  double step_size = (grid_max - grid_min) / grid_size;

  const unsigned int kNumSimulations = 10000;

  std::string p1 = "Player 1";
  std::string p2 = "Player 2";

  o << "spw_1"
    << ","
    << "spw_2";

  for (unsigned int i = 0; i < kMaxBPNo; ++i) {
    o << "," << Tools::ToString(i);
  }

  bool bo5 = false;

  for (unsigned int i = 0; i < grid_size; ++i) {
    double cur_spw_1 = i * step_size + grid_min;

    for (unsigned int j = 0; j < grid_size; ++j) {
      double cur_spw_2 = j * step_size + grid_min;
      std::map<std::string, double> cur_spws;

      cur_spws[p1] = cur_spw_1;
      cur_spws[p2] = cur_spw_2;

      IIDMCModel cur_simulations(p1, p2, bo5, cur_spws, kNumSimulations);

      std::vector<Match> matches = cur_simulations.matches();

      auto result = MatchStats::NumberBreakPointsHist(matches);

      auto bpdist_p1 = result[p1];
      auto bpdist_p2 = result[p2];

      o << cur_spw_1 << "," << cur_spw_2;
      std::cout << cur_spw_1 << "," << cur_spw_2;

      for (unsigned int k = 0; k < kMaxBPNo; ++k) {
        auto cur_it = bpdist_p2.find(k);
        if (cur_it == bpdist_p2.end()) {
          o << "," << 0;
          std::cout << "," << 0;
        } else {
          o << "," << cur_it->second;
          std::cout << "," << cur_it->second;
        }
      }

      o << std::endl;
      std::cout << std::endl;
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

void CalculateNonIIDPredictionsATP() {
  bool bo5 = true;

  std::vector<std::string> years{"2014", "2014", "2015", "2015"};

  std::vector<std::string> tournaments{"Wimbledon", "US Open", "French Open",
                                       "Australian Open"};

  std::vector<std::string> to_predict{
      "wimbledon_prediction_2014.csv", "usopen_prediction_2014.csv",
      "frenchopen_prediction_2015.csv", "ausopen_prediction_2015.csv"};

  std::vector<std::string> names{"wimbledon2014", "usopen2014",
                                 "frenchopen2015", "ausopen2015"};

  for (unsigned int i = 0; i < to_predict.size(); ++i) {
    std::string cur_output_name = std::string("iid_vs_non_iid_prediction_") +
                                  names[i] + std::string(".csv");

    std::string cur_prediction_file = to_predict[i];

    run_model(cur_prediction_file, cur_output_name, tournaments[i], years[i],
              bo5, false);
  }
}

int main() { ExportBPGrid(); }
