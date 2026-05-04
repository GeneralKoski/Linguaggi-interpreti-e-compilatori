extern "C" int printf(const char *, ...);

struct Logger {
  void printf(const char *msg);
};

void Logger::printf(const char *msg) {
  ::printf("[Logger] %s\n", msg);
}

int main() {
  Logger log;
  log.printf("hello from method");
  ::printf("hello from libc\n");
  return 0;
}
