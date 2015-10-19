#include "tiebreak.h"

Tiebreak::Tiebreak(std::string server, std::string returner,
                   std::vector<Point> points)
    : server_(server), returner_(returner), points_(points) {
  Score last_point_score = points.back().score();

  unsigned int server_points = last_point_score.player_points(server_);
  unsigned int returner_points = last_point_score.player_points(returner_);

  if (winner() == server_) {
    ++server_points;
  } else {
    ++returner_points;
  }

  final_score_ =
      std::pair<unsigned int, unsigned int>(server_points, returner_points);
}

std::string Tiebreak::winner() const { return points_.back().winner(); }

std::string Tiebreak::server() const { return server_; }

std::ostream &operator<<(std::ostream &o, const Tiebreak &t) {
  o << t.winner() << " wins tiebreak with score "
    << ((t.winner() == t.server_) ? t.final_score_.first
                                  : t.final_score_.second) << "-"
    << ((t.winner() == t.returner_) ? t.final_score_.first
                                    : t.final_score_.second);
  return o;
}

const std::vector<Point> &Tiebreak::points() const { return points_; }
