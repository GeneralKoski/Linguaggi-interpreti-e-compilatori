# Piano di lavoro — LLVM IR tour

Tre fasi: **progetto tecnico** → **presentazione** → **preparazione orale**.

---

## 🛠 FASE 1 — Costruzione del progetto (~12-15h)

### Obiettivo finale
Avere un set di esempi di codice C/C++ → LLVM IR → IR ottimizzato che illustrino tutti i punti chiave della presentazione, ciascuno provato e funzionante.

### Step 1.1 — Setup ambiente (30 min)
- [ ] Verifica Clang installato:
  ```bash
  clang --version  # dovresti avere già 14+
  opt --version    # se manca:
  brew install llvm
  echo 'export PATH="/opt/homebrew/opt/llvm/bin:$PATH"' >> ~/.zshrc
  ```
- [ ] Crea cartella di lavoro
  ```bash
  mkdir -p ~/llvm-tour-demo
  cd ~/llvm-tour-demo
  ```

### Step 1.2 — Studiare LLVM IR (4-5h)
- [ ] Leggi `7_Rappresentazione_intermedia_(IR)/LLVM-IR.pdf` (slide del prof)
- [ ] Leggi (skim) la **LangRef** ufficiale: https://llvm.org/docs/LangRef.html
  - Focus su: Module Structure, Type System, Constants, Instruction Reference
- [ ] Leggi il "Kaleidoscope tutorial" capitolo 3-7: https://llvm.org/docs/tutorial/
  - Anche solo skimming, ti dà familiarità con come si genera IR
- [ ] Familiarizza con i pass principali di `opt`:
  ```bash
  opt --print-passes | head -50
  ```

### Step 1.3 — Preparare gli esempi (3-4h)
Crea 5-6 mini-programmi C che illustrino punti chiave. Salvali tutti in `~/llvm-tour-demo/`:

#### Esempio 1: `simple.c` — funzione semplice
```c
int square(int x) { return x * x; }
```
Mostra: struttura modulo, function definition, BB unico, ret.

#### Esempio 2: `branch.c` — if/else con phi (CHIAVE)
```c
int max(int a, int b) {
    if (a > b) return a;
    else return b;
}
```
Mostra: BB multipli, branch, **funzione phi** dopo `mem2reg`.

#### Esempio 3: `loop.c` — loop semplice
```c
int sum(int n) {
    int s = 0;
    for (int i = 0; i < n; i++) s += i;
    return s;
}
```
Mostra: header, body, latch del loop. Dopo `-O2`: induction var optimization.

#### Esempio 4: `array.c` — GEP
```c
int access(int* arr, int i) { return arr[i]; }
struct Point { int x, y; };
int getx(struct Point* p) { return p->x; }
```
Mostra: GEP per array e per struct.

#### Esempio 5: `licm.c` — invariant code motion
```c
void scale(int* a, int n, int x, int y) {
    for (int i = 0; i < n; i++) a[i] = (x*y) + i;
}
```
Mostra: prima/dopo `-licm` il `mul x*y` esce dal loop.

#### Esempio 6: `mem2reg.c` — alloca → SSA
```c
int compute(int x) {
    int t = x * 2;
    t = t + 5;
    return t;
}
```
Mostra: prima alloca/load/store ovunque, dopo `mem2reg` solo registri SSA.

### Step 1.4 — Generare gli IR e fare i diff (2-3h)
Per ogni esempio:
- [ ] Genera l'IR base:
  ```bash
  clang -O0 -S -emit-llvm -fno-discard-value-names simple.c -o simple.ll
  ```
- [ ] Genera versione con `mem2reg` solo:
  ```bash
  opt -passes=mem2reg simple.ll -S -o simple.mem.ll
  ```
- [ ] Genera versione `-O2`:
  ```bash
  clang -O2 -S -emit-llvm -fno-discard-value-names simple.c -o simple.opt.ll
  ```
- [ ] Salva i diff fra le versioni:
  ```bash
  diff simple.ll simple.mem.ll > simple.diff1.txt
  diff simple.mem.ll simple.opt.ll > simple.diff2.txt
  ```

### Step 1.5 — Annotare manualmente l'IR (3-4h)
Per ogni esempio chiave, scrivi un file `.md` con l'IR commentato riga per riga. Esempio:

