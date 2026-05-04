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
| 1 | `01_basic.cpp` | 1 hit corretto | _ | _ |
| 2 | `02_namespace_collision.cpp` | 1 hit (solo `::printf`, ignora `mylib::printf`) | _ | _ |
| 3 | `03_macro_indirection.cpp` | 2 hit (vede dentro la macro) | _ | _ |
| 4 | `04_template.cpp` | 3 hit (template + 2 istanze) | _ | _ |

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

Compila qui il caso più convincente trovato durante il test (probabile: snippet 02 o 03).

```
Snippet: _
clang-tidy: _ hit
LLM: _ hit (di cui _ falsi positivi / _ falsi negativi)
Cosa l'LLM ha sbagliato: _
Spiegazione tecnica: _
```

---

## Take-away per il seminario

Aggiornare dopo i test reali:

- clang-tidy vince dove **conta determinismo**, **scope/tipi** e **automazione CI**
- L'LLM vince dove **non hai un check pronto** e ti serve un'analisi una-tantum
- Sono complementari: un LLM può aiutare a **scrivere** un nuovo check, clang-tidy lo applica.
