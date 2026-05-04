# Glossario italiano ↔ inglese

| Italiano | English | Note |
|---|---|---|
| Analisi lessicale | Lexical analysis / Scanning | Tokenizzazione |
| Analisi sintattica | Parsing / Syntax analysis | Costruzione del parse tree |
| Analisi di semantica statica | Static semantic / Context-sensitive analysis | Type checking, scope, ... |
| Analisi dipendente dal contesto | Context-sensitive analysis | Sinonimo di sopra |
| Analizzatore lessicale | Lexer / Scanner | |
| Analizzatore sintattico | Parser | |
| Linguaggio sorgente | Source language (L) | |
| Linguaggio macchina/target | Target language (M) | |
| Rappresentazione intermedia | Intermediate Representation (IR) | |
| Lessema | Lexeme | La stringa effettiva |
| Categoria lessicale | Token category / part of speech | Es. KEYWORD |
| Token | Token | `⟨categoria, lessema⟩` |
| Espressione regolare | Regular expression (RE / regex) | |
| Automa a stati finiti deterministico | Deterministic Finite Automaton (DFA / DFSA) | |
| Automa a stati finiti non deterministico | Non-deterministic FA (NFA) | |
| Automa a pila | Pushdown automaton (PDA) | |
| Grammatica libera dal contesto | Context-free grammar (CFG) | |
| Albero di derivazione | Parse tree / Concrete syntax tree | |
| Albero sintattico astratto | Abstract Syntax Tree (AST) | |
| Tabella dei simboli | Symbol table | |
| Grammatica con attributi | Attribute grammar | |
| Attributo sintetizzato | Synthesized attribute | Da figli |
| Attributo ereditato | Inherited attribute | Da padre/fratelli |
| Traduzione guidata dalla sintassi | Syntax-directed translation (SDT) | |
| Codice a 3 indirizzi | Three-address code | |
| Forma SSA | Static Single Assignment | |
| Grafo del flusso di controllo | Control-flow graph (CFG) | |
| Blocco di base | Basic block (BB) | |
| Blocco di base esteso | Extended basic block (EBB) | |
| Funzione di trasferimento | Transfer function | |
| Punto fisso | Fixed point / fixpoint | |
| Punto fisso minimo | Least fixed point (lfp) | |
| Reticolo / Lattice | Lattice | |
| Selezione delle istruzioni | Instruction selection | |
| Allocazione registri | Register allocation | |
| Schedulazione delle istruzioni | Instruction scheduling | |
| Spilling | Spilling | LOAD/STORE temp |
| Colorazione di grafi | Graph coloring | Per allocazione |
| Record di attivazione | Activation record / Stack frame | |
| Convenzione di chiamata | Calling convention / linkage | |
| Tabella virtuale | Virtual table (vtable) | OOL |
| Spedizione dinamica | Dynamic dispatch | |
| Eliminazione codice morto | Dead-code elimination (DCE) | |
| Propagazione di costanti | Constant propagation / folding | |
| Srotolamento del ciclo | Loop unrolling | |
| Sollevamento (di codice) | Code hoisting | |
| Inlining (espansione) | Inlining | |
| Numerazione dei valori | Value numbering | |
| Variabile viva | Live variable | |
| Definizione che raggiunge | Reaching definition | |
| Espressione disponibile | Available expression | |
| Dominatore | Dominator | |
| Albero dei dominatori | Dominator tree | |
| Interpretazione astratta | Abstract interpretation | |
| Connessione di Galois | Galois connection | |
| Astrazione / concretizzazione | α / γ (abstraction / concretisation) | |
| Allargamento (operatore) | Widening operator (∇) | |
| Restringimento | Narrowing operator (△) | |
| Interprete | Interpreter | |
| Compilatore | Compiler | |
| Compilazione anticipata | Ahead-of-time (AOT) | |
| Compilazione al volo | Just-in-time (JIT) | |
| Bytecode | Bytecode | |
| Macchina virtuale | Virtual machine (VM) | |

## Parsing (LL/LR)
| Italiano | English | Note |
|---|---|---|
| Insieme dei primi simboli | FIRST set | Terminali che possono iniziare una derivazione di α |
| Insieme dei seguiti | FOLLOW set | Terminali che possono seguire un non-terminale |
| Item LR(1) | LR(1) item | `[A → α·β, look]` |
| Chiusura | Closure | `closure(I)` aggiunge gli item derivati |
| Goto | Goto | Transizione `goto(I, X)` tra stati LR |
| Tabella ACTION/GOTO | ACTION/GOTO table | Tabelle del parser LR |
| Conflitto shift-reduce | Shift-reduce conflict | Stato con shift e reduce possibili |
| Conflitto reduce-reduce | Reduce-reduce conflict | Più reduce possibili in stesso stato |
| Handle | Handle | Sottostringa riducibile in un derivazione rightmost |
| Manico (handle) | Handle | Sinonimo italiano (raro) |
| Ricorsione sinistra | Left recursion | Da eliminare per LL(1) |
| Fattorizzazione sinistra | Left factoring | Per rendere una grammatica LL(1) |
| Discesa ricorsiva | Recursive descent | Implementazione manuale di LL(k) |
| Token fittizio | Pseudo-token / phony token | In Bison con `%prec` |

## Middle end (analisi avanzate)
| Italiano | English | Note |
|---|---|---|
| Punto fisso massimo | Greatest fixed point (gfp) | Per problemi con meet ⋂ |
| Espressioni "molto occupate" | Very busy expressions | Anticipabili su tutti i path |
| Eliminazione globale di sotto-espressioni comuni | Global Common Subexpression Elimination (GCSE) | Usa Available Expressions |
| Eliminazione di ridondanze parziali | Partial Redundancy Elimination (PRE) | Lazy Code Motion |
| Spostamento di codice fuori dal loop | Loop-Invariant Code Motion (LICM) | |
| Variabile di induzione | Induction variable (IV) | Cambia in modo regolare nel loop |
| Riduzione di forza | Strength reduction | Sostituisce mul con add nei loop |
| Promozione memoria → registro | mem2reg | LLVM pass classico |
| Scomposizione di aggregati | SROA (Scalar Replacement of Aggregates) | |
| Combinatore peephole | instcombine | Pass LLVM peephole |

## IR / Code shape
| Italiano | English | Note |
|---|---|---|
| Indirizzamento di elemento | `getelementptr` (GEP) | Calcola indirizzi in LLVM |
| Conta riferimenti | Reference counting | GC sull'assegnamento |
| Riferimento debole | Weak reference | Spezza i cicli del refcount |
| Codifica numerica/posizionale (booleani) | Numerical / positional encoding | |
| Mossa condizionale | Conditional move (cmov) | Evita branch |
| Istruzione con predicato | Predicated instruction | ARM, IA-64 |
| Catena degli access link | Access link chain / static link chain | Per scope nested |
| Display | Display | Array di frame pointer (alternativa al static link) |
| Passaggio per valore | By-value parameter passing | |
| Passaggio per riferimento | By-reference parameter passing | |
| Aliasing | Aliasing | Due nomi che riferiscono allo stesso storage |
| Bitcast / conversione di tipo | bitcast / sext / zext / trunc / ptrtoint / inttoptr | LLVM conversions |
| Chiamata con unwind | `invoke` | Eccezioni in LLVM |
| Landing pad | landingpad | Punto di atterraggio per exception handling |
