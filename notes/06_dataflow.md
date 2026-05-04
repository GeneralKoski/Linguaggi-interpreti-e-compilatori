# Data-flow analysis: framework comune

Tutti i problemi DFA classici si esprimono come sistema di equazioni su un **lattice finito** `(L, ⊑)` con funzioni di trasferimento monotone. Risolvendo iterativamente fino al punto fisso si ottiene la soluzione.

## Schema generale (forward)
```
In(entry) = init
Out(n) = f_n(In(n))
In(n) = ⨆_{p ∈ pred(n)} Out(p)         con ⨆ = meet (∩) o join (∪)
```

## Schema generale (backward)
```
Out(exit) = init
In(n) = f_n(Out(n))
Out(n) = ⨆_{s ∈ succ(n)} In(s)
```

## Tipologie

| Problema | Direzione | Meet | Init | Equazione di trasferimento |
|---|---|---|---|---|
| **Reaching Definitions** | forward | ∪ | ∅ | `Out(n) = Gen(n) ∪ (In(n) \ Kill(n))` |
| **Available Expressions** | forward | ∩ | tutte | `Out(n) = Expr(n) ∪ (In(n) \ Kill(n))` |
| **Live Variables** | backward | ∪ | ∅ | `In(n) = UEVar(n) ∪ (Out(n) \ VarKill(n))` |
| **Very Busy Expressions** | backward | ∩ | tutte | `In(n) = Expr(n) ∪ (Out(n) \ Kill(n))` |
| **Constant Propagation** | forward | merge ad-hoc | ⊥ | f_n via interpretazione astratta |

## Perché funziona (correttezza/terminazione)
- Lattice **finito** + funzioni **monotone** ⇒ ogni catena ascendente è finita
- **Knaster-Tarski:** monotonia + lattice completo ⇒ esistenza di lfp e gfp
- **Kleene:** se F continua (e ⊥ esiste), `lfp(F) = ⨆ Fⁿ(⊥)`
- L'algoritmo iterativo è una **iterazione di Kleene**: parte da ⊥, applica F, accumula → converge in tempo finito

## Velocità di convergenza
Dipende dall'ordine di visita dei nodi:
- **Forward problem** → reverse post-order (RPO)
- **Backward problem** → post-order (= RPO sul reverse CFG)

Numero massimo di passate per convergere: lunghezza del cammino più lungo nel CFG (per un problema "rapid"). Per problemi rapid, **2 passate** bastano (con l'ordine giusto).

## Esempio: Live Variables passo per passo
```
        BB1: a = 1; b = 2
              |
        BB2: c = a + b
              |
              v
        BB3: if (c > 0)
            /          \
       BB4: x = c       BB5: x = -c
            \          /
             v        v
              BB6: print(x)
```

UEVar e VarKill:
- BB1: UE=∅, Kill={a,b}
- BB2: UE={a,b}, Kill={c}
- BB3: UE={c}, Kill=∅
- BB4: UE={c}, Kill={x}
- BB5: UE={c}, Kill={x}
- BB6: UE={x}, Kill=∅

LiveOut iterativo (dal exit):
- LiveOut(BB6) = ∅ → LiveIn(BB6) = {x}
- LiveOut(BB4) = LiveIn(BB6) = {x} → LiveIn(BB4) = `UE ∪ (LiveOut \ Kill)` = {c} ∪ ({x}\{x}) = {c}
- LiveOut(BB5) = {x} → LiveIn(BB5) = {c}
- LiveOut(BB3) = LiveIn(BB4) ∪ LiveIn(BB5) = {c} → LiveIn(BB3) = {c} ∪ ({c}\∅) = {c}
- LiveOut(BB2) = {c} → LiveIn(BB2) = {a,b} ∪ ({c}\{c}) = {a,b}
- LiveOut(BB1) = {a,b} → LiveIn(BB1) = ∅ ∪ ({a,b}\{a,b}) = ∅

Insight: `x` è live solo da `BB4`/`BB5` in poi, `c` solo da `BB2` in poi, `a,b` solo in `BB1→BB2`. Coerente con l'intuizione.

## Applicazioni delle DFA
| Analisi | Uso |
|---|---|
| Live Variables | Register allocation (interferenza), DCE, uninitialized var detection |
| Reaching Definitions | UD-chain, costanti, propagazione |
| Available Expressions | Common subexpression elimination, redundancy |
| Very Busy Expressions | Code hoisting (loop-invariant code motion) |
| Constant Propagation | Costant folding, dead branch elimination |

## Da DFA a interpretazione astratta
La DFA è un caso particolare di interpretazione astratta dove:
- Il lattice è **finito** (es. `℘(Vars)`)
- Non serve **widening** (la convergenza è garantita dalla finitezza)
- Le funzioni di trasferimento sono "concrete" (semplici union/diff su set)

Quando il lattice diventa **infinito** (intervals, polyhedra), serve il **widening** per garantire terminazione.