```llvm
; ModuleID = 'simple.c'
target datalayout = "e-m:o-..."  ; <-- little endian, ELF mangling
target triple = "arm64-apple-..."

define i32 @square(i32 %x) {     ; <-- function: return i32, takes i32 %x
entry:
  %0 = mul nsw i32 %x, %x        ; <-- nsw = no signed wrap (UB se overflow)
  ret i32 %0                      ; <-- return value
}
```

Questo ti **costringe** a capire ogni token, e ti darà materiale pronto per le slide.

### Deliverable Fase 1
✅ 6 file `.c` di esempio
✅ Per ognuno, `.ll` originale, `.mem.ll`, `.opt.ll`, e file diff
✅ Annotazioni Markdown spiegando ogni IR riga per riga

---

## 🎤 FASE 2 — Preparazione presentazione (~10-12h)

### Obiettivo finale
Slide pronte (~12) + demo provata + script verbale.

### Step 2.1 — Outline (1h)
Struttura proposta (15 min totali):
- Hook + intro: 1 min
- Tour LLVM IR: 5 min
- Ottimizzazioni con opt: 5 min
- Wrap-up + take-away: 1 min
- Buffer Q&A nel tempo: 3 min

### Step 2.2 — Slide bozza (3-4h)
Tool: **Marp** è ideale qui (markdown + monospace native + theme bianco/nero pulito).

Proposta slide:
1. **Cover** — "LLVM IR: il cuore della compilazione moderna"
2. **Da Chris Lattner a oggi** — 1 timeline LLVM + lista linguaggi che lo usano (Rust/Swift/Julia/Zig)
3. **Pipeline Clang** — diagramma `.c → .ll → .opt.ll → .s → .o`
4. **Anatomia di un modulo IR** — `target datalayout`, function, BB, terminatori
5. **Tipi LLVM** — `iN`, `float`, ptr, array, struct
6. **Esempio 1: square()** — IR completo annotato
7. **SSA + phi** — Esempio 2 (max), prima e dopo `mem2reg`
8. **Memoria: alloca/load/store + GEP** — Esempio 4
9. **🔧 DEMO LIVE** — 3-4 min al terminale
10. **opt: una pipeline modulare** — `mem2reg`, `instcombine`, `gvn`, `licm`
11. **LICM in azione** — diff prima/dopo Esempio 5
12. **Take-away** — perché LLVM ha vinto + risorse

### Step 2.3 — Snippet di codice nelle slide (1-2h)
- [ ] Mai più di **15 righe di IR per slide**
- [ ] Font: **Fira Code** o **JetBrains Mono**
- [ ] **Highlight a colori** delle parti chiave
- [ ] Sempre un **commento verbale** sopra: "Notate il phi qui:..."

### Step 2.4 — Demo live (2-3h)
Setup:
- [ ] Terminale dedicato con prompt minimal e font grande (24+pt)
- [ ] VS Code con i file aperti, font 18+pt
- [ ] **History pre-popolata** con i comandi:
  ```bash
  # ~/.zsh_history pulita o uso di un alias temporaneo
  alias d1="clang -O0 -S -emit-llvm fact.c -o fact.ll && cat fact.ll"
  alias d2="opt -passes=mem2reg fact.ll -S | tee fact.mem.ll"
  alias d3="opt -O2 fact.ll -S | tee fact.opt.ll"
  alias diff_1_2="diff -u fact.ll fact.mem.ll | colordiff"
  ```
- [ ] **Piano B**: video registrato della demo (3 min)

Demo step-by-step:
1. Apri `branch.c` (max function): "ecco il sorgente"
2. `clang -O0 -S -emit-llvm` → IR pieno di alloca/load/store
3. `opt -passes=mem2reg` → guarda, è apparsa la **phi**
4. `opt -O2` → tutto collassato in `select` o branch ottimizzato
5. Apri `licm.c`: "ecco un loop con calcolo invariante"
6. `clang -O0` poi `opt -passes=mem2reg,licm` → vedi che `mul x*y` è uscito dal loop

### Step 2.5 — Script + frasi-chiave (1-2h)
- [ ] Frase di apertura: "Quando scrivete `clang -O2 file.c`, in mezzo c'è un linguaggio intermedio che decide la qualità del vostro binario. Si chiama LLVM IR."
- [ ] Frase di transizione alla demo: "Smettiamo di parlare di IR e guardiamolo davvero."
- [ ] Frase finale: "L'IR non è un dettaglio implementativo: è il vero contratto tra il vostro codice e il backend che genera l'assembly."

