# Piano di lavoro — clang-tidy custom check

Tre fasi: **progetto tecnico** → **presentazione** → **preparazione orale**.

---

## ✅ Stato attuale (2026-05-04, fine giornata)

**Fase 1 (tecnica) — 100% chiusa lato Claude:**
- ✅ Argomento confermato dal prof, scope invariato
- ✅ Toolchain: `clang`, `ninja`, `bear`, `cmake`, `FileCheck` installati/buildati
- ✅ `llvm-project` clonato, configurato e buildato (`clang-tidy` 23.0.0git pronto in `~/llvm-project/build/bin/`)
- ✅ Check `misc-no-printf` scritto, installato, ricompilato e testato
- ✅ Demo locali: 3 hit su `demo/bad.cpp`, 0 su `demo/good.cpp`
- ✅ 4 snippet di confronto pronti — il rilevamento clang-tidy è quello atteso
- ✅ `bear_demo/`: `bear -- make` → `compile_commands.json` → 5/5 hit reali
- ✅ **Lit test ufficiale**: integrato in `clang-tools-extra/test/clang-tidy/checkers/misc/`, `PASS (1 of 1)` con `llvm-lit`
- ✅ **Run su progetti reali** (richiesta del prof):
  - `fmt` (3 TU, 0.7s) → 0 hit (clean by design ✅)
  - `tinyxml2` (2 TU, 0.2s) → **28 hit, 0 falsi positivi** vs 42 match grep — gap di 14 = valore AST
- ✅ Bozza `SLIDES.md` (13 slide, 15 min)

**Resta da fare lato utente** (non tecnico-implementativo):
- ⏳ Confronto LLM reale sui 4 snippet (richiede sessione Claude/ChatGPT separata)

**Fase 2 (presentazione) ~5% fatta:** solo outline testuale, slide vere non ancora prodotte.

**Fase 3 (orale) 0% fatta:** vedi `study_method/STUDY_PLAN.md`.

Lista completa azionabile in **`TODO.md`**, output reali in **`RESULTS.md`**.

---

## 📌 Indicazioni dal prof (incontro 2026-05-04)

Il prof ha confermato l'argomento senza obiezioni e ha aggiunto 3 suggerimenti concreti per arricchire il seminario:

1. **Test su progetto reale, non solo esempi piccoli** — oltre ai file `.cc` di esempio, lanciare il check su una libreria/progetto open source di dimensioni medio-grandi per mostrare valore pratico e gestione di falsi positivi/negativi a scala.
2. **Usare Bear per generare `compile_commands.json`** — `bear -- make` (o equivalente) intercetta la build di un progetto esistente e produce il database di compilazione necessario a clang-tidy per analizzarlo correttamente. Da mostrare anche in demo.
3. **Confronto clang-tidy vs IA su esempi piccoli** — su 2-3 snippet, far girare il check e in parallelo chiedere a un LLM di trovare lo stesso problema. Discutere differenze: determinismo, fix-it, falsi positivi, scalabilità, costo.

Questi 3 punti vanno integrati: nelle slide finali (sezione "valore pratico" + "limiti vs IA"), nella demo (Bear + run su progetto reale), nei deliverable di Fase 1.

---

## 🛠 FASE 1 — Costruzione del progetto (~25-30h)

### Obiettivo finale
Avere un check clang-tidy funzionante, testato, con esempi che lo triggherrano e altri che no. Tutto buildato dal sorgente di LLVM.

### Step 1.1 — Setup ambiente (3-5h)
- [ ] Installare prerequisiti: `cmake` ✅, `ninja` ❌, `bear` ❌, Python 3, Git
  ```bash
  brew install ninja bear
  ```
- [ ] Clonare `llvm-project` in una directory di lavoro (NON dentro questo repo)
  ```bash
  cd ~
  git clone --depth 1 https://github.com/llvm/llvm-project.git
  ```
