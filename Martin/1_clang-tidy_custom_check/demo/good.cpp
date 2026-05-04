// Esempio "pulito" che il check NON deve segnalare.

#include <iostream>

namespace mylib {
int printf(const char *);
}

int main() {
  std::cout << "Hello, world!\n";
  mylib::printf("non è quello di libc, ok");
  return 0;
}
