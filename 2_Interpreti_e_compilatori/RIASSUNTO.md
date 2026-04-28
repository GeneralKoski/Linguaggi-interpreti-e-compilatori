# 2. Interpreti e compilatori

## Definizioni chiave

**Interprete (per il linguaggio L):** programma che prende in input un programma scritto in L e lo **esegue**, producendo l'output corrispondente.

**Compilatore (da L a M):** programma che prende un programma in L e lo **traduce** in un programma equivalente in M. Per eseguire il compilato serve poi un interprete per M (anche l'hardware è un interprete del linguaggio macchina).

**Compilatore "ottimizzante":** traduce migliorando una metrica (tempo, memoria, energia). In senso matematico l'ottimalità è impossibile → si usano **euristiche** che funzionano bene nei casi comuni ma senza garanzie di ottimalità.

## Compilazione vs interpretazione

| Caratteristica | Compilazione (off-line) | Interpretazione (on-line) |
|---|---|---|
| Errori | Identifica errori prima dell'esecuzione | A run-time |
| Efficienza | Spostamento calcoli a tempo di compilazione | Decisioni run-time |
| Costrutti alto livello | Praticabili | Spesso troppo costosi |

- **Tipicamente compilati:** FORTRAN, Pascal, C, C++, OCaml (ma esistono interpreti, e.g. cling per C++)
- **Tipicamente interpretati:** PHP, R, Matlab (ma possono essere compilati)
- **Approcci misti:** Java (bytecode + JVM + JIT), Python, SQL (interprete + ottimizzazione + prepared statements compilati)

## Compromessi
- Tempo di compilazione e dimensione del compilato devono restare accettabili
- Esempio: programmazione generica — template C++ vs generics Java rappresentano due punti diversi nel trade-off

## Perché studiare i compilatori
1. **Applicazioni di teoria:** RE/automi (lessicale), CFG/PDA (sintassi), punto fisso & approssimazione (ottimizzazione)
2. **Algoritmi e strutture dati sofisticati:** hash, alberi, grafi, greedy, DP, pattern matching, scheduling, graph coloring
3. **System/software engineering:** interazione con OS e architettura, design pattern (es. Visitor), gestione progetti grandi
4. **Implementare DSL** (domain specific languages): scripting per grafica, videogiochi, automazione, data science, generatori di doc come Doxygen/Javadoc
5. **NON** principalmente per scrivere il prossimo compilatore C++ da zero (raramente serve, oltre il programmatore medio), ma per estendere compilatori esistenti (Clang/LLVM è aperto a contributi)

## Libri "Dragon" di riferimento
- Green Dragon (1977, Aho-Ullman) — *Principles of Compiler Design*
- Red Dragon (1986) — 1ª ediz. *Compilers: Principles, Techniques, and Tools*
- Purple Dragon (2006) — 2ª ediz. (Aho-Lam-Sethi-Ullman)
- LLVM ha anch'esso un drago come logo

## Libro adottato
**Keith D. Cooper & Linda Torczon — *Engineering a Compiler*, 2nd ed., Morgan Kaufmann, 2011**

## Punti da ricordare per l'orale
- Distinzione netta interprete vs compilatore, e perché alla fine serve sempre *qualche* interprete (l'hardware)
- Il significato pratico di "ottimizzante": euristico, non ottimale
- Saper portare un esempio di approccio misto (Java/JVM/JIT è il caso classico)
