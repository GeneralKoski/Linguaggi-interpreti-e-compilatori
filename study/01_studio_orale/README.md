# 01 — Studio orale 9 capitoli + parsing

Stima: **12-14h**, distribuite su 4-6 sessioni da 2-3h.

## Metodo (per ogni lezione)

1. Leggi il `RIASSUNTO.md` della cartella
2. Apri il PDF docente e leggilo facendo riferimento al riassunto
3. Se serve approfondimento: slide EaC (inglese) o libro Cooper-Torczon
4. Alla fine, sezione **"Punti chiave per l'orale"** del riassunto → rispondi a voce
5. Annota le domande su cui esiti per il blocco 03

## Ordine

| # | Argomento | Sorgente | Tempo |
|---|---|---|---|
| 1 | Introduzione al corso | [`../../1_Introduzione_al_corso/RIASSUNTO.md`](../../1_Introduzione_al_corso/RIASSUNTO.md) | 20 min |
| 2 | Interpreti e compilatori | [`../../2_Interpreti_e_compilatori/RIASSUNTO.md`](../../2_Interpreti_e_compilatori/RIASSUNTO.md) | 30 min |
| 3 | Struttura del compilatore | [`../../3_Struttura_del_compilatore/RIASSUNTO.md`](../../3_Struttura_del_compilatore/RIASSUNTO.md) | 1h |
| 4 | Analisi lessicale (RE, NFA, DFA, Hopcroft, Flex) | [`../../4_Analisi_lessicale/RIASSUNTO.md`](../../4_Analisi_lessicale/RIASSUNTO.md) | 2h |
| 5 | Esercitazione lexer C++ | [`../../5_Esercitazione_su_analisi_lessicale/RIASSUNTO.md`](../../5_Esercitazione_su_analisi_lessicale/RIASSUNTO.md) | 1h |
| 6 | **Parsing — analisi sintattica** (lacuna chiusa il 2026-05-05) | [`../../notes/08_parsing.md`](../../notes/08_parsing.md) | 2h |
| 7 | Analisi dipendente da contesto + TinyP | [`../../6_Analisi_dipendente_da_contesto/RIASSUNTO.md`](../../6_Analisi_dipendente_da_contesto/RIASSUNTO.md) | 2h |
| 8 | IR, LLVM, GEP, code shape, invoke, access link | [`../../7_Rappresentazione_intermedia_(IR)/RIASSUNTO.md`](../../7_Rappresentazione_intermedia_(IR)/RIASSUNTO.md) | 2-3h |
| 9 | Middle end: dataflow, Available/Very Busy, induction var, passes | [`../../8_Il_middle_end_analisi_e_ottimizzazioni/RIASSUNTO.md`](../../8_Il_middle_end_analisi_e_ottimizzazioni/RIASSUNTO.md) | 2-3h |
| 10 | Interpretazione astratta: Knaster-Tarski, Kleene, soundness, widening | [`../../9_Cenni_su_interpretazione_astratta/RIASSUNTO.md`](../../9_Cenni_su_interpretazione_astratta/RIASSUNTO.md) | 1.5h |

> **Nota:** il parsing non ha cartella di lezione dedicata — il file `notes/08_parsing.md` (~670 righe) è la fonte unica. Va inserito tra il lexer e l'analisi dipendente da contesto.

## Argomenti caldi (alta probabilità all'orale)

Da `notes/01_esame.md`:
- Specifica vs implementazione del lexer (RE vs DFA), Thompson + subset + Hopcroft
- CFG, parsing top-down/bottom-up, derivazione, ambiguità
- Attribute grammars (sintetizzati vs ereditati) vs ad-hoc SDT
- Tipi di IR (strutturale/lineare/ibrida), 3-address code, SSA
- Procedure abstraction: activation record, calling convention, run-time vs compile-time
- vtable e dynamic dispatch
- LVN, dominator trees, dataflow (Live Variables come caso canonico)
- Punti fissi, monotonia, lattice → ponte con interpretazione astratta
- Galois connection
- Esempi concreti di Flex e di interprete/AST (TinyP)

## Punti su cui spesso ci si incarta

- **Compile-time vs run-time**: cosa esiste solo a compile-time (parse tree, symbol table, AST) vs solo a run-time (activation record, vtable, heap)
- **Punto fisso & monotonia**: non basta dire "esiste"; saper giustificare con Knaster-Tarski/Kleene
- Le 3 strategie del `case` (cascade, binary, jump table) e quando si scelgono
- Differenza LL (top-down predittivo) vs LR (bottom-up shift-reduce)

## Output atteso

Alla fine di questo blocco devi essere in grado di:
- Descrivere a voce l'intera pipeline di compilazione di un piccolo programma C
- Disegnare 2-3 esempi RE → NFA → DFA → DFA minimo
- Rispondere alle "Punti chiave per l'orale" di tutti e 9 i RIASSUNTO senza esitare
