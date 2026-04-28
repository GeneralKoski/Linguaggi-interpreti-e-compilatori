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

## IR e back end
14. Classificazione delle IR (strutturale/lineare/ibrida).
15. Cos'è il **3-address code** e quali rappresentazioni esistono (quadruples, triples, indirect)?
16. Cos'è la **forma SSA** e a cosa serve la funzione `φ`?
17. **Activation record / stack frame**: cosa contiene? Compile-time vs run-time.
18. **Calling convention**: chi la stabilisce? Esempi.
19. Come si supporta una chiamata di metodo virtuale in un OOL (vtable, single inheritance).
20. Code shape per il `case` statement: tre strategie e quando si scelgono.
21. Come si traduce uno short-circuit `&&` in IR (positional encoding)?

## Middle end
22. Scope di ottimizzazione: locale, regionale, globale, whole-program. Esempi.
23. Algoritmo di **Local Value Numbering**: descrizione e complessità. Estensione **superlocale**.
24. **Dominator tree**: definizione e algoritmo iterativo per il calcolo.
25. Cosa è il **dataflow analysis**? Formula generale di un problema dataflow (forward/backward, ⋂/⋃).
26. **Live Variables**: equazioni, perché backward, applicazioni (uninitialized vars, dead store elim).
27. Spiega perché un sistema di equazioni dataflow ammette un punto fisso (Knaster-Tarski / Kleene).
28. Differenza tra problemi forward e backward; esempi.

## Interpretazione astratta
29. Chi ha formalizzato l'interpretazione astratta?
30. Cos'è la **semantica concreta** di un programma e perché non è calcolabile?
31. Cosa significa che un'astrazione è **sound**? E **complete**?
32. Cos'è una **Galois connection** `(α, γ)`?
33. Esempi di **domini astratti** classici, in ordine crescente di precisione.
34. Cosa è un **widening operator** e perché serve?
35. Connessione tra DFA del middle end e interpretazione astratta.

## Strumenti
36. Struttura di un sorgente **Flex** (3 sezioni). Cos'è `yylex`? E `yytext`/`yylineno`?
37. Cosa fa uno **start state** in Flex? Quando si usa? (Esempio: commenti multi-linea)
38. Cosa fa **Bison** e in che relazione sta con Yacc? Cos'è `$$` e `$1`...`$n`?
39. Pipeline di compilazione **Clang/LLVM**: dal `.c` al binario, passando per LLVM IR.

## Domande "trick"
40. Si può scrivere un compilatore senza il middle end? (Sì, si chiama compilatore a 2 passi.)
41. Si può usare un AST come IR di middle end? (Sì, ma scomodo per molte ottimizzazioni.)
42. È sempre vero che eliminare codice "morto" preserva la semantica? (Bisogna stare attenti agli effetti collaterali; "morto" significa risultato non osservabile.)
43. Perché il `case` con range densi conviene il jump table, mentre con range sparsi conviene la binary search? (Trade-off spazio/tempo.)
44. In un `for (int i = 0; i < N; ++i)`, perché `N` non costante può comunque consentire alcune ottimizzazioni? (Loop-invariant code motion, strength reduction su `i`.)
