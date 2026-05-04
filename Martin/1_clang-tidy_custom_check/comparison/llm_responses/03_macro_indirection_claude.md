# Snippet 03_macro_indirection — Claude Opus 4.7

**File testato:** `comparison/snippets_neutral/03_macro_indirection.cpp`
**Data:** 2026-05-05
**Modello:** Claude Opus 4.7

## Ground truth (clang-tidy)
**2** warning, nei punti dove la macro `LOG(x)` viene chiamata (righe 6 e 7).
La diagnostica indica che il `printf` è dentro l'espansione della macro `LOG`. Il check lavora sull'AST post-expansion, quindi vede `printf` anche se al call site (`LOG("avvio")`) non è testualmente presente.

## Risposta integrale dell'LLM (1ª chat fresca)

> # Analisi del codice
>
> Il codice in sé compila e funziona correttamente, ma ci sono diverse pratiche discutibili rispetto al C++ moderno.
>
> ## Problema 1 (riga 1): dichiarazione manuale di `printf`
> Ridichiarare funzioni della libreria standard è fragile e non idiomatico.
> Fix: `#include <cstdio>` e `std::printf`. Meglio in C++23: `#include <print>` e `std::println("[LOG] {}", "avvio");`.
>
> ## Problema 2 (riga 3): macro per il logging
> `#define LOG(x) printf("[LOG] " x "\n")` ha tre debolezze:
> 1. **Funziona solo con string literal** (concatenazione di letterali a tempo di preprocessing). `LOG(myString)` con `std::string` non compila.
> 2. Le macro non rispettano namespace né scope.
> 3. Niente type-checking.
>
> Fix: sostituire con una funzione `inline` con formattazione moderna.
> ```cpp
> inline void log(std::string_view msg) {
>     std::println("[LOG] {}", msg);
> }
> ```
>
> ## Problema 3 (riga 7, 8): `\n` invece di flush esplicito
> Per logging si preferisce `std::cerr` (unbuffered) così i log non si perdono in caso di crash.
>
> ## Versione finale modernizzata
> ```cpp
> inline void log(std::string_view msg) {
>     std::cerr << "[LOG] " << msg << '\n';
> }
> ```

## Seconda esecuzione (chat fresca, stesso prompt)

> # Analisi del codice
>
> ## Problema 1 — Riga 1: dichiarazione manuale di `printf`
> Fragile e non idiomatico. Fix: `#include <print>` (C++23) o `#include <cstdio>`.
>
> ## Problema 2 — Riga 3: macro `LOG` invece di una funzione
> Sfrutta la concatenazione di string literal a tempo di compilazione. Si rompe silenziosamente con qualsiasi cosa che non sia uno string literal: `LOG(myString)` non compila.
>
> Fix:
> ```cpp
> inline void log(std::string_view msg) {
>     std::println("[LOG] {}", msg);
> }
> ```
>
> Variante con argomenti variadici tipizzati:
> ```cpp
> template <typename... Args>
> void log(std::format_string<Args...> fmt, Args&&... args) {
>     std::print("[LOG] ");
>     std::println(fmt, std::forward<Args>(args)...);
> }
> ```
>
> ## Problema 3 — Riga 9: `return 0` ridondante
>
> ## Versione corretta
> ```cpp
> #include <print>
> #include <string_view>
> void log(std::string_view msg) {
>     std::println("[LOG] {}", msg);
> }
> int main() {
>     log("avvio");
>     log("fine");
> }
> ```

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 3 (riga 1, riga 3 macro, riga 7-8 buffering). 2ª: 3 (riga 1, riga 3 macro, riga 9 return 0). |
| Ha visto il `printf` nascosto nella macro? | **sì, indirettamente**: identifica la macro come problema centrale e nota che "espande in printf". Tuttavia non separa "macro fragile (string literal only)" da "macro espande in printf da sostituire" — li unisce in un'unica raccomandazione (refactor a funzione + `std::println`). |
| Suggerisce di rimuovere/refactorare la macro? | sì, in entrambe le chat: macro → `inline void log(std::string_view)` con `std::println`. |
| Falsi positivi | Riga 1 (extern "C") e return 0 ridondante: fuori scope rispetto a `misc-no-printf`. |
| Falsi negativi | 0 — il fix proposto sostituisce de facto entrambe le occorrenze di `printf` (sotto la macro), in linea con clang-tidy. |
| Deterministico (1ª = 2ª)? | sì sul nucleo — stessa analisi della macro, stessa fix `std::println`/`inline void log(string_view)`. La 1ª aggiunge buffering/`std::cerr`, la 2ª aggiunge variant variadica template. Differenze "decorative", non sostanziali. |

## Verdetto sintetico

Claude **vede il `printf` dentro la macro** come parte del problema "macro fragile" e propone una soluzione che de facto coincide con il fix di clang-tidy (sostituire con funzione che usa `std::println`). **Determinismo alto**: stessa diagnosi, stessa fix top in entrambe le chat. Tuttavia non distingue formalmente i 2 hit del check (uno per ciascuna chiamata `LOG`) — vede la macro come issue unica.
