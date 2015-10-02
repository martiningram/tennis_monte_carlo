#ifndef SET_H
#define SET_H

#include "service_game.h"
#include <memory>
#include "tiebreak.h"

class Set {
  std::string server_;
  std::string returner_;

  bool server_won_;

  std::vector<ServiceGame> games_;

  std::pair<unsigned int, unsigned int> final_score_;

  std::shared_ptr<Tiebreak> tiebreak_;

 public:
  Set(std::string server, std::string returner, std::vector<ServiceGame> games,
      Tiebreak *tiebreak);

  std::string winner() const;
  std::string last_server() const;
  const std::string &server() const;
  const std::string &returner() const;
  const std::pair<unsigned int, unsigned int> &final_score() const;
};

#endif
