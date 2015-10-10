#ifndef SCORE_H
#define SCORE_H

#include <vector>
#include <string>
#include <iostream>

class Score {
  std::string server_;
  std::string receiver_;

  std::vector<std::pair<unsigned int, unsigned int>> set_score_;
  std::pair<unsigned int, unsigned int> game_score_;

  unsigned int sets_won_server_, sets_won_receiver_;

 public:
  Score(std::string server, std::string receiver);

  void PlayerWinsPoint(std::string player);
  void PlayerWinsGame(std::string player);
  void PlayerWinsSet(std::string player);

  friend std::ostream &operator<<(std::ostream &, const Score &);

  unsigned int player_games(std::string player) const;
  unsigned int player_points(std::string player) const;

  unsigned int sets_won(std::string player) const;

  const std::vector<std::pair<unsigned int, unsigned int>> &set_score() const;
};

#endif
