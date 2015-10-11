#ifndef CSV_FILE_H
#define CSV_FILE_H

#include <fstream>
#include <functional>
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
  const std::map<std::string, std::string> &cur_line() const;

  static std::vector<std::map<std::string, std::string>> FindInFile(
      std::string filename,
      std::function<bool(const std::map<std::string, std::string> &)>);

 private:
  std::ifstream i_;
  std::vector<std::string> columns_;
  std::map<std::string, std::string> cur_line_;
};

#endif
