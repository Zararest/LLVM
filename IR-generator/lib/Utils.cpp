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

} // namespace utils