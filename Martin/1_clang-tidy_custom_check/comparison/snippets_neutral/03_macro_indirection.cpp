extern "C" int printf(const char *, ...);

#define LOG(x) printf("[LOG] " x "\n")

int main() {
  LOG("avvio");
  LOG("fine");
  return 0;
}