- [ ] Configurare la build con `clang-tools-extra` abilitato
  ```bash
  cd llvm-project
  cmake -S llvm -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
    -DLLVM_TARGETS_TO_BUILD="X86;AArch64" \
    -DLLVM_USE_LINKER=lld \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++
  ```
- [ ] Buildare clang-tidy
  ```bash
  ninja -C build clang-tidy
  ```
  ⏱ Su M1/M2: ~30-45 min. Su Intel: 1-2h. Spazio disco: ~10-15 GB.
- [ ] Verificare: `./build/bin/clang-tidy --version`

### Step 1.2 — Studiare l'AST di Clang (3-4h)
- [ ] Familiarizzare con `clang -Xclang -ast-dump -fsyntax-only file.cc`
- [ ] Leggere documentazione AST matchers: https://clang.llvm.org/docs/LibASTMatchersReference.html
- [ ] Leggere il tutorial ufficiale: https://clang.llvm.org/extra/clang-tidy/Contributing.html
- [ ] Esplorare 2-3 check esistenti come riferimento:
  - `clang-tools-extra/clang-tidy/modernize/UseAutoCheck.cpp` (semplice, da imitare)
  - `clang-tools-extra/clang-tidy/readability/MagicNumbersCheck.cpp` (più ricco)
  - `clang-tools-extra/clang-tidy/bugprone/UndefinedMemoryManipulationCheck.cpp` (matcher complessi)

### Step 1.3 — Scegliere il check da implementare (1h)
Scegli UNO. Vuoi un check con:
- Match chiaro e dimostrabile
- Pochi falsi positivi/negativi gestibili in 15 min
- Q&A circoscritto

**Candidati ordinati per facilità:**
1. **NoCStyleCastCheck** — segnala `(int)x` e suggerisce `static_cast<int>(x)`
2. **NoPrintfCheck** — segnala chiamate a `printf` e suggerisce `std::format`/`fmt::print`
3. **NoMagicNumbersCheck** — letterali numerici diversi da 0, 1, -1 (variante didattica del check esistente)
4. **VirtualWithoutOverrideCheck** — segnala metodi che dovrebbero avere `override` ma non ce l'hanno
5. **NoSingletonCheck** — segnala il pattern Singleton (più ambizioso)

**Mia raccomandazione:** parti da **#1 (NoCStyleCast)** per il primo prototipo, poi se hai tempo passa a uno più ricco.

### Step 1.4 — Scaffolding del check (2-3h)
- [ ] Aggiungere il check a un modulo (es. `misc`):
  ```bash
  cd clang-tools-extra/clang-tidy/misc
  # crea NoCStyleCastCheck.h e NoCStyleCastCheck.cpp
  ```
- [ ] Scrivere lo scheletro:
  ```cpp
  // NoCStyleCastCheck.h
  #pragma once
  #include "../ClangTidyCheck.h"
  namespace clang::tidy::misc {
  class NoCStyleCastCheck : public ClangTidyCheck {
  public:
    NoCStyleCastCheck(StringRef Name, ClangTidyContext *Ctx)
        : ClangTidyCheck(Name, Ctx) {}
    void registerMatchers(ast_matchers::MatchFinder *Finder) override;
    void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  };
  }
  ```
- [ ] Registrare il check nel `MiscTidyModule.cpp`:
  ```cpp
  CheckFactories.registerCheck<NoCStyleCastCheck>("misc-no-cstyle-cast");
  ```
- [ ] Aggiornare `CMakeLists.txt` del modulo
- [ ] Rebuild: `ninja -C build clang-tidy`

### Step 1.5 — Implementazione del matcher e logica (4-6h)
- [ ] Scrivere `registerMatchers`:
  ```cpp
  void NoCStyleCastCheck::registerMatchers(MatchFinder *Finder) {
    Finder->addMatcher(
      cStyleCastExpr(unless(isExpansionInSystemHeader())).bind("cast"),
      this);
  }
  ```
