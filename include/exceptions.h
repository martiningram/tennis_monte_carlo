#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

class NoDataException : public std::exception {
  virtual const char* what() const throw() { return "Not enough data."; }
};

#endif
