extern "C" int printf(const char *, ...);

template <typename T>
void log_value(T value) {
  printf("v=%d\n", static_cast<int>(value));
}

int main() {
  log_value(1);
  log_value(2.5);
  return 0;
}
