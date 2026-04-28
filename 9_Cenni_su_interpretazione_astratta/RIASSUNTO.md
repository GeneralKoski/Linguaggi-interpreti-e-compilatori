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
- **Insieme parzialmente ordinato (poset)** `(L, ≤)`
- **Lattice completo:** ogni sottoinsieme ha sup (⊔) e inf (⊓)
- **Ordine astratto:** `a ≤ a'` significa "a è più preciso di a'"
- **Funzioni di astrazione e concretizzazione:** `α: P → A` e `γ: A → P` formano una **Galois connection** (`α(c) ≤ a ⇔ c ⊆ γ(a)`)

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
