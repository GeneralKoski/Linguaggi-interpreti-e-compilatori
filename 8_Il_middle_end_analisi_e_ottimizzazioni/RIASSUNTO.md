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

**Equazioni** (forma standard con set difference `\`):
```
LiveOut(nf) = ∅       (exit node)
LiveOut(n) = ⋃_{m ∈ succ(n)} ( UEVar(m) ∪ (LiveOut(m) \ VarKill(m)) )
```
- `UEVar(n)` = nomi usati prima di essere definiti in `n` (Upward-Exposed)
- `VarKill(n)` = nomi definiti in `n`
- direzione: **backward** (LiveOut dipende dai successori)
- meet operator: **∪** (un nome è live se lo è in *qualche* successore)

**Risolutore (worklist o round-robin):**
```
inizializza tutti i LiveOut = ∅
ripeti:
    for ogni nodo n:
        ricalcola LiveOut(n)
finché qualcosa cambia
```
La funzione di trasferimento è **monotona** su lattice finito (powerset) → terminazione garantita per Knaster-Tarski; partendo da ∅ si converge al **minimo punto fisso (lfp)**, che corrisponde al "least solution" cercato.

**Applicazioni di Live:** trovare variabili usate prima di inizializzazione (errore logico), eliminare store dead, register allocation (interferenza).

### Forward problems
- **Reaching definitions:** quali def. raggiungono un punto
- **Available expressions:** espressioni già calcolate disponibili al riuso
- **Constant propagation**

### Backward problems
- **Live variables**
- **Very busy expressions / Anticipability**

### Available Expressions in dettaglio
Un'espressione `e = x op y` è **available** in un punto `p` se su **ogni** cammino dall'entry a `p` è stata calcolata e né `x` né `y` sono stati ridefiniti dopo l'ultimo calcolo.

**Equazioni** (forward, meet = ⋂):
```
AvailIn(entry) = ∅
AvailIn(n)     = ⋂_{m ∈ pred(n)} AvailOut(m)        per n ≠ entry
AvailOut(n)    = DEExpr(n) ∪ ( AvailIn(n) \ ExprKill(n) )
```
- `DEExpr(n)` = Downward-Exposed expressions: espressioni calcolate in `n` i cui operandi non vengono ridefiniti più tardi nello stesso BB
- `ExprKill(n)` = espressioni "killate" in `n` (qualcuno dei loro operandi viene ridefinito)
- **Init del resto:** `AvailIn(n) = U` (universo di tutte le espressioni) — è cruciale: con ⋂ partire da ∅ darebbe il punto fisso banale, si vuole il **massimo** punto fisso (le espressioni che resistono su tutti i cammini)

**Esempio piccolo (4 BB, diamond):**
```
B1: a = x + y          DEExpr={x+y}     ExprKill=∅
    ↓
B2: b = x + y          DEExpr={x+y}     ExprKill=∅
    ↓
B3: x = 1              DEExpr=∅         ExprKill={x+y, x+...}
    ↓
B4: c = x + y          DEExpr={x+y}     ExprKill=∅
```
Path lineare, dopo iterazione:
- `AvailIn(B2) = {x+y}` → in `B2` posso evitare di ricalcolare `x+y` (riuso `a`)
- `AvailIn(B3) = {x+y}` ma `B3` killa → `AvailOut(B3) = ∅`
- `AvailIn(B4) = ∅` → `x+y` in B4 va calcolato di nuovo (giusto: `x` è cambiato)

**Uso: GCSE (Global Common Subexpression Elimination).**
Algoritmo:
1. Calcola `AvailIn` per ogni BB.
2. Per ogni occorrenza di `e = x op y` nel BB `n` con `e ∈ AvailIn(n)` (o disponibile a metà BB): trova le definizioni "fonte" (su tutti i predecessori).
3. Sostituisci con un nome temporaneo `t` definito alle fonti, e usa `t` qui.

GCSE **non** cattura ridondanze parziali (su solo alcuni cammini): per quelle serve **PRE (Partial Redundancy Elimination)**, di cui Lazy Code Motion è la formulazione classica.

**Cosa potrebbe chiedere il prof:** "perché `AvailIn` si inizializza a U e non a ∅?" — perché il meet è ⋂ e si vuole il greatest fixed point; ∅ è già un punto fisso banale ma non informativo.

### Very Busy Expressions in dettaglio
Un'espressione `e` è **very busy** (anticipabile) all'uscita di `n` se su **ogni** cammino da `n` all'exit `e` viene calcolata **prima** che i suoi operandi vengano ridefiniti. Intuizione: è certo che la calcoleremo, allora tanto vale anticiparla.

**Equazioni** (backward, meet = ⋂):
```
VeryBusyOut(exit) = ∅
VeryBusyOut(n)    = ⋂_{m ∈ succ(n)} VeryBusyIn(m)
VeryBusyIn(n)     = UEExpr(n) ∪ ( VeryBusyOut(n) \ ExprKill(n) )
```
- `UEExpr(n)` = Upward-Exposed expressions: espressioni usate in `n` prima che i loro operandi siano ridefiniti dentro `n`
- Init resto: `VeryBusyOut(n) = U`

**Esempio (diamond):**
```
        B1
       /   \
     B2     B3
     y=a+b  z=a+b
       \   /
        B4
```
Sia `B2` che `B3` calcolano `a+b`. Allora `a+b ∈ VeryBusyOut(B1)`: certamente sarà calcolata in qualunque cammino.

**Uso: code hoisting.** Si solleva il calcolo di `a+b` in `B1`:
```
B1: t = a + b
B2: y = t        (sostituita)
B3: z = t        (sostituita)
```
Vantaggi: riduzione code size (un calcolo invece di due), niente perdita di performance se entrambi i rami sono certi. **Attenzione:** NON è una "speculation" — la very busy garantisce che il calcolo sarebbe avvenuto comunque. Per espressioni potenzialmente faulty (divisione, deref) bisogna ulteriore analisi (anticipability + safety).

**Cosa potrebbe chiedere il prof:** "differenza tra hoisting via very busy e loop-invariant code motion?" — LICM solleva fuori da un loop un calcolo invariante (anche se in teoria potrebbe non eseguirsi mai); very busy hoisting solleva tra blocchi su un join, garantendo che il calcolo era inevitabile.

### Induction Variables e Strength Reduction
Una **induction variable** (IV) è una variabile che in ogni iterazione di un loop viene incrementata di una costante. Forme:
- **Basic IV:** `i = i + c` (con `c` costante invariante nel loop)
- **Derived IV:** `j = a*i + b` con `a, b` invarianti — funzione affine di una basic IV

**Identificazione:** sul CFG ridotto al loop body, una variabile è basic IV se la sua sola definizione nel loop è del tipo `i := i ± c`. Si itera per scoprire le derived: `j := a*i + b` o `j := j ± c'` dove c' è invariante.

**Strength reduction:** sostituisci operazioni costose con equivalenti più economiche sfruttando la regolarità della IV. Esempio canonico:
```c
for (i = 0; i < n; i++) a[i] = 0;
```
Naïve: ogni iterazione calcola `&a[i] = &a + i*4` → moltiplicazione per ogni iterazione. Dopo strength reduction:
```c
p = &a;
for (i = 0; i < n; i++) { *p = 0; p += 4; }
```
La moltiplicazione è sostituita da un'addizione (più una init). Combinato con **IV elimination** si può anche eliminare `i` se non serve a nulla, usando solo `p` e un `p_end`:
```c
for (p = &a, end = &a + 4*n; p < end; p += 4) *p = 0;
```

**Cosa potrebbe chiedere il prof:** "perché strength reduction è importante anche su CPU moderne dove la moltiplicazione è veloce?" — perché abilita **vectorization** e **address mode folding**, e perché su loop tight ogni ciclo conta. Inoltre su array indexing è il prerequisito per riconoscere stride patterns.

### Dead Code Elimination (DCE)
**Idea:** un'istruzione che definisce una variabile `v` e non ha side-effect è inutile se `v` non è live dopo l'istruzione. Usa l'analisi di **liveness** (backward, già descritta).

**Algoritmo iterativo:**
```
ripeti:
    calcola LiveOut per ogni BB
    per ogni istruzione I = "v := expr" senza side-effect:
        se v ∉ Live dopo I:  rimuovi I
finché nessuna rimozione
```
Iterativo perché rimuovere `v := w + 1` può rendere dead un precedente `w := ...` (se `w` non è più usato). Le istruzioni con side-effect (store, call a funzione potenzialmente impura, I/O, volatile) **non** si rimuovono mai per DCE puro.

**Versione SSA-based aggressive (ADCE, Aggressive Dead Code Elimination):** parte assumendo morto tutto e marca live solo ciò che è raggiungibile a ritroso da istruzioni "essenziali" (return, store, call con side-effect, branch). Ribalta il default ed elimina anche cicli di codice morto auto-sostenuto (es. `i := i+1` non usato).

**Cosa potrebbe chiedere il prof:** "perché DCE va iterato?" — perché è **monotono ma non one-shot**: ogni rimozione riduce il live set e può rendere dead altre istruzioni. Convergenza garantita: il numero di istruzioni decresce strettamente, è finito.

### Pass LLVM standard (cosa fanno)
- **mem2reg** (`-mem2reg`, oggi parte di `-sroa`): promuove le `alloca` di scalari (locali con indirizzo non preso) a registri SSA, inserendo le `φ`. Frontend Clang emette tutte le locali come alloca/load/store; mem2reg trasforma in IR SSA "vera". È il **gateway pass:** quasi tutte le ottimizzazioni successive lavorano molto meglio dopo mem2reg.
- **SROA (Scalar Replacement of Aggregates):** spezza `alloca` di struct/array in tante alloca di campi/elementi separati, poi promuove ognuno a registro (chiama mem2reg internamente). Esempio: `struct Point { int x, y; }` locale → due variabili scalari `x`, `y` indipendenti, i `getelementptr` spariscono.
- **instcombine:** **peephole** sull'IR. Centinaia di pattern di riscrittura locali: `x + 0 → x`, `x * 2 → x << 1`, fusione di `zext`+`shl`, semplificazione di compare, canonicalizzazione (es. costante a destra). È il pass più "tattico" di LLVM e gira spesso (alternato con simplifycfg).
- **LICM (Loop-Invariant Code Motion):** identifica istruzioni dentro un loop i cui operandi sono invarianti (definiti fuori dal loop o tutti loop-invariant) e le solleva nel **preheader**. Richiede che l'istruzione sia safe-to-speculate (no side-effect, o dominanza dell'exit). Combinato con la promozione di load/store invarianti su memoria che non aliasa (loop-invariant memory motion).

**Cosa potrebbe chiedere il prof:** "perché LLVM fa generare al frontend tutte le locali come alloca anziché direttamente in SSA?" — perché generare SSA da un AST richiede dominator + φ-placement; è molto più semplice emettere alloca/load/store e lasciare a mem2reg il lavoro (separazione di concerns frontend/middle-end).

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
