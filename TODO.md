# TODO — esame Linguaggi, Interpreti e Compilatori

Stato: 2026-05-05.
Per il dettaglio del lavoro tecnico già fatto vedi `Martin/1_clang-tidy_custom_check/{PLAN,TODO,RESULTS}.md`.

---

## ✅ Già chiuso

- Argomento del seminario confermato dal prof: **clang-tidy custom check** (`misc-no-printf`)
- Build LLVM da sorgente, check scritto+installato, lit test ufficiale PASS
- Run reali: `fmt` (0/0), `tinyxml2` (28/0 falsi positivi)
- Bear demo Make funzionante (5/5)
- Documentazione `.rst` ufficiale del check + entry in `list.rst`
- **Confronto LLM reale completato (2026-05-05)**: 6 snippet × 2 modelli × 2 chat fresche = 24 run. Wow moment confermato sullo snippet 06 (overload resolution): clang-tidy 100% deterministico, LLM 50/50 sul finding chiave. Vedi `Martin/1_clang-tidy_custom_check/comparison/COMPARISON.md`.
- **Slide v1 (2026-05-05)**: `SLIDES.md` riscritto con frontmatter Marp + tema custom, esportato in `SLIDES.pdf` 13 pagine. Diagramma architettura inline a 7 box colorati. Resta solo un pass di review a freddo.

---

## 🎯 Da fare — in ordine di priorità

### ~~1. Confronto LLM reale~~ ✅ FATTO 2026-05-05
6 snippet (estesi da 4: aggiunti 05_method_collision e 06_using_template_alias) × 2 modelli × 2 chat fresche = 24 run completati. Risultati integrali in `Martin/1_clang-tidy_custom_check/comparison/`.

**Wow moment vero**: snippet 06 (overload resolution dopo `using logging::printf;`). clang-tidy = 1 hit deterministico sempre nello stesso punto; LLM = 2/4 run sbagliano il finding chiave, lo stesso modello dà risposte opposte tra chat fresche.

**Pattern stabili**: ChatGPT non propone mai `std::print`/`std::println` come fix primario (0/6, solo come alternativa C++23 condizionale); Claude lo fa quando appropriato (4/6); determinismo Claude > ChatGPT.

**Bonus**: LLM trova bug fuori scope (troncamento `2.5→2`, `nullptr` UB) → conferma complementarità.

### 2. Mail al prof (~30 min) — tra ~2 settimane (intorno al 2026-05-18)
Incontro fatto oggi 2026-05-04: scrivere subito sa di "non ho fatto nulla, dammi una data". Tra 2 settimane invece arrivi con il check già funzionante e una proposta concreta.

Cosa scrivere:
- Riassumere brevemente cosa è stato fatto (check `misc-no-printf` operativo, lit test PASS, run reali su `fmt`/`tinyxml2`, demo Bear pronto, confronto LLM su 6 snippet × 2 modelli completato)
- Proporre 1-2 date tentative (preavviso ≥ 30 giorni)
- Chiedere se preferisce slide via mail prima per controllo

### ~~3. Slide vere~~ ✅ v1 FATTA 2026-05-05
`SLIDES.md` con frontmatter Marp + tema custom (palette blu/rosso/verde, JetBrains Mono, gradient cover); diagramma architettura inline (HTML+CSS, 7 box colorati); export `SLIDES.pdf` 13 pagine. Resta solo un pass di review a freddo prima di mandarle al prof.

### 4. Demo cronometrata (~3-4h)
- Terminale dedicato: prompt corto, font ≥ 18pt, sfondo chiaro
- Pre-popolare `~/.bash_history` con i comandi della demo (basta freccia ↑)
- Provare la demo **almeno 5 volte**, target 4-5 min per la sezione "demo live"
- **Piano B**: registrare un video da 2'30" (QuickTime) come backup se la live si rompe

### 5. Script verbale (~2h)
- 1-2 frasi chiave per ogni slide
- Apertura forte (15s) + chiusura forte (15s)
- Transizioni ("Detto questo...", "Vediamo ora...")

### 6. Studio orale dei 9 capitoli (~10-12h)
Seguire `study_method/STUDY_PLAN.md`:
- Lez 1-3 (Intro, Interpreti vs Compilatori, Struttura): 1.5h
- Lez 4-5 (Analisi lessicale, Flex): 2h
- Lez 6 (Analisi dipendente da contesto, TinyP): 2h
- Lez 7 (IR, LLVM, GEP, code shape): 2-3h
- Lez 8 (Middle end, dataflow, ottimizzazioni): 2-3h
- Lez 9 (Interpretazione astratta): 1.5h

Per ogni lezione: leggi `RIASSUNTO.md` → apri PDF docente → "Punti chiave per l'orale" → rispondi a voce alle domande.

### 7. Memorizzazione cheatsheet (~3h)
`notes/02_cheatsheet.md`:
- Equazioni dataflow (Live, Reaching, Available)
- Algoritmo LVN
- Algoritmo Hopcroft (worklist)
- Tabella domini astratti
- Formula GEP

### 8. 44 domande tipo (~2-3h)
`notes/04_domande_tipo.md`:
- Risposta a voce, cronometrando 2-3 min ognuna
- Per quelle su cui esiti: torna al RIASSUNTO o al PDF
- Riprova il giorno dopo

### 9. Domande "incrocio" (~2-3h)
- Pipeline completa di compilazione di un C++ con virtual methods
- Constant propagation con loop (dataflow + astratta)
- Middle end Clang vs interpretazione astratta

### 10. Ripasso intensivo (~2h, sera prima dell'orale)
- Solo "Punti chiave per l'orale" dei 9 RIASSUNTO
- Glossario `notes/03_glossario.md`
- **Niente cose nuove**

---

## ⏱ Bilancio ore

| Blocco | Ore |
|---|---|
| Mail prof + confronto LLM | 1.5-2.5h |
| Slide + demo + script | 11-14h |
| Studio orale + cheatsheet + domande | 17-22h |
| **Totale** | **~30-38h** |

## 📅 Ordine di lavoro suggerito

```
Sett. corrente : Fase 1 100% chiusa, slide v1 fatte — script verbale + review a freddo
+1             : demo cronometrata + video backup + pass review slide
+2             : MAIL AL PROF + eventuali rifiniture slide
+3             : studio orale lez 1-5
+4             : studio orale lez 6-9 + cheatsheet
+5             : SEMINARIO + 44 domande
+6             : domande incrocio
giorno -1      : ripasso leggero
giorno 0       : ORALE
```

Buffer 1 settimana ovunque per imprevisti.

---

## 📎 Riferimenti rapidi

- Stato dettagliato Fase 1: `Martin/1_clang-tidy_custom_check/RESULTS.md`
- Piano lavoro Fase 1-2-3: `Martin/1_clang-tidy_custom_check/PLAN.md`
- Coda Fase 1: `Martin/1_clang-tidy_custom_check/TODO.md`
- Bozza slide: `Martin/1_clang-tidy_custom_check/SLIDES.md`
- Setup blind test LLM: `Martin/1_clang-tidy_custom_check/comparison/`
- Programma corso e modalità d'esame: vedi cartelle `1_*` ... `9_*`
- Metodo di studio: `study_method/STUDY_PLAN.md`
- Cheatsheet + domande tipo: `notes/`

---

**Quando torno qui in futuro**, partiamo da questo file: spunto cosa è stato fatto e proseguiamo dal punto successivo.
