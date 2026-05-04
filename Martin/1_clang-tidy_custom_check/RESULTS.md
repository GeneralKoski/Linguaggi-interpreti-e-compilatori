# Output reale del check `misc-no-printf` (run del 2026-05-04)

Build: LLVM 23.0.0git, Apple Silicon, Release con assertions OFF.

## demo/bad.cpp — 3 hit attesi

```
warning: uso di 'printf' sconsigliato in C++ moderno; usare 'std::print' [misc-no-printf]
    8 |   printf("Hello, world!\n");
      |   ^~~~~~
      |   std::print
    9 |   printf("n = %d\n", n);
   10 |   printf("two args: %d %s\n", n, "ok");
```
✅ 3 warning, fix-it preciso (sostituisce solo il token `printf`).

## demo/good.cpp — atteso: silenzio

✅ 0 warning. `mylib::printf` non viene segnalato (scope corretto).

## comparison/snippets

| # | File | Hit attesi | Hit ottenuti | Note |
|---|------|-----------|--------------|------|
| 1 | 01_basic.cpp | 1 | 1 | caso base |
| 2 | 02_namespace_collision.cpp | 1 (solo `::printf`) | 1 | **AST distingue lo scope** — `mylib::printf` ignorato |
| 3 | 03_macro_indirection.cpp | 2 (dentro macro `LOG`) | 2 | clang-tidy **vede l'AST espanso**: la macro non lo nasconde |
| 4 | 04_template.cpp | 1 generico + 2 istanze | 3 | il template viene segnalato sia in forma generica sia per ciascuna istanziazione |

### Punto chiave per il seminario

Sui tre casi "non triviali" (scope, macro, template) un linter **testuale** o un LLM che ragiona per pattern di stringa sbaglierebbe: il vantaggio decisivo di clang-tidy è lavorare sull'**AST arricchito di semantica** (scope risolti, macro espanse, template istanziati).