- [ ] Scrivere `check` per emettere la diagnostica + fix-it:
  ```cpp
  void NoCStyleCastCheck::check(const MatchFinder::MatchResult &Result) {
    const auto *Cast = Result.Nodes.getNodeAs<CStyleCastExpr>("cast");
    diag(Cast->getBeginLoc(),
         "C-style cast vietato; usa static_cast/reinterpret_cast/const_cast")
      << FixItHint::CreateReplacement(Cast->getSourceRange(),
                                       suggestModernCast(Cast));
  }
  ```
- [ ] Build + test rapido su un file `.cc` con 1-2 cast

### Step 1.6 — Test del check (3-4h)
- [ ] Scrivere file di test: `test/clang-tidy/checkers/misc/no-cstyle-cast.cpp`
  ```cpp
  // RUN: %check_clang_tidy %s misc-no-cstyle-cast %t
  void f() {
    int x = (int)3.14; // CHECK-MESSAGES: warning: C-style cast
    int y = static_cast<int>(3.14); // OK
  }
  ```
- [ ] Lanciare i test:
  ```bash
  ninja -C build check-clang-tools
  ```
- [ ] Iterare finché tutti passano
- [ ] Test manuale su file più complessi (es. la cartella TinyP del corso)

### Step 1.6b — Test su progetto reale con Bear (3-4h) ⭐ richiesta del prof
- [ ] Installare Bear: `brew install bear`
- [ ] Scegliere un progetto open source C/C++ medio (es. una libreria header-only o un tool CLI: `jq`, `redis`, `zstd`, una libreria di tua scelta)
- [ ] Generare il compile DB:
  ```bash
  cd path/to/project
  bear -- make    # oppure: bear -- cmake --build build
  # produce compile_commands.json
  ```
- [ ] Lanciare il check sull'intero progetto:
  ```bash
  ~/llvm-project/build/bin/clang-tidy -p . -checks='-*,misc-no-cstyle-cast' src/*.c
  # oppure run-clang-tidy per parallelismo
  run-clang-tidy -p . -checks='-*,misc-no-cstyle-cast'
  ```
- [ ] Annotare: quanti hit, quanti falsi positivi/negativi, tempo di esecuzione
- [ ] Salvare 2-3 hit "interessanti" da mostrare in demo

### Step 1.6c — Confronto con IA su esempi piccoli (2-3h) ⭐ richiesta del prof
- [ ] Preparare 3-4 snippet brevi (5-15 righe) con il pattern target
- [ ] Per ognuno: lanciare il check e registrare output (warning + fix-it)
- [ ] Stesso snippet → chiedere a un LLM (Claude/ChatGPT) "trova problemi in questo codice"
- [ ] Tabella di confronto: precisione, determinismo, fix-it applicabile, scalabilità, costo per file
- [ ] Identificare 1 caso dove clang-tidy vince chiaramente e 1 dove l'IA può completare

### Step 1.7 — Documentazione check (1-2h)
- [ ] Scrivere il file `.rst` di doc:
  ```bash
  # docs/clang-tidy/checks/misc/no-cstyle-cast.rst
  ```
- [ ] Aggiungerlo all'indice `docs/clang-tidy/checks/list.rst`

### Deliverable Fase 1
✅ Eseguibile `clang-tidy` con il tuo check funzionante
✅ Set di test che passano
✅ 2-3 file `.cc` di esempio (con bug e senza) per la demo
✅ `compile_commands.json` generato con Bear su progetto reale + log di esecuzione
✅ Tabella di confronto clang-tidy vs LLM su snippet piccoli
✅ Repository git locale con il tuo lavoro

---

## 🎤 FASE 2 — Preparazione presentazione (~10-12h)

### Obiettivo finale
Slide pronte (≤ 12-13) + demo provata almeno 3 volte + script verbale di esposizione.

