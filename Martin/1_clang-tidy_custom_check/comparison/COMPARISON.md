# clang-tidy vs LLM — confronto su esempi piccoli

Stesso pattern (`printf` libc da sostituire con `std::print`) sottoposto a:
- **clang-tidy** con il check `misc-no-printf` (output reale, vedi `RESULTS.md`)
- **Claude Opus 4.7** e **ChatGPT 5.5** con il prompt in `llm_responses/_PROMPT.md`

Le risposte integrali degli LLM stanno in `llm_responses/0N_*_{claude,chatgpt}.md`.
Il confronto si fa su `snippets_neutral/` (versione senza commenti rivelatori).

---

## Tabella riassuntiva — da compilare dopo i test

| # | Snippet | clang-tidy (ground truth) | Claude Opus 4.7 | ChatGPT 5.5 |
|---|---|---|---|---|
| 1 | `01_basic.cpp` | 1 hit corretto (riga 4, fix `std::print`) | 3 issue (printf trovato, fix `std::println` ✓), det. alto | 3-4 issue (printf trovato, fix `std::puts`/`std::cout`, **mai** `std::print`), det. medio |
| 2 | `02_namespace_collision.cpp` | 1 hit (solo `::printf`, ignora `mylib::printf`) | scope OK ✓, fix `std::println` ✓, det. alto | scope OK ✓, fix `std::printf` (no `std::print` in 1ª chat), trova linker error fuori scope, det. medio |
| 3 | `03_macro_indirection.cpp` | 2 hit (vede dentro la macro) | macro vista ✓, fix `std::println` ✓, det. alto | macro vista ✓, fix `std::cout`/`std::printf` (mai `std::print`), det. medio (3 vs 5 issue) |
| 4 | `04_template.cpp` | 3 hit (template + 2 istanze) | trova il bug semantico (`2.5→2`) ⭐, fix `std::println` ✓, det. alto | trova il bug semantico ⭐, fix `std::cout`/`std::format` (mai `std::print`), det. medio (concept vs overload) |
| 5 | `05_method_collision.cpp` | 2 hit (solo `::printf`, ignora `Logger::printf` metodo) | scope OK ✓ (passa la trappola), fix `std::printf` qualificato, det. alto | scope OK ✓ (passa la trappola), trova bug nullptr ⭐, fix `std::printf`, det. medio |
| 6 | `06_using_template_alias.cpp` | 1 hit (overload resolution dirotta `::printf(...)` al template) | **1ª: ✓ scope-aware** ("::printf risolve all'using-decl"). **2ª: ✗ scope-blind** ("::printf bypassa il wrapper"). Det. **rotto** sul finding chiave. | **1ª: ✗ scope-blind** ("::printf potrebbe non esistere"). **2ª: ✓ scope-aware** ("la riga 26 non è davvero direct libc"). Det. **rotto** sul finding chiave. |

Per ognuna: `N hit / N falsi positivi / N falsi negativi / det.: sì-no`.

---

## Dimensioni di confronto qualitative

| Dimensione | clang-tidy | LLM (Claude) | LLM (ChatGPT) |
|---|---|---|---|
| Determinismo | totale (AST + matcher) | _ | _ |
| Falsi positivi su scope | bassi | _ | _ |
| Costo per file | ms locale | secondi + token + cloud | secondi + token + cloud |
| Scalabilità (10⁵ file) | parallelizzabile, prevedibile | non praticabile (costo, rate limit) | idem |
| Fix-it applicabile da CI | sì, idempotente | no senza orchestrazione | no senza orchestrazione |
| Comprensione "intenzione" | nulla | spesso buona | spesso buona |
| Onboarding | richiede C++ + AST matchers | nessuno | nessuno |

---

## 🎯 "Wow moment" della demo

