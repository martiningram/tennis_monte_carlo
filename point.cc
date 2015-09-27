#include "point.h"
#include <assert.h>

Point::Point(std::string server, std::string returner, Score score)
    : server_(server), returner_(returner), score_(score) {}

void Point::set_server_won(bool server_won) {
  assert(!server_won_);
  server_won_.reset(new bool(server_won));
}

const Score &Point::score() const { return score_; }

std::string Point::server() const { return server_; }

std::string Point::returner() const { return returner_; }

bool Point::server_won() const {
  assert(server_won_);
  return (*server_won_);
}

std::string Point::winner() const {
  assert(server_won_);
  if (*server_won_) {
    return server_;
  } else {
    return returner_;
  }
}

bool Point::is_tiebreak() const {
  return (score_.player_games(server_) == 6 &&
          score_.player_games(returner_) == 6);
}

bool Point::is_point_before_bp() const {
  if (is_tiebreak()) {
    return false;
  }

  else {
    // 0-30, 15-30 or 30-30:
    bool before_deuce = (score_.player_points(returner_) == 2 &&
                         score_.player_points(server_) <= 2);

    // 40-40:
    bool deuce =
        score_.player_points(returner_) > 2 &&
        score_.player_points(returner_) == score_.player_points(server_);

    return before_deuce || deuce;
  }
}

bool Point::is_set_up() const {
  return (score_.sets_won(server_) > score_.sets_won(returner_));
}

bool Point::is_set_down() const {
  return (score_.sets_won(server_) < score_.sets_won(returner_));
}

bool Point::is_break_point() const {
  if (is_tiebreak()) {
    return false;
  }

  else {
    return (score_.player_points(returner_) >= 3 &&
            (static_cast<int>(score_.player_points(returner_)) -
             static_cast<int>(score_.player_points(server_))) == 1);
  }
}

std::ostream &operator<<(std::ostream &o, const Point &p) {
  o << p.server_ << ((*(p.server_won_)) ? (" won ") : (" lost ")) << "point at "
    << p.score_.player_points(p.server_) << "-"
    << p.score_.player_points(p.returner_);
  return o;
}
