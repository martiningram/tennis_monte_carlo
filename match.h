#ifndef MATCH_H
#define MATCH_H

#include "set.h"
#include <string>
#include <vector>

class Match {
  std::string server_;
  std::string returner_;

  std::vector<Set> sets_;

 public:
  Match(std::string server, std::string returner, std::vector<Set> sets);
};

#endif