```
Snippet: 06_using_template_alias.cpp
clang-tidy: 1 hit deterministico (riga 6, std::printf dentro logging::printf)

LLM (4 chat fresche, 2 modelli):
  - Claude 1ª:  ✓ scope-aware ("::printf risolve all'using-decl, non a libc")
  - Claude 2ª:  ✗ scope-blind ("::printf bypassa il wrapper") — opposto della 1ª
  - ChatGPT 1ª: ✗ scope-blind ("::printf potrebbe non esistere a livello globale")
  - ChatGPT 2ª: ✓ scope-aware ("la riga 26 non è davvero 'direct libc'")
  → 2 su 4 run sbagliano il finding centrale, e i due modelli sono incoerenti
    con sé stessi tra chat fresche.

Cosa l'LLM ha sbagliato: dopo `using logging::printf;`, il `::printf("...", 1)`
con argomento int risolve via overload resolution al template `logging::printf`
(match esatto > variadic libc). Metà dei run lo capisce, metà no.

Spiegazione tecnica: il check `misc-no-printf` usa
  callExpr(callee(functionDecl(hasName("::printf"))))
e questo matcher lavora sull'AST POST-resolution. Quando il compilatore ha
deciso che la chiamata risolve a `logging::printf`, il `functionDecl` non è
più `::printf` di libc → no match. Un'analisi testuale (regex su "::printf")
flaggerebbe tutto. Un LLM, leggendo il sorgente, può ragionare bene su questo
oppure no — non c'è garanzia.

Take-away: clang-tidy = deterministico, allineato a ciò che il compilatore
ha effettivamente scelto. LLM = analisi di alto livello, valore aggiunto su
issue fuori scope (bug semantico 2.5→2 in snippet 04, nullptr in snippet 05),
ma instabile sul finding chiave proprio dove la precisione conta di più.
```

---

## Take-away per il seminario (post-run reale, 6 snippet × 2 modelli × 2 chat = 24 run)

**Cosa clang-tidy fa meglio:**
- **Determinismo totale** sul finding e sulla fix-it. Stesso input → stesso output, sempre.
- **Scope/overload resolution accurata**: il matcher AST traccia ciò che il compilatore ha scelto, non il testo (snippet 02, 05, 06).
- **Idempotenza in CI**: un fix-it applicato in modo prevedibile, parallelizzabile su 10⁵ file.

**Cosa l'LLM fa meglio:**
- **Bug semantici fuori dal mandato del check**: troncamento `2.5→2` (snippet 04, entrambi i modelli), `%s` con `nullptr` UB (snippet 05, ChatGPT).
- **Suggerimenti di refactoring di alto livello**: rinominare `Logger::printf`, sostituire macro con funzioni, usare `std::string_view`, applicare concept C++20.
- **Spiegazione contestuale**: l'LLM sa *perché* `printf` è sconsigliato in C++ moderno e propone alternative.

**Pattern stabili emersi (osservazioni vere, non opinioni):**
1. **ChatGPT 5.5 non propone mai `std::print`/`std::println` come fix primario** (0/6 snippet). Lo menziona occasionalmente come ultima alternativa C++23 condizionale (snippet 02, 03, 04 — solo in alcune delle 2 chat). I fix raccomandati sono `std::cout`/`std::printf`/`std::puts`/`std::format`/`std::fputs`. Claude Opus 4.7 lo propone come fix primario quando appropriato (4/6 snippet).
2. **Determinismo Claude > ChatGPT** sui primi 5 snippet (Claude propone fix consistenti tra chat fresche, ChatGPT varia: `std::cout` vs `std::puts`, `concept Streamable` vs overload, ecc.).
3. **Su snippet 06 (overload resolution), entrambi i modelli falliscono il determinismo**: 2 su 4 run interpretano correttamente l'AST post-resolution, 2 su 4 no. 50/50 sul finding più importante del file.

**Onestà del confronto:**
- I "wow moment" attesi su snippet 02 (namespace) e 05 (metodo) **non si sono materializzati**: entrambi gli LLM passano la trappola di scope. Quindi i modelli moderni non sono LLM "regex-based" ingenui.
- Il vero wow moment è snippet 06: dove serve precisione AST, l'LLM fluttua tra esecuzioni, clang-tidy no.

**Conclusione per il seminario:**
- clang-tidy e LLM sono **complementari**, non sostituibili. Per "vai a sostituire ogni `printf` libc con `std::print` su 50.000 file", clang-tidy è l'unica scelta sensata. Per "leggi questo PR e dimmi se c'è un bug nascosto", l'LLM trova cose che il check perde (e viceversa).
- Un LLM può **aiutare a scrivere** un nuovo check (impalcatura, matcher, test), poi clang-tidy lo applica per sempre in modo idempotente. Workflow ibrido.
