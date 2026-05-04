# Prompt da usare (identico per tutti i test)

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

1. **Chat fresca per ogni run.** Sempre: anche la 2ª esecuzione dello stesso snippet va in una chat nuova. Riusare la stessa chat contamina il test perché il modello vede la sua risposta precedente nel contesto e tende a ripetersi (non staremmo misurando determinismo, ma coerenza intra-conversazione).
2. **Stesso prompt esatto** per tutti i test (6 snippet × 2 LLM × 2 run = 24 chat totali).
3. **Usa `snippets_neutral/`**, non `snippets/`. La versione neutrale non ha commenti che rivelano la risposta.
4. **Determinismo:** se le due chat fresche dello stesso (snippet, modello) danno risposte diverse (righe segnalate diverse, conclusioni diverse), annotalo come "non deterministico".

## LLM da testare

- Claude Opus 4.7
- ChatGPT 5.5

## Cosa registrare in ogni file di risposta

- Risposta integrale 1ª chat (copia-incolla)
- Risposta integrale 2ª chat (copia-incolla)
- Numero di righe segnalate
- Falsi positivi (ha segnalato qualcosa che NON è un problema?)
- Falsi negativi (ha mancato qualcosa che clang-tidy ha trovato?)
- Determinismo (la 2ª chat ha dato lo stesso output della 1ª?)
