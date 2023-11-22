#pragma once

#include <string>

#define DEBUG

#ifdef DEBUG
#define DEBUG_EXPR(expr) (expr)
#else
#define DEBUG_EXPR(expr)
#endif

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

unsigned long long stoull(const std::string &Str);

} // namespace utils