# 6. Analisi dipendente dal contesto (semantic elaboration)

## Materiali
- `15-Context_sensitive_Analysis_I.pptx`, `16-Context_sensitive_Analysis_II.pptx` (slide del docente)
- `L16ContextSensitive-1up.pdf`, `L17AdHoc-1up.pdf`, `L18TypeAnalysis-1up.pdf` (slide EaC, Comp 412)
- `TinyP/` — implementazione completa di un toy language (mini compilatore in C++)

## Cos'è la "context-sensitive analysis"
Anche detta **semantic elaboration**: tutto quello che è "deeper than syntax", cioè **non esprimibile con una CFG**. Domande tipiche:
- `x` è scalare, array, funzione? È stato dichiarato?
- L'espressione `x*y+z` è type-consistent?
- `a[i,j,k]` ha 3 dimensioni?
- Quanti argomenti vuole `printf`?
- `*p` punta a memoria allocata? `p` e `q` aliasano?
- `x` è inizializzata prima dell'uso?

Le risposte dipendono da **valori**, non da parti del discorso, richiedono informazione **non locale** e **calcolo**.

## Due strade
### 1. Approccio formale: Attribute Grammars (AG)
- CFG **augmentata da regole** che calcolano *attributi* su ogni nodo del parse tree
- Le regole sono funzionali → definiscono univocamente i valori
- Due tipi di attributi:
  - **Sintetizzati (synthesized):** dipendono dai figli → S-attributed grammars, valutabili in single-pass bottom-up (LR)
  - **Ereditati (inherited):** dipendono da padre/fratelli → esprimono il contesto naturalmente, ma più difficili da valutare in fase di parsing

**Esempio classico:** numeri binari con segno. Si calcolano `pos` (posizione, ereditato) e `val` (valore, sintetizzato) per `Number → Sign List`.

**Metodi di valutazione:**
- *Dynamic, dependence-based:* costruisci il parse tree, costruisci il grafo delle dipendenze, topological sort, valuta
- *Rule-based (treewalk):* ordine fisso analizzato a generation-time
- *Oblivious (passes/dataflow):* ignora regole/parse tree, scegli un ordine convenuto

**Problemi pratici delle AG:**
- "Copy rules" che fanno solo copia di attributi → overhead cognitivo e di memoria
- Non locale = molte regole di propagazione
- Bisogna costruire l'intero parse tree e poi cercarci o copiare al root

### 2. Approccio ad-hoc (vince in pratica)
- **Symbol table** centralizzata (hash) come repository unico dei fatti
- **Ad-hoc syntax-directed translation:** ad ogni produzione del parser bottom-up si associa uno snippet di codice eseguito alla riduzione
- In Yacc/Bison: `$$`, `$1`, ..., `$n` per gli attributi
- Si fa lookup/insert nella symbol table; gli attributi locali (es. `cost`) si propagano via `$$ ← $1 + COST(...) + $3`
- "Trick" del *split production* per inserire un'azione iniziale: aggiungi `Init → ε { cost ← 0 }` prima di `Block`

In **parsing**, i formalismi (CFG, LL/LR) hanno vinto. Nella **CSA** dominano le tecniche ad-hoc.

## Type analysis (slide L18)
- Un sistema di tipi assegna un tipo ad ogni espressione e verifica consistenza
- Tabelle di tipi binarie per gli operatori (es. `F+`, `F-`, `F*`, `F/`) gestiscono promozioni implicite (`int16 × float → float`, ...)
- Esempio di SDT: `Expr → Expr + Term  { $$ = F+($1, $3) }`
- L'intera analisi tipica produce un **AST arricchito** con tipo, scope, conversioni implicite

### Esempio di propagazione tipi su `(a + b) * c`
Sia `a: int`, `b: float`, `c: int`. Tabella `F+`:
```
F+      | int   | float
--------+-------+-------
int     | int   | float    (con promozione implicita int→float)
float   | float | float
```
Tabella `F*` analoga.

Parser bottom-up, riduzioni nell'ordine in cui escono dallo shift-reduce. Per ogni nodo dell'AST associo un attributo `.type` e (eventualmente) `.coerce` (cast da inserire).

