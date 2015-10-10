#include "score.h"
#include <assert.h>

Score::Score(std::string server, std::string receiver)
    : server_(server),
      receiver_(receiver),
      set_score_(std::vector<std::pair<unsigned int, unsigned int>>{
          std::pair<unsigned int, unsigned int>(0, 0)}),
      game_score_(std::pair<unsigned int, unsigned int>(0, 0)),
      sets_won_server_(0),
      sets_won_receiver_(0) {}

unsigned int Score::player_games(std::string player) const {
  if (player == server_) {
    return set_score_.back().first;
  } else {
    return set_score_.back().second;
  }
}

unsigned int Score::player_points(std::string player) const {
  assert(player == server_ || player == receiver_);
  if (player == server_) {
    return game_score_.first;
  } else {
    return game_score_.second;
  }
}

void Score::PlayerWinsPoint(std::string player) {
  assert(player == server_ || player == receiver_);
  if (player == server_) {
    ++game_score_.first;
  } else {
    ++game_score_.second;
  }
}

void Score::PlayerWinsGame(std::string player) {
  if (player == server_) {
    ++set_score_.back().first;
  } else {
    ++set_score_.back().second;
  }

  game_score_ = std::pair<unsigned int, unsigned int>(0, 0);
}

void Score::PlayerWinsSet(std::string player) {
  set_score_.push_back(std::pair<unsigned int, unsigned int>(0, 0));

  if (player == server_) {
    ++sets_won_server_;
  } else {
    ++sets_won_receiver_;
  }

  game_score_ = std::pair<unsigned int, unsigned int>(0, 0);
}

unsigned int Score::sets_won(std::string player) const {
  if (player == server_) {
    return sets_won_server_;
  } else {
    return sets_won_receiver_;
  }
}

const std::vector<std::pair<unsigned int, unsigned int>> &Score::set_score()
    const {
  return set_score_;
}

std::ostream &operator<<(std::ostream &o, const Score &s) {
  o << s.server_ << " vs. " << s.receiver_ << ": ";

  for (std::pair<unsigned int, unsigned int> p : s.set_score_) {
    o << p.first << "-" << p.second << " ";
  }

  o << s.game_score_.first << ":" << s.game_score_.second;
  return o;
}
