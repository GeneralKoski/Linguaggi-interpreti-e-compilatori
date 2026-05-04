#include "logger.h"

extern "C" int printf(const char *, ...);
extern "C" int fprintf(void *, const char *, ...);

void log_info(const char *msg) { printf("[INFO] %s\n", msg); }
void log_err(const char *msg) { printf("[ERR ] %s\n", msg); }
