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

## Run su progetto multi-file con Bear (`bear_demo/`)

Build esistente intercettata con `bear -- make`, poi clang-tidy lanciato con `-p .` sul `compile_commands.json` generato.

| File | printf veri | hit clang-tidy |
|------|------------|----------------|
| main.cpp | 2 | 2 ✅ |
| logger.cpp | 2 | 2 ✅ |
| net.cpp | 1 | 1 ✅ |
| util.cpp | 0 (file pulito) | 0 ✅ |

Output integrale: `bear_demo/run_output.txt`. Totale: **5/5 hit**, 0 falsi positivi.

### Punto chiave Bear

Il `Makefile` non sa nulla di clang-tidy: Bear si **intercetta sopra** la build esistente e produce il database. Per il seminario è il pattern realistico: un team eredita un progetto con build proprio (Make/CMake/Bazel) e vuole aggiungerci un linter senza toccare il sistema di build.

## Lit test ufficiale (build farm di llvm-project)

Test inserito in `clang-tools-extra/test/clang-tidy/checkers/misc/no-printf.cpp` ed eseguito con la pipeline lit standard:

```bash
CLANG=/usr/bin/clang ~/llvm-project/build/bin/llvm-lit \
  ~/llvm-project/clang-tools-extra/test/clang-tidy/checkers/misc/no-printf.cpp -v
```

Risultato:
```
PASS: Clang Tools :: clang-tidy/checkers/misc/no-printf.cpp (1 of 1)
Testing Time: 0.27s
Total Discovered Tests: 1
  Passed: 1 (100.00%)
```

✅ Sia `CHECK-MESSAGES` che `CHECK-FIXES` (verifica del fix-it applicato) passano.

### Note di setup

Per far girare il lit harness sono necessari due binari oltre a `clang-tidy`:
- `FileCheck` (compilato con `ninja FileCheck`)
- un `clang` da usare come driver — qui ho passato il sistema (`CLANG=/usr/bin/clang`) per evitare di buildare anche `clang` (~altri 30+ minuti)
