// Snippet 3: chiamata via macro — il nome "printf" non appare a livello di token
// nel sito di chiamata. Un LLM via grep textuale può perderla.
// clang-tidy vede l'AST espanso e segnala comunque.

extern "C" int printf(const char *, ...);

#define LOG(x) printf("[LOG] " x "\n")

int main() {
  LOG("avvio");
  LOG("fine");
  return 0;
}
