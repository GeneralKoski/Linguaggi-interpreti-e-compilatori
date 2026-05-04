# Possibili domande aperte all'orale

## Introduzione & struttura
1. Differenza tra **interprete** e **compilatore**. Esempi di linguaggi tipicamente interpretati, compilati e ad approccio misto.
2. Cosa significa "**compilatore ottimizzante**"? È davvero ottimo?
3. Disegna lo schema di un compilatore a **due passi** e a **tre passi**, indicando le responsabilità di front-end / middle-end / back-end.
4. Cosa è la **rappresentazione intermedia (IR)** e perché è utile?
5. Esempi reali di IR (LLVM bitcode, GCC RTL/GIMPLE).

## Front end
6. Specifica **vs** implementazione di un lexer: linguaggio adeguato per ciascuno e perché.
7. Come si passa da una **RE** a un **DFA** minimo? (Thompson + subset + Hopcroft)
8. Che cosa garantisce la **regola del lessema più lungo**? Quando si applica la **priorità per ordine**?
9. Definisci **CFG**, derivazione top-down e bottom-up, parse tree e AST. Differenza tra parse tree e AST.
10. Cosa fa un **parser LL** vs un **parser LR**? Vantaggi e limiti.
11. Cos'è la **context-sensitive analysis**? Esempi di domande non risolvibili da una CFG.
12. Cos'è una **attribute grammar**? Differenza tra attributi sintetizzati ed ereditati. Quando un AG si valuta in single-pass?
13. Perché in pratica si usano tecniche **ad-hoc SDT** invece di AG?

## Analisi sintattica
14. Calcola `FIRST` e `FOLLOW` per la grammatica `E → T E'; E' → + T E' | ε; T → F T'; T' → * F T' | ε; F → ( E ) | id`. (Traccia: `FIRST(E)=FIRST(T)=FIRST(F)={(, id}`; `FIRST(E')={+, ε}`; `FIRST(T')={*, ε}`. `FOLLOW(E)=FOLLOW(E')={), $}`; `FOLLOW(T)=FOLLOW(T')={+, ), $}`; `FOLLOW(F)={*, +, ), $}`.)
15. Costruisci la tabella di parsing LL(1) per la grammatica sopra. (Traccia: una riga per non-terminale, una colonna per ogni terminale + `$`. Ogni produzione `A → α` va nelle celle `[A, a]` con `a ∈ FIRST(α)`; se `ε ∈ FIRST(α)`, anche per `a ∈ FOLLOW(A)`. La grammatica è LL(1): nessuna cella ha più di una produzione.)
16. Per la grammatica `S → if E then S else S | if E then S | a`, costruisci gli item LR(1) del primo stato e identifica eventuali conflitti. (Traccia: lo stato 0 contiene i kernel item delle tre produzioni di `S`; dopo aver visto `if E then S`, sull'input `else` si presenta un classico conflitto **shift-reduce** — il dangling-else.)
17. Spiega cos'è un conflitto **shift-reduce** e fai un esempio concreto. Come si risolve? (Traccia: lo stato del parser ha sia un'azione di shift che una di reduce sullo stesso lookahead. Si risolve riscrivendo la grammatica, oppure tramite direttive di precedenza/associatività in Bison — convenzionalmente shift vince.)
18. Spiega la differenza tra **LR(1) canonico** e **LALR(1)**. Perché Bison usa LALR(1)? (Traccia: LR(1) tiene distinti gli stati con stesso core ma diverso lookahead; LALR(1) li fonde. Tabella molto più piccola, stessa potenza per la maggior parte delle grammatiche pratiche; può introdurre conflitti reduce-reduce che LR(1) non avrebbe.)
19. Cos'è un token fittizio `%prec` in Bison? Quando serve? (Traccia: associa a una produzione la precedenza di un token fittizio dichiarato con `%nonassoc`/`%left`/`%right`, utile per disambiguare produzioni come l'unario meno (`expr: '-' expr %prec UMINUS`) che dovrebbe avere precedenza maggiore del binario.)

