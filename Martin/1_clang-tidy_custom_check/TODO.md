# TODO — cosa manca per arrivare all'esame

Stato: 2026-05-04, dopo il completamento della Fase 1 tecnica.
Stima totale residua: **~30-35h**.

---

## ⚙️ Fase 1 (tecnica) — coda residua (~3-5h)

- [ ] **Confronto LLM reale sui 4 snippet** (1-2h)
  - Per ognuno dei file in `comparison/snippets/` chiedere a Claude e ChatGPT: *"trova problemi e suggerisci fix in questo C++ moderno"*
  - Annotare in `comparison/COMPARISON.md` cosa l'LLM dice davvero (la tabella attuale è predittiva)
  - Identificare almeno **1 caso dove l'LLM sbaglia** (probabile: snippet 02 namespace o snippet 03 macro) per usarlo come punto di forza in slide

- [x] **Lit test via build farm** ✅ FATTO 2026-05-04
  - `FileCheck` buildato, test copiato in `clang-tools-extra/test/clang-tidy/checkers/misc/`
  - `CLANG=/usr/bin/clang llvm-lit ... -v` → `PASS (1 of 1)` in 0.27s
  - Pattern `CHECK-MESSAGES` raccorciato per matchare il prefisso del messaggio (la nota `(richiede ...)` interrompeva la corrispondenza esatta col tag `[misc-no-printf]`)

- [x] **Run su progetto open source reale** ✅ FATTO 2026-05-04
  - **fmt** (3 TU, 0.7s, 0 hit) — caso negativo: codebase clean by design
  - **tinyxml2** (2 TU, 0.2s, 28 hit, 0 falsi positivi) — caso positivo
  - **Insight chiave** per le slide: grep dice 42, clang-tidy dice 28 → i 14 di scarto sono commenti/stringhe/`vfprintf`/`snprintf`/macro. È il valore esatto dell'analisi AST sulla regex testuale
  - Bear NON funziona con ninja su macOS (SIP/sandbox); per progetti CMake serve `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`. Bear resta utile per Make → vedi `bear_demo/`
  - Output integrali: `real_world_runs/{fmt,tinyxml2}_output.log`

---

## 🎤 Fase 2 (presentazione) — ~12-16h

### Slide vere (~6-8h)
- [ ] Scegliere tool: **Marp** (markdown→PDF, comodo per chi lavora da terminale) o **Keynote/Google Slides**
- [ ] Convertire `SLIDES.md` in slide vere, una per blocco `## Slide N`
- [ ] Font monospace per il codice: JetBrains Mono o Fira Code
- [ ] Evidenziare in colore le righe chiave dei matcher (slide 5, 8)
- [ ] Diagramma slide 3 (frontend Clang → AST → matchers → check) — disegnare in Excalidraw o similare
- [ ] Esportare in PDF

### Demo (~3-4h)
- [ ] Terminale dedicato con prompt corto, font 18pt+, sfondo scuro chiaro
- [ ] Pre-popolare `~/.bash_history` con i comandi della demo (così basta freccia ↑)
- [ ] Provare la demo cronometrata **almeno 5 volte**, target 4-5 min per la sezione "demo live"
- [ ] **Piano B**: registrare un video da 2'30" come backup (QuickTime + zoom)

### Script verbale (~2h)
- [ ] Scrivere 1-2 frasi chiave per ogni slide
- [ ] Frase di apertura forte (15 secondi)
- [ ] Frase di chiusura forte (15 secondi)
- [ ] Identificare le transizioni ("Detto questo...", "Vediamo ora...")

### Logistica (~1h)
- [ ] **Mail al prof** per fissare data tentativa (preavviso 30 giorni!)
- [ ] Mandare slide al prof prima della data per controllo

---

## 📚 Fase 3 (orale teoria) — ~15-20h

L'orale vale **70%** e copre **tutto il programma**, non solo il seminario.

Seguire `study_method/STUDY_PLAN.md`. Riassunto:

- [ ] **Studio sistematico** dei 9 capitoli (10-12h)
  - Lezione 1-3 (Intro, Interpreti vs Compilatori, Struttura): 1.5h
  - Lezione 4-5 (Analisi lessicale, Flex): 2h
  - Lezione 6 (Analisi dipendente da contesto, TinyP): 2h
  - Lezione 7 (IR, LLVM, GEP, code shape): 2-3h
  - Lezione 8 (Middle end, dataflow, ottimizzazioni): 2-3h
  - Lezione 9 (Interpretazione astratta): 1.5h
- [ ] **Cheatsheet**: memorizzare `notes/02_cheatsheet.md` (3h)
- [ ] **44 domande tipo**: rispondere a voce, cronometrando 2-3 min ognuna (`notes/04_domande_tipo.md`) (2-3h)
- [ ] **Domande "incrocio"**: pipeline completa, constant propagation, middle end vs astratta (2-3h)
- [ ] **Ripasso ad alta intensità** sera prima dell'orale (2h)

---

## 🚧 Rischi residui

| Rischio | Mitigazione |
|---|---|
| Demo live si rompe per bug ambiente | Video backup pronto + screenshot |
| Date orale troppo vicine al seminario | Mandare mail al prof subito per buffer |
| Studio orale tirato all'ultimo → bocciatura | Iniziare il giorno dopo che hai mandato la mail |
| Disco pieno (LLVM build = 13GB) | Già in 40GB liberi, ok ma non scaricare altro |

---

## 📅 Timeline aggiornata

Da oggi (2026-05-04) all'esame, ipotizzando seminario a fine giugno + orale luglio:

| Settimana | Focus |
|-----------|-------|
| Sett. corrente | Coda Fase 1 (LLM reale, lit test, libreria reale) |
| +1 | Slide + diagramma + script |
| +2 | Demo cronometrata + video backup + mail al prof |
| +3 | Studio orale lezioni 1-5 |
| +4 | Studio orale lezioni 6-9 + cheatsheet |
| +5 | **SEMINARIO** + 44 domande tipo |
| +6 | Domande incrocio + ripasso |
| Giorno 0 | **ORALE** |

Buffer: 1 settimana ovunque per imprevisti.
