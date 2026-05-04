# TODO — cosa manca per arrivare all'esame

Stato: 2026-05-05. **Fase 1 tecnica completamente chiusa.**
Stima totale residua: **~26-30h**.

---

## ⚙️ Fase 1 (tecnica) — TUTTO FATTO ✅

- [x] **Confronto LLM reale sui 6 snippet** ✅ FATTO 2026-05-05
  - Estesi i 4 snippet originali a 6: aggiunti `05_method_collision.cpp` (trappola falso positivo, metodo `Logger::printf`) e `06_using_template_alias.cpp` (caso composito: using-decl + template wrapper + function pointer + macro variadic + istanze template)
  - 24 chat fresche totali (6 snippet × 2 modelli × 2 run) per testare anche il determinismo tra esecuzioni
  - Risultati integrali in `comparison/llm_responses/0[1-6]_*_{claude,chatgpt}.md` e tabella consolidata in `comparison/COMPARISON.md`
  - **Wow moment confermato: snippet 06**. Su `::printf("...", 1)` dopo `using logging::printf;`, la overload resolution sceglie il template (`logging::printf<int>`), non libc. clang-tidy lo capisce sempre (1 hit deterministico). LLM: **2 su 4 run sbagliano** il finding chiave, e i due modelli sono incoerenti con sé stessi tra chat fresche.
  - **Pattern stabili**: ChatGPT non propone mai `std::print`/`std::println` come fix primario (0/6 snippet, solo come alternativa C++23 condizionale); Claude lo propone come fix primario quando appropriato (4/6); determinismo Claude > ChatGPT sui primi 5 snippet.
  - **Bonus inatteso**: gli LLM trovano bug fuori scope del check ma reali (troncamento `2.5 → 2` snippet 04, `%s` con `nullptr` UB snippet 05) — conferma "complementarità", non "sostituibilità".

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
| Disco quasi pieno (LLVM build occupa parecchio) | Verificare con `df -h ~`. Se serve, dopo il seminario si può cancellare `~/llvm-project/build/` (10+ GB) |
| Demo SDK macOS non passato | Usare sempre `--extra-arg=-isysroot $(xcrun --show-sdk-path)` nei comandi clang-tidy mostrati in slide |
| Bear non funziona con ninja su mac | Usare `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` per CMake. Bear va riservato al `bear_demo` Make |

---

## 📅 Timeline aggiornata

Da oggi (2026-05-05) all'esame, ipotizzando seminario a fine giugno + orale luglio:

| Settimana | Focus |
|-----------|-------|
| Sett. corrente | Fase 1 chiusa al 100% — partire con bozza slide |
| +1 | Slide + diagramma + script |
| +2 | Demo cronometrata + video backup + mail al prof |
| +3 | Studio orale lezioni 1-5 |
| +4 | Studio orale lezioni 6-9 + cheatsheet |
| +5 | **SEMINARIO** + 44 domande tipo |
| +6 | Domande incrocio + ripasso |
| Giorno 0 | **ORALE** |

Buffer: 1 settimana ovunque per imprevisti.
