# Schema di studio — Linguaggi, interpreti e compilatori

Percorso consigliato per documentarsi, studiare e prepararsi all'esame.

---

## 🎯 Step 0 — Inquadramento (15 min)

Prima di tutto, leggi:

1. **`notes/00_INDEX.md`** — mappa di tutto il materiale
2. **`notes/01_esame.md`** — modalità d'esame (orale vs seminario vs progetto, scadenze)
3. **`notes/05_pipeline_compilatore.md`** — vista d'insieme: dal `.c` al binario, end-to-end

Obiettivo: capire **dove si va a parare** prima di iniziare a studiare i singoli pezzi.

---

## 📚 Step 1 — Studio lineare per argomento

Procedi in ordine, una cartella alla volta. Per ognuna:

1. Leggi il **`RIASSUNTO.md`** della cartella
2. Apri il **PDF del docente** (in italiano) e leggilo facendo riferimento al riassunto
3. Se ti serve approfondimento, apri le **slide EaC** (in inglese) o il **libro Cooper-Torczon**
4. Ferma alla sezione "**Punti chiave per l'orale**" del riassunto e prova a rispondere a voce

| Ordine | Cartella | Tempo stimato | Note |
|---|---|---|---|
| 1 | `1_Introduzione_al_corso/` | 20 min | Generalità + scaletta |
| 2 | `2_Interpreti_e_compilatori/` | 30 min | Definizioni base |
| 3 | `3_Struttura_del_compilatore/` | 1h | Overview front/middle/back end |
| 4 | `4_Analisi_lessicale/` | 2h | RE, NFA, DFA, Hopcroft, Flex |
| 5 | `5_Esercitazione_su_analisi_lessicale/` | 1h | **Apri il `lexer-cpp.ll` e leggilo** |
| 6 | `6_Analisi_dipendente_da_contesto/` | 2h | AG, ad-hoc SDT, type checking + TinyP |
| 7 | `7_Rappresentazione_intermedia_(IR)/` | 2h | IR, LLVM, procedure abstraction, code shape |
| 8 | `8_Il_middle_end_analisi_e_ottimizzazioni/` | 2h | LVN, dominators, dataflow |
| 9 | `9_Cenni_su_interpretazione_astratta/` | 1.5h | Cousot, Galois, widening |

**Totale primo passaggio:** ~12 ore. Distribuiscile su 4-6 sessioni di 2-3 ore.

---

## 🔧 Step 2 — Pratica (laboratorio)

Toccare il codice rende tutto più concreto e ti dà esempi pronti per l'orale.

### 2a. Lexer C++ (cartella 5)
```bash
cd 5_Esercitazione_su_analisi_lessicale
make
echo 'int main() { return 0; }' | ./lexer-cpp
```
- Modifica una regola, ricompila, osserva il cambiamento
- Aggiungi una keyword tua e prova
- Spiegati a voce ogni sezione del `lexer-cpp.ll`

### 2b. TinyP (cartella 6)
```bash
cd 6_Analisi_dipendente_da_contesto/TinyP
make all          # richiede flex, bison, clang++, libgmp
```
Quattro binari prodotti:
- `tinyP-pp` — pretty printer
- `tinyP-astdump` — dump AST
- `tinyP-run` — interprete (esecuzione concreta)
- `tinyP-concrete` — collecting semantics (insieme stati raggiungibili)

Scriviti un programmino TinyP semplice e prova tutti e 4:
```
x = input[0, 10];
y = 0;
while (x > 0) {
  y = y + x;
  x = x - 1;
}
output y;
```

Osservazione utile: il `collecting.cc` è il **ponte concettuale** verso l'interpretazione astratta della cartella 9.

### 2c. LLVM playground
```bash
echo 'int square(int x) { return x*x; }' > sq.c
clang -O0 -S -emit-llvm sq.c -o sq.ll        # IR senza ottimizzazioni
clang -O2 -S -emit-llvm sq.c -o sq.opt.ll    # IR ottimizzato
diff sq.ll sq.opt.ll                          # vedi cosa ha fatto opt
```
Confronta i due file: noterai mem2reg, instcombine e altre ottimizzazioni viste nella cartella 8.

---

## 🧠 Step 3 — Approfondimento mirato

Solo per gli argomenti su cui ti senti debole o vuoi padroneggiare meglio.

