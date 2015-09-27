#include "iid_mc_model.h"
#include "adjusted_mc_model.h"
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "model_data.h"

void test_model() {
  std::vector<ModelData> m =
      ModelData::ImportFromFile("atp_points_predicted.csv");

  for (const ModelData &test : m) {
    bool bo5 = true;

    const unsigned int kSimulations = 10000;

    AdjustedMCModel adj(test.p1(), test.p2(), bo5, test, kSimulations);

    std::map<std::string, double> iid_probs;

    iid_probs[test.p1()] = test.ServeWinProbabilityIID(test.p1());
    iid_probs[test.p2()] = test.ServeWinProbabilityIID(test.p2());

    IIDMCModel iid_model(test.p1(), test.p2(), bo5, iid_probs, kSimulations);

    const std::vector<Match> &matches = adj.matches();
    const std::vector<Match> &iid_matches = iid_model.matches();

    unsigned int i = std::count_if(
        matches.begin(), matches.end(),
        [test](const Match &m) { return m.winner() == test.p1(); });

    unsigned int iid_i = std::count_if(
        iid_matches.begin(), iid_matches.end(),
        [test](const Match &m) { return m.winner() == test.p1(); });

    std::cout << "Match of " << test.p1() << " against " << test.p2()
              << std::endl;
    std::cout << "IID probability: "
              << (iid_i / static_cast<double>(kSimulations)) * 100 << "%"
              << std::endl;
    std::cout << "Non-IID probability: "
              << (i / static_cast<double>(kSimulations)) * 100 << "%"
              << std::endl;
  }
}

int main() { test_model(); }