### Step 2.1 — Outline (1h)
- [ ] Scrivi su carta/markdown la **struttura narrativa** delle 12 slide
- [ ] Identifica i **3 momenti chiave**: hook iniziale, demo, take-away finale
- [ ] Tempi target: 1 min hook + 9 min contenuto/demo + 4 min Q&A reale = 15 min

### Step 2.2 — Prima bozza slide (3-4h)
Tool consigliati: **Keynote**, **Google Slides**, oppure **Marp** (markdown → slide).

Slide proposte:
1. **Cover** (titolo, nome, data)
2. **Il problema** — una review pattern ripetitiva (1 fotina di un PR su GitHub)
3. **Architettura clang-tidy** — diagramma frontend Clang → AST → matchers → check
4. **Quick recap dell'AST di Clang** — `clang -ast-dump` su 5 righe di codice
5. **AST Matchers DSL** — esempi: `cxxMethodDecl(isVirtual())`, `callExpr(...)`
6. **Architettura di un check** — header + cpp scheletro
7. **Il nostro check (intro)** — cosa segnaliamo, perché
8. **registerMatchers** — codice sullo schermo
9. **check() + diagnostic + fix-it** — codice
10. **Demo live (parte 1)** — il check su esempi piccoli + confronto con LLM sullo stesso input
11. **Demo live (parte 2)** — Bear + run su progetto reale (compile_commands.json)
12. **Test, CI, take-away** — quando vale la pena scrivere check custom vs affidarsi a IA + risorse

### Step 2.3 — Codice sulle slide (1-2h)
- [ ] Snippet di codice **non più di 15 righe per slide**
- [ ] Usa **font monospace ben leggibile** (Fira Code, JetBrains Mono)
- [ ] **Evidenzia con colore** le righe importanti
- [ ] Sopra ogni snippet: 1 riga di contesto verbale

### Step 2.4 — Demo live (2-3h, INCLUDI un piano B)
- [ ] Prepara un **terminale dedicato** con prompt corto e font grande
- [ ] **File di esempio già aperti** in editor (VS Code zoomato)
- [ ] **Comandi storici già in history** per non doverli digitare
  ```bash
  # bash_history pre-popolato con:
  ./build/bin/clang-tidy -checks='-*,misc-no-cstyle-cast' good.cc
  ./build/bin/clang-tidy -checks='-*,misc-no-cstyle-cast' bad.cc
  ./build/bin/clang-tidy -checks='-*,misc-no-cstyle-cast' --fix bad.cc
  ```
- [ ] **Piano B**: registra un video da 3 min della demo, da usare se la live fallisce
- [ ] Prova la demo **almeno 5 volte** in condizioni diverse

### Step 2.5 — Script verbale (2h)
- [ ] Scrivi su carta cosa dirai per ogni slide (1-2 frasi chiave)
- [ ] Identifica **transizioni** tra slide (parole-ponte: "Detto questo...", "Vediamo ora...")
- [ ] **Frase di apertura forte**: 1 hook in 15 secondi che catturi
- [ ] **Frase di chiusura forte**: 1 take-away in 15 secondi

### Step 2.6 — Prove cronometrate (2-3h)
- [ ] Prova solo davanti allo specchio: 1 volta (target 13-14 min, lascia margine)
- [ ] Prova davanti a un amico/familiare: 2 volte
- [ ] Prova registrandoti video: 1 volta, riguardalo onestamente
- [ ] Aggiusta in base a dove rallenti, perdi tempo, balbetti

### Deliverable Fase 2
✅ File slide finalizzato (`.key`/`.pdf`)
✅ Script verbale (anche solo bullet)
✅ Demo provata e cronometrata
✅ Video di backup demo

---

## 📚 FASE 3 — Preparazione orale teoria (~15-20h)

L'orale **vale 70%** e copre **tutto il programma**, NON solo il tuo seminario. Il seminario ti sblocca l'accesso e fa fino a 30%; l'orale è dove fai il voto.

