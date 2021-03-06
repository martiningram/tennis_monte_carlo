#include "match.h"
#include <algorithm>
#include <sstream>

Match::Match(std::string server, std::string returner, std::vector<Set> sets,
             bool best_of_five)
    : server_(server),
      returner_(returner),
      sets_(sets),
      best_of_five_(best_of_five) {
  // Find winner:
  unsigned int server_sets = std::count_if(
      sets.begin(), sets.end(),
      [this](const Set &s) { return s.winner() == this->server_; });

  bool server_won = best_of_five_ ? server_sets == 3 : server_sets == 2;

  if (server_won) {
    winner_ = server_;
  } else {
    winner_ = returner_;
  }
}

std::string Match::final_score() const {
  std::stringstream ss;

  for (const Set &s : sets_) {
    if (s.server() == winner_) {
      ss << s.final_score().first << "-" << s.final_score().second << " ";
    } else {
      ss << s.final_score().second << "-" << s.final_score().first << " ";
    }
  }

  std::string final_score = ss.str();

  final_score.pop_back();

  return final_score;
}

std::string Match::winner() const { return sets_.back().winner(); }

const std::vector<Set> &Match::sets() const { return sets_; }

unsigned int Match::total_games() const {
  unsigned int sum = 0;
  for (const Set &s : sets_) {
    sum += s.final_score().first + s.final_score().second;
  }
  return sum;
}

std::string Match::server_at_start() const { return server_; }

std::pair<std::string, std::string> Match::players() const {
  return std::make_pair(server_, returner_);
}

std::vector<Point> Match::GetServicePoints(std::string player) const {
  std::vector<Point> pts;
  for (const Set &s : sets_) {
    for (const ServiceGame &g : s.games()) {
      if (!(g.server() == player)) {
        continue;
      }
      const std::vector<Point> &cur_pts = g.points();
      for (const Point &pt : cur_pts) {
        pts.push_back(pt);
      }
    }
    if (s.tiebreak()) {
      for (const Point &pt : s.tiebreak()->points()) {
        pts.push_back(pt);
      }
    }
  }
  return pts;
}
