# 9. Cenni di Interpretazione Astratta

## Materiali
- `cousot-tutorial.pdf` — *A Tutorial on Abstract Interpretation*, Patrick Cousot, VMCAI'05
- `mine-tutorial.pdf` — tutorial più avanzato di Antoine Miné
- `note.txt` — note del docente con link ufficiali

## Note dal docente
> "Nelle lezioni finali del corso, si introduce l'Interpretazione Astratta, formalizzata da Patrick e Radhia Cousot."

Risorse di partenza segnalate:
- https://www.di.ens.fr/~cousot/AI/IntroAbsInt.html (intuizione)
- https://www.di.ens.fr/~cousot/publications.www/CousotCousot-Marktoberdorf-2009.pdf (versione estesa)
- https://www.di.ens.fr/~cousot/COUSOTpapers/JLP92.shtml (esempi intuitivi)

---

## Cos'è l'interpretazione astratta

**Interpretazione astratta** = teoria dell'**approssimazione** della semantica dei programmi. Inventata da Patrick e Radhia Cousot (1977).

### Punto di partenza: la semantica concreta
- Modello matematico di **tutte le esecuzioni possibili in tutti gli ambienti possibili**
- È un oggetto matematico **infinito**, non calcolabile
- **Tutte le proprietà non banali sono indecidibili** (corollario del teorema di Rice; esempio: terminazione, via diagonalizzazione tipo `P ≡ while termination(P) do skip od`)

### Tre approcci alla verifica
| Tecnica | Garanzia | Problema |
|---|---|---|
| **Test/debug** | considera un sottoinsieme di esecuzioni | nessuna garanzia, copertura limitata |
| **Verifica formale (model checking, deduzione)** | totale | richiede intervento umano (invarianti, modello) |
| **Static analysis (interpr. astratta)** | parziale ma automatica | può dare falsi allarmi |

### L'idea chiave
Considera una **semantica astratta** che è un **soprainsieme** della semantica concreta:
- Se la semantica astratta non interseca la "zona proibita" (errori), allora **neanche la concreta** la interseca
- → la semantica astratta è **sound** (corretta)
- Si accetta di perdere precisione (potenziali falsi positivi) per guadagnare automatizzazione e terminazione

### Proprietà richieste dell'astrazione
1. **Sound** — nessun errore possibile viene dimenticato (no falsi negativi sugli errori)
2. **Precisa** abbastanza da evitare falsi allarmi
3. **Semplice** abbastanza da evitare esplosione combinatoria

### Il dial precisione/efficienza
- Astrazione troppo grossolana → tanti falsi allarmi (poca precisione)
- Astrazione troppo fine → potenzialmente non terminante o intrattabile

---

## Lattice e ordini parziali

L'apparato matematico:
- **Insieme parzialmente ordinato (poset)** `(L, ⊑)`
- **Lattice completo:** ogni sottoinsieme ha sup (⊔) e inf (⊓), in particolare ⊥ = ⊔ ∅ e ⊤ = ⊓ ∅
- **Ordine astratto:** `a ⊑ a'` significa "a è almeno preciso quanto a'" (cioè a è "più piccolo" / più informativo)
- **Funzioni di astrazione e concretizzazione:** `α: P → A` (astrazione) e `γ: A → P` (concretizzazione) formano una **Galois connection** sse:
  - α e γ sono **monotone**
  - `α(p) ⊑_A a ⇔ p ⊑_P γ(a)` (proprietà di adjunctness)
  - conseguenza: `p ⊑_P γ(α(p))` (sound) e `α(γ(a)) ⊑_A a` (riduce ridondanza)

**Knaster-Tarski:** se `(L, ⊑)` è lattice completo e `F: L → L` è monotona, allora F ha un **minimo punto fisso** `lfp(F) = ⊓ {x | F(x) ⊑ x}` e un massimo `gfp(F) = ⊔ {x | x ⊑ F(x)}`.

**Kleene fixpoint theorem:** se F è anche **ω-continua** (preserva ⊔ di catene crescenti) e L ha ⊥, allora `lfp(F) = ⊔_{n≥0} Fⁿ(⊥)`. È la base del **calcolo iterativo** del fixpoint: parto da ⊥, applico F, prendo l'unione, ripeto.

