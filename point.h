#ifndef POINT_H
#define POINT_H

class Point {
  std::string server_;
  std::string returner_;

  Score score_;

  bool *server_won_;
  bool tiebreak_;

 public:
  Point(std::string server, std::string returner, Score score);

  void set_server_won(bool server_won);

  std::string server() const;
  std::string returner() const;
  bool server_won() const;
  std::string winner() const;
};

#endif
