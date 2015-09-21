#ifndef SERVICE_GAME_H
#define SERVICE_GAME_H

#include "point.h"
#include <iostream>

class ServiceGame {
  std::string server_;
  std::string returner_;

  std::vector<Point> points_;

  std::string winner_;

  std::pair<unsigned int, unsigned int> final_score_;

 public:
  ServiceGame(std::string server, std::string returner,
              std::vector<Point> points);

  friend std::ostream &operator<<(std::ostream &, const ServiceGame &);

  std::string winner() const;
  std::string server() const;
};

#endif