```
1. atom 'a' (int)         → Expr.type = int
2. atom 'b' (float)       → Expr.type = float
3. Expr1 + Expr2:
     F+(int, float) = float
     → Expr.type = float
     → su Expr1 inserisci coercion int → float (es. nodo Cast in AST)
4. atom 'c' (int)          → Expr.type = int
5. Expr3 * Expr4:
     F*(float, int) = float
     → Expr.type = float
     → su Expr4 inserisci coercion int → float
```

AST risultante (arricchito):
```
       *  : float
      / \
     +    Cast(int→float) : float
   : float          \
   /     \           c : int
  Cast    b
 (int→fl) : float
   |
   a : int
```

L'output dell'analisi semantica statica non è solo "ok / non ok": è un AST in cui ogni nodo ha tipo, e i cast impliciti del linguaggio sono diventati nodi espliciti che il middle end potrà ottimizzare o tradurre in IR.

---

## TinyP — il mini compilatore di esempio

### Linguaggio (vedi `README`)
Grammatica BNF semplice con:
- statement: `;`, assegnamento `id = aexpr;`, `if`, `if/else`, `while`, `do/while`, blocco `{...}`, `assert(bexpr);`, `output aexpr;`
- bexpr: `or`, `and`, `not`, parentesi, batom (confronti aritmetici, `true`, `false`)
- aexpr: `+`, `-`, `*`, `/`, `%`, unari, parentesi, atom
- atom: int_literal, ident, `input[lo, hi]` (input non deterministico in un range)

### Architettura del codice (file principali)
- **`ast.hh`/`ast_visitor.hh`** — gerarchia di nodi AST (smart pointers `unique_ptr`, deep clone, **Visitor pattern** via `accept()/visit()`)
- **`lexer.ll`** — lexer Flex
- **`parser.yy`** — parser Bison
- **`pretty_printer.cc`** — un visitor che stampa il programma (formattato)
- **`ast_dumper.cc`** — un visitor che stampa l'AST
- **`interpreter.cc`** — un visitor che esegue il programma (semantica concreta)
- **`collecting.cc`** — *collecting semantics* (insiemi di stati raggiungibili) — base per analisi statiche e per il successivo argomento di Interpretazione Astratta
- **`utils.cc/.hh`** — utility (gestione errori, GMP)
- **`tinyP-pp.cc`**, **`tinyP-astdump.cc`**, **`tinyP-run.cc`**, **`tinyP-concrete.cc`** — eseguibili separati che combinano il visitor scelto

Build con `make all` → 4 binari (`tinyP-pp`, `tinyP-astdump`, `tinyP-run`, `tinyP-concrete`). Dipendenze: `flex`, `bison`, `clang++` (C++14), libreria **GMP** (`gmpxx`/`gmp`) per interi a precisione arbitraria.

### Pattern e tecniche utilizzate (utili da spiegare all'orale)
- **Visitor pattern** per separare la struttura dei nodi dalle operazioni (clone/dump/print/run/collect) → fondamentale per i compilatori
- AST **immutabile** (copie/spostamenti vietati, deep clone esplicito)
- Macro `ACCEPT_VISITOR` per ridurre boilerplate
- **GMP** per non avere overflow nel mock di esecuzione concreta
- Eseguibili distinti per ogni "fase" → mostra la separazione delle responsabilità

## Punti chiave per l'orale
- Cos'è un'**attribute grammar**, attributi **sintetizzati vs ereditati**, dipendence graph aciclico
- Differenza tra approccio AG (formalismo) e ad-hoc SDT (pratica), e perché in pratica vince l'ad-hoc
- Ruolo della **symbol table** come repository centralizzato
- Esempi di domande risolvibili solo dalla CSA (tipi, dimensioni array, dichiarazioni, alias)
- Type system: regole come `β |= e1: int  β |= e2: int  ⇒  β |= e1 == e2 : bool`
- Conoscere TinyP a sufficienza per parlarne come **caso di studio**: il visitor pattern, la separazione lexer/parser/AST/visitor, l'uso della collecting semantics come ponte verso l'interpretazione astratta
