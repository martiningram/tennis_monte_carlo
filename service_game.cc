#include "service_game.h"
#include <vector>

ServiceGame::ServiceGame(std::string server, std::string returner,
                         std::vector<Point> points)
    : server_(server), returner_(returner), points_(points) {}
