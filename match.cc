#include "match.h"
#include <algorithm>
#include <sstream>

Match::Match(std::string server, std::string returner, std::vector<Set> sets,
             bool best_of_five)
    : server_(server),
      returner_(returner),
      sets_(sets),
      best_of_five_(best_of_five) {
  // Find winner:
  unsigned int server_sets = std::count_if(
      sets.begin(), sets.end(),
      [this](const Set &s) { return s.winner() == this->server_; });

  bool server_won = best_of_five_ ? server_sets == 3 : server_sets == 2;

  if (server_won) {
    winner_ = server_;
  } else {
    winner_ = returner_;
  }
}

std::string Match::final_score() const {
  std::stringstream ss;

  for (const Set &s : sets_) {
    if (s.server() == winner_) {
      ss << s.final_score().first << "-" << s.final_score().second << " ";
    } else {
      ss << s.final_score().second << "-" << s.final_score().first << " ";
    }
  }

  std::string final_score = ss.str();

  final_score.pop_back();

  return final_score;
}

std::string Match::winner() const { return sets_.back().winner(); }
