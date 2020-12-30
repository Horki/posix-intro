#ifndef CUSTOM_EXCEPTION_HH_
#define CUSTOM_EXCEPTION_HH_

#include <cerrno>   // errno
#include <cstring>  // strerror
#include <iostream>
#include <string>

class BaseExcInterface {
 public:
  virtual std::string what() const = 0;
  virtual ~BaseExcInterface() = 0;
};
BaseExcInterface::~BaseExcInterface() {}

class Exc : public BaseExcInterface {
 protected:
  std::string msg;

 public:
  Exc(const std::string& msg) : msg{msg} {}

  std::string what() const { return msg + ":" + strerror(errno); }
};

class PIDException : public Exc {
 public:
  PIDException(const std::string& msg) : Exc{msg} {}

  std::string what() const { return "PID: " + Exc::what(); }
};

class OpenException : public Exc {
 public:
  OpenException(const std::string& msg) : Exc{msg} {}

  std::string what() const { return "Open: " + Exc::what(); }
};

#endif