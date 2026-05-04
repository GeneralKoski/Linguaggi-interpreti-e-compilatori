# Snippet 01_basic — ChatGPT 5.5

**File testato:** `comparison/snippets_neutral/01_basic.cpp`
**Data:** 2026-05-05
**Modello:** ChatGPT 5.5

## Ground truth (clang-tidy)
1 warning su riga 4 (`printf("hello\n");`), suggerisce `std::print`.

## Risposta integrale dell'LLM (1ª chat fresca)

> Ecco i problemi. Pochi, per fortuna.
>
> | Riga | Problema | Fix |
> |---:|---|---|
> | 1 | Dichiarazione manuale di `printf`: rischi mismatch con la dichiarazione reale della libreria standard. | `#include <cstdio>` |
> | 4 | Uso di `printf` senza namespace standard: includendo `<cstdio>`, `printf` è garantito come `std::printf`. | `std::printf(...)` |
> | 4 | `printf` è variadica e non type-safe: qui non crea bug, ma in generale `printf("%d", value)` può esplodere male se il tipo non combacia. | Per output semplice, preferisci `std::puts` o `std::cout` |
> | 5 | `return 0;` superfluo in `main`: in C++ se `main` termina senza `return`, equivale a `return 0;`. | Puoi rimuoverlo |
>
> Versione corretta minimale con `<cstdio>`:
> ```cpp
> #include <cstdio>
> int main() {
>     std::printf("hello\n");
> }
> ```
>
> Versione ancora più adatta al caso specifico:
> ```cpp
> #include <cstdio>
> int main() {
>     std::puts("hello");
> }
> ```
>
> Versione C++-style:
> ```cpp
> #include <iostream>
> int main() {
>     std::cout << "hello\n";
> }
> ```
>
> Il fix più sensato qui è `std::puts("hello");`: niente format string, niente varargs, niente mini-portale verso il caos.

## Seconda esecuzione (chat fresca, stesso prompt)

> Ecco i problemi, con righe numerate.
>
> ## Riga 1
> **Problema:** dichiari manualmente `printf`. È fragile: rischi mismatch di firma, namespace sbagliati, warning strani.
> **Fix:** usa `<cstdio>` e `std::printf`:
> ```cpp
> #include <cstdio>
> int main() {
>     std::printf("hello\n");
>     return 0;
> }
> ```
>
> ## Riga 4
> **Problema:** stai usando `printf` per stampare una stringa statica. Non è "sbagliato", ma in C++ moderno è più idiomatico usare `std::cout`.
> **Fix C++ idiomatico:**
> ```cpp
> #include <iostream>
> int main() {
>     std::cout << "hello\n";
>     return 0;
> }
> ```
>
> ## Riga 5
> **Problema:** `return 0;` in `main` è superfluo.
>
> ## Versione consigliata
> ```cpp
> #include <iostream>
> int main() {
>     std::cout << "hello\n";
> }
> ```
>
> ## Versione se vuoi restare con `printf`
> ```cpp
> #include <cstdio>
> int main() {
>     std::printf("hello\n");
> }
> ```

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 4 (riga 1, riga 4 ×2, riga 5). 2ª: 3 (riga 1, riga 4, riga 5). |
| Ha trovato il `printf`? | sì |
| Suggerisce `std::print`? | **no** — propone `std::puts` / `std::cout` / `std::printf`, **mai** `std::print`/`std::println` (la fix-it esatta di clang-tidy). |
| Falsi positivi | Osservazioni di stile fuori scope: `extern "C"` manuale (riga 1) e `return 0` ridondante (riga 5). |
| Falsi negativi | Non manca il `printf`, ma manca la fix moderna C++23. |
| Deterministico (1ª = 2ª)? | **parzialmente** — stesso problema centrale, ma: (a) numero di issue diverso (4 vs 3), (b) fix top divergente: 1ª chat raccomanda `std::puts`, 2ª raccomanda `std::cout`. |

## Verdetto sintetico

ChatGPT trova il `printf` ma **non propone mai `std::print`/`std::println`**, divergendo dalla fix di clang-tidy. Le due chat fresche divergono sul fix raccomandato (`std::puts` vs `std::cout`) e sul numero di issue segnalate → determinismo medio. Come Claude, aggiunge osservazioni stilistiche fuori scope. Già su uno snippet trivial emergono due differenze interessanti per il seminario: (1) ChatGPT non conosce/non considera `std::print`, (2) la fix raccomandata non è stabile tra esecuzioni.
