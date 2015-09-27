#ifndef POINT_H
#define POINT_H

#include <string>
#include <memory>
#include "score.h"
#include <iostream>

class Point {
  std::string server_;
  std::string returner_;

  Score score_;

  std::shared_ptr<bool> server_won_;

 public:
  Point(std::string server, std::string returner, Score score);

  friend std::ostream &operator<<(std::ostream &, const Point &);

  void set_server_won(bool server_won);

  bool is_break_point() const;
  bool is_tiebreak() const;
  bool is_point_before_bp() const;
  bool is_set_up() const;
  bool is_set_down() const;

  const Score &score() const;

  std::string server() const;
  std::string returner() const;
  bool server_won() const;
  std::string winner() const;
};

#endif
