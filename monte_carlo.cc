#include "mc_model.h"

int main() {
  std::string p1("Roger Federer");
  std::string p2("Rafael Nadal");

  std::map<std::string, double> probs;

  probs[p1] = 0.7;
  probs[p2] = 0.65;

  IIDMCModel m(p1, p2, true, probs);
}
