# 04 — Domande incrocio

Stima: **2-3h**. Da fare dopo i blocchi 01-03.

Sono domande che attraversano più capitoli. All'orale sono quelle che alzano il voto: dimostrano di aver capito la **pipeline come un tutto unico**, non i pezzi isolati.

Per ognuna: prepara una risposta a voce di **5-7 minuti**, cronometrandoti. Usa carta/lavagna immaginaria per disegnare.

---

## D1 — Pipeline completa di compilazione di un C++ con virtual methods

> *"Mi spieghi cosa succede da `g++ foo.cpp` fino al binario, prendendo come esempio una classe con un metodo virtuale?"*

Cosa toccare (in ordine):
1. **Front end**: lexer (Flex-style, RE→DFA), parser (CFG, LL/LR, parse tree)
2. **Analisi semantica**: name resolution, type checking, costruzione **vtable** (compile-time) + slot del puntatore vtable nell'oggetto
3. **AST → IR** (LLVM): code shape per chiamata virtuale → load del vptr, GEP nello slot del metodo, indirect call (`call` su funzione caricata)
4. **Middle end**: passes su IR (LVN locale, dataflow globale, devirtualization se possibile, inlining)
5. **Back end**: instruction selection, register allocation, calling convention, activation record
6. **Run-time**: vtable in memoria read-only, vptr nell'oggetto, dynamic dispatch via doppia indirezione

Punti che il prof ama sentire:
- Distinzione **compile-time** (vtable layout, slot index) vs **run-time** (vptr load + indirect call)
- Perché il compilatore in genere **non può devirtualizzare** senza analisi inter-procedurale
- Cost model: 2 load + 1 indirect call vs static call

---

## D2 — Constant propagation con loop: dataflow vs interpretazione astratta

> *"Come si propaga una costante attraverso un loop? Lo si fa a livello di dataflow classico o serve interpretazione astratta?"*

Cosa toccare:
1. **Constant propagation come dataflow**: lattice `⊥ → costanti → ⊤` (3 livelli + costanti), forward, meet = `⊓` (se discordi → `⊤`)
2. **Problema con i loop**: dipendenza ciclica nel CFG → serve **iterazione fino al punto fisso**
3. **Esistenza del punto fisso**: lattice finito + funzione monotona → Knaster-Tarski garantisce minimo punto fisso, Kleene dà l'algoritmo (iterazione da `⊥`)
4. **Quando il dataflow non basta**: lattice **infinito** (es. intervalli) → l'iterazione può non terminare → serve **widening** (interpretazione astratta)
5. **Esempio concreto**: `for (i=0; i<10; i++) x = 5;` — `x` è costante 5 anche dopo il loop; `i` non è costante (dataflow → `⊤`), ma con dominio Intervals → `[0,10]`

Punti chiave:
- Constant prop è **già interpretazione astratta** in piccolo (dominio finito → no widening)
- Galois connection: concretizzazione γ + astrazione α; soundness = "ciò che dimostriamo astratto vale concreto"
- Distinzione **may** (esiste un'esecuzione) vs **must** (tutte le esecuzioni)

---

## D3 — Middle end di Clang vs interpretazione astratta

> *"Cosa fa il middle end di LLVM/Clang ha qualche legame con l'interpretazione astratta?"*

Cosa toccare:
1. **Middle end LLVM**: passes su IR SSA — mem2reg, SROA, instcombine, GVN, LICM, loop unroll, inline, dead code elimination
2. **Quasi tutti i passes sono basati su dataflow**: Available Expressions (GVN/CSE), Live Variables (DCE), Reaching Defs, Very Busy (hoisting)
3. **Dataflow = caso particolare di interpretazione astratta** su lattice finito con funzioni monotone
4. **LLVM non usa interpretazione astratta "vera"** (con widening su domini infiniti) nei suoi passes principali: i suoi domini sono finiti per design (insiemi di variabili, espressioni, definizioni)
5. **Strumenti di analisi statica veri** (es. Clang Static Analyzer, Frama-C, Astrée) usano interpretazione astratta su domini ricchi (Intervals, Octagons, Polyhedra) con widening
6. Differenza di obiettivo: middle end **ottimizza** (deve essere veloce e sound), AI **dimostra proprietà** (può essere lenta e precisa)

Punti chiave:
- Available Expressions = forward/must (∩) → caso elementare di AI
- Soundness garantita dalla monotonia + lattice finito (Kleene termina)
- Perché l'industria sta integrando AI nei compilatori (es. Rust borrow checker, Polly)

---

## Come usarle in pratica

1. Prima lettura: leggi questa scaletta e prova a esporre a voce. Se inciampi, torna al RIASSUNTO del capitolo specifico.
2. Seconda passata (giorno dopo): cronometro 5 min, niente note davanti.
3. Per ognuna prepara **un esempio concreto** (foglio o lavagna) da spiegare mentre parli — mostra che sai applicarla.
