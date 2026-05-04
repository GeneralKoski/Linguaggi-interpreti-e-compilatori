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
- **Script verbale (2026-05-05)**: `Martin/1_clang-tidy_custom_check/SCRIPT.md` con frasi pivot tecnico-fluide per ogni slide, apertura/chiusura forte (~15s ognuna), transizioni esplicite, Q&A con 6 domande probabili + risposte preparate, note operative.
- **Materiali studio orale (2026-05-05)**: audit completo dei 9 RIASSUNTO + `notes/`. Chiuse lacune bloccanti per orale 70% (era assente l'analisi sintattica → nuovo `notes/08_parsing.md` ~670 righe; corretti errori in Live equation, Galois connection, dominator init; aggiunte sezioni in IR (GEP, invoke, conversioni, refcount, access link), Middle End (Available Expr+GCSE, Very Busy+hoisting, IV+strength reduction, mem2reg/SROA/instcombine/LICM), Interpretazione Astratta (Knaster-Tarski, Kleene, soundness con dim, esempio Intervals)). Glossario esteso, 13 nuove domande (44 → 57).

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

### 4. Demo cronometrata + pratica script (~4-5h)
**Pre-requisito:** rileggi prima `Martin/1_clang-tidy_custom_check/SCRIPT.md` per allineare i comandi della demo con le frasi-pivot delle slide 10 e 11.

- Terminale dedicato: prompt corto, font ≥ 18pt, sfondo chiaro
- Pre-popolare `~/.bash_history` con i comandi della demo (basta freccia ↑)
- Provare la demo **almeno 5 volte**, target 4-5 min per la sezione "demo live"
- **Piano B**: registrare un video da 2'30" (QuickTime) come backup se la live si rompe
- **Pratica voce alta dello script:** leggere `SCRIPT.md` 2-3 volte cronometrando l'intero seminario (target 15 min), per memorizzare i punti pivot. Alla terza lettura non dovresti più dipendere dal foglio.

### ~~5. Script verbale~~ ✅ FATTO 2026-05-05
`Martin/1_clang-tidy_custom_check/SCRIPT.md`: frasi pivot tecnico-fluide per ogni slide, apertura/chiusura forte, transizioni esplicite, Q&A con 6 domande probabili + risposte preparate, note operative. Resta solo la pratica a voce alta (2-3 volte cronometrate).

### 6. Studio orale dei 9 capitoli (~12-14h)
Seguire `study_method/STUDY_PLAN.md`:
- Lez 1-3 (Intro, Interpreti vs Compilatori, Struttura): 1.5h
- Lez 4-5 (Analisi lessicale, Flex): 2h
- **Parsing (analisi sintattica) — `notes/08_parsing.md`** (CFG, LL(1), LR(1), conflitti, LALR/Bison): 2h. Lacuna chiusa nel 2026-05-05; non c'è una cartella di lezione dedicata, il file `notes/08_parsing.md` è la fonte unica.
- Lez 6 (Analisi dipendente da contesto, TinyP): 2h
- Lez 7 (IR, LLVM, GEP, code shape, invoke, conversioni, access link): 2-3h
- Lez 8 (Middle end, dataflow, Available/Very Busy, induction var, LLVM passes): 2-3h
- Lez 9 (Interpretazione astratta, Knaster-Tarski, Kleene, soundness, widening): 1.5h

Per ogni lezione: leggi `RIASSUNTO.md` → apri PDF docente → "Punti chiave per l'orale" → rispondi a voce alle domande.

### 7. Memorizzazione cheatsheet (~3h)
`notes/02_cheatsheet.md`:
- Equazioni delle 4 DFA classiche (Live, Reaching, Available, Very Busy) + tabella riepilogo direzione/meet/init/fixpoint
- Algoritmo LVN
- Algoritmo Hopcroft (worklist) con definizione precisa di `δ⁻¹(s, c)`
- FIRST/FOLLOW (algoritmo) + tabella domini astratti
- Formula GEP, Galois connection, Knaster-Tarski/Kleene
- Calling convention, activation record, vtable

### 8. 57 domande tipo (~3-4h)
`notes/04_domande_tipo.md` (era 44, +13 dopo l'audit del 2026-05-05):
- Sezioni: Introduzione, Front end, **Analisi sintattica** (nuova), IR e back end, Middle end, Interpretazione astratta, Strumenti, Trick
- Risposta a voce, cronometrando 2-3 min ognuna
- Le 13 domande nuove hanno traccia di risposta — usala come autovalutazione (rispondi prima, poi confronta)
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

## ⏱ Bilancio ore residue (post-2026-05-05)

| Blocco | Stato | Ore residue |
|---|---|---|
| Confronto LLM | ✅ fatto | — |
| Slide v1 (PDF) | ✅ fatto | — |
| Script verbale | ✅ fatto | — |
| Audit materiali studio | ✅ fatto | — |
| Demo cronometrata + video backup + pratica voce alta | ⏳ | 4-5h |
| Pass review slide a freddo | ⏳ | 0.5h |
| Mail al prof | ⏳ | 0.5h |
| Studio orale 9 capitoli + parsing | ⏳ | 12-14h |
| Cheatsheet + 57 domande + incrocio | ⏳ | 7-10h |
| Ripasso intensivo | ⏳ | 2h |
| **Totale residuo** | | **~26-32h** |

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
- **Slide finale (PDF):** `Martin/1_clang-tidy_custom_check/SLIDES.pdf` (sorgente: `SLIDES.md`)
- **Script verbale del seminario:** `Martin/1_clang-tidy_custom_check/SCRIPT.md`
- **Confronto LLM completo:** `Martin/1_clang-tidy_custom_check/comparison/COMPARISON.md` + `comparison/llm_responses/`
- Programma corso e modalità d'esame: vedi cartelle `1_*` ... `9_*`
- Metodo di studio: `study_method/STUDY_PLAN.md`
- **Cheatsheet, glossario, parsing, dataflow, 57 domande:** `notes/` (indice in `notes/00_INDEX.md`)

---

**Quando torno qui in futuro**, partiamo da questo file: spunto cosa è stato fatto e proseguiamo dal punto successivo.
