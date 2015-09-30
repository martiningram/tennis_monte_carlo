#include "tools.h"

std::string& Tools::Trim(std::string& str, char c) {
  str.erase(0, str.find_first_not_of(c));
  str.erase(str.find_last_not_of(c) + 1);
  return str;
}