Esempi di domini astratti classici:
- **Sign domain:** `{⊥, neg, zero, pos, ⊤}`
- **Intervals:** `[a, b]` con `a, b ∈ ℤ ∪ {±∞}`
- **Polyhedra:** insiemi di disuguaglianze affini (potente, costoso)
- **Octagons:** un compromesso tra intervals e polyhedra
- **Constants:** `{⊥} ∪ ℤ ∪ {⊤}`

## Punti fissi e widening

La semantica concreta di un programma con loop si esprime come **minimo punto fisso** di una funzione `F` sul lattice. Per programmi reali, `F` ha molte iterazioni e l'analisi può non terminare.

**Soluzione:** **widening operator** `∇`. Combina due elementi del lattice "spingendo verso ⊤" per garantire convergenza in un numero finito di passi (a costo di precisione). Si recupera precisione localmente con il **narrowing** `△`.

Esempio classico (intervals): `[1,5] ∇ [1,7] = [1,+∞]` (perché il limite superiore è cresciuto, lo proietta su +∞).

---

## Funzioni monotone, ω-continue e catene

### Definizioni formali
Sia `(L, ⊑)` un poset.

- **Funzione monotona:** `F: L → L` è monotona se
  ```
  ∀ x, y ∈ L:   x ⊑ y  ⇒  F(x) ⊑ F(y)
  ```
  Cioè F preserva l'ordine. È la condizione minima per Knaster-Tarski.

- **Catena ascendente:** una sequenza `x_0 ⊑ x_1 ⊑ x_2 ⊑ ...` di elementi di L. La catena può essere infinita.

- **Sup di una catena:** `⊔_{n} x_n` è il minimo upper bound della catena. In un lattice completo esiste sempre.

- **Funzione ω-continua (Scott-continua su catene numerabili):** `F` è ω-continua se per ogni catena ascendente `(x_n)_{n∈ℕ}`,
  ```
  F( ⊔_n x_n ) = ⊔_n F(x_n)
  ```
  Cioè F **commuta con il sup di catene crescenti**. È più forte della monotonia (ogni ω-continua è monotona, non viceversa).

### Perché serve la distinzione
- **Knaster-Tarski** (solo monotonia + lattice completo): garantisce esistenza di lfp/gfp ma li caratterizza in modo non costruttivo (`lfp(F) = ⊓ {x | F(x) ⊑ x}`).
- **Kleene** (ω-continuità + ⊥): dà la **costruzione iterativa** `lfp(F) = ⊔_n Fⁿ(⊥)`, che è ciò che il compilatore/analizzatore effettivamente computa.

Su lattici **finiti** ogni catena ascendente stabilizza in un numero finito di passi (Ascending Chain Condition, ACC), quindi monotonia ⇒ ω-continuità di fatto: il fixpoint si raggiunge in un numero finito di iterazioni. Su lattici **infiniti** (intervals con limiti in ℤ ∪ {±∞}, polyhedra) servono **widening** per forzare terminazione.

**Cosa potrebbe chiedere il prof:** "qual è la differenza pratica tra monotonia e continuità nell'interpretazione astratta?" — su domini finiti (sign, costanti, DFA classico) basta monotonia; su domini infiniti (intervals, polyhedra) anche con monotonia il calcolo iterativo non termina, da qui il widening.

---

## Soundness: enunciato preciso

**Setup.** Siano:
- `(C, ⊑_C)` lattice concreto, `F: C → C` monotona (semantica concreta espressa come fixpoint, es. collecting semantics).
- `(A, ⊑_A)` lattice astratto.
- `(α, γ)` Galois connection tra C e A.
- `F#: A → A` astrazione di F.

