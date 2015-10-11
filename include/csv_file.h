#ifndef CSV_READER_H
#define CSV_READER_H

#include <fstream>
#include <map>
#include <vector>
#include <string>

class CSVFile {
 public:
  CSVFile(std::string csv_file);
  bool NextLine();
  bool ColumnExists(std::string name) const;
  std::string FetchCol(std::string col_name) const;
  const std::vector<std::string> &col_names() const;

 private:
  std::ifstream i_;
  std::vector<std::string> columns_;
  std::map<std::string, std::string> cur_line_;
};

#endif
