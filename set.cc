#include "set.h"
#include "tiebreak.h"
#include <algorithm>

Set::Set(std::string server, std::string returner,
         std::vector<ServiceGame> games, Tiebreak *tiebreak)
    : server_(server), returner_(returner), games_(games), tiebreak_(tiebreak) {
  unsigned int server_games = std::count_if(
      games_.begin(), games_.end(),
      [this](const ServiceGame &g) { return g.winner() == server_; });
  unsigned int returner_games = std::count_if(
      games_.begin(), games_.end(),
      [this](const ServiceGame &g) { return g.winner() == returner_; });

  if (tiebreak_) {
    if (tiebreak_->winner() == server_) {
      ++server_games;
    } else {
      ++returner_games;
    }
  }

  final_score_ =
      std::pair<unsigned int, unsigned int>(server_games, returner_games);
}

const std::pair<unsigned int, unsigned int> &Set::final_score() const {
  return final_score_;
}

std::string Set::winner() const {
  if (tiebreak_) {
    return tiebreak_->winner();
  } else {
    return games_.back().winner();
  }
}

std::string Set::last_server() const {
  if (tiebreak_) {
    return tiebreak_->server();
  } else {
    return games_.back().server();
  }
}

const std::string &Set::server() const { return server_; }

const std::string &Set::returner() const { return returner_; }
