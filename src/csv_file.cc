#include "csv_file.h"

#include <assert.h>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <tools.h>

CSVFile::CSVFile(std::string csv_file) {
  // Open:
  i_.open(csv_file);
  assert(i_.good());

  // Read headers:
  std::string headers, cur_header;
  std::getline(i_, headers);
  std::istringstream first_line(headers);
  while (std::getline(first_line, cur_header, ',')) {
    // Make sure to trim off "s introduced by e.g. R:
    Tools::Trim(cur_header, '"');
    columns_.push_back(cur_header);
  }
}

bool CSVFile::NextLine() {
  // Read line:
  std::string cur_line, cur_entry;
  bool success = static_cast<bool>(std::getline(i_, cur_line));

  // Might have to do something else if this is called repeatedly after giving
  // false the first time...?
  if (!success) {
    return false;
  }

  // Could make this more resilient by reading into vector first, checking size
  // before immediately reading. But for well-formatted csvs, this should be
  // fine.
  std::istringstream entries(cur_line);
  for (unsigned int i = 0; i < columns_.size(); ++i) {
    std::getline(entries, cur_entry, ',');
    Tools::Trim(cur_entry, '"');
    const std::string &cur_col = columns_[i];
    cur_line_[cur_col] = cur_entry;
  }

  return true;
}

std::string CSVFile::FetchCol(std::string col_name) const {
  auto it = cur_line_.find(col_name);
  assert(it != cur_line_.end());
  return it->second;
}

bool CSVFile::ColumnExists(std::string name) const {
  return std::find(columns_.begin(), columns_.end(), name) != columns_.end();
}
