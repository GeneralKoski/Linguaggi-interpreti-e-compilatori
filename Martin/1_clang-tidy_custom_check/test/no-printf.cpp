// RUN: %check_clang_tidy %s misc-no-printf %t

extern "C" int printf(const char *fmt, ...);

void caller() {
  printf("hello %d\n", 42);
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: uso di 'printf' sconsigliato in C++ moderno
  // CHECK-FIXES: std::print("hello %d\n", 42);

  int x = 1;
  printf("%d", x);
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: uso di 'printf' sconsigliato in C++ moderno
  // CHECK-FIXES: std::print("%d", x);
}

namespace mylib {
int printf(const char *);
}

void ok() {
  // Non deve triggerare: printf locale, non quello di libc
  mylib::printf("nope");
}
