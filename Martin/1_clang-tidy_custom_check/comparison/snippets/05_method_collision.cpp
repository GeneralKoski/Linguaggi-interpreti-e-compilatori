// Snippet 5: trappola di falso positivo — metodo `printf` su classe utente.
// Il matcher AST con hasName("::printf") + callee FunctionDecl ignora il
// metodo `Logger::printf` (è un CXXMethodDecl, non la free function di libc).
// Un LLM tende a flaggare ogni occorrenza testuale del nome.

extern "C" int printf(const char *, ...);

struct Logger {
  void printf(const char *msg);
};

void Logger::printf(const char *msg) {
  ::printf("[Logger] %s\n", msg); // questo SÌ deve essere segnalato
}

int main() {
  Logger log;
  log.printf("hello from method"); // NON deve essere segnalato (metodo)
  ::printf("hello from libc\n");   // deve essere segnalato
  return 0;
}
