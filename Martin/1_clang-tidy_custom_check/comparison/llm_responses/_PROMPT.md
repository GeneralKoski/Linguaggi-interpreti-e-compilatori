# Prompt da usare (identico per tutti gli 8 test)

Apri una **chat fresca** (no contesto pregresso) e incolla questo, sostituendo `<CODICE>` con il contenuto del file in `snippets_neutral/0N_*.cpp`:

---

```
Trova problemi in questo C++ moderno e suggerisci fix.
Sii specifico: cita il numero di riga, descrivi il problema,
e proponi il codice corretto.

<CODICE>
```

---

## Note importanti per la validità del test

1. **Chat fresca per ogni snippet.** Non riusare la stessa conversazione: il contesto contamina le risposte successive.
2. **Stesso prompt esatto** per tutti e 8 i test (4 snippet × 2 LLM).
3. **Usa `snippets_neutral/`**, non `snippets/`. La versione neutrale non ha commenti che rivelano la risposta.
4. **Per il check di determinismo:** dopo la prima risposta, ripeti il **medesimo prompt** una seconda volta nella stessa chat. Se la risposta cambia (parole, righe, conclusioni diverse), annotalo come "non deterministico".

## LLM da testare

- Claude Opus 4.7
- ChatGPT 5.5

## Cosa registrare in ogni file di risposta

- Risposta integrale (copia-incolla)
- Numero di righe segnalate
- Falsi positivi (ha segnalato qualcosa che NON è un problema?)
- Falsi negativi (ha mancato qualcosa che clang-tidy ha trovato?)
- Determinismo (la seconda esecuzione ha dato lo stesso output?)
