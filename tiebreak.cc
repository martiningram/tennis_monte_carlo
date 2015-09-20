#include "tiebreak.h"

Tiebreak::Tiebreak(std::string server, std::string returner,
                   std::vector<Point> points)
    : server_(server), returner_(returner), points_(points) {}

std::string Tiebreak::winner() const { return points_.back().winner(); }

std::string Tiebreak::server() const { return server_; }
