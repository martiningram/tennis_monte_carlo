#ifndef SET_H
#define SET_H

class Set {
  std::string server_;
  std::string returner_;

  bool server_won_;

  std::vector<ServiceGame> games_;

  std::pair<unsigned int, unsigned int> final_score_;

  Tiebreak* tiebreak_;

 public:
  Set(std::string server, std::string returner, std::vector<ServiceGame> games,
      Tiebreak* tiebreak);

  ~Set();

  std::string winner() const;
  std::string last_server() const;
};

#endif
