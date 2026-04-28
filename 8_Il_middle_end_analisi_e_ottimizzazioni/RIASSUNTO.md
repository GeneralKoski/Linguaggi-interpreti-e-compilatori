# 8. Middle end: analisi e ottimizzazioni

## Materiali
- Slide del docente / EaC moderne (`27`, `29`, `30`)
- Slide EaC versione 1up: dominators (`08`), global optimization (`09`), DFA (`13`), proliferation (`14`), clean (`19`)

---

## Concetti generali

**Optimization** = analisi della IR + riscrittura della IR per migliorare una metrica (tempo, spazio, energia), **preservando la semantica** (definita dai valori delle variabili named).

> "Nothing optimal about optimization" — i risultati ottimi richiedono condizioni irrealistiche; le tecniche reali sono **euristiche**.

### Trasformazioni tipiche
- Costanti: scopri propaga (constant propagation, constant folding)
- Spostare un calcolo dove eseguito meno volte (code motion, hoisting fuori dai loop)
- Specializzare per contesto
- Eliminazione ridondanze
- Dead code / unreachable code elimination
- Riconoscere idiomi e tradurli efficientemente (peephole)
- Loop unrolling, inlining

### Scope di ottimizzazione
| Scope | Cosa |
|---|---|
| **Local** | un solo basic block |
| **Regional** | sotto-grafo del CFG: EBB, loops, dominator regions |
| **Global** (intraprocedural) | intera procedura, gestisce cicli |
| **Whole-program** (interprocedural) | multipli procedimenti, call graph |

---

## Local Value Numbering (LVN) — slide 27
Tecnica chiave per individuare ed eliminare ridondanze in un singolo basic block.

**Idea:** assegna un value number `V(n)` a ogni espressione tale che `V(x+y) = V(j)` ⇔ `x+y` e `j` hanno sempre lo stesso valore. Si usa un'**hash table** `<op, V(o1), V(o2)> → VN`.

**Algoritmo:**
```
per ogni operazione o = <op, o1, o2> nel BB:
    vn1 = lookup(o1); vn2 = lookup(o2)
    h = hash(<op, vn1, vn2>)
    se h già presente: sostituisci o con un riferimento
    altrimenti registra <h → VN nuovo>
    se o1 e o2 sono costanti: pre-calcola e sostituisci con loadI
```
Lineare nel numero di operazioni (con buon hashing).

**Estensioni:** identità algebriche per operatore (case statement); commutatività; constant folding.

---

## Superlocal VN + Loop Unrolling — slide 29 (regional)

### Extended Basic Block (EBB)
Insieme massimale di blocchi `B1, …, Bn` dove ogni `Bi` (eccetto `B1`) ha **un solo predecessore** e quel predecessore è in EBB. Forma un albero con root `B1`.

### Superlocal VN
Applica LVN ai cammini dell'EBB usando una **scoped hash table** (push/pop tra blocchi). Riconosce ridondanze tra blocchi dell'EBB. Limite: non funziona ai punti di confluenza (più predecessori, es. uscita di if/else o loop).

**Forma SSA** (Static Single Assignment) come supporto: ogni nome ha un'unica definizione, le funzioni `φ` ai merge.

### Loop Unrolling
Replica il body del loop k volte, riducendo overhead (test, branch, increment). Espone più ILP. Diversi modi: cleanup loop per resto, knot unrolling, software pipelining.

---

## Dominator Trees — slide 08 EaC

Definizioni:
- `d` **dominates** `n` (`d dom n`) se ogni cammino dall'entry a `n` passa per `d`. Per def. ogni nodo si domina.
- `d` **strictly dominates** `n` (`d sdom n`) se `d dom n` e `d ≠ n`.
- **Immediate dominator** `idom(n)`: il dominator più vicino a `n`. Esiste unico per ogni nodo (eccetto entry).
- **Dominator tree:** padre di `n` = `idom(n)`. Compatto e utile.

**Algoritmo classico (data-flow):**
```
DOM(n0) = {n0}
DOM(n) = {n} ∪ ∩_{p ∈ pred(n)} DOM(p)
```
Iterativo fino a punto fisso. Esistono algoritmi più efficienti (Lengauer-Tarjan, O(E·α(V)) ).

**Dominator-based VN:** estende SVN ai blocchi dominati invece che agli EBB.

---

## Global Data-Flow Analysis — slide 30, 09, 13

### Data-flow analysis
Forma di **ragionamento a compile-time sui valori a run-time**. Si formula come **sistema di equazioni** su insiemi associati ai nodi del CFG, risolto con un **fixed-point iterativo**.

### Live Variables (esempio canonico, problema backward)
`v` è **live** in `p` se esiste un cammino da `p` a un uso di `v` lungo cui `v` non viene ridefinita.

**Equazioni:**
```
LiveOut(nf) = ∅       (exit node)
LiveOut(n) = ∪_{m ∈ succ(n)} ( UEVar(m) ∪ (LiveOut(m) ∩ ¬VarKill(m)) )
```
- `UEVar(n)` = nomi usati prima di essere definiti in `n` (Upward-Exposed)
- `VarKill(n)` = nomi definiti in `n`

**Risolutore:**
```
inizializza tutti i LiveOut = ∅
ripeti:
    for ogni nodo n:
        ricalcola LiveOut(n)
finché qualcosa cambia
```
Le equazioni sono **monotone crescenti** su insieme finito → terminazione garantita; soluzione di **massimo punto fisso** unica.

**Applicazioni di Live:** trovare variabili usate prima di inizializzazione (errore logico), eliminare store dead, register allocation (interferenza).

### Forward problems
- **Reaching definitions:** quali def. raggiungono un punto
- **Available expressions:** espressioni già calcolate disponibili al riuso
- **Constant propagation**

### Backward problems
- **Live variables**
- **Very busy expressions / Anticipability**

### Velocità di convergenza
Dipende dall'ordine di visita: per backward problems conviene ordine **post-order del reverse CFG** (post-order del CFG visitato al rovescio).

---

## Proliferation & Clean (slide 14, 19)

- **Proliferation:** dopo molte ottimizzazioni il codice tende ad accumulare nodi inutili (jump-to-jump, BB vuoti, codice morto residuo). Servono passi di pulizia.
- **Clean / CFG simplification (Cooper-Harvey-Kennedy):** algoritmo per pulire il CFG eliminando:
  1. Branch a un BB con un solo successore → rimpiazza con il successore
  2. Empty block → bypassa
  3. Branch ridondante (entrambi i target uguali) → jump non condizionale
  4. Combina blocchi consecutivi con un solo predecessore/successore
- Iterare finché stabile (è anch'esso un punto fisso).

---

## Punti chiave per l'orale
- Saper distinguere local/regional/global/whole-program
- Spiegare l'algoritmo di **LVN** e come si estende a **superlocal VN** (con scoped hash) e a **dominator-based VN**
- Definire **dominator** e saper costruire un dominator tree (algoritmo iterativo)
- Formulare le equazioni **LiveOut** e spiegare perché il punto fisso esiste ed è unico (monotonia + insieme finito → teorema di Knaster–Tarski/Kleene)
- Forward vs backward DFA
- Spiegare il ruolo della **SSA form** nelle ottimizzazioni moderne
- Conoscere alcuni passi di pulizia (CFG simplification)
