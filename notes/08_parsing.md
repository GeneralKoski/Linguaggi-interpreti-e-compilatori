# Analisi sintattica (parsing)

L'analisi sintattica è la fase del compilatore che, a partire dal flusso di token prodotto dal lexer, costruisce un albero di derivazione (o direttamente un AST) verificando che la sequenza appartenga al linguaggio descritto da una grammatica context-free. È il secondo stadio della pipeline (vedi `05_pipeline_compilatore.md`) ed è strettamente legata all'analisi semantica che la segue (vedi `06_dataflow.md` per il post-processing su CFG).

---

## 1. CFG, derivazioni, alberi

### Definizione formale

Una grammatica context-free è una quadrupla `G = ⟨S, N, T, P⟩` dove:
- `N` = insieme finito di **non terminali** (simboli sintattici, es. `Expr`, `Stmt`)
- `T` = insieme finito di **terminali** (token, es. `id`, `+`, `(`)
- `P` = insieme finito di **produzioni** della forma `A → α` con `A ∈ N` e `α ∈ (N ∪ T)*`
- `S ∈ N` = simbolo iniziale (start symbol)

Convenzione: maiuscole per non terminali, minuscole/simboli per terminali, `ε` per la stringa vuota.

Esempio (grammatica G1, espressioni aritmetiche):
```
E → E + T | T
T → T * F | F
F → ( E ) | id
```

### Derivazioni

Una derivazione `α ⇒ β` è l'applicazione di una produzione `A → γ` a una occorrenza di `A` in `α`. Si scrive `⇒*` per la chiusura riflessivo-transitiva.

- **Leftmost (LM):** ad ogni passo si riscrive il non terminale più a sinistra. Notazione `⇒_lm`.
- **Rightmost (RM, o canonica):** ad ogni passo si riscrive il non terminale più a destra. Notazione `⇒_rm`.

I parser top-down producono derivazioni leftmost; i bottom-up producono derivazioni rightmost al contrario (riduzioni).

Esempio LM su `id + id * id` con G1:
```
E ⇒ E + T ⇒ T + T ⇒ F + T ⇒ id + T ⇒ id + T * F
  ⇒ id + F * F ⇒ id + id * F ⇒ id + id * id
```

Esempio RM stessa stringa:
```
E ⇒ E + T ⇒ E + T * F ⇒ E + T * id ⇒ E + F * id
  ⇒ E + id * id ⇒ T + id * id ⇒ F + id * id ⇒ id + id * id
```

### Alberi sintattici

L'**albero di derivazione** (parse tree, o concrete syntax tree) ha:
- radice = `S`
- ogni nodo interno è un non terminale, i suoi figli sono il corpo della produzione applicata
- foglie = terminali (lette in ordine danno la stringa)

L'**AST** (abstract syntax tree) è una versione compatta: rimuove parentesi, terminali ridondanti, nodi unari di catena (es. `T → F → id` collassa a `id`). È la struttura dati passata alle fasi successive (analisi semantica, IR generation).

Esempio: per `id + id * id`, il parse tree concreto ha 11 nodi; l'AST è semplicemente:
```
    +
   / \
  id   *
      / \
     id  id
```

### Ambiguità

Una grammatica `G` è **ambigua** se esiste almeno una stringa con due o più alberi di derivazione distinti (equivalentemente: due derivazioni leftmost distinte).

Esempio classico — espressioni senza precedenza:
```
E → E + E | E * E | id
```
La stringa `id + id * id` ammette due alberi:
- `(id + id) * id`
- `id + (id * id)`

Esempio classico — **dangling else**:
```
S → if E then S
  | if E then S else S
  | other
```
La stringa `if E1 then if E2 then S1 else S2` può essere parserizzata legando `else` al primo o al secondo `if`.

L'ambiguità è un problema grammaticale, non del parser: il parser non può "indovinare" il significato.

### Risoluzione: riscrittura della grammatica

