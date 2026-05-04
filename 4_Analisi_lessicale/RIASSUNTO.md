# 4. Analisi lessicale

Materiali: `Analisi-Lessicale.pdf`, `DFA.pdf`, `Flex.pdf` + slide EaC (`05-Regular_Expressions_NFAs_DFAs.ppt`, `06-Building_Scanners.ppt`, `07-DFA_Minimization.ppt`).

## Obiettivo
Trasformare la sequenza di caratteri in una sequenza di **token** `⟨part-of-speech, lexeme⟩`.
Esempi: `⟨KWD, while⟩`, `⟨IDENT, somma⟩`, `⟨INT, 42⟩`, `⟨FLOAT, 3.1415⟩`, `⟨STR, "Hello"⟩`.

- **Specifica:** RE (espressioni regolari) → comprensibili al progettista
- **Implementazione:** DFSA (deterministic finite-state automaton) → efficiente
- **Generazione automatica:** strumenti tipo Flex partono dalla specifica e producono il riconoscitore

## Linguaggi formali (richiami)
- Σ = alfabeto, Σ\* = stringhe finite, ε = stringa vuota
- Linguaggio L ⊆ Σ\*
- Problema di decisione: dato s ∈ Σ\*, decidere in tempo finito se s ∈ L

## Espressioni regolari
Sintassi induttiva:
- ε ∈ RExpr; a ∈ RExpr per ogni a ∈ Σ
- Se e ∈ RExpr: `(e)`, `e*` ∈ RExpr
- Se e₁, e₂ ∈ RExpr: `e₁e₂`, `e₁|e₂` ∈ RExpr

Precedenza: `*` > concatenazione > `|`.

Semantica `L: RExpr → ℘(Σ*)`:
- L(ε) = {ε}; L(a) = {a}
- L(e₁e₂) = L(e₁)·L(e₂) (concatenazione di linguaggi)
- L(e₁|e₂) = L(e₁) ∪ L(e₂)
- L(e\*) = ⋃ᵢ₌₀^∞ L(e)ⁱ

Abbreviazioni: `e+` (positiva), `e?` (opzionale), `[a₁…aₙ]`, `[a₁-a₂]`, `[^…]`.

## Token tipici dei linguaggi di programmazione
Parole chiave, identificatori, costanti (int/float/char/string), operatori, punteggiatura, commenti.

### Esempi di RE
- Identificatori: `[a-zA-Z_]([a-zA-Z_]|[0-9])*`
- Interi: `[0-9]+` (attenzione: accetta `000000`, non gestisce il segno; il segno va spesso al parser)
- Float: `[+-]?[0-9]+\.[0-9]*` (il segno è opzionale; nelle classi `[+-]` il `+` è letterale, non quantificatore. Spesso si lascia il segno al parser per evitare ambiguità con espressioni binarie come `a+1`)
- Char: `'[^']'` (come si codifica `'\''`?)
- Commento C++ riga: `//[^\n]*\n`
- Commento multiriga (C/C++/Java/SQL): `/\*([^*]|\*+[^/*])*\*+/` — corretto ma illeggibile, meglio start state in Flex.

### Risoluzione ambiguità
1. **Lessema più lungo** vince: `forwhile` è UN identificatore, non `for` + `while`; `>>` è SHIFT_RIGHT non `> >`
2. A parità di lunghezza: **ordine delle regole** (priorità). Tipicamente le keyword sono dichiarate PRIMA degli IDENT.

Curiosità: `std::vector<std::list<int>>` era errore lessicale fino a C++03 (i due `>` venivano interpretati come `>>`); risolto in C++11.

---

## DFA (Deterministic Finite Automaton)
Quintupla `M = ⟨Σ, Q, δ, q₀, F⟩`:
- Σ alfabeto
- Q insieme finito di stati
- δ: Q × Σ → Q funzione di transizione
- q₀ ∈ Q stato iniziale
- F ⊆ Q stati accettanti

Configurazione `(q, x) ∈ Q × Σ*`. Transizione `(q, ax) → (q', x)` se δ(q,a)=q'. Linguaggio riconosciuto: stringhe che da q₀ portano in uno stato di F.

