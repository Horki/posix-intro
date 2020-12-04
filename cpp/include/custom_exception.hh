#ifndef CUSTOM_EXCEPTION_HH_
#define CUSTOM_EXCEPTION_HH_

#include <cerrno>   // errno
#include <cstring>  // strerror
#include <iostream>
#include <string_view>

struct Exc {
 protected:
  std::string msg;

 public:
  Exc(const std::string& msg) : msg{msg} {}
};

class BaseExcInterface {
 public:
  virtual std::string what() const = 0;
};

class PIDException : public Exc, BaseExcInterface {
 public:
  PIDException(const std::string& msg) : Exc{msg} {}

  std::string what() const { return "PID: " + msg + ":" + strerror(errno); }
};

class OpenException : public Exc, BaseExcInterface {
 public:
  OpenException(const std::string& msg) : Exc{msg} {}

  std::string what() const { return "Open: " + msg + ":" + strerror(errno); }
};

#endif