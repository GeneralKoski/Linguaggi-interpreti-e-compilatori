# Snippet 06_using_template_alias — ChatGPT 5.5

**File testato:** `comparison/snippets_neutral/06_using_template_alias.cpp`
**Data:** 2026-05-05
**Modello:** ChatGPT 5.5

## Ground truth (clang-tidy) — VERIFICATA con run reale
**1** solo hit: `return std::printf(fmt, args...);` dentro `logging::printf` (riga 6).

**Tutto il resto NON è segnalato** — e il motivo è il punto interessante:

- `printf("via using: %d\n", 42)` → `logging::printf` (using-declaration) ✓ ovvio
- `::printf("direct libc: %d\n", 1)` → **sorpresa**: dopo `using logging::printf`, la overload resolution preferisce il template (match esatto su `int`) alla variadic `...` di libc. Quindi `::printf(...)` con argomenti tipizzati chiama in realtà `logging::printf`!
- `TRACE("via macro: %d\n", 3)` → idem, dopo l'espansione `::printf(...)` con `int` risolve al template
- `emit(7)`, `emit(2.5)` → istanze del template `emit`, internamente chiamano `logging::printf`
- `detail::kPrintfPtr(...)` → chiamata via puntatore, non è un `callee(functionDecl)` matchabile

## Risposta integrale dell'LLM (1ª chat fresca)

> ### Riga 10: `using logging::printf;`
> Stai portando `logging::printf` nel namespace globale. È un cocktail di name lookup, overload e portabilità. Fix: rimuovere la using.
>
> ### Riga 13: `typedef` vs `using`
>
> ### Riga 14: `static PrintfPtr kPrintfPtr = &std::printf;`
> Mutabile, dovrebbe essere `const`/`inline constexpr`.
>
> ### Riga 17: `#define TRACE(...) ::printf(__VA_ARGS__)`
> ⚠️ **"`::printf` non è portabile con `#include <cstdio>`. In C++, `<cstdio>` garantisce `std::printf`; la versione nel namespace globale può esserci, ma non devi contarci. Quindi questo può non compilare su implementazioni conformi."** ← ChatGPT 1ª lo legge come problema di portabilità (`::printf` potrebbe non esistere), NON come ambiguità di overload resolution con il wrapper.
>
> ### Riga 21: chiamata non qualificata `printf(...)` in `emit`
> "Oggi risolve verso `logging::printf` perché hai fatto `using logging::printf` alla riga 10, ma è fragile."
> Inoltre `static_cast<int>(2.5) → 2` (truncation).
>
> ### Riga 25: `printf("via using: %d\n", 42)` dipende dall'using globale
>
> ### Riga 26: `::printf("direct libc: %d\n", 1)`
> ⚠️ **"non portabile con `<cstdio>`. `::printf` potrebbe non esistere."** ← interpreta `::printf` come "global namespace printf da libc, forse non disponibile", NON come "qui risolve al template via overload resolution". **Manca il finding centrale.**
>
> ### Riga 28: `TRACE(...)` eredita il problema della macro 17
>
> ### Riga 30: `emit(2.5)` stampa `2`, non `2.5`
>
> Versione corretta: rinomina TRACE in `logging::trace`, mantiene `&std::printf` per il puntatore.

## Seconda esecuzione (chat fresca, stesso prompt)

> | Riga | Problema | Fix |
> |---:|---|---|
> | 10 | ⭐ **"`using logging::printf;` porta `logging::printf` nel namespace globale. Dopo questa riga, `::printf(...)` può riferirsi al tuo wrapper, non necessariamente alla `printf` C globale. Quindi la riga 26 non è davvero 'direct libc'."** | Non importare nel globale. |
> | 13 | typedef → using | |
> | 14 | static + mutabile | inline constexpr |
> | 17 | ⭐ **"`#define TRACE(...) ::printf(__VA_ARGS__)` usa `::printf`, che dopo la riga 10 può puntare al wrapper `logging::printf`, non alla libc."** | `#define TRACE(...) ::logging::printf(__VA_ARGS__)` |
> | 21 | chiamata non qualificata `printf` in template | qualifica `logging::printf` o `std::printf` |
> | 21 | `static_cast<int>(2.5) → 2` | overload o constraint sui tipi |
> | 26 | ⭐ **"`::printf("direct libc: %d\n", 1)` è semanticamente fuorviante: non è garantito che sia la libc globale."** | usa `std::printf` |
> | 28 | TRACE eredita il problema 17 | |
> | 30 | `emit(2.5)` tronca | overload |
>
> Versione corretta: rinomina wrapper in `logging::print`, macro `::logging::print(...)`, overload `emit(int)`/`emit(double)`.

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 8 issue. 2ª: 9 issue. |
| Ha distinto `logging::printf` da `::printf` (using-decl)? | **1ª: NO ✗** (legge `::printf` come "potrebbe non esistere a livello globale", problema di portabilità, non riconosce l'overload resolution). **2ª: SÌ ✓** ("`::printf(...)` può riferirsi al tuo wrapper, non necessariamente alla `printf` C globale", "la riga 26 non è davvero 'direct libc'"). |
| Ha riconosciuto la chiamata via puntatore come non flaggabile? | né 1ª né 2ª: trattate come chiamata libc legittima. |
| Ha capito che `emit(...)` chiama `logging::printf`, non `::printf`? | 1ª: sì in `emit`, ma per l'using globale, non per overload resolution. 2ª: sì. |
| Ha visto il `::printf` dentro la macro `TRACE`? | sì entrambe. La 1ª lo legge come problema di portabilità, la 2ª come ambiguità con il wrapper. |
| Falsi positivi | Molti issue fuori scope: typedef, static_cast, internal linkage, perfect forwarding. |
| Falsi negativi | 1ª: manca il finding centrale (overload resolution dirotta `::printf` al template). 2ª: lo individua. |
| Deterministico (1ª = 2ª)? | **NO**, in modo netto sul punto chiave. |

## Verdetto sintetico

⭐⭐⭐ **VERO WOW MOMENT — confermato anche su ChatGPT.** Stesso pattern di Claude:

- **ChatGPT 1ª**: legge `::printf("direct libc...")` come problema di portabilità ("`::printf` potrebbe non esistere"), MISSA il finding centrale (overload resolution).
- **ChatGPT 2ª**: legge esplicitamente che "`::printf(...)` può riferirsi al tuo wrapper" e che "la riga 26 non è davvero 'direct libc'" — finding allineato all'AST.

**Stesso modello, due chat fresche, due interpretazioni opposte del punto critico.** Esattamente come è successo a Claude.

Bilancio combinato dei 4 run su snippet 06:
- **Claude 1ª**: ✓ scope-aware
- **Claude 2ª**: ✗ scope-blind ("::printf bypassa il wrapper")
- **ChatGPT 1ª**: ✗ scope-blind ("::printf potrebbe non esistere")
- **ChatGPT 2ª**: ✓ scope-aware

**2 su 4 LLM run sbagliano il finding chiave.** clang-tidy lo ottiene sempre giusto, perché lavora sull'AST post-resolution, non sul testo.

Per la slide: "Quattro chat fresche, due modelli, lo stesso prompt: 50/50 sul finding più importante. clang-tidy: 100% deterministico, sempre 1 hit nello stesso punto."

Inoltre, conferma il pattern stabile da snippet 01: ChatGPT non propone mai `std::print`/`std::println` come fix primario (6/6 snippet).
