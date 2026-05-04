// Esempio "pulito" che il check NON deve segnalare.

namespace mylib {
int printf(const char *);
}

void write(const char *);

int main() {
  write("Hello, world!\n");
  mylib::printf("non è quello di libc, ok");
  return 0;
}
