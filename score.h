#ifndef SCORE_H
#define SCORE_H

class Score {
  std::string server_;
  std::string receiver_;

  std::vector<std::pair<unsigned int, unsigned int>> set_score_;
  std::pair<unsigned int, unsigned int> game_score_;

 public:
  Score(std::string server, std::string receiver,
        std::vector<std::pair<unsigned int, unsigned int>> set_score,
        std::pair<unsigned int, unsigned int> game_score);

  void PlayerWinsPoint(std::string player);
  void PlayerWinsGame(std::string player);
  void PlayerWinsSet(std::string player);

  unsigned int server_games() const;
  unsigned int returner_games() const;
  unsigned int server_points() const;
  unsigned int returner_points() const;

  unsigned int sets_won(std::string player) const;
};

#endif
