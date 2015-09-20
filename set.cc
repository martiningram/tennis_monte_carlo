#include "set.h"

Set::Set(std::string server, std::string returner,
         std::vector<ServiceGame> games, Tiebreak *tiebreak)
    : server_(server),
      returner_(returner),
      games_(games),
      tiebreak_(tiebreak) {}

Set::~Set() {
  if (tiebreak) {
    delete tiebreak_;
  }
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
