#include "logger.h"
#include "net.h"

extern "C" int printf(const char *, ...);

int main(int argc, char **argv) {
  printf("avvio app, argc=%d\n", argc);
  log_info("inizio rete");
  net_connect("example.com", 80);
  printf("fine\n");
  return 0;
}
