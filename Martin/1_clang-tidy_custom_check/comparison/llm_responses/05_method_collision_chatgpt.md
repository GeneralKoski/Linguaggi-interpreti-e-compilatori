# Snippet 05_method_collision — ChatGPT 5.5

**File testato:** `comparison/snippets_neutral/05_method_collision.cpp`
**Data:** 2026-05-05
**Modello:** ChatGPT 5.5

## Ground truth (clang-tidy)
**2** warning, entrambi sulle chiamate a `::printf` di libc:
- riga 8 (`::printf("[Logger] %s\n", msg);`)
- riga 14 (`::printf("hello from libc\n");`)

**Non** segnala `log.printf("hello from method")` (riga 13): è un `CXXMemberCallExpr` su `Logger::printf`, non `::printf`.

## Risposta integrale dell'LLM (1ª chat fresca)

> ## Riga 1: dichiarazione manuale di `printf`
> Fragile. Fix: `#include <cstdio>` e `std::printf(...)`.
>
> ## Riga 4: metodo chiamato `printf`
> Tecnicamente non illegale ("Logger::printf sta nello scope della classe e ::printf nello scope globale"), ma pessima leggibilità. Fix: rinominare in `log` o `write`.
>
> ## Riga 8: uso di `::printf` invece di `std::printf`
> Dipende dal namespace globale. Fix: `std::printf("[Logger] %s\n", msg);`.
>
> ## Riga 8: possibile crash se `msg == nullptr`
> `%s` con puntatore nullo è UB. Fix: `std::printf("[Logger] %s\n", msg ? msg : "<null>");`.
>
> ## Riga 14: uso diretto di `::printf`
> Stesso problema. Fix: `std::printf("hello from libc\n");`.
>
> ## Versione corretta consigliata
> ```cpp
> #include <cstdio>
> struct Logger { void log(const char* msg); };
> void Logger::log(const char* msg) {
>     std::printf("[Logger] %s\n", msg ? msg : "<null>");
> }
> int main() {
>     Logger log;
>     log.log("hello from method");
>     std::printf("hello from libc\n");
>     return 0;
> }
> ```
>
> ## Variante ancora più C++ moderna
> ```cpp
> #include <iostream>
> #include <string_view>
> struct Logger { void log(std::string_view msg); };
> void Logger::log(std::string_view msg) {
>     std::cout << "[Logger] " << msg << '\n';
> }
> ```

## Seconda esecuzione (chat fresca, stesso prompt)

> ## Riga 1: dichiarazione manuale di `printf`
> Fix: `#include <cstdio>`.
>
> ## Riga 4: metodo chiamato `printf`
> Pessima idea di leggibilità. Tu hai evitato l'ambiguità usando `::printf`, ma il codice resta inutilmente ingannevole. Fix: rinomina in `log`/`info`/`write`/`print`.
>
> ## Riga 8: possibile crash se `msg == nullptr`
> Fix: `std::printf("[Logger] %s\n", msg ? msg : "(null)");`.
>
> ## Riga 4 e 7: meglio `std::string_view`
> ```cpp
> void Logger::log(std::string_view msg) {
>     std::printf("[Logger] %.*s\n", static_cast<int>(msg.size()), msg.data());
> }
> ```
>
> ## Versione consigliata finale
> ```cpp
> #include <cstdio>
> #include <string_view>
> struct Logger { void log(std::string_view msg); };
> void Logger::log(std::string_view msg) {
>     std::printf("[Logger] %.*s\n", static_cast<int>(msg.size()), msg.data());
> }
> int main() {
>     Logger logger;
>     logger.log("hello from method");
>     std::printf("hello from libc\n");
> }
> ```

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | 1ª: 5 (riga 1, riga 4, riga 8 ::printf, riga 8 nullptr, riga 14). 2ª: 4 (riga 1, riga 4, riga 8 nullptr, riga 4/7 string_view). |
| Ha riconosciuto `Logger::printf` come metodo distinto? | **sì, esplicitamente** in entrambe le chat ("Logger::printf sta nello scope della classe e ::printf nello scope globale", "Tu hai evitato l'ambiguità usando ::printf"). |
| Falsi positivi (es. ha flaggato `log.printf`?) | **NO** — non confonde mai `log.printf("...")` con `::printf` libc. ✓ Passa la trappola. |
| Falsi negativi | 0 sui `::printf` libc. **Bonus inatteso:** trova il **bug del nullptr** (riga 8: `%s` con puntatore nullo è UB), che clang-tidy non segnala — issue reale fuori scope del check. Come negli snippet 01-04, **non propone mai `std::print`/`std::println`**: la fix è `std::printf` qualificato. |
| Deterministico (1ª = 2ª)? | **parzialmente** — diagnosi centrale identica (rename, nullptr, qualifica `std::printf`), ma: (a) numero issue diverso (5 vs 4), (b) la 1ª chat propone come "variante moderna" `std::cout`+`string_view`, la 2ª invece propone direttamente `std::printf("%.*s", ...)` con `string_view` come fix primario. |

## Verdetto sintetico

⭐ **ChatGPT passa la trappola di falso positivo come Claude**: distingue `Logger::printf` da `::printf` esplicitamente. Determinismo medio (5 vs 4 issue, fix moderno divergente).

**Bonus interessante:** ChatGPT trova il **bug del nullptr** (riga 8) — `%s` con `nullptr` è UB. Claude in questo snippet NON l'ha individuato. È il primo finding "ChatGPT > Claude" su un bug reale fuori dal mandato del check.

Per la slide: entrambi i modelli passano il test di scope, quindi su questa trappola specifica non c'è il "wow moment" sognato. Ma:
1. Conferma il pattern stabile: ChatGPT non propone mai `std::print`/`std::println` come fix primario (5/5 snippet finora);
2. ChatGPT trova un bug reale che Claude perde (nullptr → UB);
3. Determinismo: Claude alto, ChatGPT medio (pattern stabile).
