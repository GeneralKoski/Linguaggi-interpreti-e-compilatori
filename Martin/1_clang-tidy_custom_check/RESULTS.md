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

## comparison/snippets — 6 casi (4 originali + 2 estensioni del 2026-05-05)

| # | File | Hit attesi | Hit ottenuti | Note |
|---|------|-----------|--------------|------|
| 1 | 01_basic.cpp | 1 | 1 | caso base |
| 2 | 02_namespace_collision.cpp | 1 (solo `::printf`) | 1 | **AST distingue lo scope** — `mylib::printf` ignorato |
| 3 | 03_macro_indirection.cpp | 2 (dentro macro `LOG`) | 2 | clang-tidy **vede l'AST espanso**: la macro non lo nasconde |
| 4 | 04_template.cpp | 1 generico + 2 istanze | 3 | il template viene segnalato sia in forma generica sia per ciascuna istanziazione |
| 5 | 05_method_collision.cpp | 2 (solo `::printf`, ignora `Logger::printf`) | 2 | **trappola di falso positivo**: `log.printf("...")` è `CXXMemberCallExpr`, non `::printf` |
| 6 | 06_using_template_alias.cpp | **1** (solo `std::printf` dentro il wrapper) | 1 | **caso composito**: dopo `using logging::printf;`, l'overload resolution dirotta `::printf("...", int)` al template `logging::printf<int>`. Solo la chiamata esplicita `std::printf` dentro il wrapper viene flaggata. |

### Punto chiave per il seminario

Sui cinque casi "non triviali" (scope, macro, template, metodo, overload resolution composita) clang-tidy lavora sull'**AST arricchito di semantica** (scope risolti, macro espanse, template istanziati, overload resolution post-compilazione). In particolare lo snippet 6 mostra che il matcher non guarda il testo `::printf`: traccia esattamente la `FunctionDecl` che il compilatore ha effettivamente scelto.

## Confronto clang-tidy vs LLM (run reali del 2026-05-05)

24 chat fresche totali: 6 snippet × {Claude Opus 4.7, ChatGPT 5.5} × 2 esecuzioni indipendenti. Metodologia in `comparison/llm_responses/_PROMPT.md`. Risposte integrali e analisi per snippet in `comparison/llm_responses/0[1-6]_*_{claude,chatgpt}.md`. Tabella consolidata + take-away in `comparison/COMPARISON.md`.

### Risultato sintetico

| Snippet | Claude (sintesi 2 chat) | ChatGPT (sintesi 2 chat) |
|---|---|---|
| 01 basic | trova printf ✓, fix `std::println` ✓, det. alto | trova printf ✓, fix `std::cout`/`std::puts` (mai `std::print`), det. medio |
| 02 namespace | scope OK ✓, fix `std::println` ✓, det. alto | scope OK ✓, fix `std::printf`, trova linker error fuori scope, det. medio |
| 03 macro | macro vista ✓, fix `std::println` ✓, det. alto | macro vista ✓, fix `std::cout`/`std::printf`, det. medio (3 vs 5 issue) |
| 04 template | trova bug semantico (`2.5→2`) ⭐, fix `std::println` ✓, det. alto | trova bug semantico ⭐, fix `std::cout`/`std::format`, det. medio (concept vs overload) |
| 05 method | scope OK ✓ (passa la trappola), fix `std::printf` qualificato, det. alto | scope OK ✓, trova bug nullptr ⭐, fix `std::printf`, det. medio |
| 06 composito | **1ª: scope-aware ✓, 2ª: scope-blind ✗** — det. **rotto** sul finding chiave | **1ª: scope-blind ✗, 2ª: scope-aware ✓** — det. **rotto** sul finding chiave |

### Wow moment confermato — snippet 06

```
clang-tidy: 1 hit deterministico (riga 6, std::printf dentro logging::printf)

LLM (4 chat fresche, 2 modelli):
  Claude 1ª:  ✓ "::printf risolve all'using-decl, non a libc"
  Claude 2ª:  ✗ "::printf bypassa il wrapper" — opposto della 1ª
  ChatGPT 1ª: ✗ "::printf potrebbe non esistere a livello globale"
  ChatGPT 2ª: ✓ "la riga 26 non è davvero 'direct libc'"
  → 2/4 run sbagliano il finding centrale.
  → I due modelli sono incoerenti con sé stessi tra chat fresche.
```

