#ifndef MATCH_H
#define MATCH_H

#include "set.h"
#include <string>
#include <vector>

class Match {
  std::string server_;
  std::string returner_;
  std::string winner_;

  std::vector<Set> sets_;

  bool best_of_five_;

 public:
  Match(std::string server, std::string returner, std::vector<Set> sets,
        bool best_of_five);

  std::string final_score() const;

  const std::vector<Set> &sets() const;

  std::string server_at_start() const;

  unsigned int total_games() const;

  std::string winner() const;
};

#endif
