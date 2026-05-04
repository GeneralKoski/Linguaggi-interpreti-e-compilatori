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

### `getelementptr` (GEP) in dettaglio
GEP **calcola un indirizzo**, non accede mai alla memoria (niente load/store). È la sola operazione LLVM che fa aritmetica di puntatori type-aware.

Sintassi: `getelementptr <ty>, <ty>* %p, <idx0>, <idx1>, ...`

**Regola degli indici (il classico tranello):**
- Il **primo indice** scorre il puntatore stesso come se fosse un array (`%p[idx0]`). Quasi sempre `0` se non si sta indicizzando un array di top-level.
- Gli **indici successivi** scendono dentro il tipo composto: per uno `struct` selezionano il campo (devono essere costanti `i32`), per un `[N x T]` o `<N x T>` selezionano l'elemento.

**Esempio numerico** su `%S = type { i32, [10 x i32], i64 }` con `%p : %S*` e accesso a `S.array[3]`:
```llvm
%ptr = getelementptr %S, %S* %p, i32 0, i32 1, i32 3
```
Layout (assumendo allineamento naturale, no padding rilevante tra i32 e [10 x i32]):
- field 0 (`i32`)        offset 0
- field 1 (`[10 x i32]`) offset 4 — elemento 3 è a +12 dentro l'array → offset totale `4 + 3*4 = 16`
- field 2 (`i64`)        offset 48

Il primo `0` dice "non scorrere il puntatore" (resto dentro la struct puntata). Se metti `getelementptr %S, %S* %p, i32 1` ottieni il puntatore alla **struct successiva** (offset = `sizeof(%S)`), idioma equivalente a `&p[1]` in C.

