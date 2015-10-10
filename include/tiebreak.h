#ifndef TIEBREAK_H
#define TIEBREAK_H

#include "point.h"
#include <vector>
#include <string>

class Tiebreak {
  std::string server_;
  std::string returner_;
  std::vector<Point> points_;

  std::pair<unsigned int, unsigned int> final_score_;

 public:
  Tiebreak(std::string server, std::string returner, std::vector<Point> points);

  friend std::ostream &operator<<(std::ostream &, const Tiebreak &);

  std::string winner() const;
  std::string server() const;
};

#endif
