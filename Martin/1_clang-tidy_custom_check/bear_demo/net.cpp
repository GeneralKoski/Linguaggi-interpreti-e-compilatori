#include "net.h"
#include "logger.h"

extern "C" int printf(const char *, ...);

int net_connect(const char *host, int port) {
  printf("connecting to %s:%d\n", host, port);
  log_info("ok");
  return 0;
}
