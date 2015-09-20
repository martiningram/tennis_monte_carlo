#ifndef SERVICE_GAME_H
#define SERVICE_GAME_H

#include "point.h"

class ServiceGame {
  std::string server_;
  std::string returner_;

  bool server_won_;

  std::vector<Point> points_;

 public:
  ServiceGame(std::string server, std::string returner,
              std::vector<Point> points);

  std::string winner() const;
  std::string server() const;
};

#endif
