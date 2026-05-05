# 07 — Mail al prof

Stima: **30 min**. Da inviare **intorno al 2026-05-18** (~2 settimane dopo l'incontro del 2026-05-04).

## Perché aspettare 2 settimane

Scrivere subito dopo l'incontro suona come "non ho fatto nulla, dammi una data". Tra 2 settimane invece arrivi con il check già funzionante, il confronto LLM completo e una proposta concreta.

A questo punto avrai anche slide + script provati e demo cronometrata: se chiede di vederli, sono spedibili lo stesso giorno.

## Destinatario

- **Enea Zaffanella** — `enea.zaffanella@unipr.it`

## Cosa scrivere

1. **Riepilogo concreto del fatto** (3-4 punti, brevi):
   - Check `misc-no-printf` operativo, lit test ufficiale PASS
   - Run reali: `fmt` (0 hit / 0 FP), `tinyxml2` (28 hit / 0 FP)
   - Demo Bear su Make funzionante
   - Confronto clang-tidy vs LLM su 6 snippet × 2 modelli × 2 chat fresche = 24 run; wow moment confermato sull'overload resolution dopo `using`

2. **Proposta date**: 1-2 date tentative, **preavviso ≥ 30 giorni** dalla data della mail

3. **Domanda aperta**: se preferisce vedere le slide via mail prima per controllo

## Template (da rifinire prima di mandare)

```
Oggetto: Seminario LIC — proposta date e stato lavoro

Buongiorno Professore,

a seguito dell'incontro del 4 maggio, le scrivo per fare il punto sul
seminario sul check personalizzato per clang-tidy e proporre una data.

Stato del lavoro:
- Check `misc-no-printf` scritto e installato nel build LLVM, lit test
  ufficiale PASS
- Run su progetti reali: fmt (0 hit, 0 falsi positivi) e tinyxml2
  (28 hit, 0 falsi positivi)
- Demo con Bear su un piccolo Makefile funzionante
- Confronto clang-tidy vs LLM (Claude e ChatGPT) su 6 snippet
  in 2 chat fresche per modello (24 run totali). Il caso più interessante
  è la risoluzione di overload dopo `using logging::printf;`:
  clang-tidy 100% deterministico, gli LLM sbagliano il finding chiave
  in metà delle run.

Le slide sono pronte in versione 1 (13 slide, ~15 minuti) e ho preparato
uno script verbale per il seminario; resta da fare il pass di pratica
cronometrata.

Le proporrei come date tentative: [DATA 1] o [DATA 2].
Se preferisce, posso inviarle slide e script via mail per un controllo
preliminare prima di concordare la data definitiva.

Resto a disposizione anche per un breve incontro su Teams.

Grazie e buona giornata,
Martin Trajkovski
```

## Note

- **Tono**: formale ma non rigido. Il prof è disponibile (lui stesso ha scritto "anche da remoto via Microsoft Teams" nel ricevimento).
- **Date proposte**: scegli un giorno feriale, non a ridosso di festività. ≥ 30 gg dalla mail.
- **Allegati**: niente al primo invio. Solo se li chiede lui.
- **Se non risponde in 5-7 giorni**: una mail di follow-up gentile.
