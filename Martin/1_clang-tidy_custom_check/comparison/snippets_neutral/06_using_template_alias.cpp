#include <cstdio>

namespace logging {
template <typename... Args>
int printf(const char *fmt, Args... args) {
  return std::printf(fmt, args...);
}
}

using logging::printf;

namespace detail {
typedef int (*PrintfPtr)(const char *, ...);
static PrintfPtr kPrintfPtr = &std::printf;
}

#define TRACE(...) ::printf(__VA_ARGS__)

template <typename T>
void emit(T value) {
  printf("v=%d\n", static_cast<int>(value));
}

int main() {
  printf("via using: %d\n", 42);
  ::printf("direct libc: %d\n", 1);
  detail::kPrintfPtr("via pointer: %d\n", 2);
  TRACE("via macro: %d\n", 3);
  emit(7);
  emit(2.5);
  return 0;
}