**Precedenza e associatività via stratificazione**: si introduce un livello (non terminale) per ogni livello di precedenza, e si rende `+` associativo a sinistra usando ricorsione sinistra:
```
E → E + T | T          (+ left-assoc, bassa precedenza)
T → T * F | F          (* left-assoc, alta precedenza)
F → ( E ) | id
```
Ora `id + id * id` ha un solo albero, con `*` più "in profondità".

**Dangling else**: si forza `else` a legarsi al `then` più vicino:
```
S  → matched | unmatched
matched   → if E then matched else matched | other
unmatched → if E then S
          | if E then matched else unmatched
```

Alternativa pratica: lasciare la grammatica ambigua e dichiarare al parser generator (Bison) `%nonassoc IFX; %nonassoc ELSE` con `ELSE` a precedenza maggiore.

### Domande tipiche all'orale (sez. 1)
- *Quando una grammatica è ambigua?* → Esiste una stringa con due alberi di derivazione (o due LM derivation).
- *Come elimini l'ambiguità per le espressioni?* → Stratifico per precedenza e uso ricorsione sinistra per associatività a sinistra.
- *Differenza parse tree vs AST?* → Il primo riflette ogni produzione applicata; l'AST è astratto, omette nodi inutili e parentesi.

---

## 2. Gerarchia dei parser

### Top-down vs bottom-up

| Aspetto | Top-down | Bottom-up |
|---|---|---|
| Costruzione albero | dalla radice alle foglie | dalle foglie alla radice |
| Derivazione prodotta | leftmost | rightmost (al contrario) |
| Operazioni | predict + match | shift + reduce |
| Implementazione tipica | recursive descent / LL(1) | LR(1), LALR(1) |
| Ricorsione sinistra | NON ammessa | ammessa |
| Potenza | minore | maggiore |

### Classi di grammatiche

- **LL(k):** parsing top-down con `k` token di lookahead. LL(1) è il caso pratico.
- **LR(k):** parsing bottom-up con `k` token di lookahead. LR(0), SLR(1), LALR(1), LR(1) sono varianti per ordine crescente di potenza.
- **SLR(1):** LR(0) + lookahead derivato da `FOLLOW`.
- **LALR(1):** LR(1) con stati a stesso core fusi → tabelle compatte (Bison/Yacc).
- **LR(1) canonico:** massima potenza pratica, ma tabelle enormi.

### Inclusioni (per le classi di grammatiche)

```
LL(1)  ⊊  LALR(1)  ⊊  LR(1)  ⊊  CFG non ambigue  ⊊  CFG
SLR(1) ⊊  LALR(1)
LL(0)  ⊊  LL(1)  ⊊  LL(2)  ...
```
Tutte le grammatiche LR(k) per `k ≥ 1` sono non ambigue. Una grammatica ambigua non può essere LR(k) per nessun `k`.

---

## 3. Parser top-down: LL(1)

### Idea

Da uno stato `(stack, input)` si espande il non terminale in cima allo stack consultando il prossimo token. La scelta della produzione è deterministica grazie a una tabella `M[A, a]`.

### FIRST e FOLLOW

**FIRST(α)** = insieme dei terminali che possono iniziare una stringa derivabile da `α`. Se `α ⇒* ε`, allora `ε ∈ FIRST(α)`.

Algoritmo (per ogni simbolo `X`):
```
FIRST(a) = {a}                        per a terminale
FIRST(ε) = {ε}
Per X → Y1 Y2 ... Yn:
  aggiungi a FIRST(X) tutti i simboli di FIRST(Y1) \ {ε}
  se ε ∈ FIRST(Y1): aggiungi FIRST(Y2) \ {ε}
  ... e così via
  se ε ∈ FIRST(Yi) per ogni i: aggiungi ε a FIRST(X)
Iterare fino a punto fisso.
```

