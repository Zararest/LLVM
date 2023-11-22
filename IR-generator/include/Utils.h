#pragma once

#include <string>

namespace utils {

void reportFatalError(const std::string &Msg);
void reportWarning(const std::string &Msg);

} // namespace utils