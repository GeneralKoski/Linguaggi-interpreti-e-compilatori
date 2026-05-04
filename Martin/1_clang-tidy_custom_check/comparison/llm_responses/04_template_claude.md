# Snippet 04_template — Claude Opus 4.7

**File testato:** `comparison/snippets_neutral/04_template.cpp`
**Data:** YYYY-MM-DD
**Modello:** Claude Opus 4.7

## Ground truth (clang-tidy)
**3** warning: 1 sul template generico (riga 5) + 2 sulle istanziazioni concrete (`log_value<int>` e `log_value<double>`).

## Risposta integrale dell'LLM

> [INCOLLA QUI LA RISPOSTA INTEGRALE]

## Seconda esecuzione (stesso prompt, chat nuova)

> [INCOLLA QUI LA SECONDA RISPOSTA]

## Analisi

| Campo | Valore |
|---|---|
| Numero di problemi segnalati | _ |
| Ha segnalato solo il template, le istanze, o entrambi? | _ |
| Falsi positivi | _ |
| Falsi negativi | _ |
| Deterministico (1ª = 2ª)? | sì/no |

## Verdetto sintetico

[1 frase: l'LLM ha gestito bene/male questo snippet]

Punto narrativo: clang-tidy distingue il template "fonte" dalle sue istanze. Per una codebase con tanti `log_value<T>` istanziati, conoscere ogni call site è utile per fix-it precisi.
