#pragma once

#include <string>

namespace utils {

template <typename It>
class IteratorRange {
  It Begin;
  It End;

public:
  IteratorRange(It Begin, It End) : Begin{Begin}, End{End} {}

  It begin() { return Begin; }
  It end() { return End; }
};

template <typename It>
IteratorRange<It> makeRange(It Begin, It End) {
  return IteratorRange<It>{Begin, End};
}

void reportFatalError(const std::string &Msg);
void reportWarning(const std::string &Msg);

} // namespace utils