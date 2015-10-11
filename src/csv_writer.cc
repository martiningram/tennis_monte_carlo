#include "csv_writer.h"
#include <assert.h>
#include <algorithm>

CSVWriter::CSVWriter(std::string csv_name,
                     const std::map<std::string, std::string> &first_line) {
  o_.open(csv_name);
  assert(o_.good());

  for (const auto &p : first_line) {
    headers_.push_back(p.first);
  }

  // Write all but the last element:
  for (unsigned int i = 0; i < headers_.size() - 1; ++i) {
    o_ << headers_[i] << ",";
  }

  // Write the last element and terminate line:
  o_ << headers_.back() << std::endl;

  // Write the first line:
  WriteLine(first_line);
}

void CSVWriter::WriteLine(std::map<std::string, std::string> line) {
  assert(o_.good());
  // Ensure the line is complete:
  // Same number of elements:
  assert(line.size() == headers_.size());
  // Same element names:
  for (const auto &p : line) {
    assert(std::find(headers_.begin(), headers_.end(), p.first) !=
           headers_.end());
  }

  // Write to file:
  for (unsigned int i = 0; i < headers_.size() - 1; ++i) {
    const std::string &cur_header = headers_[i];
    o_ << line[cur_header] << ",";
  }

  // Write last element and terminate:
  o_ << line[headers_.back()] << std::endl;
}

CSVWriter::~CSVWriter() { o_.close(); }
