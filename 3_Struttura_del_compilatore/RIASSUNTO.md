# 3. Struttura del compilatore

## Visuale ad alto livello
```
codice L → [compilatore] → codice M     +  errori/warning
```
Requisiti: riconoscere programmi validi/invalidi, generare codice corretto, gestire memoria, interagire con OS (linker dinamico).

## Compilatore a due passi (front end + back end)
```
codice L → [front end] → IR → [back end] → codice M
```
- **Front end:** dipende dal linguaggio sorgente L
- **Back end:** dipende dalla macchina target M
- Vantaggio: si possono combinare più front end con più back end (riuso)

## Compilatore a tre passi
```
L → [front end] → IR → [middle end] → IR → [back end] → M
```
Il middle end fa **analisi e ottimizzazioni indipendenti dalla macchina**.

## La rappresentazione intermedia (IR)
Tipologie:
- **Strutturali:** alberi, grafi, DAG (AST)
- **Lineari:** 3-address code, stack-machine code
- **Ibride:** CFG (control flow graph) di basic block
Esempi reali: GCC → RTL, LLVM → LLVM bitcode.

---

## Front end: decomposizione
```
codice L → [Lexer] → tokens → [Parser] → IR → [Checker] → IR
```

### Lexer (analisi lessicale)
- Input: caratteri → Output: sequenza di token `⟨part-of-speech, lexeme⟩`
- **Specifica:** RE (espressioni regolari)
- **Implementazione:** DFSA (automa a stati finiti deterministico), spesso generato automaticamente

### Parser (analisi sintattica)
- Input: token → Output: IR strutturale (parse tree o, più comunemente, AST)
- **Specifica:** CFG (grammatica libera dal contesto), `G = ⟨S, N, T, P⟩`
- **Implementazione:** PDA non deterministico, codificato a mano (recursive descent + backtracking) o generato (LL, LR, ...)

### Checker (analisi di semantica statica / context-sensitive)
- Aggiunge informazione dipendente dal contesto: tipi, conversioni implicite, risoluzione overloading, scope
- **Specifica:** linguaggio naturale (standard) o regole formali
- **Implementazione:** SDT (syntax directed translation) o algoritmi di visita ad-hoc
- Spesso fuso col parser → costruzione diretta dell'AST arricchito

Esempio mostrato: dump dell'AST di un piccolo programma C/C++ via `clang -Xclang -ast-dump`. Lo stesso codice produce alberi differenti per C vs C++ (es. il `==` ritorna `int` in C, `bool` in C++ con cast implicito).

---

## Back end: decomposizione
```
IR → [Instruction selection] → [Register allocation] → [Instruction scheduling] → codice M
```
Tutti problemi NP-completi → si usano euristiche.

- **Instruction selection:** scegliere quali istruzioni macchina implementano le operazioni IR (pattern matching su tree/DAG)
- **Register allocation:** mappare un numero infinito di valori virtuali sui registri fisici → **graph coloring**, con **spilling** (LOAD/STORE in memoria)
- **Instruction scheduling:** riordinare istruzioni per riempire le pipeline e usare le unità funzionali parallele; può modificare il "live range" dei valori e quindi influenzare l'allocazione

Esempio mostrato: `a ← b·c+d; e ← f+a` schedulato su due unità funzionali con load/store latency 2 → riduzione drastica dei NOP.

---

## Middle end
- Suddiviso in **passi** (analisi + trasformazioni), spesso ripetuti
- Ogni passo deve preservare la semantica
- Esempi di passi:
  - **Analisi:** identificazione costanti, codice morto, aliasing
  - **Trasformazioni:** propagazione di costanti, dead-code elimination, function inlining, loop unrolling

## Altri benefici della struttura modulare
- Front end riutilizzabili per implementare interpreti
- JIT: scelta dei passi del middle end a run-time
- Strumenti come `clang-tidy` riusano il front end
- Integrazione con IDE/editor/debugger

## Punti chiave per l'orale
- Saper disegnare i diagrammi front/middle/back end
- Sapere distinguere analisi **lessicale**, **sintattica**, **semantica statica** in termini di formalismo (RE/CFG/contesto), riconoscitore (DFSA/PDA/algoritmi ad-hoc), input e output
- Conoscere problemi NPC del back end e relative euristiche
