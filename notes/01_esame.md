# Modalità d'esame

## ⚠️ Modalità ufficiale (confermata dal docente via mail)

L'esame è composto da **due parti separate, in date diverse**:

### 1. Seminario (prerequisito per accedere all'orale)
- **Presentazione con slide, ~15 minuti**
- Argomento **concordato direttamente col docente** (Enea Zaffanella)
- L'argomento va **individuato almeno 30 giorni prima** della data del seminario
- **Si svolge in una data precedente all'appello**, NON nello stesso giorno dell'orale
- Il docente **consiglia di fissare un appuntamento** per parlare dell'argomento prima di scegliere

### 2. Orale
- Esame **solo orale** con domande aperte su tutto il programma
- Si tiene nella **data dell'appello** (giorni dopo il seminario)
- Per accedervi è obbligatorio aver già fatto il seminario

### Riferimenti ufficiali
- Documento ufficiale modalità: https://corsi.unipr.it/it/ugov/degreecourse/329435
- Docente: Enea Zaffanella — `enea.zaffanella@unipr.it`
- **Ricevimento:** anche **da remoto via Microsoft Teams**

## Cosa fare in pratica (timeline operativa)

```
T − ≥30 gg   →  Contatta il prof e fissa un appuntamento (Teams o in presenza)
T − ≥30 gg   →  Concorda l'argomento del seminario
T − N gg     →  Prepara slide (~15 min di esposizione)
T − qualche gg → Svolgi il seminario
T (appello)  →  Esame orale
```

1. **Scrivi al prof** (`enea.zaffanella@unipr.it`) chiedendo un appuntamento Teams per discutere il seminario
2. **Concorda l'argomento** con almeno 30 giorni di margine prima della data del seminario
3. **Prepara le slide** (15 min di esposizione → ~10-15 slide effettive)
4. **Svolgi il seminario** nella data fissata (precedente all'appello)
5. **Studia per l'orale** sul resto del programma
6. **Sostieni l'orale** alla data dell'appello

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