## IR e back end
20. Classificazione delle IR (strutturale/lineare/ibrida).
21. Cos'è il **3-address code** e quali rappresentazioni esistono (quadruples, triples, indirect)?
22. Cos'è la **forma SSA** e a cosa serve la funzione `φ`?
23. **Activation record / stack frame**: cosa contiene? Compile-time vs run-time.
24. **Calling convention**: chi la stabilisce? Esempi.
25. Come si supporta una chiamata di metodo virtuale in un OOL (vtable, single inheritance).
26. Code shape per il `case` statement: tre strategie e quando si scelgono.
27. Come si traduce uno short-circuit `&&` in IR (positional encoding)?
28. Spiega come funziona `getelementptr` in LLVM IR. Calcola gli offset per accedere all'elemento `[5]` dell'array dentro `struct { i32; [10 x i32]; i64 }`. (Traccia: GEP fa solo aritmetica di indirizzi, non dereferenzia. Layout: campo 0 = `i32` (offset 0, 4B) + padding fino a 4 (allineamento `[10 x i32]`), campo 1 = array a offset 4, elemento `[5]` a offset `4 + 5*4 = 24`. GEP: `getelementptr %S, ptr %p, i32 0, i32 1, i32 5`.)
29. Differenza tra `call` e `invoke` in LLVM. Quando si usa l'uno e quando l'altro? (Traccia: `call` è una chiamata normale; `invoke` è una chiamata che può lanciare un'eccezione e specifica due successori — `normal` e `unwind` (landing pad). Si usa `invoke` solo nelle funzioni con unwinding semantics, tipicamente C++ con `-fexceptions`.)
30. Spiega la differenza tra **passaggio per valore** e **passaggio per riferimento**. Costi, side-effect, alias. (Traccia: by-value copia il dato sullo stack del callee — costoso per oggetti grandi, niente side-effect sull'argomento, niente alias. By-ref passa un puntatore — costo costante, side-effect possibili, introduce alias che limita le ottimizzazioni del compilatore.)
31. Cosa sono gli **access link** (o static link)? Mostra con un esempio annidato a 2 livelli su come si accede a una variabile non locale. (Traccia: l'AR di una funzione annidata contiene un puntatore all'AR del suo "padre lessicale". In `f → g → h` per leggere `x` di `f` da `h`, si segue `access_link` di `h` per arrivare a quello di `g`, poi un altro hop fino a `f`. Costo: numero di link = differenza di profondità di scope.)

## Middle end
32. Scope di ottimizzazione: locale, regionale, globale, whole-program. Esempi.
33. Algoritmo di **Local Value Numbering**: descrizione e complessità. Estensione **superlocale**.
34. **Dominator tree**: definizione e algoritmo iterativo per il calcolo.
35. Cosa è il **dataflow analysis**? Formula generale di un problema dataflow (forward/backward, ⋂/⋃).
36. **Live Variables**: equazioni, perché backward, applicazioni (uninitialized vars, dead store elim).
37. Spiega perché un sistema di equazioni dataflow ammette un punto fisso (Knaster-Tarski / Kleene).
38. Differenza tra problemi forward e backward; esempi.
39. Scrivi le equazioni di **Available Expressions**, specifica la direzione, il meet operator, l'init. Mostra come si applicano per fare **GCSE**. (Traccia: forward, meet = `∩`. `IN[B] = ∩ OUT[P]` per `P ∈ pred(B)`; `OUT[B] = GEN[B] ∪ (IN[B] − KILL[B])`. Init: `OUT[entry]=∅`, `OUT[B]=U` (universo) altrove. Per GCSE: se un'espressione `e` è in `IN[B]` allora il suo valore è già stato calcolato — si può sostituire con una temp che lo memorizza.)
40. Cos'è una **Very Busy Expression**? Scrivi le equazioni e spiega l'uso per **code hoisting**. (Traccia: un'espressione è very busy in `B` se sarà sicuramente valutata su ogni cammino prima di essere ridefinita. Backward, meet = `∩`. `OUT[B] = ∩ IN[S]` per `S ∈ succ(B)`; `IN[B] = USE[B] ∪ (OUT[B] − KILL[B])`. Permette di anticipare (hoisting) il calcolo nel punto in cui diventa busy, riducendo dimensione del codice senza aumentare il numero di esecuzioni dinamiche.)
41. Cos'è una **induction variable**? Come la identifichi e a cosa serve? (Traccia: variabile il cui valore in un loop forma una progressione lineare rispetto al contatore — *basic IV* `i = i + c`, *derived IV* `j = a*i + b`. Si individuano scansionando il body del loop. Servono per **strength reduction** (sostituire moltiplicazioni con addizioni), eliminazione di IV ridondanti, semplificazione della guardia del loop.)

## Interpretazione astratta
42. Chi ha formalizzato l'interpretazione astratta?
43. Cos'è la **semantica concreta** di un programma e perché non è calcolabile?
44. Cosa significa che un'astrazione è **sound**? E **complete**?
45. Cos'è una **Galois connection** `(α, γ)`?
46. Esempi di **domini astratti** classici, in ordine crescente di precisione.
47. Cosa è un **widening operator** e perché serve?
48. Connessione tra DFA del middle end e interpretazione astratta.

## Strumenti
49. Struttura di un sorgente **Flex** (3 sezioni). Cos'è `yylex`? E `yytext`/`yylineno`?
50. Cosa fa uno **start state** in Flex? Quando si usa? (Esempio: commenti multi-linea)
51. Cosa fa **Bison** e in che relazione sta con Yacc? Cos'è `$$` e `$1`...`$n`?
52. Pipeline di compilazione **Clang/LLVM**: dal `.c` al binario, passando per LLVM IR.

## Domande "trick"
53. Si può scrivere un compilatore senza il middle end? (Sì, si chiama compilatore a 2 passi.)
54. Si può usare un AST come IR di middle end? (Sì, ma scomodo per molte ottimizzazioni.)
55. È sempre vero che eliminare codice "morto" preserva la semantica? (Bisogna stare attenti agli effetti collaterali; "morto" significa risultato non osservabile.)
56. Perché il `case` con range densi conviene il jump table, mentre con range sparsi conviene la binary search? (Trade-off spazio/tempo.)
57. In un `for (int i = 0; i < N; ++i)`, perché `N` non costante può comunque consentire alcune ottimizzazioni? (Loop-invariant code motion, strength reduction su `i`.)
