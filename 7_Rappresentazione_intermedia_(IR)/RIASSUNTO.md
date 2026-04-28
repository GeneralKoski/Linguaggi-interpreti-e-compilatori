# 7. Rappresentazione Intermedia (IR) e Code Shape

## Materiali
- Slide del docente: `LLVM-IR.pdf`
- Slide EaC: `17-Intermediate_Representations.pptx`, `18-The_Procedure_Abstraction_I.pptx`, `20-Procedure_Abstraction_III.pptx`, `22-Support_for_Object_Oriented_Languages.pptx`, `23-Support_for_Inheritance_in_OOLs.pptx`, `24/25/26-Code Shape I/II/III.pptx`

---

## Cos'è una IR?
Codifica la conoscenza che il compilatore ha del programma. Sta tra front end e back end. Decisioni di design influenzano velocità ed efficienza dell'intero compilatore.

### Proprietà importanti di una IR
- Facilità di generazione
- Facilità di manipolazione
- Dimensione (per procedura)
- Espressività ("freedom of expression")
- **Livello di astrazione** (alto = AST, basso = simil-assembly)

### Tre categorie principali
| Categoria | Esempi | Caratteristiche |
|---|---|---|
| **Strutturale** | Trees, DAG | Grafica, alta astrazione, pesante (size), source-to-source |
| **Lineare** | 3-address code, stack-machine code | Pseudo-codice di una macchina astratta, compatta, facile da riordinare |
| **Ibrida** | Control-flow graph (CFG) di basic block | Lineare dentro i BB, grafica tra BB |

