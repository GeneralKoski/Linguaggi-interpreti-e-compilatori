# Snippet 06_using_template_alias — Claude Opus 4.7

**File testato:** `comparison/snippets_neutral/06_using_template_alias.cpp`
**Data:** 2026-05-05
**Modello:** Claude Opus 4.7

## Ground truth (clang-tidy) — VERIFICATA con run reale
**1** solo hit: `return std::printf(fmt, args...);` dentro `logging::printf` (riga 6).

**Tutto il resto NON è segnalato** — e il motivo è il punto interessante:

- `printf("via using: %d\n", 42)` → `logging::printf` (using-declaration) ✓ ovvio
- `::printf("direct libc: %d\n", 1)` → **sorpresa**: dopo `using logging::printf`, sia il template che `::printf` libc sono nel namespace globale; la overload resolution preferisce il template (match esatto su `int`) alla variadic `...` di libc. Quindi `::printf(...)` con argomenti tipizzati chiama in realtà `logging::printf`!
- `TRACE("via macro: %d\n", 3)` → idem, dopo l'espansione `::printf(...)` con `int` risolve al template
- `emit(7)`, `emit(2.5)` → istanze del template `emit`, internamente chiamano `logging::printf`
- `detail::kPrintfPtr(...)` → chiamata via puntatore, non è un `callee(functionDecl)` matchabile

**Take-away didattico:** il matcher AST traccia esattamente ciò che overload resolution ha scelto, non il testo. Un lettore umano vede `::printf(...)` e assume libc.

## Risposta integrale dell'LLM (1ª chat fresca)

> ## 1. Riga 6 — Variadic template che propaga `...` non funziona con `printf`
> Il compilatore non può applicare `__attribute__((format(printf, 1, 2)))` perché qui i tipi sono generici, perdi `-Wformat`. Fix: macro variadic con va_list o `std::print` C++23.
>
> ## 2. Riga 12 — `using logging::printf;` collide con `std::printf` da `<cstdio>`
> ⭐ "A riga 31 (`::printf("direct libc: %d\n", 1);`) il commento dice 'direct libc' ma in realtà **è ambiguo — in molti compilatori risolverà alla using-declaration, non alla libc, quindi il commento mente**." Fix: rimuovere using-declaration e qualificare esplicitamente.
>
> ## 3. Riga 17 — `&std::printf` è formalmente undefined behavior
> Lo standard C++ proibisce di prendere l'indirizzo delle funzioni della libreria standard. Fix: lambda/funzione propria.
>
> ## 4. Riga 21 — La macro `TRACE` usa `::printf` che è ambiguo
> Stesso issue del punto 2 — `::printf` non è univoco. Fix: macro che chiama esplicitamente `::logging::printf`.
>
> ## 5. Riga 26 — `static_cast<int>(value)` perde informazioni silenziosamente
> `emit(2.5)` tronca a `2` senza warning. Fix: `if constexpr` o `std::print`.
>
> ## 6. Generale — niente `inline` sulla variabile `kPrintfPtr`
>
> Riepilogo prioritizzato: i tre fix che farei subito sono **#3** (UB formale), **#2** (using-decl crea ambiguità che rende il resto del file fuorviante) e **#5** (silent truncation).

## Seconda esecuzione (chat fresca, stesso prompt)

> ## Problema 1 — Riga 7: il template `printf` non fa perfect forwarding
> Fix: `Args&&...` + `std::forward`.
>
> ## Problema 2 — Riga 12: `using logging::printf;` collide con `std::printf`
> "La risoluzione di overload in `main` con `printf("...", 42)` può essere ambigua o, peggio, 'funzionante' in modo dipendente dall'implementazione." Fix: rimuovere using.
>
> ## Problema 3 — Riga 16: `typedef` invece di `using`
>
> ## Problema 4 — Riga 17: `&std::printf` non garantito
>
> ## Problema 5 — Riga 21: la macro `TRACE` usa `::printf` qualificato globalmente
> ⚠️ **"`::printf` salta deliberatamente il wrapper `logging::printf`. Se il punto del wrapper è centralizzare il logging, questa macro bypassa tutto. È un bug semantico."** ← qui Claude **sbaglia**: dopo `using logging::printf`, `::printf("...", 3)` con argomento int risolve al template `logging::printf`, NON alla libc. La macro NON bypassa il wrapper, lo chiama!
>
> ## Problema 6 — Righe 25-26: `printf("%d", value)` con template è bug latente
> `static_cast<int>(2.5) → 2`. Fix: `std::format`/`std::print`.
>
> ## Problema 7 — Righe 30-31: chiamate inconsistenti allo "stesso" `printf`
> "Quattro modi diversi di chiamare `printf` che fanno cose semanticamente diverse: solo la prima passa dal wrapper." ← anche qui Claude **sbaglia**: per via dell'overload resolution, le chiamate `::printf("...", int)`, `TRACE(...)`, e `emit(...)` chiamano TUTTE `logging::printf`. Solo la chiamata via `detail::kPrintfPtr` salta davvero il wrapper.

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 6 issue. 2ª: 8 issue. |
| Ha distinto `logging::printf` da `::printf` (using-decl)? | **1ª: SÌ ✓** ("`::printf("direct libc")` ... in molti compilatori risolverà alla using-declaration, non alla libc"). **2ª: NO ✗** (afferma erroneamente che `::printf` "salta il wrapper"). |
| Ha riconosciuto la chiamata via puntatore come non flaggabile? | Né 1ª né 2ª commentano la chiamata via puntatore come "non matchabile da un check". La trattano come una chiamata libc legittima. |
| Ha capito che `emit(...)` chiama `logging::printf`, non `::printf`? | 1ª: implicitamente sì (vede tutto come governato dall'using). 2ª: no (dice che il wrapper viene aggirato). |
| Ha visto il `::printf` dentro la macro `TRACE`? | sì entrambe, ma con interpretazioni opposte. |
| Falsi positivi | Molti issue fuori scope di `misc-no-printf`: forwarding, UB di `&std::printf`, typedef vs using, troncamento `2.5→2`. |
| Falsi negativi | 1ª: 0. 2ª: errore semantico vero — sostiene che il wrapper viene aggirato quando non lo è. |
| Deterministico (1ª = 2ª)? | **NO**, e in modo significativo. |

## Verdetto sintetico

⭐⭐⭐ **VERO WOW MOMENT.** Su questo snippet emerge la differenza più forte di tutto il confronto:

1. **clang-tidy**: 1 hit deterministico, basato sull'AST post-overload-resolution. Sempre lo stesso output.
2. **Claude 1ª**: ✓ riconosce esplicitamente che `::printf("direct libc")` risolve alla using-declaration, NON alla libc — finding allineato all'AST.
3. **Claude 2ª**: ✗ afferma il contrario — sostiene che `::printf(...)` è libc e che la macro `TRACE` "bypassa il wrapper".

**Lo stesso modello, sullo stesso input, con due chat fresche, dà risposte semanticamente opposte sul punto centrale.** Questo è il messaggio onesto del seminario: l'LLM può anche essere bravo (1ª chat ha capito tutto), ma non c'è garanzia di stabilità tra esecuzioni. clang-tidy ti dà la stessa risposta sempre.

Inoltre Claude solleva (in entrambe le chat) issue di alto livello fuori scope del check (UB formale, truncation, forwarding) — qualità review umana superiore, ma instabile sul punto chiave.

**Per la slide:** "Stesso codice, stesso modello, due esecuzioni, due interpretazioni opposte di overload resolution → clang-tidy sempre 1 hit, sempre nello stesso punto."
