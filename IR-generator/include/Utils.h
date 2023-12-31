#pragma once

#include <string>
#include <cstring>
#include <type_traits>

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

template<class To, class From>
std::enable_if_t<
    sizeof(To) == sizeof(From) &&
    std::is_trivially_copyable_v<From> &&
    std::is_trivially_copyable_v<To>,
    To>
bit_cast(const From& src) noexcept {
  static_assert(std::is_trivially_constructible_v<To>,
      "This implementation additionally requires "
      "destination type to be trivially constructible");

  To dst;
  std::memcpy(&dst, &src, sizeof(To));
  return dst;
}

void reportFatalError(const std::string &Msg);
void reportWarning(const std::string &Msg);

unsigned long long stoll(const std::string &Str);

} // namespace utils