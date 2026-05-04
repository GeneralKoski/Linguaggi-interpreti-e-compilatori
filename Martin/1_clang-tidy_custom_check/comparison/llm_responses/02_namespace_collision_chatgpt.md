# Snippet 02_namespace_collision — ChatGPT 5.5

**File testato:** `comparison/snippets_neutral/02_namespace_collision.cpp`
**Data:** 2026-05-05
**Modello:** ChatGPT 5.5

## Ground truth (clang-tidy)
**1** warning su riga 9 (`::printf(...)`).
**Non** segnala `mylib::printf` su riga 8 — è una funzione user-defined nel namespace `mylib`, non `::printf` di libc.

## Risposta integrale dell'LLM (1ª chat fresca)

> ## Riga 2 / Riga 8: `mylib::printf` è solo dichiarata, non definita
> `mylib::printf` ha una dichiarazione ma nessuna definizione. Compila, ma fallisce al link con `undefined reference to mylib::printf(char const*)`.
> Fix:
> ```cpp
> namespace mylib {
> int printf(const char *msg) { return std::printf("%s\n", msg); }
> }
> ```
> Però richiede `#include <cstdio>`.
>
> ## Riga 5: dichiarare manualmente `printf` è una pessima idea
> `extern "C" int printf(const char *, ...);` — sta ridichiarando manualmente una funzione della libreria C. Funziona spesso, ma è fragile e non moderno. Fix: `#include <cstdio>` e usa `std::printf`.
>
> ## Riga 9: `::printf` dipende dal global namespace
> Con C++ moderno, usando `<cstdio>`, la forma portabile è `std::printf`. Fix: `std::printf("hello from libc\n");`.
>
> ## Versione corretta
> ```cpp
> #include <cstdio>
> namespace mylib {
> int printf(const char *msg) { return std::printf("%s\n", msg); }
> }
> int caller() {
>     mylib::printf("greeting from mylib");
>     std::printf("hello from libc\n");
>     return 0;
> }
> ```
>
> ## Versione ancora più pulita
> Dato che `mylib::printf` non è davvero un `printf` variadico, chiamarlo `printf` è un piccolo atto di terrorismo semantico. Meglio rinominarlo `print_line`.

## Seconda esecuzione (chat fresca, stesso prompt)

> ## Riga 2: `mylib::printf` è solo dichiarata, non definita
> Questa funzione esiste solo come promessa. Se nessun `.cpp` la definisce, fallisce in linking con `undefined reference to mylib::printf(char const*)`.
> Fix: definirla, oppure non dichiararla.
>
> ## Riga 5: non dichiarare `printf` a mano
> `extern "C" int printf(const char *, ...);` — può funzionare ma è una pessima idea. Fix: `#include <cstdio>`, poi `std::printf("hello from libc\n");`.
>
> ## Riga 8: `mylib::printf("greeting from mylib")` non stampa newline
> Manca `\n`. Output brutto o bufferizzato in modo sorprendente.
>
> ## Riga 2: nome fuorviante, `mylib::printf` non è davvero un `printf`
> La firma accetta solo `const char*`, non variadica. Trappola. Fix: rinominala (`mylib::print`).
>
> ## Versione corretta semplice
> ```cpp
> #include <cstdio>
> namespace mylib {
> int print_line(const char* msg) { return std::printf("%s\n", msg); }
> }
> int caller() {
>     mylib::print_line("greeting from mylib");
>     std::printf("hello from libc\n");
>     return 0;
> }
> ```
>
> ## Fix migliore, se sei in C++23
> ```cpp
> #include <print>
> namespace mylib {
> void print_line(std::string_view msg) { std::println("{}", msg); }
> }
> int caller() {
>     mylib::print_line("greeting from mylib");
>     std::println("hello from libc");
>     return 0;
> }
> ```

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 3 issue (riga 2, riga 5, riga 9). 2ª: 4 issue (riga 2 ×2, riga 5, riga 8). |
| Ha riconosciuto `mylib::printf` come distinto da `::printf`? | **sì** — non lo confonde con libc. Lo flagga però per altre ragioni: dichiarazione senza definizione (linker error reale ma fuori scope del check), nome fuorviante. |
| Falsi positivi | Nessun "mylib::printf = libc"; ma raccoglie issue fuori scope (linker error, missing newline, return 0). |
| Falsi negativi | 0 sul `::printf` libc (sempre individuato). Però la fix top raccomandata è `std::printf` (= mantenere printf, solo qualificarlo), **non** `std::print`/`std::println` come clang-tidy. `std::println` compare solo nella 2ª chat come "fix migliore se sei in C++23", non come raccomandazione primaria. |
| Deterministico (1ª = 2ª)? | **parzialmente** — issue centrali coerenti (linker error, ridichiarazione manuale, nome fuorviante), ma: (a) numero di issue diverso (3 vs 4), (b) la 1ª non menziona il newline mancante, (c) la 1ª non propone `std::print`/`std::println` mentre la 2ª sì come ultima opzione. |

## Verdetto sintetico

⭐ **Punto importante:** ChatGPT, come Claude, **non confonde** `mylib::printf` con `::printf` di libc. Quindi su questo snippet *entrambi* gli LLM passano il test di scope — il "wow moment" che mi aspettavo non si materializza qui.

Tuttavia ChatGPT mostra altre debolezze concrete:
1. **Trova un problema reale fuori dal mandato del check** (link error: `mylib::printf` dichiarata ma non definita) e lo presenta come issue principale — può confondere chi cerca solo issue di "C++ moderno";
2. **Fix raccomandato instabile**: la 1ª chat propone `std::printf` (mantieni printf, qualifica), la 2ª chat aggiunge `std::println` solo come ultima opzione condizionale a C++23. Claude invece propone `std::println` come fix primario in entrambe le chat;
3. **Determinismo medio**: numero di issue diverso (3 vs 4) e fix-it raccomandato divergente.

Lo snippet è ancora utile per la slide: serve per dire che entrambi gli LLM individuano il problema centrale ma divergono sul *fix*, mentre clang-tidy applica esattamente la stessa fix in modo idempotente.
