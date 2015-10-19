#include "tools.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "exceptions.h"

std::string& Tools::Trim(std::string& str, char c) {
  str.erase(0, str.find_first_not_of(c));
  str.erase(str.find_last_not_of(c) + 1);
  return str;
}

double Tools::ReadDouble(std::istringstream& iss) {
  std::string string_version;
  std::getline(iss, string_version, ',');
  if (string_version == "NA") {
    throw NoDataException();
  }
  return std::stod(string_version);
}

int Tools::ReadInt(std::istringstream& iss) {
  std::string string_version;
  std::getline(iss, string_version, ',');
  return std::stoi(string_version);
}

std::string Tools::ReadString(std::istringstream& iss) {
  std::string string_version;
  std::getline(iss, string_version, ',');
  return string_version;
}

bool Tools::FileExists(std::string filename) {
  std::ifstream i;
  i.open(filename);
  return (i.good());
}

std::string Tools::ToString(double d) {
  std::stringstream s;
  s << d;
  return s.str();
}

std::string Tools::ToString(unsigned int i) {
  std::stringstream s;
  s << i;
  return s.str();
}

std::vector<std::string> Tools::Split(std::string s, char on) {
  std::istringstream i(s);
  std::vector<std::string> result;
  std::string cur_tok;

  while (std::getline(i, cur_tok, on)) {
    result.emplace_back(cur_tok);
  }

  return result;
}

std::vector<std::map<std::string, std::string>> Tools::Filter(
    const std::vector<std::map<std::string, std::string>>& rows,
    std::function<bool(const std::map<std::string, std::string>&)> f) {
  std::vector<std::map<std::string, std::string>> filtered;
  for (auto const& row : rows) {
    if (f(row)) {
      filtered.emplace_back(row);
    }
  }
  return filtered;
}

std::string Tools::ToLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}
