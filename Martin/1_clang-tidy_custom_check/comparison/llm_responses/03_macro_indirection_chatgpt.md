# Snippet 03_macro_indirection — ChatGPT 5.5

**File testato:** `comparison/snippets_neutral/03_macro_indirection.cpp`
**Data:** 2026-05-05
**Modello:** ChatGPT 5.5

## Ground truth (clang-tidy)
**2** warning, nei punti dove la macro `LOG(x)` viene chiamata (righe 6 e 7).
La diagnostica indica che il `printf` è dentro l'espansione della macro `LOG`. Il check lavora sull'AST post-expansion, quindi vede `printf` anche se al call site non è testualmente presente.

## Risposta integrale dell'LLM (1ª chat fresca)

> ### Riga 1: dichiarazione manuale di `printf`
> Stai ridichiarando `printf` a mano. È pessima abitudine in C++ moderno. Fix: `#include <cstdio>` e `std::printf("...");`.
>
> ### Riga 3: macro fragile
> `LOG(x)` funziona solo se `x` è una string literal, perché sfrutta la concatenazione automatica delle stringhe:
> ```cpp
> "[LOG] " "avvio" "\n"
> ```
> Ma `LOG(msg)` con `const char* msg = "avvio"` diventa `printf("[LOG] " msg "\n")`, sintassi illegale.
>
> Fix minimo mantenendo `printf`:
> ```cpp
> #include <cstdio>
> void log(const char* msg) { std::printf("[LOG] %s\n", msg); }
> ```
>
> ### Riga 3: macro senza type safety
> Una macro non rispetta scope, tipi, overload, namespace. Meglio una funzione.
> Fix migliore:
> ```cpp
> #include <iostream>
> #include <string_view>
> void log(std::string_view msg) { std::cout << "[LOG] " << msg << '\n'; }
> ```
>
> ## Versione corretta consigliata
> ```cpp
> void log(std::string_view msg) { std::cout << "[LOG] " << msg << '\n'; }
> int main() { log("avvio"); log("fine"); return 0; }
> ```
>
> ## Se vuoi restare su `printf`
> ```cpp
> void log(const char* msg) { std::printf("[LOG] %s\n", msg); }
> ```

## Seconda esecuzione (chat fresca, stesso prompt)

> | Riga | Problema | Fix |
> |---:|---|---|
> | 1 | Dichiarazione manuale di `printf` | `#include <cstdio>` e `std::printf`, oppure `std::puts` se non formatti nulla |
> | 3 | Macro `LOG(x)` troppo rigida | Funziona solo con string literal. Usa una funzione |
> | 3 | Macro senza protezioni | Le macro non rispettano scope, tipi, overload, namespace. Sostituire con `inline` function |
> | 3 | Uso implicito di formato `printf` | Innocuo qui ma rischio di format string bugs se la macro viene estesa |
> | 6-7 | `LOG("avvio")` e `LOG("fine")` funzionano ma dipendono dal dettaglio della macro | Chiama una funzione `log("avvio");` |
>
> ### Versione corretta semplice
> ```cpp
> #include <cstdio>
> inline void log(const char* message) { std::printf("[LOG] %s\n", message); }
> ```
>
> ### Versione moderna e type-safe
> ```cpp
> #include <iostream>
> #include <string_view>
> inline void log(std::string_view message) { std::cout << "[LOG] " << message << '\n'; }
> ```
>
> ### Se vuoi restare su C-style ma senza formato
> ```cpp
> void log(std::string_view message) {
>     std::fputs("[LOG] ", stdout);
>     std::fwrite(message.data(), 1, message.size(), stdout);
>     std::fputc('\n', stdout);
> }
> ```

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 3 (riga 1, riga 3 ×2). 2ª: 5 (riga 1, riga 3 ×3, righe 6-7). |
| Ha visto il `printf` nascosto nella macro? | **sì** — entrambe le chat identificano la macro come fragile e espongono che espande in `printf`. |
| Suggerisce di rimuovere/refactorare la macro? | sì, in entrambe: macro → `inline void log(string_view)` con `std::cout`. |
| Falsi positivi | Riga 1 (extern "C") fuori scope. Issue duplicate sulla macro nella 2ª chat (3 voci diverse per lo stesso problema). |
| Falsi negativi | 0 sul `printf` (sostituito de facto). Però **come negli snippet 01 e 02, non propone mai `std::print`/`std::println`**: la fix raccomandata è `std::cout` o `std::printf`/`std::puts`. |
| Deterministico (1ª = 2ª)? | **parzialmente** — stessa diagnosi centrale (macro fragile, ridichiarazione manuale), ma: (a) numero issue diverso (3 vs 5 — la 2ª chat duplica voci sulla macro), (b) fix proposte divergono: 1ª chat propone `std::cout`/`std::printf`, 2ª aggiunge `std::puts`/`std::fputs`/`std::fwrite`. |

## Verdetto sintetico

ChatGPT vede il `printf` dentro la macro e propone refactor a funzione, in linea con la sostanza del check. Però:
1. **Continua a non proporre `std::print`/`std::println`** — pattern stabile su tutti gli snippet finora (01, 02, 03);
2. **Determinismo medio**: la 2ª chat raccoglie 5 issue dove la 1ª ne segnalava 3, con fix-it diverse (`std::cout` vs `std::cout`+`std::puts`+`std::fputs`+`std::fwrite`).
