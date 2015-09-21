#include "service_game.h"
#include <vector>

ServiceGame::ServiceGame(std::string server, std::string returner,
                         std::vector<Point> points)
    : server_(server),
      returner_(returner),
      points_(points),
      winner_(points.back().winner()) {
  Score last_point_score = points.back().score();

  unsigned int server_points = last_point_score.player_points(server_);
  unsigned int returner_points = last_point_score.player_points(returner_);

  if (winner_ == server_) {
    ++server_points;
  }

  else {
    ++returner_points;
  }

  final_score_ =
      std::pair<unsigned int, unsigned int>(server_points, returner_points);
}

std::string ServiceGame::winner() const { return winner_; }

std::string ServiceGame::server() const { return server_; }

std::ostream &operator<<(std::ostream &o, const ServiceGame &g) {
  o << g.server_ << "'s service game finishes with score "
    << g.final_score_.first << "-" << g.final_score_.second;
  return o;
}