### Esempi notevoli
- **AST** = parse tree con nodi non-terminali ridondanti rimossi. Linearizzabile in prefisso/postfisso. S-expressions di Lisp/Scheme.
- **DAG** = AST con un singolo nodo per ogni valore distinto → rende esplicita la condivisione, codifica ridondanza.
- **Stack machine code** (`push x; push 2; push y; multiply; subtract`): compatto, nomi impliciti, usato da JVM e per trasmissione su rete.
- **Three-address code:** `x ← y op z` con un solo operatore. Rappresentazioni:
  - **Quadruples** (tabella di k×4 valori, semplice e riordinabile)
  - **Triples** (l'indice del triple stesso fa da nome — riordinabilità ridotta)
  - **Indirect triples** (lista di puntatori a triple)
- **CFG**: nodi = basic block (sequenza massimale single-entry/single-exit), archi = flusso di controllo. Standard per analisi e ottimizzazioni.

### SSA — Static Single Assignment
- Ogni nome viene definito una sola volta (una "versione")
- Nei punti di confluenza si usano le **funzioni φ** che scelgono il valore in base al ramo entrante
- Standard moderno per IR di middle end (LLVM, GCC GIMPLE)

---

## LLVM IR (slide del docente)

### Struttura di un modulo (file `.ll` o `.bc`)
- Risultato della "compilazione" di una unità di traduzione
- Contiene: variabili globali (decl/def), funzioni globali (decl/def), metadata
- Ogni decl/def ha un **linkage**: `external`, `weak`, `internal`, ...

### Conversione bitcode ↔ testo
```
clang -S -emit-llvm aaa.c -o aaa.ll      # da sorgente a testo
clang -c -emit-llvm aaa.c -o aaa.bc      # da sorgente a bitcode binario
llvm-as aaa.ll -o aaa.bc                 # testo → bitcode
llvm-dis aaa.bc -o aaa.ll                # bitcode → testo
```
Opzione utile: `-fno-discard-value-names` per mantenere nomi leggibili.

### Identificatori
- Globali: prefisso `@` (`@main`, `@x`)
- Locali: prefisso `%` (`%0`, `%tmp`)
- Anonimi: `@123`, `%5` (interi non negativi)
- RE per nomi: `[%@][-a-zA-Z$._][-a-zA-Z$._0-9]*`

### Tipi
- **Semplici:** `void`, interi `iN` (con N qualunque: `i1`, `i8`, `i32`, `i123456`), float (`half`, `float`, `double`, `fp128`, `x86_fp80`, `ppc_fp128`), puntatori `T*`, `label`, `token`
- **Composti:** array `[N x T]`, vector `<N x T>`, struct `{T1, T2, ...}` (anche packed `<{...}>`), funzioni `T (Targs)`, metadata

### Target triple e data layout
```
target triple = "x86_64-pc-linux-gnu"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
```
- `e`/`E`: little/big endian; `m:e`: ELF mangling
- `iN:abi:pref`, `fN:...`, `vN:...`, `p:...`, `a:...`, `S:size` (allineamenti)
- `n8:16:32:64`: dimensioni intere "native" del target

### Costanti
- Booleane: `true`, `false`
- Intere: `4`, `-1234`
- Float: `123.0`, `1.5e12` (solo esatte: `1.3` non è valida)
- Puntatore: `null`, `@global`
- Array/vector/struct: literal liste
- `zeroinitializer`, `undef`

### Definizioni
```llvm
%point_t = type { i32, i32 }
%my_array_t = type [100 x %point_t]

@ottavo = internal constant double 0.125, align 8
@counter1 = dso_local global i32 0, align 4
@.str = private constant [6 x i8] c"Hello\00", align 1
```

---

## Procedure Abstraction (capitolo 6 EaC)

### Le tre astrazioni della procedura
1. **Control abstraction:** entry/exit ben definiti, return mechanism, parameterization
2. **Clean name space:** scope locale, lo shadowing nasconde nomi non locali, locali invisibili dall'esterno
3. **External interface:** accesso per nome+parametri, protezione di entrambi i lati

### Run time vs Compile time (CONFUSIONE TIPICA)
- I **linkage** (chiamate, salvataggio registri, prologo/epilogo) sono codice **emesso a compile time** ma che si **esegue a run time**
- L'hardware supporta solo trasferimento di controllo (call/return); tutto il resto (preservazione contesto, scoping, parametri) è una "menzogna del compilatore"

### Activation Record (record di attivazione, frame)
Per ogni invocazione di procedura: spazio per parametri, return address, salvataggio registri, locali, riferimenti per accedere a scope non locali.
- **Stack** per casi semplici (procedure non-first-class)
- **Heap** per closures e first-class functions

### Accesso a non-locali
- **Static link:** puntatore allo scope lessicale del padre (per nesting statico)
- **Display:** array indicizzato dal livello di scope
- Linguaggio piatto (C/C++): nessuno dei due, basta lo stack

### Linkage convention
Stabilito da architettura (ISA), OS, compilatore (calling convention). Esempio: System V AMD64 ABI passa i primi 6 interi in `rdi, rsi, rdx, rcx, r8, r9`.

---

## OOL (Object-Oriented Languages) — slide 22, 23

### Differenze con un ALL (Algol-Like Language)
Sia ALL che OOL hanno bisogno di:
- meccanismo a compile-time per name resolution
- meccanismo a run-time per indirizzo da nome
- codice che mantiene strutture run-time per addressability

In più, gli OOL hanno **dynamic dispatch** e **scope data-centrici**.

### Implementazione classi (C++/Java)
- Ogni oggetto contiene i **data members** (instance variables)
- I **methods** sono condivisi tra istanze (sono `static` di classe) → si memorizzano una volta sola
- **vtable** (virtual table): tabella di puntatori a funzione per metodi virtuali; ogni oggetto contiene un puntatore alla vtable della propria classe → dispatch dinamico in O(1)
- **Single inheritance** (Java, single class hierarchy in C++): vtable concatenata, layout monotono
- **Multiple inheritance** (C++): più vtable per oggetto, thunks per aggiustare il `this` quando si accede a basi non a offset 0

---

## Code Shape (slide 24, 25, 26)

### Definizione
"All those nebulous properties of the code that effect performance" — la forma del codice generato dipende da tante decisioni del compilatore.

### Code Shape per espressioni (24)
- Postorder tree walk dell'AST → emissione ILOC (Intermediate Language for an Optimizing Compiler, 3-address)
- Pattern: `visit(left); visit(right); emit(op);`
- Decisioni: dove tenere i valori (registro vs memoria), come gestire short-circuit, riordino per associatività/commutatività di op intere

### Code Shape per array, aggregati, stringhe (25)
- Calcolo dell'indirizzo: `&A[i]` = `&A + i * elemSize`
- Multi-dim: row-major (C) vs column-major (Fortran)
- Address polynomial: `&A[i,j] = base + (i * cols + j) * elemSize`
- Stringhe: rappresentazione null-terminated vs lunghezza+buffer; SSE/AVX per copia/confronto

### Code Shape per booleani e controllo (26)
- Numerical encoding (`0`/`1`) vs **positional encoding** (etichette di branch)
- Short-circuit di `&&` e `||` → branch e merge
- `if/else`: branch + due rami + merge label
- `while`: header con test, body, branch indietro
- `case`/`switch`: cascaded if (O(n)), binary search (O(log n)), jump table (O(1) ma richiede densità)

---

## Punti chiave per l'orale
- Saper classificare le IR (strutturale/lineare/ibrida) ed elencare pro/contro
- AST vs DAG vs 3-address vs SSA (e perché SSA è oggi standard)
- Saper scrivere/leggere uno snippet LLVM IR semplice (alloca, load/store, br, getelementptr)
- Activation record, calling convention, distinzione compile-time vs run-time
- vtable, dispatch dinamico, single vs multiple inheritance
- Code shape per `case`: tre strategie e trade-off
- Short-circuit booleani come positional encoding