**Definizione (correttezza dell'astrazione del passo):**
```
F# è una sound abstraction di F   ⇔   α ∘ F  ⊑  F# ∘ α
```
equivalentemente (per adjointness): `F ∘ γ ⊑ γ ∘ F#`. Significato: applicare F nel concreto e poi astrarre è "non più informativo" che astrarre prima e applicare F# poi (cioè F# può solo perdere precisione, non guadagnarla in modo scorretto).

**Teorema (soundness del fixpoint).** Sotto le ipotesi precedenti (con monotonia di F, F# e Galois connection):
```
α( lfp(F) ) ⊑_A  lfp(F#)
```
Equivalentemente: `lfp(F) ⊑_C γ( lfp(F#) )`. Cioè il fixpoint astratto **sovra-approssima** il fixpoint concreto: nessun comportamento concreto viene "perso" dall'analisi astratta.

**Dimostrazione abbozzata (per induzione sulle iterate di Kleene).**
1. Base: `α(⊥_C) ⊑_A ⊥_A` (vero: ⊥_A è il minimo).
2. Passo: assumi `α(Fⁿ(⊥_C)) ⊑_A (F#)ⁿ(⊥_A)`. Allora
   ```
   α(F^{n+1}(⊥_C)) = α(F(Fⁿ(⊥_C)))
                   ⊑ F#(α(Fⁿ(⊥_C)))      (per soundness del passo)
                   ⊑ F#((F#)ⁿ(⊥_A))      (per monotonia di F# + IH)
                   = (F#)^{n+1}(⊥_A).
   ```
3. Per ω-continuità (o ACC) e Kleene:
   `α(lfp F) = α(⊔_n Fⁿ(⊥_C)) = ⊔_n α(Fⁿ(⊥_C)) ⊑ ⊔_n (F#)ⁿ(⊥_A) = lfp F#`.

(Il passaggio `α(⊔ ...) = ⊔ α(...)` usa che α preserva i sup, vero in una Galois connection.)

**Conseguenza pratica.** Se la verifica astratta dimostra che `lfp(F#)` non interseca un insieme di stati erronei, allora `lfp(F)` (le esecuzioni concrete) nemmeno: l'analisi è **conservativa** sul lato giusto. Falsi positivi possibili (lfp(F#) può essere strettamente più grande di α(lfp F)), falsi negativi no.

**Cosa potrebbe chiedere il prof:** "cosa garantisce la soundness e cosa NON garantisce?" — garantisce che ogni proprietà di sicurezza dimostrata nell'astratto vale nel concreto; NON garantisce completezza (proprietà vere nel concreto possono non essere dimostrabili nell'astratto → falsi allarmi).

---

## Esempio numerico: fixpoint su Intervals con widening

Programma:
```
x := 1;
while (x < 100) do
   x := x + 1
od
```

CFG con punti di programma:
- `p0`: prima dell'assegnamento iniziale
- `p1`: header del while (entrata del loop, dopo init e prima del test)
- `p2`: dentro il body (dopo il test true)
- `p3`: dopo il loop (test false)

Equazioni semantiche (forward) sul dominio **Intervals**:
```
X(p0) = ⊤                                  (irrilevante)
X(p1) = [1,1]  ⊔  X(p2)                    (init ⊔ back-edge dal body)
X(p2) = X(p1) ⊓ [-∞, 99]   poi  +1         (filtro `x < 100`, poi incremento)
X(p3) = X(p1) ⊓ [100, +∞]                  (filtro `x ≥ 100`)
```
Operatore `+1` su intervals: `[a,b] +1 = [a+1, b+1]`.

**Iterazione di Kleene partendo da ⊥** (calcolo solo `X(p1)`):

| iter | X(p1)        | X(p2) = (X(p1)⊓[-∞,99])+1 |
|------|--------------|---------------------------|
| 0    | ⊥            | ⊥                         |
| 1    | [1,1]        | [2,2]                     |
| 2    | [1,2]        | [2,3]                     |
| 3    | [1,3]        | [2,4]                     |
| ...  | ...          | ...                       |
| k    | [1,k]        | [2,k+1]                   |

La sequenza `[1,1] ⊑ [1,2] ⊑ [1,3] ⊑ ...` è una catena ascendente infinita: senza widening **non termina**, anche se F è monotona (il dominio Intervals non soddisfa ACC).

**Applicazione del widening ∇** all'iterazione `k+1` su `p1`:
```
X(p1)_new = X(p1)_old  ∇  ( [1,1] ⊔ X(p2) )
```
Definizione di `∇` su intervals: `[a,b] ∇ [a',b'] = [if a'<a then -∞ else a, if b'>b then +∞ else b]` (estremi che crescono vengono spinti a infinito).

- iter 1: `X(p1) = [1,1]`
- iter 2 con widening: `[1,1] ∇ [1,2] = [1, +∞]` (il bound superiore è cresciuto → +∞)

Ora il punto fisso si stabilizza:
- `X(p2) = ([1,+∞] ⊓ [-∞,99]) + 1 = [1,99] + 1 = [2,100]`
- `X(p1)_next = [1,1] ⊔ [2,100] = [1,100]`, ma `X(p1) ∇ [1,100] = [1, +∞]` (stabile)

**Sovra-approssimazione finale:**
- `X(p1) = [1, +∞]`
- `X(p3) = [1, +∞] ⊓ [100, +∞] = [100, +∞]`

L'analisi conclude `x ∈ [100, +∞]` dopo il loop. Il valore concreto reale è `x = 100`, quindi abbiamo perso la **upper bound**: tipica perdita di precisione del widening.

**Recupero con narrowing △.** Una iterazione di narrowing dopo aver raggiunto il post-fixpoint ricalcola usando il filtro:
```
X(p1) △ ([1,1] ⊔ X(p2)) = [1,+∞] △ [1,100] = [1, 100]
```
(il narrowing accetta di restringere `+∞` a `100` perché il nuovo valore è più piccolo ma "stabile"). Ora `X(p3) = [100,100]` — precisione recuperata.

**Cosa potrebbe chiedere il prof:** "perché Intervals ha bisogno di widening e Sign no?" — Sign è un dominio **finito** (5 elementi), quindi soddisfa ACC e Kleene termina. Intervals ha catene infinite (`[1,1] ⊑ [1,2] ⊑ ...`) e senza widening il calcolo iterativo non termina su loop con conteggi grandi/sconosciuti.

---

## Esempio dal Cousot tutorial
Il programma:
```
{n0 ≥ 0}
n := n0; i := n;
while (i ≠ 0) do
   j := 0;
   while (j ≠ i) do j := j+1 od;
   i := i-1
od
{n0 = n, i = 0, n0 ≥ 0}
```
Analisi con dominio dei poliedri (disuguaglianze affini) → si dimostra automaticamente:
- nessun overflow (`j < n0`, `i ≥ 0`)
- assenza di errori run-time

Astrazione = **polyhedral abstraction** (disuguaglianze affini). Teoria sottostante = abstract interpretation.

---

## Applicazioni pratiche
- **Astrée** (analizzatore di Cousot): verificato per software avionico Airbus, ricerca esaustiva di runtime errors in C critico
- **Compilatori ottimizzanti:** constant propagation, range analysis, alias analysis sono tutte istanze di interpr. astratta
- **Formal verification & static analysis tools:** Polyspace, IKOS, Sparrow, Infer, ...

---

## Connessione col resto del corso (TinyP, slide 6 lezione)
- Il modulo `collecting.cc` di TinyP implementa la **collecting semantics**: per ogni punto del programma raccoglie l'insieme di **stati concreti** che possono essere raggiunti
- Da lì, sostituendo `℘(State)` con un dominio astratto `A` e gli operatori puntuali con i loro corrispondenti astratti, si ottiene un analizzatore astratto
- Il `tinyP-concrete` mostra in piccolo il principio: enumerare gli stati raggiungibili (concreto, finito solo per input bounded). Una versione astratta li approssimerebbe.

---

## Punti chiave per l'orale
- Sapere a chi si deve la teoria (Patrick + Radhia Cousot, 1977)
- Capire la **dualità soundness/completeness** e perché si rinuncia alla completezza per la decidibilità
- Sapere cos'è una **Galois connection** `(α, γ)` e a cosa serve
- Conoscere alcuni **domini astratti**: sign, intervals, octagons, polyhedra (e relativo trade-off)
- **Punto fisso, widening e narrowing**: capire perché servono e cosa garantiscono
- Esempio applicativo: Astrée, e perché l'industria critica li usa
- Connessione con DFA del middle end: anche le analisi data-flow sono "interpretazione astratta semplificata" (lattice finito → no widening)
