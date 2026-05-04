// Snippet 2: caso ambiguo per un LLM — printf ma in namespace utente.
// clang-tidy lavora sull'AST e distingue ::printf da mylib::printf.
// Un LLM in genere si fida del nome e segnala falsi positivi.

namespace mylib {
int printf(const char *);
}

extern "C" int printf(const char *, ...);

int caller() {
  mylib::printf("ok, non è quello di libc"); // NON deve essere segnalato
  ::printf("invece questo sì\n");            // deve essere segnalato
  return 0;
}
