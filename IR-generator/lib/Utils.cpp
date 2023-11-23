#include "Utils.h"

#include <iostream>

namespace utils {

void reportFatalError(const std::string &Msg) {
  std::cerr << "Error: " << Msg << std::endl;
  exit(-1);
}

void reportWarning(const std::string &Msg) {
  std::cerr << "Warning: " << Msg << std::endl;
}

unsigned long long stoll(const std::string &Str) {
  auto Val = 0ull;
  try {
    Val = std::stoll(Str);
  } catch (std::invalid_argument &E) {
    reportFatalError("Can't reconnize number: {" + Str + "}");
  }
  return Val;
}

} // namespace utils