## Da RE a DFA (slide EaC: 05, 06, 07)
Pipeline completa di costruzione di uno scanner:
1. **RE → NFA** (Thompson's construction, Ken Thompson CACM 1968)
2. **NFA → DFA** (subset construction, Rabin & Scott 1959)
3. **DFA → DFA minimo** (Hopcroft o Brzozowski)
4. **DFA → codice** (table-driven o direct-coded)
5. (DFA → RE: all-pairs all-paths, fuori dal flusso principale)

### Thompson's construction (slide 05)
Idea: per ogni RE atomica e operatore costruisci un piccolo NFA con un solo stato iniziale e uno finale, poi combina via ε-mosse seguendo la struttura sintattica della RE.
- **Atomo `a`:** due stati, transizione `a` dall'uno all'altro
- **Concatenazione `e1 e2`:** ε-mossa dall'accettante di `N(e1)` all'iniziale di `N(e2)`
- **Alternanza `e1 | e2`:** nuovo stato iniziale con due ε-mosse a `N(e1)` e `N(e2)`; nuovo stato finale ricevuto da entrambi
- **Kleene `e*`:** nuovo iniziale e finale con ε-mosse, loop ε dall'accettante di `N(e)` al suo iniziale
Esempio dalle slide: `a(b|c)*`.

### Subset construction (slide 05/06)
Gli stati del DFA sono **insiemi di stati dell'NFA**. Si parte da `ε-closure(s0)` e si calcola `move(S, a)` poi `ε-closure(...)` per ogni input. Possibile blow-up esponenziale (`2^|Q_NFA|` nel caso peggiore), ma in pratica i DFA sono ragionevoli.

### Minimizzazione di Hopcroft (slide 07) ⚠️ NOTA
Il `note.txt` precisa: *"si è considerato solo l'algoritmo di minimizzazione di Hopcroft"* (Brzozowski viene menzionato come alternativa ma non studiato).

**Idea:** raffinamento di partizioni.
- Partizione iniziale `P0 = {F, S-F}` (accettanti vs non accettanti)
- A ogni passo, per ogni `p ∈ P` e ogni simbolo `a ∈ Σ`, controlla se `p` ha stati che vanno su simboli `a` in classi diverse della partizione corrente: se sì, **splitta** `p`
- Termina quando nessun split è più possibile → ogni classe è uno stato del DFA minimo

**Algoritmo a worklist** (dalle slide):
```
W = {F, S-F};  P = {F, S-F}
while W ≠ ∅:
   estrai s ∈ W
   for each c ∈ Σ:
      I = δ⁻¹(s, c)             // pre-immagine della classe s su c:
                                // I = { q ∈ Q | δ(q, c) ∈ s }
                                // (insieme degli stati che leggendo c finiscono dentro la classe s)
      for each p ∈ P:
         if p ∩ I ≠ ∅ and p \ I ≠ ∅:
            split p in p1 = p ∩ I, p2 = p \ I
            P = (P \ {p}) ∪ {p1, p2}
            aggiorna W (aggiunge la più piccola tra p1, p2 — questo è il trucco di Hopcroft)
```
**Complessità:** O(|Σ|·n·log n). Il trucco di aggiungere alla worklist solo la classe più piccola dopo lo split è ciò che porta il fattore log n invece di n.

### Table-driven scanner (slide 06)
- Skeleton + tabella di transizione: `state ← δ(state, char)` finché si può
- **Character classification:** raggruppa caratteri con identico comportamento (es. tutte le cifre, tutte le lettere) → comprime drasticamente la tabella δ
- **Build the lexeme:** mentre si transita si concatena `yytext`
- **Riconoscere parole successive:** scanning fino a errore o EOF, poi rollback all'ultimo stato accettante (stack-based)
- **Quadratic rollback** è un rischio teorico per certe RE patologiche (`ab | (ab)*c`); flex e simili lo evitano con ottimizzazioni

## Flex (generatore di lexer)
- Versione free di Lex (1975), produce codice C (anche compilabile come C++; modalità `--c++` "is a mess" e va evitata)
- Pipeline tipica:
  ```
  flex -o lexer.c lexer.ll
  gcc -Wall -o lexer lexer.c
  ./lexer < input
  ```

### Struttura del file `.ll` (3 sezioni separate da `%%`)
```
/* sezione definizioni */
%%
/* sezione regole */
%%
/* sezione codice utente */
```

### Sezione regole
Formato: `pattern   azione_C`. Definisce `int yylex()` che legge un lessema e ritorna il token (intero). Variabili globali: `yytext` (puntatore al lessema), `yyleng`, `yylineno`.
- Pattern e azione **sulla stessa riga**
- `|` come azione = "stessa azione del pattern successivo"
- L'**ordine delle regole** stabilisce la priorità

### Pattern Flex
| Pattern | Significato |
|---|---|
| `c` | carattere letterale |
| `\c` | escape |
| `(p)`, `p1 p2`, `p1\|p2` | grouping, concat, alternativa |
| `p*`, `p+`, `p?` | Kleene, positiva, opzionale |
| `p{m,M}` | iterazione limitata |
| `.` | qualsiasi tranne newline |
| `[chars]`, `[^chars]` | classi |
| `"string"` | match letterale |
| `{name}` | richiama un pattern definito |
| `p1/p2` | trailing context |
| `^p`, `p$` | inizio/fine riga |

### Sezione definizioni
- **Literal block** `%{ ... %}`: codice C copiato verbatim
- Pattern con nome: `DIGIT [0-9]` poi usabili come `{DIGIT}`
- **Opzioni utili (sempre):** `%option noyywrap`, `%option nodefault`
- **Opzioni utili (talvolta):** `%option yylineno`, `%option case-insensitive`
- **Start states / start conditions:** `%x NOMESTATO` (esclusivo) o `%s` (shared). Si entra con `BEGIN NOMESTATO;` e una regola si applica solo nello stato `NOMESTATO` se prefissata da `<NOMESTATO>pattern`.

### Esempio: commenti multiriga con start state
```
%x COMMENT
%%
"/*"                    { BEGIN COMMENT; }
<COMMENT>"*/"           { BEGIN INITIAL; }
<COMMENT>([^*]|\n)+|.   /* skip */
<COMMENT><<EOF>>        { error(); return 0; }
```

### Sezione codice utente
Definizione delle funzioni ausiliarie e talvolta `main`. Best practice: tenere le definizioni in un file `.cc` separato.

## Punti chiave per l'orale
- Differenza tra **specifica** (RE) e **implementazione** (DFA)
- Tappe della costruzione automatica: Thompson → subset → minimizzazione (Hopcroft)
- Le due regole di disambiguazione (lessema più lungo, priorità per ordine)
- Saper scrivere RE per identificatori, numeri e commenti
- Struttura di un sorgente Flex e ruolo di `yylex`/`yytext`/start states
