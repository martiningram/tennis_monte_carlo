#include "iid_mc_model.h"
#include "adjusted_mc_model.h"
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>

void test_win_prob() {
  std::string p1("Roger Federer");
  std::string p2("Rafael Nadal");

  std::map<std::string, double> probs;

  std::ofstream o;

  o.open("probability_non_iid_bo3_curve.csv");

  o << "p1"
    << ","
    << "p2"
    << ","
    << "pwin" << std::endl;

  probs[p2] = 0.6;
  probs[p1] = 0.6;

  unsigned int max_step = 100;

  for (unsigned int i = 0; i < max_step; ++i) {
    probs[p1] = i * 0.01;

    std::cout << "On step " << i << " of " << max_step << std::endl;

    o << probs[p1] << "," << probs[p2] << ",";

    unsigned int num_matches = 50000;

    AdjustedMCModel m(p1, p2, false, probs, 0.26, num_matches);

    const std::vector<Match> matches = m.matches();

    unsigned int num_won_p1 =
        std::count_if(matches.begin(), matches.end(),
                      [p1](const Match &m) { return m.winner() == p1; });

    double fraction_won = static_cast<float>(num_won_p1) / num_matches;

    o << fraction_won << std::endl;

    /*  for (const Match &match : matches) {*/
    // std::cout << match.final_score() << std::endl;
    /*}*/
  }

  o.close();
}

void verbose_test() {
  std::string p1("Roger Federer");
  std::string p2("Rafael Nadal");

  std::map<std::string, double> probs;

  probs[p2] = 0.5;
  probs[p1] = 0.4;

  unsigned int num_matches = 1;

  AdjustedMCModel m(p1, p2, true, probs, 0.26, num_matches, true);
}

int main() { test_win_prob(); }
