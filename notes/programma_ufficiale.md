# Programma ufficiale del corso

Estratto dal sito UniPR: https://corsi.unipr.it/it/ugov/degreecourse/329435
A.A. 2025/26 — codice 1009064 — 9 CFU — 72 ore — Docente: Enea Zaffanella

## Obiettivi formativi
Lo studente verrà introdotto alle problematiche principali relative allo sviluppo di interpreti e compilatori per un linguaggio di programmazione. Per ognuna verranno mostrati i risultati teorici, gli strumenti e le tecniche che consentono di arrivare alla definizione di una corrispondente soluzione algoritmica.

Il seminario è descritto come "ottima occasione per affinare le proprie abilità comunicative (sia nella forma scritta che in quella orale) in un contesto tecnico-specialistico".

## Prerequisiti
Conoscenza di almeno un linguaggio di programmazione (imperativo); conoscenza delle principali tecniche algoritmiche per la risoluzione di problemi e per lo sviluppo di strutture dati efficienti.

## Contenuti dell'insegnamento (macro-aree)
1. Introduzione: interpreti e compilatori
2. Analisi lessicale
3. Analisi sintattica
4. Analisi di semantica statica
5. Rappresentazioni intermedie (IR)
6. Analisi e ottimizzazione della IR

## Programma esteso (sintesi per macro-aree)

### Introduzione e struttura
- Interpreti vs compilatori: definizione, analogie, differenze, motivazioni di studio
- Compilatore a 2 passi: front-end (lexer + parser + checker) e back-end (instruction selection, scheduling, register allocation)
- Compilatori ottimizzanti e middle-end: passi di analisi vs trasformazione, dipendenze e invalidazione

### Analisi lessicale
- Espressioni regolari: sintassi e semantica
- Specifica token dei linguaggi di programmazione
- Tool **Flex**: regole, sintassi pattern, opzioni, blocchi letterali, pattern con nome, start state
- Esercitazione: lexer per sottoinsieme di C++
- DFA e NFA, RE→NFA (Thompson), NFA→DFA (subset construction)
- **Minimizzazione DFA: algoritmo di Hopcroft**
- Cenni: traduzione DFA→RE

### Analisi sintattica
- CFG, derivazioni sinistre/destre, ambiguità
- Parsing top-down vs bottom-up
- **Grammatiche LL(1):** eliminazione ricorsione sinistra, fattorizzazione sinistra
- **Insiemi FIRST e FOLLOW**
- Top-down: discesa ricorsiva e tabelle di parsing
- **Grammatiche LR(1):** shift-reduce, riduzioni, derivazioni destre, handle
- Tabelle ACTION e GOTO
- Item LR(1), insieme canonico di stati LR(1), funzioni `closure()` e `goto()`
- Conflitti shift-reduce e reduce-reduce: significato e risoluzione
- **Grammatiche a precedenza di operatori**
- Tool **Bison**: token, attributi, associatività, precedenza, produzioni e azioni
- Esempio pratico: interpreti/compilatori per espressioni aritmetiche con Bison
- Token fittizi per associare precedenze a regole
- Confronto top-down vs bottom-up (espressività)

### Analisi semantica statica (context-sensitive)
- Esempi di costrutti dipendenti dal contesto
- **Grammatiche con attributi**: sintetizzati ed ereditati, funzionalità e non ciclicità
- **Ad-hoc SDT** (syntax directed translation): informazione non locale, regole non funzionali, adattamento dei parser
- Costruzione AST dal parse tree
- Analisi semantica statica via visite generalizzate sull'AST
- Tipologie di IR: su grafo, lineare, ibrida (CFG)
- Esercitazione: tool **`clang-tidy`**

### Astrazione procedurale (parte 1)
- Naming e scope lessicale
- Passaggio parametri e ritorno valori
- Astrazione del controllo (invocazione/ritorno)
- Tipi di allocazione: statica (data segment) e automatica (stack)
- Strutturazione del **record di attivazione (AR)**: dati e info di controllo
- **Catena degli AR e degli access link** per indirizzare informazione non locale
- **Convenzioni di chiamata:** sequenze pre-call, prologo, epilogo, post-call
- Passaggio parametri **per valore vs per riferimento**

### OOL (Object-Oriented Languages)
- Puntatore alle informazioni dinamiche di tipo
- **Tabella dei metodi virtuali (vtable)**
- Layout dei dati con **ereditarietà singola** e **multipla**
- Caso dei linguaggi aperti a modifiche della struttura dei tipi

