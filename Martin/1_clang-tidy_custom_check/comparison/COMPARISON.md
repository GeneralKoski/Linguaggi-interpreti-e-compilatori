# clang-tidy vs LLM — confronto su esempi piccoli

Stesso pattern (`printf` libc da sostituire con `std::print`) sottoposto a:
- **clang-tidy** con il nostro check `misc-no-printf`
- **LLM** (Claude/ChatGPT) con il prompt: *"trova problemi e suggerisci fix in questo C++ moderno"*

Tabella da popolare a build completata.

| # | Snippet | clang-tidy | LLM |
|---|---|---|---|
| 1 | `01_basic.cpp` — chiamata diretta | hit + fix-it preciso | hit, fix-it testuale |
| 2 | `02_namespace_collision.cpp` — `mylib::printf` vs `::printf` | distingue (AST sa lo scope) | rischio falso positivo su `mylib::printf` |
| 3 | `03_macro_indirection.cpp` — call via macro `LOG(...)` | segnala (vede l'AST espanso) | rischio falso negativo (grep testuale) |
| 4 | `04_template.cpp` — call dentro template | segnala il template + istanze | dipende da quanto profondo entra |

## Dimensioni di confronto

| Dimensione | clang-tidy | LLM |
|---|---|---|
| Determinismo | totale (AST + matcher) | stocastico |
| Falsi positivi su scope | bassi | medi/alti |
| Costo per file | ms locale | secondi + token + cloud |
| Scalabilità (10⁵ file) | parallelizzabile, prevedibile | non praticabile |
| Fix-it applicabile da CI | sì, idempotente | no senza orchestrazione |
| Comprensione "intenzione" | nulla | spesso buona |
| Onboarding | richiede C++ + AST matchers | nessuno |

## Take-away per il seminario

- clang-tidy vince dove **conta determinismo**, **scope/tipi** e **automazione CI**
- L'LLM vince dove **non hai un check pronto** e ti serve un'analisi una-tantum
- Sono complementari: l'LLM può aiutare a **scrivere** un nuovo check, clang-tidy lo applica.
