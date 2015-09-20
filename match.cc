#include "match.h"

Match::Match(std::string server, std::string returner, std::vector<Set> sets)
    : server_(server), returner_(returner), sets_(sets) {}
