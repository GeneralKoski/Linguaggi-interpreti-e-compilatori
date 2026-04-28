# Modalità d'esame

## ⚠️ Modalità ufficiale (dalla slide del docente)

**Esame orale** con domande aperte su tutto il programma.

**In alternativa** (a scelta dello studente):
- **Seminario di approfondimento** (max 15 minuti) su un argomento del programma, oppure
- **Sviluppo e presentazione di un progetto software**

L'argomento del seminario o progetto è **assegnato dal docente, su richiesta dello studente**, **almeno 30 giorni prima dell'appello**.

## Cosa fare in pratica
1. **Decidi presto** se preferisci orale puro o seminario/progetto
2. Se vuoi seminario/progetto, contatta il docente (`enea.zaffanella@unipr.it`) almeno **30+ giorni prima** della data di appello desiderata e proponi/richiedi un argomento
3. Per l'orale "puro": preparati ad aspettarti domande su qualsiasi capitolo (lexer, parser, CSA, IR, middle end, back end, interpretazione astratta)

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
