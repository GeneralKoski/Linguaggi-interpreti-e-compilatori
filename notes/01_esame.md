# Modalità d'esame

## ⚠️ Modalità ufficiale (sito UniPR + mail del docente)

L'esame è composto da **due parti separate, in date diverse**:

### 1. Seminario (prerequisito per accedere a TUTTI gli appelli dell'anno)
- **Presentazione con slide, ~15 minuti**
- **Argomento + materiale a supporto** concordati col docente **almeno 30 giorni prima** della data del seminario
- Al termine della presentazione si rispondono a **domande sull'argomento del seminario**
- **Una volta svolto**, il seminario dà accesso a **tutti gli appelli previsti per quell'anno accademico** (non va rifatto se ti ripresenti)
- Il docente consiglia di **fissare un appuntamento** per discutere l'argomento (anche da remoto via Microsoft Teams)

### 2. Orale (per ogni appello a cui ti presenti)
- **Colloquio orale** con domande aperte sugli argomenti del programma del corso
- Si tiene nella **data dell'appello**

### Valutazione finale
- **30% seminario** (presentazione + risposte alle domande sull'argomento)
- **70% colloquio orale**

### Riferimenti ufficiali
- Documento ufficiale modalità: https://corsi.unipr.it/it/ugov/degreecourse/329435
- Docente: Enea Zaffanella — `enea.zaffanella@unipr.it`
- **Ricevimento:** anche **da remoto via Microsoft Teams**

## Cosa fare in pratica (timeline operativa)

```
S − ≥30 gg   →  Contatta il prof, fissa appuntamento (Teams o presenza)
S − ≥30 gg   →  Concorda argomento + materiale a supporto
S − N gg     →  Studia a fondo l'argomento e prepara slide (~15 min)
S            →  Svolgi il seminario + Q&A sull'argomento
... (vale per tutti gli appelli successivi nell'anno accademico)
T1, T2, ...  →  Sostieni l'orale a uno (o più) degli appelli
```

Dove `S` = data del seminario, `T1, T2, ...` = date degli appelli orali.

1. **Scrivi al prof** chiedendo un appuntamento Teams per discutere il seminario
2. **Concorda argomento + materiale** con almeno 30 giorni di margine
3. **Prepara le slide** (15 min → ~10-15 slide) + studia bene l'argomento per le domande
4. **Svolgi il seminario** (vale per tutti gli appelli dell'anno)
5. **Studia per l'orale** sul resto del programma
6. **Sostieni l'orale** in uno degli appelli

## ⚠️ Implicazioni strategiche

### Il seminario "sblocca" l'anno
Non c'è urgenza di farlo a ridosso di un appello specifico: una volta fatto, vale per **tutti gli appelli dell'a.a.**. Conviene farlo presto per:
- togliersi il pensiero
- avere più appelli a disposizione
- evitare il rischio di doverlo rifare nell'anno accademico successivo

### Il seminario pesa il 30%
Non è una pura formalità. Curarlo bene può alzarti significativamente il voto finale.

Esempio: se prendi 27 al seminario e 30 all'orale, il voto pesato è circa **29.1**. Se prendi 24 al seminario e 30 all'orale, scende a **28.2**.

## Argomenti caldi (probabili all'orale)
- Specifica vs implementazione del lexer (RE vs DFA), costruzione DFA da RE (Thompson + subset + Hopcroft)
- CFG, parsing top-down/bottom-up, derivazione, ambiguità
- Attribute grammars (sintetizzati vs ereditati) e perché in pratica vince l'ad-hoc SDT
- Tipi di IR (strutturale/lineare/ibrida), 3-address code, SSA
- Procedure abstraction: activation record, calling convention, run-time vs compile-time
- vtable e dynamic dispatch in OOL
- Local Value Numbering, dominator trees, dataflow (Live Variables come caso canonico)
- Punti fissi, monotonia, lattice → ponte con interpretazione astratta
- Cosa è la Galois connection e perché serve (interpretazione astratta)
- Esempi concreti di Flex (esercitazione lexer C++) e di interprete/AST (TinyP)

## Argomenti su cui spesso ci si "incarta"
- **Compile-time vs run-time:** sapere chiaramente cosa esiste solo in fase di compilazione (parse tree, symbol table compilatore, AST) e cosa solo a run-time (activation record, vtable, heap)
- **Punto fisso & monotonia:** non basta dire "esiste un punto fisso"; saperne giustificare l'esistenza (Knaster-Tarski/Kleene) e l'unicità del minimo
- Le 3 strategie di compilazione del `case` (cascade, binary, jump table) e quando si scelgono
- Differenza tra parser **LL** (top-down predittivo) e **LR** (bottom-up shift-reduce)

## Consigli operativi
- Studiare in parallelo le slide del **docente** (italiano, sintetiche) e quelle di **EaC** (inglese, dettagliate)
- Provare a buildare ed eseguire **TinyP** e il **lexer C++** dell'esercitazione: avere un esempio concreto da citare aiuta tantissimo all'orale
- Sapere descrivere a voce un intero "compiler pipeline" usando come esempio un piccolo programma C
- Avere pronti 2-3 esempi di RE → NFA → DFA → DFA minimo (anche piccolissimi) da disegnare
