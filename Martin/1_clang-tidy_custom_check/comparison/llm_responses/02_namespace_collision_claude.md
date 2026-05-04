# Snippet 02_namespace_collision — Claude Opus 4.7

**File testato:** `comparison/snippets_neutral/02_namespace_collision.cpp`
**Data:** YYYY-MM-DD
**Modello:** Claude Opus 4.7

## Ground truth (clang-tidy)
**1** warning su riga 9 (`::printf(...)`).
**Non** segnala `mylib::printf` su riga 8 — è una funzione user-defined nel namespace `mylib`, non `::printf` di libc.

## Risposta integrale dell'LLM

> [INCOLLA QUI LA RISPOSTA INTEGRALE]

## Seconda esecuzione (stesso prompt, chat nuova)

> [INCOLLA QUI LA SECONDA RISPOSTA]

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | _ |
| Ha riconosciuto `mylib::printf` come distinto da `::printf`? | sì/no |
| Falsi positivi (es. ha flaggato `mylib::printf`?) | _ |
| Falsi negativi | _ |
| Deterministico (1ª = 2ª)? | sì/no |

## Verdetto sintetico

⭐ **Snippet chiave per la slide.** Se l'LLM ha flaggato `mylib::printf` come errore, qui hai la prova viva del valore dell'AST sullo scope.

[1 frase: l'LLM ha gestito bene/male questo snippet]
