#include "mc_model.h"
#include <random>
#include <algorithm>

MCModel::MCModel(std::string p1, std::string p2, bool best_of_five,
                 unsigned int num_matches)
    : p1_(p1), p2_(p2), kNumMatches_(num_matches) {
  for (unsigned int i = 0; i < kNumMatches_; ++i) {
    matches_played_.push_back(PlayMatch());
  }
}

Match MCModel::PlayMatch() {
  // Coin toss:
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 1);

  unsigned int result = distribution(generator);

  std::string cur_server;
  std::string cur_returner;

  if (result == 0) {
    cur_server = p1_;
    cur_returner = p2_;
  } else {
    cur_server = p2_;
    cur_returner = p1_;
  }

  std::string server_at_start = cur_server;
  std::string returner_at_start = cur_returner;

  std::vector<Set> sets;

  // Initialise score:
  Score cur_score(cur_server, cur_returner,
                  std::vector<std::pair<unsigned int, unsigned int>>{
                      std::pair<unsigned int, unsigned int>(0, 0)},
                  std::pair<unsigned int, unsigned int>(0, 0));

  unsigned int target = (best_of_five_) ? 3 : 2;

  // Play the match:
  while (std::max(cur_score.sets_won(cur_server),
                  cur_score.sets_won(cur_returner)) != target) {
    sets.push_back(PlaySet(cur_server, cur_returner, cur_score));

    if (sets.back().last_server() == cur_server) {
      std::swap(cur_server, cur_returner);
    }
  }

  return Match(server_at_start, returner_at_start, sets);
}

Set MCModel::PlaySet(std::string cur_server, std::string cur_returner,
                     Score &cur_score) {
  std::string server_at_start = cur_server;
  std::string returner_at_start = cur_returner;
  std::vector<Game> games;

  // Play until the set may be over:
  while (cur_score.server_games() < 6 && cur_score.returner_games() < 6) {
    games.push_back(PlayGame(cur_server, cur_score));
    std::swap(cur_server, cur_returner);
  }

  // The set has now either been won, or it is (6,5) or (5,6).
  if (std::min(cur_score.server_games(), cur_score.returner_games()) == 5) {
    games.push_back(PlayGame(cur_server, cur_score));
    std::swap(cur_server, cur_returner);
  }

  Tiebreak *t;

  // Now we might be playing a tiebreak:
  if (cur_score.server_games() == 6 && cur_score.returner_games() == 6) {
    t = PlayTiebreak(cur_server, cur_returner, cur_score);
    std::swap(cur_server, cur_returner);
  } else {
    // We did not play a tiebreak this set.
    t = nullptr;
  }

  Set result(server_at_start, returner_at_start, games, t);
  cur_score.PlayerWinsSet(result.winner());

  return result;
}

Tiebreak *PlayTiebreak(std::string cur_server, std::string cur_returner,
                       Score &cur_score) {
  std::string server_at_start = cur_server;
  std::string returner_at_start = cur_returner;

  std::vector<Point> points;

  while (cur_score.server_points() < 7 && cur_score.returner_points() < 7) {
    if ((cur_score.server_points() + cur_score.returner_points()) % 2 == 1) {
      // Change the server
      std::swap(cur_server, cur_returner);
    }
    Point cur_point(cur_server, cur_returner, cur_score);
    PlayPoint(cur_point);
    std::string point_winner =
        (cur_point.server_won()) ? (cur_server) : (cur_returner);
    cur_score.PlayerWinsPoint(point_winner);
    points.push_back(cur_point);
  }

  // Check if the tiebreak is over:
  if (std::abs(cur_score.server_points() - cur_score.returner_points()) < 2) {
    if ((cur_score.server_points() + cur_score.returner_points()) % 2 == 1) {
      // Change the server
      std::swap(cur_server, cur_returner);
    }
    // Need to continue until a player has an advantage:
    while (
        std::abs(cur_score.server_points() - cur_score.returner_points() < 2)) {
      Point cur_point(cur_server, cur_returner, cur_score);
      PlayPoint(cur_point);
      std::string point_winner =
          (cur_point.server_won()) ? (cur_server) : (cur_returner);
      cur_score.PlayerWinsPoint(point_winner);
      points.push_back(cur_point);
    }
  }

  // The tiebreak is now over; update score and return it.
  cur_score.PlayerWinsGame(points.back().winner());
  return new Tiebreak(server_at_start, returner_at_start, points);
}

ServiceGame MCModel::PlayGame(std::string cur_server, std::string cur_returner,
                              Score &cur_score) {
  std::vector<Point> points;

  while (cur_score.server_points() < 4 && cur_score.returner_points() < 4) {
    Point cur_point(cur_server, cur_returner, cur_score);
    PlayPoint(cur_point);
    std::string point_winner =
        (cur_point.server_won()) ? (cur_server) : (cur_returner);
    cur_score.PlayerWinsPoint(point_winner);
    points.push_back(cur_point);
  }

  // Check if the game is over:
  if (std::abs(cur_score.server_points() - cur_score.returner_points()) < 2) {
    // Need to continue until a player has an advantage:
    while (
        std::abs(cur_score.server_points() - cur_score.returner_points() < 2)) {
      Point cur_point(cur_server, cur_returner, cur_score);
      PlayPoint(cur_point);
      std::string point_winner =
          (cur_point.server_won()) ? (cur_server) : (cur_returner);
      cur_score.PlayerWinsPoint(point_winner);
      points.push_back(cur_point);
    }
  }

  // The game is over. Update the score and construct it.
  cur_score.PlayerWinsGame(points.back().winner());
  return ServiceGame(cur_server, cur_returner, points);
}

void MCModel::PlayPoint(Point &p) {
  double win_prob = ServeWinProbability(p);

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  bool won = (distribution(generator) <= win_prob);

  p.set_server_won(won);

  return won;
}
