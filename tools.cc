#include "tools.h"
#include <string>
#include <sstream>
#include <iostream>
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
