#include "mc_model.h"
#include <algorithm>
#include "tiebreak.h"
#include <iostream>
#include <chrono>

MCModel::MCModel(std::string p1, std::string p2, bool best_of_five,
                 unsigned int num_matches, bool verbose)
    : generator_(std::time(0)),
      p1_(p1),
      p2_(p2),
      best_of_five_(best_of_five),
      kNumMatches_(num_matches),
      verbose_(verbose) {}

Match MCModel::PlayMatch() {
  // Coin toss:
  int result;

  for (unsigned int i = 0; i < 10; ++i) {
    std::uniform_int_distribution<int> distribution(0, 1);
    result = distribution(generator_);
    if (verbose_) {
      std::cout << "The generator produced: " << result << std::endl;
    }
  }

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

  if (verbose_) {
    std::cout << server_at_start << " is serving first; " << returner_at_start
              << " second." << std::endl;
  }

  std::vector<Set> sets;

  // Initialise score:
  Score cur_score(cur_server, cur_returner);

  unsigned int target = (best_of_five_) ? 3 : 2;

  // Play the match:
  while (std::max(cur_score.sets_won(cur_server),
                  cur_score.sets_won(cur_returner)) != target) {
    sets.push_back(PlaySet(cur_server, cur_returner, cur_score));

    if (sets.back().last_server() == cur_server) {
      std::swap(cur_server, cur_returner);
    }
  }

  if (verbose_) {
    std::cout << cur_score << std::endl;
  }

  return Match(server_at_start, returner_at_start, sets, best_of_five_);
}

Set MCModel::PlaySet(std::string cur_server, std::string cur_returner,
                     Score &cur_score) {
  std::string server_at_start = cur_server;
  std::string returner_at_start = cur_returner;
  std::vector<ServiceGame> games;

  // Play until the set may be over:
  while (cur_score.player_games(cur_server) < 6 &&
         cur_score.player_games(cur_returner) < 6) {
    games.push_back(PlayGame(cur_server, cur_returner, cur_score));
    if (verbose_) {
      std::cout << games.back() << std::endl;
    }
    std::swap(cur_server, cur_returner);
  }

  // The set has now either been won, or it is (6,5) or (5,6).
  if (std::min(cur_score.player_games(cur_server),
               cur_score.player_games(cur_returner)) == 5) {
    games.push_back(PlayGame(cur_server, cur_returner, cur_score));
    std::swap(cur_server, cur_returner);
  }

  Tiebreak *t;

  // Now we might be playing a tiebreak:
  if (cur_score.player_games(cur_server) == 6 &&
      cur_score.player_games(cur_returner) == 6) {
    t = PlayTiebreak(cur_server, cur_returner, cur_score);
    std::swap(cur_server, cur_returner);
    if (verbose_) {
      std::cout << (*t) << std::endl;
    }
  } else {
    // We did not play a tiebreak this set.
    t = nullptr;
  }

  Set result(server_at_start, returner_at_start, games, t);
  cur_score.PlayerWinsSet(result.winner());

  if (verbose_) {
    std::cout << result.winner() << " wins a set." << std::endl;
  }

  return result;
}

Tiebreak *MCModel::PlayTiebreak(std::string cur_server,
                                std::string cur_returner, Score &cur_score) {
  std::string server_at_start = cur_server;
  std::string returner_at_start = cur_returner;

  std::vector<Point> points;

  while (cur_score.player_points(cur_server) < 7 &&
         cur_score.player_points(cur_returner) < 7) {
    if ((cur_score.player_points(cur_server) +
         cur_score.player_points(cur_returner)) %
            2 ==
        1) {
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

  auto difference = [cur_server, cur_returner](const Score &s) {
    return std::abs(static_cast<int>(s.player_points(cur_server)) -
                    static_cast<int>(s.player_points(cur_returner)));
  };

  // Check if the tiebreak is over:
  if (difference(cur_score) < 2) {
    if ((cur_score.player_points(cur_server) +
         cur_score.player_points(cur_returner)) %
            2 ==
        1) {
      // Change the server
      std::swap(cur_server, cur_returner);
    }
    // Need to continue until a player has an advantage:
    while (difference(cur_score) < 2) {
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

  while (cur_score.player_points(cur_server) < 4 &&
         cur_score.player_points(cur_returner) < 4) {
    Point cur_point(cur_server, cur_returner, cur_score);
    PlayPoint(cur_point);
    std::string point_winner =
        (cur_point.server_won()) ? (cur_server) : (cur_returner);
    cur_score.PlayerWinsPoint(point_winner);
    points.push_back(cur_point);
  }

  auto difference = [cur_server, cur_returner](const Score &s) {
    return std::abs(static_cast<int>(s.player_points(cur_server)) -
                    static_cast<int>(s.player_points(cur_returner)));
  };

  // Check if the game is over:
  while (difference(cur_score) < 2) {
    Point cur_point(cur_server, cur_returner, cur_score);
    PlayPoint(cur_point);
    std::string point_winner =
        (cur_point.server_won()) ? (cur_server) : (cur_returner);
    cur_score.PlayerWinsPoint(point_winner);
    points.push_back(cur_point);
  }

  // The game is over. Update the score and construct it.
  cur_score.PlayerWinsGame(points.back().winner());

  if (verbose_) {
    std::cout << "The score is now: " << cur_score.player_games(p1_) << "-"
              << cur_score.player_games(p2_) << std::endl;
  }

  return ServiceGame(cur_server, cur_returner, points);
}

void MCModel::PlayPoint(Point &p) {
  double win_prob = ServeWinProbability(p);

  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  bool won = (distribution(generator_) <= win_prob);

  p.set_server_won(won);

  if (verbose_) {
    std::cout << p << std::endl;
  }
}