### Step 2.6 — Prove cronometrate (2h)
- [ ] Prova 1: davanti allo specchio
- [ ] Prova 2: davanti a un amico (chiedi domande random)
- [ ] Prova 3: registrandoti
- [ ] Tempo target: 13-14 min (lascia 1-2 min buffer)

### Deliverable Fase 2
✅ Slide finali (PDF)
✅ Demo cronometrata e provata 3+ volte
✅ Video backup
✅ Script verbale

---

## 📚 FASE 3 — Preparazione orale teoria (~15-20h)

L'orale **vale 70%** e copre **tutto il programma**, NON solo LLVM IR.

### Step 3.1 — Studio sistematico (10-12h)
Segui il `study_method/STUDY_PLAN.md` Step 1, in ordine:

- [ ] **Lezione 1-3** (Intro, Interpreti vs Compilatori, Struttura): 1.5h
- [ ] **Lezione 4-5** (Analisi lessicale, Flex): 2h
- [ ] **Lezione 6** (Analisi dipendente da contesto, TinyP): 2h
- [ ] **Lezione 7** (IR, LLVM, procedure abstraction, code shape): 2-3h ← qui sei già forte
- [ ] **Lezione 8** (Middle end, dataflow, ottimizzazioni): 2-3h
- [ ] **Lezione 9** (Interpretazione astratta): 1.5h

Per ogni lezione:
1. Leggi il `RIASSUNTO.md`
2. Apri il PDF del docente
3. Risp.ondi a voce alle "Domande chiave per l'orale"

### Step 3.2 — Memorizzazione cheatsheet (3h)
- [ ] Equazioni dataflow (Live, Reaching, Available)
- [ ] Algoritmo LVN
- [ ] Algoritmo Hopcroft (worklist)
- [ ] Domini astratti (sign, interval, congruence)
- [ ] Formula GEP (sei già forte qui dal seminario)

### Step 3.3 — Domande tipo (2-3h)
- [ ] Tutte le 44 in `notes/04_domande_tipo.md`, **a voce, cronometrate**
- [ ] Quelle su cui esiti → studia di più

### Step 3.4 — Domande "incrocio" (2-3h)
- "Spiegami pipeline completa di compilazione C++"
- "Come si propaga una costante con loop?"
- "Differenza middle end Clang vs interpretazione astratta?"
- (Sul tuo seminario il prof scaverà — preparati a dettagli su SSA, phi, mem2reg, GEP, opt pipeline)

### Step 3.5 — Ripasso finale (2h, ultimo giorno)
- [ ] Solo i "Punti chiave per l'orale" dei 9 RIASSUNTO
- [ ] Glossario
- [ ] Niente di nuovo la sera prima

### Deliverable Fase 3
✅ Studio completo 9 capitoli
✅ 44 domande tipo affrontate
✅ Cheatsheet memorizzato

---

## 📅 Timeline suggerita

| Settimana | Attività |
|---|---|
| **Sett. -6** | Mail al prof, appuntamento Teams |
| **Sett. -5** | Conferma argomento, inizia studio LLVM IR (Fase 1.2) |
| **Sett. -4** | Crea esempi e annotazioni IR (Fase 1.3-1.5) |
| **Sett. -3** | Slide + demo live (Fase 2.1-2.4) |
| **Sett. -2** | Prove + studio orale parallelo (Fase 2.5-2.6 + Fase 3.1) |
| **Sett. -1** | **SEMINARIO** + studio intensivo orale (Fase 3.2-3.4) |
| **Giorno -1** | Ripasso leggero (Fase 3.5) |
| **Giorno 0** | **ORALE** |

⚠️ Margine: 1 settimana buffer per imprevisti.

## 💡 Tips specifici per LLVM tour

- **Non perdere tempo a spiegare TUTTI i pass**: scegline 4 (mem2reg, instcombine, gvn, licm) e fai bene quelli
- **Il prof si aspetta che tu sappia spiegare phi**: prepara un esempio "minimale" con un solo if/else
- **Q&A trabocchetto:** "Cosa succede se chiami una funzione esterna in mezzo al loop?" (alias analysis blocca LICM)
- **Demo killer:** mostra lo stesso codice con `-O0` vs `-O3` e fai notare quanto si è ridotto