**Cosa potrebbe chiedere il prof:** "se hai `%S* %p` e vuoi l'elemento k dell'array interno, quanti indici servono e cosa sono?". Risposta: tre — `0` (non scorrere il puntatore), `1` (campo array), `k` (elemento). Possibile follow-up: differenza tra `getelementptr inbounds` e senza (UB se fuori dall'oggetto allocato vs comportamento definito modulo overflow).

### `invoke` vs `call` e exception handling
- `call` = chiamata ordinaria; se la funzione chiamata fa unwind (eccezione C++ / longjmp di certi runtime) l'effetto è **undefined** dal punto di vista del frontend EH.
- `invoke` = chiamata che può fare unwind. Aggiunge due label terminator-style:
  ```llvm
  %r = invoke i32 @foo(i32 %x) to label %normal unwind label %lpad
  ```
  - `to %normal` = ramo del ritorno normale
  - `unwind %lpad` = ramo se la chiamata propaga un'eccezione
- Il blocco `%lpad` deve iniziare con una `landingpad`, che descrive i tipi catturabili / cleanup e produce il `{i8*, i32}` (eccezione + selector) usato dal personality runtime (`__gxx_personality_v0` per Itanium C++ ABI).

**Cosa potrebbe chiedere il prof:** "perché LLVM ha bisogno di un terminator dedicato per le chiamate che possono fare unwind?" — perché l'EH è control flow a tutti gli effetti e il CFG/dominator analysis devono vederlo esplicitamente.

### Conversioni LLVM
| Istruzione | Domini | Uso tipico |
|---|---|---|
| `bitcast` | stesso size, reinterpret | cambio di tipo puntatore (pre-opaque-pointers), reinterpret di vector |
| `ptrtoint` | `ptr → iN` | hash, taggature, stampa indirizzi |
| `inttoptr` | `iN → ptr` | rimettere un intero in un puntatore (rischioso, può inibire alias analysis) |
| `sext` | `iN → iM`, M>N | estensione **con segno** (replica MSB) |
| `zext` | `iN → iM`, M>N | estensione **senza segno** (zero-pad) |
| `trunc` | `iN → iM`, M<N | tronca i bit alti |
| `fptosi` / `fptoui` | float → int | conversione con/senza segno (round verso zero) |
| `sitofp` / `uitofp` | int → float | conversione |
| `fpext` / `fptrunc` | float → float | cambio precisione |

**Regola pratica:** mai `bitcast` tra interi e float (usa `bitcast` con stesso size se proprio serve raw bits, ma è quasi sempre un bug); `sext` per signed `int → long`, `zext` per unsigned o per estendere `i1` (boolean) a `i32`.

**Cosa potrebbe chiedere il prof:** "che differenza c'è tra `sext i1 → i32` e `zext i1 → i32`?" — `sext` produce `0` o `0xFFFFFFFF` (replica del bit di segno), `zext` produce `0` o `1`. Nei booleani vuoi quasi sempre `zext`.

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
- **Static link (access link):** puntatore al frame lessicalmente padre. Per accedere a una variabile di livello `k` da una procedura di livello `n`, si segue la catena `n−k` volte.
- **Display:** array (globale o nel frame) indicizzato dal livello lessicale: `display[k]` = frame attualmente attivo del livello `k`. Accesso O(1) ma serve manutenzione su entry/exit.
- Linguaggio piatto (C/C++): nessuno dei due, basta lo stack.

**Esempio static link a 2 livelli (Pascal-like):**
```pascal
procedure outer;             { livello 1 }
  var x: integer;
  procedure middle;          { livello 2 }
    var y: integer;
    procedure inner;         { livello 3 }
    begin
      y := y + 1;            { 1 hop di static link }
      x := x + y;            { 2 hop di static link }
    end;
  begin inner end;
begin middle end;
```
Quando `inner` esegue `x := x + y`:
1. carica static link dal proprio frame → punta al frame di `middle` (livello 2)
2. carica static link da quel frame → punta al frame di `outer` (livello 1)
3. accede a `x` come `[outer_frame + offset(x)]`
Per `y` basta un solo hop. Il compilatore conosce a compile-time la differenza di livello tra uso e definizione e genera il numero giusto di dereferenze.

In linguaggi con **closures first-class** (JS, Scheme) lo static link non basta perché un frame può sopravvivere alla procedura: si alloca su heap (capture environment) e il puntatore alla closure incapsula codice + environment.

**Cosa potrebbe chiedere il prof:** "cosa succede a static link quando una funzione viene restituita come valore?" — in Pascal/Ada non si può (errore di scope), in JS/Scheme richiede heap allocation del frame catturato.

### Reference counting per GC sull'assegnamento
Strategia di gestione automatica della memoria alternativa a tracing GC: ogni oggetto sull'heap ha un contatore `rc`. Sull'assegnamento `p := q` si genera codice del tipo:
```
tmp := q
if (tmp != null) tmp.rc++       // incrementa nuovo target
if (p != null && --p.rc == 0)   // decrementa vecchio target
    free(p)                      // se rc azzerato, libera (e cascata sui figli)
p := tmp
```
**Pro:** deallocazione deterministica, no pause globali, locality friendly.
**Contro:**
- **Cycles problem:** un ciclo di puntatori (`a → b → a`) ha rc ≥ 1 anche se irraggiungibile → leak. Si risolve con **weak references** (riferimenti che non incrementano rc, tipici per back-pointers parent/child) o con un cycle collector ausiliario (es. CPython).
- Costo di ogni assegnamento (incremento+decremento+possibile free).
- Thread-safety: rc richiede operazioni atomiche → costose.

Usato in: Swift (ARC), Objective-C, CPython, COM, std::shared_ptr di C++.

**Cosa potrebbe chiedere il prof:** "perché Java non usa refcount?" — per i cicli e per il costo di ogni store di reference; preferisce tracing GC generazionale.

### Reference vs value parameter passing
| Modalità | Cosa si passa | Costo | Side-effect | Aliasing |
|---|---|---|---|---|
| **By value** | copia del dato | O(sizeof) per copia | nessuno (callee modifica la copia) | nessuno |
| **By reference** | indirizzo del dato | O(1) (un puntatore) | callee può modificare l'originale | sì, problema serio |
| **By value-result (copy-in/copy-out)** | copia in entrata + copia in uscita | 2× O(sizeof) | osservabile a fine call | no durante la call |
| **By name** (Algol60) | testuale, rivalutato a ogni uso | costoso, sorprese semantiche | sì | — |

**Alias problem:** se passo `f(&x, &x)` o `f(&a[i], &a[j])` con `i==j`, dentro `f` due parametri formali diversi puntano alla stessa cella. Inibisce ottimizzazioni (constant prop, CSE) perché il compilatore deve assumere che ogni store via un parametro possa modificare l'altro. C99 introduce `restrict` per promettere assenza di alias.

**Cosa potrebbe chiedere il prof:** "perché Fortran è storicamente più ottimizzabile di C?" — perché il suo standard vieta l'aliasing tra parametri, quindi il compilatore può riordinare load/store liberamente.

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

#### Stringhe C-style vs Pascal-style
| Aspetto | C (null-terminated) | Pascal (length-prefixed) |
|---|---|---|
| Layout | `[c0, c1, ..., cn-1, '\0']` | `[len, c0, c1, ..., cn-1]` (1 o più byte di prefisso) |
| `strlen` | O(n), scansione fino a `'\0'` | O(1), legge il prefisso |
| Lunghezza max | illimitata | limitata da bit del prefisso (Pascal classico: 255) |
| Embedded `'\0'` | impossibile (tronca) | possibile |
| Memoria | n+1 byte | n + sizeof(len) byte |
| Bug famosi | buffer overrun, off-by-one, assenza terminatore | nessuno equivalente |

I linguaggi moderni (Rust `String`, Java, C# `string`, std::string) usano varianti length-prefixed, spesso con SSO (small string optimization).

**Cosa potrebbe chiedere il prof:** "perché `strlen` in C è O(n) e che impatto ha sulle performance?" — bisogna scorrere fino al `'\0'`, e idiomi tipo `for (i=0; i<strlen(s); i++)` diventano O(n²) (errore classico).

### Code Shape per booleani e controllo (26)
- Numerical encoding (`0`/`1`) vs **positional encoding** (etichette di branch)
- Short-circuit di `&&` e `||` → branch e merge
- `if/else`: branch + due rami + merge label
- `while`: header con test, body, branch indietro
- `case`/`switch`: cascaded if (O(n)), binary search (O(log n)), jump table (O(1) ma richiede densità)

#### Numerical vs positional encoding — quando scegliere quale
- **Numerical encoding:** il booleano è un valore in registro (`0`/`1`). Esempio:
  ```
  bool b = (x < y) && (a == c);   // assegnato → si materializza il valore
  ```
  Codice: due `setcc`/compare, un `and`, un singolo store. **Niente branch interno**: piace al branch predictor e al pipeline.
- **Positional encoding:** il booleano è "dove sei nel CFG" (target di branch). Esempio:
  ```
  if ((x < y) && (a == c)) { ... }   // controllo → short-circuit naturale
  ```
  Codice: `cmp x,y; jge Lfalse; cmp a,c; jne Lfalse; <then>; Lfalse: <else>`. La seconda condizione **non si valuta** se la prima è falsa (richiesto dalla semantica `&&` di C/Java).

**Regola pratica:**
- `&&`/`||` in posizione di controllo (if/while) → **positional**, è obbligatorio per short-circuit.
- Booleano assegnato a variabile o restituito → **numerical**, evita branch e i loro misprediction.
- Espressioni miste (es. `r = (x<y) && f()`) → il compilatore ibrida: positional per la parte short-circuit, materializzazione finale a 0/1.

#### Move condizionali e istruzioni con predicato
Per evitare branch (e relative misprediction) molte ISA offrono:
- **x86:** famiglia `cmov` — `cmovz`, `cmovnz`, `cmovl`, `cmovge`, ... Forma: `cmovcc dst, src` esegue `dst = src` solo se la condizione è vera, altrimenti no-op (ma legge sempre `src`, quindi non protegge da segfault).
- **ARM (AArch32):** quasi ogni istruzione ha 4 bit di **predicate** (`addeq`, `movne`, ...) → l'istruzione viene "ritirata" come no-op se il predicato è falso. AArch64 ha rimosso la predicazione generale lasciando `csel`/`csinc`/`csneg` (counterparts di cmov).
- **Itanium / GPU:** predicazione esplicita su tutte le istruzioni.

Esempio (max in C):
```c
int m = (a > b) ? a : b;
```
Compilato senza cmov: 2 branch e merge.
Compilato con cmov:
```
mov  m, a
cmp  a, b
cmovl m, b      ; se a < b, m ← b
```
Lineare, niente branch.

**Trade-off:**
- **Pro:** elimina misprediction su pattern data-dependent imprevedibili (es. selezione tra due valori in un loop tight).
- **Contro:** entrambi i path vengono **eseguiti** (almeno la load del src) → se uno dei due è costoso (chiamata, divisione) o pericoloso (deref di puntatore potenzialmente null) il branch resta migliore. Anche su rami molto sbilanciati il predittore vince.

**Cosa potrebbe chiedere il prof:** "quando preferiresti un `cmov` a un `if`?" — quando la condizione è imprevedibile e i due rami sono cheap e side-effect free. "E quando NO?" — quando uno dei rami è una chiamata a funzione, una divisione, o un memory access che potrebbe faultare.

---

## Punti chiave per l'orale
- Saper classificare le IR (strutturale/lineare/ibrida) ed elencare pro/contro
- AST vs DAG vs 3-address vs SSA (e perché SSA è oggi standard)
- Saper scrivere/leggere uno snippet LLVM IR semplice (alloca, load/store, br, getelementptr)
- Activation record, calling convention, distinzione compile-time vs run-time
- vtable, dispatch dinamico, single vs multiple inheritance
- Code shape per `case`: tre strategie e trade-off
- Short-circuit booleani come positional encoding
