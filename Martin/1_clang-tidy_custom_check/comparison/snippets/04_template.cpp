// Snippet 4: chiamata in un template.
// L'AST mostra il template generico + le specializzazioni concrete:
// clang-tidy può scegliere se segnalare entrambi.

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