| Argomento | Risorsa primaria | Risorsa secondaria |
|---|---|---|
| Da RE a DFA | `4_Analisi_lessicale/05-Regular_Expressions_NFAs_DFAs.pptx` | EaC cap. 2 |
| Hopcroft minimization | `07-DFA_Minimization.pptx` | Wikipedia |
| Parsing LL/LR | EaC cap. 3 | (Dragon Book) |
| Attribute grammars | `L16ContextSensitive-1up.pdf` | EaC cap. 4 |
| LLVM IR | `LLVM-IR.pdf` + LangRef ufficiale | https://llvm.org/docs/LangRef.html |
| Procedure abstraction | EaC cap. 6, slide `18`/`20` | — |
| OOL (vtable, dispatch) | slide `22`/`23` | — |
| Code shape | slide `24`/`25`/`26` | EaC cap. 7 |
| Dataflow analysis | slide `30`, `13DFA` | EaC cap. 8.6 |
| Dominators | `08Dominators-1up.pdf` | Lengauer-Tarjan paper |
| Value numbering | slide `27`/`29` | EaC cap. 8.4-8.5 |
| Interpretazione astratta | `cousot-tutorial.pdf` | `mine-tutorial.pdf` |

---

## 🔁 Step 4 — Ripasso (giorni prima dell'esame)

### Giorno -7
- Rileggi tutti e 9 i `RIASSUNTO.md` saltando direttamente ai **"Punti chiave per l'orale"**
- Tempo: 1.5h

### Giorno -5
- Studia `notes/02_cheatsheet.md` riga per riga
- Memorizza le **equazioni dataflow** (Live, Reaching, Available)
- Memorizza l'**algoritmo Hopcroft** e il **LVN**
- Tempo: 2h

### Giorno -3
- `notes/04_domande_tipo.md`: prova a rispondere **a voce** alle 44 domande
- Per ogni domanda su cui esiti, torna al `RIASSUNTO.md` corrispondente
- Tempo: 3h

### Giorno -1
- Scorri il `notes/03_glossario.md` per fissare i termini
- Riguarda `notes/05_pipeline_compilatore.md` per avere chiaro il quadro d'insieme
- Riposa.

---

## 🎤 Step 5 — Strategia per l'orale

### Cosa portare in testa (sicuro)
- Schema **front-end / middle-end / back-end** disegnabile a memoria
- Pipeline **RE → NFA → DFA → DFA minimo** (con autori: Thompson, Rabin-Scott, Hopcroft)
- Differenza **specifica vs implementazione** (RE vs DFA, CFG vs PDA)
- Equazioni **Live Variables** scritte correttamente
- Definizione di **dominator** e algoritmo iterativo
- Cos'è la **forma SSA** e a cosa serve la φ
- Cos'è una **Galois connection** e perché si rinuncia alla completezza

### Esempi concreti da citare
- **Flex** (cartella 4-5): saper descrivere la struttura `.ll` e cosa fa `yylex`
- **TinyP** (cartella 6): visitor pattern, separazione lexer/parser/AST, collecting semantics
- **Clang/LLVM**: pipeline reale, comandi (`clang -emit-llvm`, `opt`, `llc`)
- **Astrée** (interpretazione astratta): caso d'uso industriale (Airbus)

### Trappole tipiche
- ❌ Confondere **compile-time** e **run-time** (es. activation record è run-time, ma il codice che lo costruisce è emesso a compile-time)
- ❌ Dire "ottimo" invece di "migliore di prima" — l'ottimo non è raggiungibile
- ❌ Non saper giustificare l'esistenza del punto fisso (Knaster-Tarski / Kleene)
- ❌ Confondere parse tree e AST (l'AST è più piccolo, perde info di parsing)

### Se ti chiedono qualcosa che non sai
- **Non bluffare.** Ammetti e ragiona ad alta voce su ciò che sai di correlato
- I docenti apprezzano chi mostra **metodo** anche di fronte a una domanda imprevista

---

## 🚀 Bonus — Se vuoi davvero fare bella figura

- Scegli **seminario o progetto** invece dell'orale puro: ti permette di prepararti su un argomento specifico in profondità (max 15 min, argomento concordato con docente almeno 30 gg prima)
- Idee per progetto: estendere TinyP con un nuovo costrutto (array, funzioni), implementare un'analisi statica nuova, scrivere un pass LLVM di esempio
- Idee per seminario: una tecnica di ottimizzazione moderna (PGO, LTO, autovectorization), un dominio astratto specifico (poliedri, octagons), un parser combinator

---

## ⏱ Budget di tempo realistico

| Profilo | Tempo totale | Distribuzione |
|---|---|---|
| **Minimo per passare** | ~15h | 12h studio + 3h ripasso |
| **Voto buono** | ~25h | 12h studio + 6h pratica + 4h approfondimenti + 3h ripasso |
| **Lode/seminario** | ~40h+ | tutto sopra + 15h sull'argomento del seminario |

---

## 📌 Reminder finale

> "Le risposte dipendono da **valori**, non da parti del discorso, e richiedono **calcolo non locale**." — questa frase su CSA + il fatto che in pratica vince l'**ad-hoc** invece del formalismo sono **la** lezione del corso. Se la capisci, hai capito l'architettura di un compilatore moderno.

In bocca al lupo. 🐺