### Step 3.1 — Studio sistematico (10-12h)
Segui il `study_method/STUDY_PLAN.md` Step 1, in ordine:

- [ ] **Lezione 1-3** (Intro, Interpreti vs Compilatori, Struttura): 1.5h
- [ ] **Lezione 4-5** (Analisi lessicale, Flex): 2h
- [ ] **Lezione 6** (Analisi dipendente da contesto, TinyP): 2h
- [ ] **Lezione 7** (IR, LLVM, procedure abstraction, code shape): 2-3h
- [ ] **Lezione 8** (Middle end, dataflow, ottimizzazioni): 2-3h
- [ ] **Lezione 9** (Interpretazione astratta): 1.5h

Per ogni lezione:
1. Leggi il `RIASSUNTO.md` della cartella
2. Apri il PDF del docente
3. Vai ai "Punti chiave per l'orale"
4. Risp.ondi a voce, da solo, le domande che ci sono lì

### Step 3.2 — Memorizzazione cheatsheet (3h)
- [ ] Stampa `notes/02_cheatsheet.md`
- [ ] Memorizza:
  - Equazioni dataflow (Live, Reaching, Available)
  - Algoritmo LVN
  - Algoritmo Hopcroft (worklist)
  - Tabella domini astratti
  - Formula GEP (LLVM)

### Step 3.3 — Domande tipo (2-3h)
- [ ] Aprire `notes/04_domande_tipo.md`
- [ ] Per ognuna delle 44 domande, prova a rispondere **a voce, cronometrando** (target: 2-3 min ogni risposta)
- [ ] Per quelle su cui esiti, torna al `RIASSUNTO.md` o al PDF
- [ ] Riprova a rispondere il giorno dopo

### Step 3.4 — Domande "incrocio" (2-3h)
Il prof può fare domande che incrociano più aree. Allenati su queste:
- "Spiegami il pipeline completo di compilazione di un programma C++ con virtual methods"
  → Dovrai parlare di lexer + parser + sema + AST + IR + middle end + back end + vtable
- "Come si propaga una costante in un programma con loop?"
  → Constant propagation + dataflow + (eventualmente) abstract interpretation
- "Cosa cambia tra il middle end di Clang e una analisi di interpretazione astratta?"
  → DFA con lattice finito vs lattice infinito + widening

### Step 3.5 — Ripasso ad alta intensità (2h, ultimo giorno)
- [ ] Leggi solo i "Punti chiave per l'orale" di tutti i 9 RIASSUNTO
- [ ] Leggi solo il glossario `notes/03_glossario.md` per fissare i termini
- [ ] **Non studiare cose nuove** la sera prima

### Deliverable Fase 3
✅ Studio completo dei 9 capitoli
✅ Tutte le 44 domande tipo affrontate almeno una volta
✅ Cheatsheet memorizzato

---

## 📅 Timeline residua (post-incontro)

| Settimana | Attività |
|---|---|
| **Sett. corrente** | Setup LLVM + Bear (Fase 1.1-1.2) |
| **+1** | Sviluppa il check (Fase 1.3-1.6) |
| **+2** | Test su progetto reale con Bear + confronto vs LLM (Step 1.6b/c) + doc (1.7) |
| **+3** | Bozza slide + demo (Fase 2.1-2.4) |
| **+4** | Script + prove cronometrate (Fase 2.5-2.6) |
| **+5** | **SEMINARIO** + inizio studio orale (Fase 3.1) |
| **+6** | Studio orale intensivo (Fase 3.2-3.4) |
| **Giorno -1** | Ripasso leggero (Fase 3.5) |
| **Giorno 0** | **ORALE** |

⚠️ Buffer: almeno 1 settimana ovunque. La build di LLVM è il rischio più alto (10-15 GB, 30-60 min su M-series).
