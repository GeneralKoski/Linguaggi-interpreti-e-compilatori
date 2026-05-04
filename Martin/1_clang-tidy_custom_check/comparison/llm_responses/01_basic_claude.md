# Snippet 01_basic — Claude Opus 4.7

**File testato:** `comparison/snippets_neutral/01_basic.cpp`
**Data:** YYYY-MM-DD
**Modello:** Claude Opus 4.7

## Ground truth (clang-tidy)
1 warning su riga 4 (`printf("hello\n");`), suggerisce `std::print`.

## Risposta integrale dell'LLM

> [INCOLLA QUI LA RISPOSTA INTEGRALE]

## Seconda esecuzione (stesso prompt, chat nuova)

> [INCOLLA QUI LA SECONDA RISPOSTA]

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | _ |
| Ha trovato il `printf`? | sì/no |
| Suggerisce `std::print`? | sì/no/altro: _ |
| Falsi positivi | _ |
| Falsi negativi | _ |
| Deterministico (1ª = 2ª)? | sì/no |

## Verdetto sintetico

[1 frase: l'LLM ha gestito bene/male questo snippet]
