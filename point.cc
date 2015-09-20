#include "point.h"
#include <assert.h>

Point::Point(std::string server, std::string returner, Score score)
    : server_(server),
      returner_(returner),
      score_(score),
      server_won(nullptr) {}

void Point::set_server_won(bool server_won) {
  assert(!server_won_);
  server_won_ = new bool(server_won);
}

Point::~Point() {
  if (server_won_) {
    delete server_won_;
  }
}

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
