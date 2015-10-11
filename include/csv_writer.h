#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <map>
#include <string>
#include <vector>
#include <fstream>

class CSVWriter {
 public:
  /**
   * @param csv_name is the output name for the csv.
   * @param first_line is the first line to write to the csv. The columns will
   * be determined automatically from the map.
   */
  CSVWriter(std::string csv_name,
            const std::map<std::string, std::string> &first_line);

  void WriteLine(std::map<std::string, std::string> line);
  ~CSVWriter();

 private:
  std::ofstream o_;
  std::vector<std::string> headers_;
};

#endif
