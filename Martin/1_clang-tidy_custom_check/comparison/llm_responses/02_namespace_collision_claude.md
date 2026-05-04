# Snippet 02_namespace_collision — Claude Opus 4.7

**File testato:** `comparison/snippets_neutral/02_namespace_collision.cpp`
**Data:** 2026-05-05
**Modello:** Claude Opus 4.7

## Ground truth (clang-tidy)
**1** warning su riga 9 (`::printf(...)`).
**Non** segnala `mylib::printf` su riga 8 — è una funzione user-defined nel namespace `mylib`, non `::printf` di libc.

## Risposta integrale dell'LLM (1ª chat fresca)

> Premetto una cosa importante: **il codice che hai postato non ha bug evidenti di linguaggio**. Compila e si comporta come ci si aspetta. Le due `printf` vivono in scope diversi (una in `mylib`, l'altra nello scope globale con linkage C), e nel `caller()` sono qualificate esplicitamente (`mylib::printf` e `::printf`), quindi non c'è ambiguità né shadowing accidentale.
>
> **Riga 2 — `int printf(const char *);`** Dichiari una funzione dentro `mylib` con lo stesso nome di una standard. Anche se lo scope la protegge, è una scelta che invita confusione, errori in chiamate non qualificate, e problemi con `using namespace mylib;` altrove. È solo dichiarata, mai definita: errore di link se chiamata.
>
> **Riga 5 — `extern "C" int printf(const char *, ...);`** Stai ridichiarando manualmente `printf`. Non farlo mai. Devi `#include <cstdio>`. Duplica una dichiarazione di sistema, perdi attributi (`format(printf, 1, 2)`), perdi controllo del compilatore sulla format string.
>
> **Riga 9 — `mylib::printf(...)`** manca `\n`, valore di ritorno scartato.
>
> **Riga 10 — `::printf("hello from libc\n");`** In C++ moderno preferisci `std::print` (C++23) o `std::format`. Fix: `#include <print>` + `std::println("hello from libc");`.
>
> **Riga 8 — `int caller()`** ritorna sempre 0 e nessuno usa quel valore: meglio `void`.
>
> **Versione finale ripulita** (C++23): `mylib::log_message(...)` + `std::println(...)`.
>
> In sintesi il problema più concreto è la riga 5: la ridichiarazione manuale di funzioni libc è una pratica da evitare sempre.

## Seconda esecuzione (chat fresca, stesso prompt)

> Il codice è sintatticamente valido e compila. Ci sono problemi di design e potenziali insidie.
>
> **Problema 1 — Riga 2:** `mylib::printf` ombreggia un nome standard. Legale ma pessima pratica: confusione, ambiguità con `using namespace mylib;`. Fix: rinomina (`log_message`, `print`, `write_line`).
>
> **Problema 2 — Riga 2:** firma non-variadic ma nome che suggerisce variadic. Tradisce le aspettative del lettore. Fix: usa `std::format_string<Args...>` o `std::string_view`.
>
> **Problema 3 — Riga 6:** ridichiarazione manuale di `::printf`. Fragile se `<cstdio>` viene incluso prima/dopo con attributi diversi; salti i controlli di formato del compilatore. Fix: `#include <cstdio>`. In C++ moderno preferisci `std::print` (C++23): `std::println("hello from std::print");`.
>
> **Problema 4 — Riga 10:** `mylib::printf("greeting from mylib")` — nessun newline, e il nome `printf` incoraggia il pattern format-string-vulnerability altrove (qui non lo è perché non è variadica).
>
> **Problema 5 — Riga 12:** `return 0;` ridondante in `caller`. Se non serve, `void caller()`.
>
> **Versione finale**: `#include <print>` + `mylib::log_message(...)` + `std::println(...)`.

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 6 (riga 2 ×2, riga 5, riga 8, riga 9, riga 10). 2ª: 5 (riga 2 ×2, riga 6, riga 10, riga 12). |
| Ha riconosciuto `mylib::printf` come distinto da `::printf`? | **sì, esplicitamente** — entrambe le chat lo dichiarano in apertura ("vivono in scope diversi", "lo scope la protegge"). |
| Falsi positivi (es. ha flaggato `mylib::printf` come libc?) | **no, mai**. Suggerisce di rinominarla per stile (problema reale di design), ma non la confonde con libc. |
| Falsi negativi | 0 sul `::printf` di libc (sempre individuato). |
| Determinismo (1ª = 2ª)? | sì sul nucleo: stessa diagnosi (scope safe, ridichiarazione manuale fragile, suggerire `std::print`). Variano i numeri di problemi raccolti e l'ordine espositivo, ma fix-it identici. |

## Verdetto sintetico

⭐ **Snippet chiave per la slide.** Claude **passa il test di scope**: distingue correttamente `mylib::printf` da `::printf`, esattamente come il matcher AST. Inoltre suggerisce `std::print`/`std::println` come fix primario, allineandosi a clang-tidy. Il "rumore" (rinominare `mylib::printf`, rimuovere `extern "C"`, `return 0` ridondante) è osservazione stilistica corretta ma fuori scope del check `misc-no-printf`.
