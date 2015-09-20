#ifndef MATCH_H
#define MATCH_H

class Match {
  std::string server_;
  std::string returner_;

  bool server_won_;

  std::vector<Set> sets_;

  std::vector<std::pair<unsigned int, unsigned int>> final_score_;

 public:
  Match(std::string server, std::string returner, std::vector<Set> sets);
};

#endif