**FOLLOW(A)** = insieme dei terminali che possono apparire immediatamente dopo `A` in qualche derivazione da `S`. Il marcatore `$` (end-of-input) è in `FOLLOW(S)`.

Algoritmo:
```
FOLLOW(S) ⊇ {$}
Per ogni produzione B → α A β:
  FOLLOW(A) ⊇ FIRST(β) \ {ε}
  se ε ∈ FIRST(β) (o β è vuoto): FOLLOW(A) ⊇ FOLLOW(B)
Iterare fino a punto fisso.
```

### Esempio numerico FIRST/FOLLOW

Grammatica G2 (G1 con ricorsione sinistra eliminata, vedi sotto):
```
E  → T E'
E' → + T E' | ε
T  → F T'
T' → * F T' | ε
F  → ( E ) | id
```

Calcolo FIRST:
- `FIRST(F) = { (, id }`
- `FIRST(T) = FIRST(F) = { (, id }` (perché `T → F T'` e `ε ∉ FIRST(F)`)
- `FIRST(T') = { *, ε }`
- `FIRST(E) = FIRST(T) = { (, id }`
- `FIRST(E') = { +, ε }`

Calcolo FOLLOW:
- `FOLLOW(E) = { $, ) }` (da `F → ( E )` e dal fatto che `E` è start)
- `FOLLOW(E') = FOLLOW(E) = { $, ) }` (da `E → T E'` con β = ε per E')
- `FOLLOW(T) = (FIRST(E') \ {ε}) ∪ FOLLOW(E) ∪ FOLLOW(E') = { +, $, ) }`
- `FOLLOW(T') = FOLLOW(T) = { +, $, ) }`
- `FOLLOW(F) = (FIRST(T') \ {ε}) ∪ FOLLOW(T) = { *, +, $, ) }`

### Tabella di parsing LL(1)

Costruzione: per ogni produzione `A → α`:
1. Per ogni `a ∈ FIRST(α) \ {ε}`: poni `M[A, a] = A → α`.
2. Se `ε ∈ FIRST(α)`: per ogni `b ∈ FOLLOW(A)` poni `M[A, b] = A → α`.

La grammatica è **LL(1)** se ogni cella della tabella contiene al più una produzione (nessun conflitto).

Tabella per G2:

|       | id            | +             | *             | (             | )             | $             |
|-------|---------------|---------------|---------------|---------------|---------------|---------------|
| `E`   | `E → T E'`    |               |               | `E → T E'`    |               |               |
| `E'`  |               | `E' → + T E'` |               |               | `E' → ε`      | `E' → ε`      |
| `T`   | `T → F T'`    |               |               | `T → F T'`    |               |               |
| `T'`  |               | `T' → ε`      | `T' → * F T'` |               | `T' → ε`      | `T' → ε`      |
| `F`   | `F → id`      |               |               | `F → ( E )`   |               |               |

Nessun conflitto → G2 è LL(1).

### Algoritmo di parsing table-driven

```
Stack: [ $, S ]
Input: w $
ripeti:
  X = top(Stack); a = next(Input)
  se X == a == $: ACCEPT
  se X è terminale:
    se X == a: pop; consume(a)
    altrimenti: ERROR
  se X è non terminale:
    se M[X, a] = X → Y1 Y2 ... Yk:
      pop X; push Yk, ..., Y2, Y1   (inverso, così Y1 è in cima)
    altrimenti: ERROR
```

### Eliminazione della ricorsione sinistra

Una grammatica `LL(1)` non può avere ricorsione sinistra (la cella `M[A, a]` per `A → A α` finirebbe in conflitto con se stessa: per espandere `A` con `A → A α` bisognerebbe già espandere `A`).

Algoritmo (immediata):
```
Per ogni A → A α1 | A α2 | ... | β1 | β2 | ... | βm  (β_i non inizia con A):
  riscrivi:
    A  → β1 A' | β2 A' | ... | βm A'
    A' → α1 A' | α2 A' | ... | αn A' | ε
```

Esempio: `E → E + T | T` diventa `E → T E'; E' → + T E' | ε`.

Per ricorsione sinistra **indiretta** (es. `A → B α; B → A β`): si ordinano i non terminali, si sostituisce `B` in `A` finché tutta la ricorsione è diretta, poi si applica l'algoritmo sopra.

### Left factoring

Quando due alternative iniziano con lo stesso prefisso il parser non può scegliere con 1 token di lookahead.

```
A → α β1 | α β2 | ... | α βn | γ
   ↓ left factoring
A  → α A' | γ
A' → β1 | β2 | ... | βn
```

Esempio classico:
```
S → if E then S else S | if E then S | other
   ↓
S  → if E then S S' | other
S' → else S | ε
```

### Recursive descent vs table-driven

- **Recursive descent**: una funzione per ogni non terminale, lo `switch` interno usa il lookahead per scegliere la produzione. Codice scritto a mano, naturale per le azioni semantiche, semplice da debuggare.
- **Table-driven**: tabella `M[A, a]` + stack esplicito + driver generico. Compatto ma meno leggibile.

Esempio recursive descent per G2 (frammento):
```c
void E()  { T(); E_prime(); }
void E_prime() {
  if (lookahead == '+') { match('+'); T(); E_prime(); }
  /* altrimenti: ε, esci */
}
void T()  { F(); T_prime(); }
void F()  {
  if (lookahead == '(') { match('('); E(); match(')'); }
  else if (lookahead == ID) match(ID);
  else error();
}
```

### Esempio end-to-end: parsing di `id + id * id` con G2

Trace `(stack | input | azione)`:
```
$ E              | id + id * id $ | E → T E'
$ E' T           | id + id * id $ | T → F T'
$ E' T' F        | id + id * id $ | F → id
$ E' T' id       | id + id * id $ | match id
$ E' T'          |    + id * id $ | T' → ε
$ E'             |    + id * id $ | E' → + T E'
$ E' T +         |    + id * id $ | match +
$ E' T           |      id * id $ | T → F T'
$ E' T' F        |      id * id $ | F → id
$ E' T' id       |      id * id $ | match id
$ E' T'          |        * id $  | T' → * F T'
$ E' T' F *      |        * id $  | match *
$ E' T' F        |          id $  | F → id
$ E' T' id       |          id $  | match id
$ E' T'          |             $  | T' → ε
$ E'             |             $  | E' → ε
$                |             $  | ACCEPT
```

### Domande tipiche all'orale (sez. 3)
- *Cosa fa FIRST? E FOLLOW?* → Definizione + uso (FIRST per scegliere, FOLLOW per gestire `ε`).
- *Quando una grammatica è LL(1)?* → Tabella senza conflitti, equivalente a: per ogni `A → α | β`, `FIRST(α) ∩ FIRST(β) = ∅` e se `ε ∈ FIRST(α)` allora `FIRST(β) ∩ FOLLOW(A) = ∅`.
- *Perché LL(1) non tollera la ricorsione sinistra?* → Loop infinito di espansioni senza consumare input.
- *Differenza tra recursive descent e table-driven?* → Stessa potenza, cambia l'implementazione.

---

## 4. Parser bottom-up: LR(1)

### Idea

Si mantiene uno **stack di stati** e si legge l'input da sinistra a destra. Ad ogni passo si decide tra:
- **shift**: spostare il prossimo token sullo stack;
- **reduce A → β**: il top dello stack contiene `β`, lo si sostituisce con `A`;
- **accept**: input riconosciuto;
- **error**.

La sequenza di reduce, letta dal basso verso l'alto, è una derivazione rightmost al contrario.

### Item LR(1)

Un **item LR(1)** è una coppia
```
[A → α · β, a]
```
- `A → αβ` è una produzione di `G`;
- il `·` (dot) indica fin dove la produzione è stata "vista" sullo stack;
- `a ∈ T ∪ {$}` è il **lookahead**: il terminale che ci si aspetta dopo aver ridotto a `A`.

Interpretazione: "potrei star riconoscendo `α β` per ridurre ad `A`, e dopo `A` mi aspetto `a`".

### closure(I)

```
closure(I):
  J = I
  ripeti:
    per ogni item [A → α · B β, a] in J,
    per ogni produzione B → γ,
    per ogni b ∈ FIRST(β a):
      aggiungi [B → · γ, b] a J
  finché J non cambia
  return J
```

Intuizione: se sullo stack potrei avere `α` e mi aspetto di leggere `B β a`, allora a partire da ora potrei iniziare a riconoscere `B → γ`, e dopo `B` mi aspetto qualcosa in `FIRST(β a)`.

### goto(I, X)

```
goto(I, X) = closure({ [A → α X · β, a] | [A → α · X β, a] ∈ I })
```

È la transizione dell'automa LR(1): dallo stato `I` leggendo `X` (terminale o non terminale) finisco nello stato risultante.

### Costruzione della collezione canonica di stati

```
C = { closure({[S' → · S, $]}) }   -- S' → S è la produzione aumentata
ripeti:
  per ogni I in C, per ogni simbolo X:
    se goto(I, X) ≠ ∅ e goto(I, X) ∉ C:
      aggiungi goto(I, X) a C
finché C non cambia
```

### Tabelle ACTION e GOTO

Per ogni stato `I_i`:
- Se `[A → α · a β, b] ∈ I_i` e `goto(I_i, a) = I_j`: `ACTION[i, a] = shift j`.
- Se `[A → α ·, a] ∈ I_i` (item completo, dot in fondo) e `A ≠ S'`: `ACTION[i, a] = reduce A → α`.
- Se `[S' → S ·, $] ∈ I_i`: `ACTION[i, $] = accept`.
- Se `goto(I_i, A) = I_j` per `A ∈ N`: `GOTO[i, A] = j`.
- Tutte le celle non riempite: error.

La grammatica è **LR(1)** se non ci sono conflitti (più di una azione in una cella).

### Algoritmo shift-reduce

```
Stack: [0]                       -- contiene stati interlavati con simboli
Input: w $
ripeti:
  s = top(Stack); a = next(Input)
  caso ACTION[s, a]:
    shift t:    push(a); push(t); consume(a)
    reduce A → β:
      pop 2*|β| elementi (simbolo+stato per ogni elemento di β)
      s' = top(Stack); push(A); push(GOTO[s', A])
      emit "A → β"     -- output della derivazione rm al contrario
    accept:    return OK
    error:     errore sintattico
```

### Esempio piccolo end-to-end

Grammatica G3 (versione semplificata):
```
S → C C
C → c C | d
```
Aumentata: `S' → S`. Nota: G3 è LR(1) ma non LL(1).

Stato `I0 = closure({[S' → · S, $]})`:
```
[S' → · S, $]
[S  → · C C, $]
[C  → · c C, c/d]
[C  → · d,   c/d]
```

`goto(I0, c) = I3`:
```
[C → c · C, c/d]
[C → · c C, c/d]
[C → · d,   c/d]
```

`goto(I0, d) = I4`:
```
[C → d ·, c/d]
```
Item completo con lookahead `c` o `d` → in stato 4 si riduce `C → d` su input `c` o `d`.

`goto(I0, S) = I1`:
```
[S' → S ·, $]
```
Stato di accept su `$`.

`goto(I0, C) = I2`:
```
[S → C · C, $]
[C → · c C, $]
[C → · d,   $]
```

E così via fino a chiudere la collezione (stati `I0..I9`). La tabella ACTION/GOTO segue la regola sopra.

Trace di parsing su `c d d $`:
```
Stack            | Input    | Azione
0                | c d d $  | shift 3
0 c 3            |   d d $  | shift 4
0 c 3 d 4        |     d $  | reduce C → d
0 c 3 C 6        |     d $  | reduce C → c C   (stato 6: C → c C ·, lookahead d)
0 C 2            |     d $  | shift 4'... ecc.
```
(Il dettaglio numerico esatto dipende dalla numerazione degli stati; ciò che conta è il pattern shift-reduce.)

### Domande tipiche all'orale (sez. 4)
- *Cos'è un item LR(1)?* → Produzione con dot e lookahead.
- *A cosa serve closure?* → Espande tutte le produzioni "candidate" a partire da uno stato.
- *Quando si fa reduce?* → Dot in fondo, lookahead corrente compatibile con quello dell'item.
- *Perché LR è più potente di LL?* → Vede l'intero `β` prima di decidere (ha tutto sullo stack), mentre LL deve decidere prima di vedere `β`.

---

## 5. Conflitti

Un conflitto è una cella della tabella ACTION con più di una azione.

### Shift-reduce

Stato in cui sono presenti contemporaneamente:
```
[A → α ·, a]            -- suggerisce reduce A → α su input a
[B → β · a γ, b]        -- suggerisce shift su a
```

**Esempio classico — dangling else:**
```
S → if E then S
  | if E then S else S
  | other
```
Dopo aver ridotto `if E then S`, leggendo `else` il parser non sa se:
- ridurre `S → if E then S` (chiudendo l'`if` interno) → `else` lega all'`if` esterno;
- o fare shift di `else` → `else` lega all'`if` interno (semantica usuale di C/Java).

**Risoluzioni:**
1. Riscrivere la grammatica (vedi sez. 1, `matched`/`unmatched`).
2. Lasciare la grammatica ambigua e dichiarare al parser generator: "in caso di shift-reduce su `else`, preferisci shift". Bison risolve di default i shift-reduce con shift.
3. Usare precedenza: `%nonassoc IFX` con priorità minore di `%nonassoc ELSE`.

### Reduce-reduce

Stato in cui due item completi richiedono di ridurre con produzioni diverse sullo stesso lookahead:
```
[A → α ·, a]
[B → α ·, a]
```
Più "grave" perché spesso indica un errore strutturale della grammatica: due non terminali riconoscono la stessa stringa nello stesso contesto.

**Esempio:**
```
stmt → expr ; | expr ;
expr → id ( params )
expr → id [ params ]
params → id | id , params
params → expr   -- ambiguità: id potrebbe essere un id semplice o un'expr-id
```
Su un singolo `id` come `params`, il parser non sa se ridurre `params → id` o `params → expr → id`.

**Risoluzioni:**
- Riscrivere la grammatica per disambiguare;
- Spostare la disambiguazione in fase semantica (es. C: `T * x` può essere espressione o dichiarazione, risolto via tabella dei tipi);
- Aumentare il lookahead (LR(2), GLR), ma non è LR(1).

### Strategie di risoluzione (riassunto)

| Tipo conflitto | Causa tipica | Fix preferito |
|---|---|---|
| Shift-reduce | Ambiguità su precedenza/associatività | `%left`, `%right` o riscrittura |
| Shift-reduce | Dangling else | Riscrittura `matched/unmatched` o default shift |
| Reduce-reduce | Due non terminali che riconoscono la stessa frase | Ristrutturare la grammatica |

---

## 6. LALR(1) e Bison

### Da LR(1) canonico a LALR(1)

LR(1) canonico produce stati distinti anche quando differiscono solo per i lookahead. LALR(1) **fonde** stati con lo stesso "core" (stessi item ignorando i lookahead) unendo i lookahead.

```
I_i = { [A → α · β, a], [B → γ ·, b] }
I_j = { [A → α · β, c], [B → γ ·, d] }
core uguale → fondi: { [A → α · β, {a,c}], [B → γ ·, {b,d}] }
```

**Trade-off:**
- Tabelle molto più piccole (tipicamente ~10x rispetto a LR(1) canonico): per questo Bison/Yacc usano LALR(1).
- Possono comparire **nuovi reduce-reduce conflict** che in LR(1) canonico non c'erano (la fusione dei lookahead può collidere).
- Non possono comparire nuovi shift-reduce (i shift-reduce dipendono dal core, non dai lookahead).

### Bison nella pratica

Struttura di un file `.y`:
```
%{
  #include <stdio.h>
  int yylex(void);
  void yyerror(const char *);
%}

%token NUM ID
%left  '+' '-'
%left  '*' '/'
%right UMINUS

%%
expr : expr '+' expr   { $$ = $1 + $3; }
     | expr '-' expr   { $$ = $1 - $3; }
     | expr '*' expr   { $$ = $1 * $3; }
     | expr '/' expr   { $$ = $1 / $3; }
     | '-' expr %prec UMINUS  { $$ = -$2; }
     | '(' expr ')'    { $$ = $2; }
     | NUM             { $$ = $1; }
     ;
%%

void yyerror(const char *s) { fprintf(stderr, "%s\n", s); }
int main(void) { return yyparse(); }
```

**Sezioni:**
1. `%{ ... %}` codice C copiato in testa al `.tab.c`;
2. dichiarazioni token e precedenze;
3. `%%` regole (produzioni con azioni semantiche `{ ... }`);
4. `%%` codice C utente (yyerror, main).

**Azioni semantiche:** `$$` è il valore del non terminale a sinistra; `$i` è il valore dell'i-esimo simbolo a destra. Si esegue al momento del reduce.

**Precedenza e associatività:**
- `%left  op` → op associativo a sinistra (`a op b op c` = `(a op b) op c`)
- `%right op` → associativo a destra (es. `=`, `**`)
- `%nonassoc op` → non associativo (`a op b op c` è errore, es. `<` in Pascal)

L'ordine delle dichiarazioni definisce la precedenza: linee successive = precedenza più alta. Bison usa la precedenza dell'**ultimo terminale della produzione** e del lookahead per risolvere shift-reduce.

**Token fittizi e `%prec`:**
Per il meno unario (`-x`) e il meno binario (`a - b`) servono precedenze diverse pur condividendo lo stesso simbolo `-`. Si introduce un token fittizio `UMINUS`:
```
%right UMINUS
...
expr : '-' expr %prec UMINUS { $$ = -$2; }
```
`%prec UMINUS` dice: "per questa produzione, usa la precedenza di `UMINUS`" (più alta di `*` e `+`).

**Error recovery:**
Bison fornisce un token speciale `error`. Aggiungendo regole come:
```
stmt : error ';'   { yyerrok; }
```
il parser, quando incontra un errore in una `stmt`, scarta token finché non trova `;` e riprende. `yyerror(msg)` è la callback per riportare errori; `yyerrok` resetta lo stato di errore.

**Output di `bison -d parser.y`:**
- `parser.tab.c` — codice C del parser (`yyparse()`);
- `parser.tab.h` — `#define` dei token (incluso da Flex);
- opzionale `parser.output` (con `-v`) — descrizione testuale di stati e conflitti, utilissima per il debug.

**Integrazione con Flex:** il lexer (`yylex`) restituisce token codificati come interi (i `#define` di `.tab.h`) e popola la variabile globale `yylval` con il valore semantico (di tipo `YYSTYPE`, dichiarabile via `%union`).

### Domande tipiche all'orale (sez. 6)
- *Differenza LR(1) vs LALR(1)?* → Stessi item, in LALR si fondono stati con stesso core; tabelle più piccole, possibili nuovi reduce-reduce.
- *Come gestisci il meno unario in Bison?* → Token fittizio + `%prec UMINUS`.
- *Cosa fa `%left`?* → Dichiara associatività a sinistra e precedenza (in base alla riga); usata da Bison per risolvere shift-reduce.
- *Cos'è il token `error`?* → Token speciale per error recovery.

---

## 7. Grammatiche a precedenza di operatori (cenni)

**Idea:** invece di stratificare la grammatica per livello di precedenza, si definisce direttamente una relazione di precedenza tra coppie di terminali (`<·`, `≐`, `·>`). Il parser sposta finché vede `<·` o `≐`, riduce quando vede `·>`.

Esempio matrice (espressioni con `+`, `*`, `id`):
```
       +    *    id   $
  +    ·>   <·   <·   ·>
  *    ·>   ·>   <·   ·>
  id   ·>   ·>        ·>
  $    <·   <·   <·
```

**Per quali grammatiche funziona:** **operator grammars**, ossia grammatiche dove nessuna produzione ha due non terminali adiacenti né il corpo `ε`. Pratico per espressioni aritmetiche; non adatto a grammatiche complete di un linguaggio.

**Vantaggi:** parser semplice, tabelle piccole, gestione naturale di precedenza/associatività.
**Svantaggi:** classe di grammatiche limitata; nessuna verifica strutturale forte (può accettare stringhe non nel linguaggio).

Concettualmente è il meccanismo dietro `%left/%right` di Bison applicato a una grammatica espressioni "ambigua" tipo `E → E + E | E * E | id`.

---

## 8. Punti chiave per l'orale

- **CFG = ⟨S, N, T, P⟩**; ambigua se una stringa ha due alberi (o due LM derivation). Il parser non risolve l'ambiguità: si risolve riscrivendo la grammatica.
- **Top-down (LL) produce LM derivation**, **bottom-up (LR) produce RM derivation al contrario**. LL non tollera ricorsione sinistra; LR sì.
- **FIRST(α)** = terminali che possono iniziare `α` (con `ε` se `α` deriva il vuoto). **FOLLOW(A)** = terminali che possono seguire `A`. Servono per costruire la tabella LL(1) e per closure/lookahead in LR.
- **LL(1)**: nessun conflitto in `M[A, a]` ⇔ per ogni `A → α | β`, `FIRST(α) ∩ FIRST(β) = ∅` e se `ε ∈ FIRST(α)` allora `FIRST(β) ∩ FOLLOW(A) = ∅`.
- **Item LR(1) = `[A → α · β, a]`**. closure espande, goto fa transizioni. Reduce quando dot è in fondo e il lookahead corrisponde.
- **Shift-reduce vs reduce-reduce**: il primo è spesso un'ambiguità (precedenza, dangling else); il secondo è quasi sempre un difetto strutturale.
- **LALR(1) = LR(1) con stati a stesso core fusi**: tabelle compatte; può introdurre nuovi reduce-reduce ma non shift-reduce. È la classe usata da Bison/Yacc.
- **Bison**: `%left/%right/%nonassoc` per precedenza, `%prec` + token fittizio per il meno unario, `error` + `yyerrok` per recovery; output `.tab.c` + `.tab.h` integrato col lexer Flex.
- **Inclusioni**: LL(1) ⊊ LALR(1) ⊊ LR(1) ⊊ CFG non ambigue ⊊ CFG.

---

## Connessioni col resto del corso

- **Pipeline (`05_pipeline_compilatore.md`):** parsing è la fase 2 (dopo lexing, prima di analisi semantica). Output: AST (o parse tree decorato).
- **Bison (`07_strumenti.md`):** è la realizzazione concreta di un parser LALR(1); le azioni semantiche tipicamente costruiscono l'AST nodo per nodo.
- **Analisi semantica:** opera sull'AST prodotto qui (type checking, name resolution). Alcune ambiguità (es. `T * x` in C) vengono risolte solo in questa fase tramite la tabella dei simboli.
- **Data-flow (`06_dataflow.md`):** opera sul CFG, derivato a sua volta dall'AST/IR.
