# Snippet 04_template — Claude Opus 4.7

**File testato:** `comparison/snippets_neutral/04_template.cpp`
**Data:** 2026-05-05
**Modello:** Claude Opus 4.7

## Ground truth (clang-tidy)
**3** warning: 1 sul template generico (riga 5) + 2 sulle istanziazioni concrete (`log_value<int>` e `log_value<double>`).

## Risposta integrale dell'LLM (1ª chat fresca)

> ## Problema 1 — Riga 7: cast unsafe e formato hardcoded
> Il template accetta qualsiasi `T` ma forza ogni valore a `int`. Con `log_value(2.5)` (riga 12), il `2.5` viene troncato a `2` silenziosamente — perdita di dati invisibile.
> Fix:
> ```cpp
> #include <iostream>
> template <typename T>
> void log_value(T value) { std::cout << "v=" << value << '\n'; }
> ```
> Oppure C++23:
> ```cpp
> #include <print>
> template <typename T>
> void log_value(T value) { std::println("v={}", value); }
> ```
>
> ## Problema 2 — Riga 1: dichiarazione manuale di `printf`
> Fix: `#include <cstdio>`.
>
> ## Problema 3 — Riga 6: parametro per valore senza vincoli
> Fix con concept (C++20):
> ```cpp
> template <typename T>
>   requires requires(std::ostream& os, const T& v) { os << v; }
> void log_value(const T& value) { std::cout << "v=" << value << '\n'; }
> ```
>
> ## Problema 4 — Riga 14: `return 0` ridondante
>
> Il cambiamento più importante è il primo: il troncamento silenzioso di `2.5` a `2` è un bug reale.

## Seconda esecuzione (chat fresca, stesso prompt)

> ## Problema 1 — Riga 4: `extern "C"` per `printf` invece di `#include <cstdio>`
> Antipratica. Fix: `#include <cstdio>`.
>
> ## Problema 2 — Riga 7: `printf("%d", ...)` con cast forzato a `int`
> Il bug più serio. `log_value(2.5)` → troncato a `2`.
> Fix idiomatico (C++23):
> ```cpp
> #include <print>
> template <typename T>
> void log_value(T value) { std::println("v={}", value); }
> ```
> Se vincolato a C++20: `std::cout << std::format("v={}\n", value);`.
> Se proprio devi restare su printf:
> ```cpp
> if constexpr (std::is_integral_v<T>) {
>     std::printf("v=%lld\n", static_cast<long long>(value));
> } else if constexpr (std::is_floating_point_v<T>) {
>     std::printf("v=%g\n", static_cast<double>(value));
> }
> ```
>
> ## Problema 3 — Riga 6: template senza vincoli
> Fix C++20:
> ```cpp
> template <typename T>
>   requires std::integral<T> || std::floating_point<T>
> void log_value(T value) { std::println("v={}", value); }
> ```
>
> ## Problema 4 — Riga 7: passaggio per valore di `T`
> ## Problema 5 — Riga 14: `return 0` ridondante
>
> Versione finale (C++23):
> ```cpp
> #include <print>
> #include <concepts>
> template <typename T>
>   requires std::integral<T> || std::floating_point<T>
> void log_value(T const& value) { std::println("v={}", value); }
> ```

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 4 (riga 1, riga 6, riga 7, riga 14). 2ª: 5 (riga 4, riga 6, riga 7 ×2, riga 14). |
| Ha segnalato solo il template, le istanze, o entrambi? | Tratta il template come fonte unica. Non distingue formalmente i 3 hit di clang-tidy (template + 2 istanze) — ne vede 1 unico problema "questo template usa printf, riscriviamolo". |
| Falsi positivi | Riga 1 (extern "C"), riga 6 (template generico), riga 14 (return 0): fuori scope rispetto a `misc-no-printf`. Non sono falsi nel senso stretto, sono issue raccolte oltre il mandato del check. |
| Falsi negativi | 0 sul `printf`. **Bonus inatteso:** trova il **bug semantico vero** che clang-tidy NON segnala — la perdita di precisione di `static_cast<int>(2.5)` → `2`. |
| Deterministico (1ª = 2ª)? | sì sul nucleo: bug del cast a `int` è il "Problema 1/2" più serio in entrambe le chat; fix primaria `std::println("v={}", value)` in entrambe. Variano i numeri di issue extra (4 vs 5) e la struttura delle alternative (la 2ª chat aggiunge il pattern `if constexpr` per chi vuole mantenere printf). |

## Verdetto sintetico

Claude **trova un bug semantico fuori scope del check** (troncamento `2.5 → 2`) e lo classifica come issue principale, **superando clang-tidy in qualità di review** ma allontanandosi dal mandato esatto del check `misc-no-printf`. Determinismo alto: stessa diagnosi top, stessa fix primaria con `std::println` (allineata a clang-tidy). Punto narrativo: per una review umana questo comportamento è **un valore aggiunto**, ma clang-tidy non è un linter generico e non vuole esserlo. Per la slide: "stesso input, due strumenti complementari, non sostituibili".
