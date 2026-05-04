// Snippet 6: caso composito — using-declaration + template wrapper +
// function pointer + macro variadic + istanze template multiple.
// Stress-test sia per il matcher (che deve seguire ciò che overload
// resolution ha effettivamente scelto, non il testo del sorgente) sia per
// l'LLM (che deve seguire ::, using, puntatori e macro).

#include <cstdio>

namespace logging {
template <typename... Args>
int printf(const char *fmt, Args... args) {
  return std::printf(fmt, args...); // SÌ: chiamata qualificata a libc
}
}

using logging::printf; // porta logging::printf nel namespace globale

namespace detail {
typedef int (*PrintfPtr)(const char *, ...);
static PrintfPtr kPrintfPtr = &std::printf; // alias a libc — NON flaggato
}

#define TRACE(...) ::printf(__VA_ARGS__) // espansione: vedi sotto

template <typename T>
void emit(T value) {
  printf("v=%d\n", static_cast<int>(value)); // logging::printf
}

int main() {
  printf("via using: %d\n", 42);              // logging::printf
  ::printf("direct libc: %d\n", 1);           // overload resolution → template!
  detail::kPrintfPtr("via pointer: %d\n", 2); // ptr indiretto, non matchato
  TRACE("via macro: %d\n", 3);                // espande in ::printf con int → template
  emit(7);
  emit(2.5);
  return 0;
}

// Ground truth VERIFICATA: 1 solo hit del check `misc-no-printf`.
//   - riga "return std::printf(fmt, args...);" → 1 hit
//
// Tutto il resto NON è segnalato. Il motivo è didatticamente importante:
//
// `using logging::printf` introduce il template nel namespace globale
// accanto a `::printf` di libc. Per overload resolution, dato un argomento
// tipizzato (es. `int`), il template `logging::printf<int>(const char*, int)`
// è un match migliore della variadic `int printf(const char*, ...)` di libc.
// Quindi `::printf("...", 1)` e `TRACE("...", 3)` chiamano in realtà il
// template, non libc — e il check, lavorando sull'AST post-resolution, lo sa.
//
// Un LLM che ragiona sul testo vede `::printf(...)` e probabilmente lo
// segnala come libc → falso positivo, esattamente la dimostrazione del
// valore di un check AST-based.