Il check usa `callExpr(callee(functionDecl(hasName("::printf"))))` e lavora sull'AST **post-resolution**. Quando il compilatore decide che la chiamata risolve a `logging::printf`, il `functionDecl` non è più `::printf` di libc → no match. Un'analisi testuale (regex su "::printf") flaggerebbe tutto. Un LLM, leggendo il sorgente, può ragionare bene su questo oppure no — non c'è garanzia tra esecuzioni.

### Pattern stabili emersi (osservazioni vere su 24 run)

1. **ChatGPT 5.5 non propone mai `std::print`/`std::println`** (0/6 snippet). Propone `std::cout`/`std::printf`/`std::puts`/`std::format`/`std::fputs`. Claude Opus 4.7 lo propone come fix primario quando appropriato (4/6 snippet).
2. **Determinismo Claude > ChatGPT** sui primi 5 snippet. Claude propone fix consistenti tra chat fresche; ChatGPT varia (`std::cout` vs `std::puts`, `concept Streamable` vs overload espliciti).
3. **Su snippet 06, entrambi i modelli rompono il determinismo**: 50/50 sul finding più importante del file.

### Bonus inatteso — gli LLM trovano bug che il check non vede

- **Snippet 04** (entrambi i modelli): `static_cast<int>(2.5)` → `2`, perdita silenziosa di precisione. Bug semantico reale fuori dal mandato di `misc-no-printf`.
- **Snippet 05** (ChatGPT): `%s` con `msg == nullptr` è UB. Issue reale fuori scope.

Conferma "complementarità", non "sostituibilità": clang-tidy è preciso e deterministico sul suo mandato; gli LLM portano valore aggiunto su issue di alto livello che il check non guarda.

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

## Run su progetti open source reali (richiesta del prof)

Su macOS, **Bear non riesce a intercettare `ninja`** (sandboxing/SIP blocca il preload trick). Per progetti CMake la via canonica è invece la flag nativa `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` che produce lo stesso `compile_commands.json`. Bear resta utile per build Make/autotools (vedi `bear_demo/`).

Inoltre, `clang-tidy` custom non eredita lo SDK macOS, quindi va passato:
```bash
--extra-arg=-isysroot --extra-arg="$(xcrun --show-sdk-path)"
```

### Progetto 1: `fmt` (fmtlib/fmt)

Caso "negativo" deliberato: fmt esiste **per evitare** printf, quindi 0 hit attesi.

| Metrica | Valore |
|---------|--------|
| TU analizzate | 3 (`format.cc`, `os.cc`, `fmt-c.cc`) |
| Tempo totale | 0.7s |
| Hit clang-tidy | 0 ✅ |
| Falsi positivi | 0 ✅ |

Risultato: il check è **silenzioso su una codebase clean by design**. Conferma 0 falsi positivi su 3 unit di compilazione che includono migliaia di righe di header fmt.

### Progetto 2: `tinyxml2` (leethomason/tinyxml2)

Caso "positivo": libreria XML leggera, usa `printf` per error reporting + un suite di test (`xmltest.cpp`) molto loquace.

| Metrica | Valore |
|---------|--------|
| TU analizzate | 2 (`tinyxml2.cpp`, `xmltest.cpp`) |
| Tempo totale | 0.2s |
| Hit clang-tidy | **28** |
| Falsi positivi | 0 |

### Confronto AST vs grep — il valore dimostrato

```
grep -h printf src/*.{cpp,h}  →  42 match
clang-tidy AST                →  28 hit
```

I **14 di scarto** che `grep` segnala ma clang-tidy ignora correttamente:
- commenti: `// printf(...)`
- stringhe: `"%s\n"` (contiene la sottostringa per coincidenza)
- macro non rilevanti: `#define TIXML_VSCPRINTF _vscprintf`
- nomi simili: `vfprintf(...)`, `snprintf(...)` — funzioni diverse

Questo è il **value-prop esatto** del seminario: l'analisi AST con scope risolti e nomi qualificati `::printf` distingue 14 falsi positivi che un linter testuale (o un LLM che ragiona su pattern di stringa) prenderebbe per veri.

Output integrali: `real_world_runs/fmt_output.log` e `real_world_runs/tinyxml2_output.log`.
