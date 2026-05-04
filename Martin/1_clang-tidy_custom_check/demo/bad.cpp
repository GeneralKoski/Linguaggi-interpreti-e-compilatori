// Esempio di codice che il check segnala.
// Usato nella demo del seminario.

extern "C" int printf(const char *fmt, ...);

int main() {
  int n = 7;
  printf("Hello, world!\n");
  printf("n = %d\n", n);
  printf("two args: %d %s\n", n, "ok");
  return 0;
}