### Generazione del codice
- Generazione IR per espressioni aritmetiche (visita AST)
- Assegnamento: uso di **reference count per garbage collection**
- Layout e indirizzamento di **array e matrici**
- Passaggio di parametri di tipo array
- Indirizzamento e accesso a campi di **record strutturati**
- **Stringhe**: C-style (terminatore) vs Pascal-style (lunghezza esplicita)
- Accesso ai singoli byte in architetture word-only
- Booleani e operatori relazionali: rappresentazione e valutazione
- **Semantica di corto-circuito**, **move condizionali**, **istruzioni con predicato**
- Generazione codice per **if-then-else** e **cicli**

### LLVM IR
- Moduli bitcode LLVM: rappresentazione binaria e testuale
- Identificatori (locali e globali), tipi (primitivi e strutturati), costanti, variabili, etichette
- Struttura funzioni: istruzioni e basic block
- **Forma SSA** (Static Single Assignment)
- Istruzioni aritmetiche, bit-a-bit, memoria, indirizzamento (array/struct), confronto, selezione condizionata
- Terminatori BB: salti condizionati/incondizionati, return, switch
- Conversioni: float-int, ptr-int, bitcast
- Call e invoke
- **Pseudo-funzioni `phi`** per merge di valori
- Tool **`opt`** per analisi e ottimizzazioni

### Ottimizzazioni IR
- Analisi vs trasformazioni
- **Scope:** locale, regionale, globale (intra-procedurale), whole-program (inter-procedurale)
- **LVN** (Local Value Numbering)
- **SVN** (Superlocal VN), **Loop Unrolling**
- **Liveness analysis** (intra-procedurale): identificare variabili usate prima dell'inizializzazione
- **Equazioni data-flow** per liveness: inizializzazione, propagazione (backward), terminazione punto fisso
- Analisi CFG: insieme dei **dominatori**, **dominatore immediato**
- **DVN** (Dominator-based Value Numbering)
- **Inlining** (inter-procedurale)
- Esercitazione passi LLVM `opt`:
  - **Simplificazione del CFG**
  - **SROA** (Scalar Replacement Of Aggregates)
  - **CSE** (Common Subexpression Elimination)
  - **LICM** (Loop Invariant Code Motion)
  - **Identificazione delle Induction Variable**
- Approfondimenti:
  - **DCE** (Dead Code Elimination) e CFG simplification
  - **Available Expressions** (forward DFA): definizione, init, propagazione, terminazione, uso per **GCSE** (Global CSE)
  - **Very Busy Expressions** e **Code Hoisting**
  - **Constant Folding** (espressioni costanti)

### Metodi formali per l'analisi
- Correttezza formale: indecidibilità, approssimazioni, falsi positivi/negativi
- **Model checking, (interactive) theorem proving, interpretazione astratta**
- Approfondimento interpretazione astratta:
  - Semantica concreta, operatori semantici come funzioni continue su reticoli completi
  - Calcoli di punto fisso: **teorema di Knaster-Tarski** e **teorema di Kleene**
  - Approssimazione e **connessioni di Galois**: esistenza della migliore approssimazione corretta
  - Domini astratti: **segni, intervalli, congruenze**

## Bibliografia
**Testo di riferimento (in inglese):**
Keith Cooper, Linda Torczon — *Engineering a Compiler*, 2nd Edition, Morgan Kaufmann, 2011

**Materiale aggiuntivo su Elly:**
- Slide delle lezioni
- Materiale per le esercitazioni passive
- Approfondimenti su argomenti specifici (in inglese)

## Modalità di verifica
**Seminario** (prerequisito, 30% del voto):
- Argomento + materiale a supporto concordati con il docente almeno **30 giorni prima** della data del seminario
- Presentazione ~15 minuti + domande sull'argomento
- Una volta svolto, dà accesso a **tutti gli appelli dell'anno accademico**

**Colloquio orale** (70% del voto):
- Domande aperte sul programma del corso

## Metodi didattici
Lezioni frontali ed esercitazioni guidate. Le esercitazioni servono a sperimentare strumenti e tecniche introdotte nelle lezioni frontali. Spesso si mostrano approcci alternativi per la stessa soluzione, per sviluppare autonomia di giudizio.

## Contatti rilevanti
- Docente: Prof. Enea Zaffanella — `enea.zaffanella@unipr.it`
- Presidente CdS: Prof. Vincenzo Bonnici
- Ricevimento: anche via Microsoft Teams
