// File "pulito": nessuna chiamata a printf — clang-tidy non deve segnalare nulla qui.

#include <iostream>

int sum(int a, int b) { return a + b; }

void greet() { std::cout << "ciao\n"; }
