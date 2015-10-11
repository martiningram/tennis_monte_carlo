#include "csv_writer.h"
#include <assert.h>
#include <algorithm>

CSVWriter::CSVWriter(std::string csv_name) : first_line_written_(false) {
  o_.open(csv_name);
  assert(o_.good());
}

void CSVWriter::WriteHeaders(const std::map<std::string, std::string> &line) {
  for (const auto &p : line) {
    headers_.push_back(p.first);
  }

  // Write all but the last element:
  for (unsigned int i = 0; i < headers_.size() - 1; ++i) {
    o_ << headers_[i] << ",";
  }

  // Write the last element and terminate line:
  o_ << headers_.back() << std::endl;
}

void CSVWriter::WriteLine(const std::map<std::string, std::string> &line) {
  // Make sure file is still writable:
  assert(o_.good());

  // Write headers if necessary:
  if (!first_line_written_) {
    WriteHeaders(line);
    first_line_written_ = true;
  }

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
    o_ << line.find(cur_header)->second << ",";
  }

  // Write last element and terminate:
  o_ << line.find(headers_.back())->second << std::endl;
}

CSVWriter::~CSVWriter() { o_.close(); }
