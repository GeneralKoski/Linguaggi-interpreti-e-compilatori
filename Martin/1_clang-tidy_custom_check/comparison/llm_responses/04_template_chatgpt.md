# Snippet 04_template — ChatGPT 5.5

**File testato:** `comparison/snippets_neutral/04_template.cpp`
**Data:** 2026-05-05
**Modello:** ChatGPT 5.5

## Ground truth (clang-tidy)
**3** warning: 1 sul template generico (riga 5) + 2 sulle istanziazioni concrete (`log_value<int>` e `log_value<double>`).

## Risposta integrale dell'LLM (1ª chat fresca)

> ### Riga 1: dichiarazione manuale di `printf`
> Fix: `#include <cstdio>`, poi `std::printf(...)`.
>
> ### Riga 5: conversione forzata a `int`
> `static_cast<int>(value)` tronca valori non interi. Alla riga 10, `log_value(2.5)` stampa `v=2`. Questo è un bug logico.
> Fix:
> ```cpp
> #include <iostream>
> template <typename T>
> void log_value(const T& value) { std::cout << "v=" << value << '\n'; }
> ```
>
> ### Riga 5: `printf` non è type-safe
> Fix con `std::format` C++20:
> ```cpp
> #include <format>
> template <typename T>
> void log_value(const T& value) { std::cout << std::format("v={}\n", value); }
> ```
>
> ### Riga 3: template troppo permissivo
> Fix con concept C++20:
> ```cpp
> template <typename T>
> concept Streamable = requires(std::ostream& os, const T& value) { os << value; };
> template <Streamable T>
> void log_value(const T& value) { std::cout << "v=" << value << '\n'; }
> ```
>
> ## Fix consigliato
> ```cpp
> template <Streamable T>
> void log_value(const T& value) { std::cout << "v=" << value << '\n'; }
> ```

## Seconda esecuzione (chat fresca, stesso prompt)

> ### Riga 1: dichiarazione manuale di `printf`
> Fix: `#include <cstdio>` e `std::printf(...)`.
>
> ### Riga 5: formato `%d` fisso per qualunque tipo `T`
> `log_value(2.5)` stampa `v=2`. Perdita silenziosa.
> Fix:
> ```cpp
> #include <iostream>
> template <typename T>
> void log_value(const T& value) { std::cout << "v=" << value << '\n'; }
> ```
>
> ### Riga 4: passaggio per valore potenzialmente inefficiente
> Fix: `void log_value(const T& value)`.
>
> ## Versione corretta consigliata
> ```cpp
> template <typename T>
> void log_value(const T& value) { std::cout << "v=" << value << '\n'; }
> ```
>
> ## Alternativa con `printf`, se proprio vuoi restare nel 1974
> ```cpp
> void log_value(int value) { std::printf("v=%d\n", value); }
> void log_value(double value) { std::printf("v=%f\n", value); }
> ```

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 4 (riga 1, riga 5 ×2, riga 3 template). 2ª: 3 (riga 1, riga 5, riga 4 pass-by-value). |
| Ha segnalato solo il template, le istanze, o entrambi? | Tratta il template come fonte unica, come Claude. Non distingue formalmente i 3 hit. |
| Falsi positivi | Riga 1 fuori scope. Issue duplicate sul template/printf nella 1ª chat. |
| Falsi negativi | 0 sul `printf`. Trova anche il bug semantico (troncamento `2.5 → 2`). **Però come negli snippet 01-03, non propone mai `std::print`/`std::println`**: la fix è `std::cout` o `std::format`+`std::cout`, mai `std::println`. |
| Deterministico (1ª = 2ª)? | **parzialmente** — bug del cast individuato in entrambe (✓), fix `std::cout` in entrambe (✓), ma: (a) numero issue diverso (4 vs 3), (b) la 1ª propone concept `Streamable`, la 2ª invece propone overload `log_value(int)` / `log_value(double)` — soluzioni filosoficamente diverse. |

## Verdetto sintetico

ChatGPT trova il bug semantico vero (`2.5 → 2`) come Claude, e propone fix che eliminano il `printf`. Conferma in modo netto i due pattern stabili da snippet 01:
1. **Mai `std::print`/`std::println`** (4/4 snippet finora);
2. **Determinismo medio**: in questo snippet, la 1ª chat propone concept `Streamable`, la 2ª invece overload espliciti per `int`/`double`. Sono soluzioni qualitativamente diverse, non solo cosmeticamente.

Per la slide: "ChatGPT raggiunge la sostanza ma con percorsi diversi tra esecuzioni — clang-tidy invece applica esattamente la stessa fix-it ogni volta".
