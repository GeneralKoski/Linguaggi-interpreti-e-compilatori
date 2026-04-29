# Piano di lavoro — Reference Counting per GC

Tre fasi: **progetto tecnico** → **presentazione** → **preparazione orale**.

---

## 🛠 FASE 1 — Costruzione del progetto (~8-10h)

### Obiettivo finale
Avere set di esempi multi-linguaggio (C++, Swift, PHP, Python) che illustrano: RC base, cycle problem, weak references, cycle collector. Tutti provati e funzionanti.

### Step 1.1 — Setup ambiente (30 min)
- [ ] Verifica tooling già presente (su macOS hai praticamente tutto):
  ```bash
  clang++ --version       # C++
  swift --version         # se manca: usa Xcode Cmd Line Tools
  php --version           # brew install php
  python3 --version
  ```
- [ ] Crea cartella di lavoro:
  ```bash
  mkdir -p ~/refcounting-demo/{cpp,swift,php,python,disasm}
  ```

### Step 1.2 — Studio teoria (3-4h)
- [ ] Leggi i capitoli sulla GC su EaC (Cooper-Torczon) — capitolo 6 "The Procedure Abstraction" tocca l'argomento, ma per la GC vera leggi:
  - Wikipedia: "Reference counting" (overview)
  - "The Garbage Collection Handbook" by Jones, Hosking, Moss (capitolo 5: Reference Counting) — usa solo le sezioni introduttive
- [ ] Documentazione ufficiale Swift ARC:
  https://docs.swift.org/swift-book/documentation/the-swift-programming-language/automaticreferencecounting/
- [ ] Articolo di Pierre-Loup Griffais o Mike Ash su come Apple implementa ARC
- [ ] CPython gc module: https://docs.python.org/3/library/gc.html
- [ ] PHP refcount + `gc_collect_cycles`: https://www.php.net/manual/en/features.gc.refcounting-basics.php

### Step 1.3 — Esempi C++ (1-2h)

#### Esempio 1: `cpp/leak.cpp` — il cycle problem
```cpp
#include <memory>
#include <iostream>
struct Node {
    std::shared_ptr<Node> next;
    int value;
    Node(int v) : value(v) { std::cout << "ctor " << v << "\n"; }
    ~Node() { std::cout << "dtor " << value << "\n"; }
};
int main() {
    auto a = std::make_shared<Node>(1);
    auto b = std::make_shared<Node>(2);
    a->next = b;
    b->next = a;  // CYCLE! Né a né b verranno distrutti
    std::cout << "a use_count: " << a.use_count() << "\n";
    return 0;
}
```
Esegui: vedi che i ~Node non vengono mai chiamati → leak.

#### Esempio 2: `cpp/fix.cpp` — fix con weak_ptr
```cpp
struct Node {
    std::weak_ptr<Node> next;  // CHANGED
    // ...
};
```
Esegui: vedi che ora i destructors vengono chiamati.

### Step 1.4 — Esempi Swift (1-2h)

#### Esempio: `swift/Person.swift`
```swift
class Person {
    let name: String
    var apartment: Apartment?
    init(name: String) { self.name = name }
    deinit { print("\(name) deinit") }
}
class Apartment {
    let unit: String
    weak var tenant: Person?  // weak per evitare cycle
    init(unit: String) { self.unit = unit }
    deinit { print("Apartment \(unit) deinit") }
}
var p: Person? = Person(name: "Alice")
var a: Apartment? = Apartment(unit: "4A")
p?.apartment = a
a?.tenant = p
p = nil; a = nil  // entrambi deinit!
```

### Step 1.5 — Esempi PHP (1h)

#### Esempio: `php/cycle.php`
```php
<?php
class Node {
    public $next;
    public $val;
    public function __construct($v) { $this->val = $v; }
    public function __destruct() { echo "destroying $this->val\n"; }
}
$a = new Node(1);
$b = new Node(2);
$a->next = $b;
$b->next = $a;
unset($a, $b);
// Senza cycle collector: nessun __destruct stampato
gc_collect_cycles();
// Dopo: i destruct vengono stampati
```

### Step 1.6 — Esempi Python (1h)

#### Esempio: `python/cycle.py`
```python
import gc, sys

class Node:
    def __init__(self, v): self.v = v
    def __del__(self): print(f"del {self.v}")

a = Node(1); b = Node(2)
a.next = b; b.next = a
print(sys.getrefcount(a))
del a, b
print(gc.get_count())
gc.collect()
```

### Step 1.7 — Disassembly Swift retain/release (1-2h)
Mostra concretamente come ARC inserisce le chiamate:
```bash
# swift/simple.swift:
# class C { var x = 0 }
# func foo(c: C) { c.x = 42 }

swiftc -O simple.swift -emit-assembly -o simple.s
# Cerca "swift_retain", "swift_release" in simple.s
grep -E "swift_(retain|release)" simple.s
```

### Deliverable Fase 1
✅ ~8-10 mini-programmi che illustrano: RC base, cycle problem, weak fix, cycle collector
✅ Output testuale/disasm per ognuno
✅ Note di lavoro su trade-off osservati

---

## 🎤 FASE 2 — Preparazione presentazione (~8-10h)

### Obiettivo finale
Slide pulite (~12) + 1-2 mini-demo + script verbale.

### Step 2.1 — Outline (1h)
Tempi target (15 min):
- Hook: 1 min
- Teoria base RC: 3 min
- Cycle problem: 3 min
- Soluzioni reali (Swift/PHP/Python): 4 min
- Trade-off RC vs tracing: 2 min
- Wrap-up: 1 min

### Step 2.2 — Slide bozza (3-4h)
Tool: **Keynote** o **Marp**.

Slide proposte:
1. **Cover** — "Reference Counting: come Swift e PHP gestiscono la memoria"
2. **Il problema** — manuale pericolosa, GC tracing imprevedibile
3. **Tre famiglie di GC** — tabella tracing/copying/RC con caratteristiche
4. **RC: idea base** — schema con counter increment/decrement
5. **Cosa emette il compilatore** — pseudo-IR per `x = y` (retain di y, release di x)
6. **Ottimizzazioni note** — ARC optimizer di LLVM (eliminazione retain/release ridondanti)
7. **🚨 Il cycle problem** — diagramma 2 oggetti che si puntano + `use_count > 0`
8. **Soluzione 1: weak references** — Swift demo concettuale
9. **Soluzione 2: cycle collector** — algoritmo di Bacon-Rajan (PHP/Python usano variante)
10. **Trade-off RC vs tracing** — tabella latency/throughput/predicibilità
11. **🔧 Mini-demo** — 1-2 min terminale (PHP/Python con e senza gc.collect)
12. **Take-away** — quando sceglieresti RC

### Step 2.3 — Diagrammi (1-2h)
Le immagini valgono di più della formula:
- [ ] Disegna in **draw.io** o **Excalidraw** il cycle problem (2 nodi che si puntano + counter > 0)
- [ ] Diagramma del weak reference (la freccia tratteggiata che non incrementa il counter)
- [ ] Diagramma del cycle collector (BFS dai roots, marca, pulisci)

### Step 2.4 — Demo (1-2h)
Demo proposta: **PHP cycle collector** (è la più chiara da mostrare).
```bash
# in un terminale:
php -d zend.enable_gc=0 cycle.php  # senza gc → nessun destruct
php cycle.php                       # con gc abilitato → destruct dopo
```

In alternativa, **Python**:
```bash
python3 cycle.py                  # mostri il refcount + gc.collect()
```

### Step 2.5 — Script + frasi-chiave (1h)
- Apertura: "Tutti voi avete usato `shared_ptr`, `Rc<T>`, oppure scritto codice Swift. Dietro c'è un'idea semplicissima: contare i riferimenti."
- Transizione cycle: "Sembra perfetto. Eppure ha un buco strutturale."
- Chiusura: "RC non è meglio o peggio di una GC tracing. È un compromesso diverso, ottimizzato per latency e predicibilità."

### Step 2.6 — Prove cronometrate (1-2h)
- [ ] Prova 1: specchio
- [ ] Prova 2: davanti a qualcuno
- [ ] Prova 3: video
- [ ] Tempo: 13-14 min

### Deliverable Fase 2
✅ Slide finali (PDF)
✅ Diagrammi pronti
✅ Demo provata 3+ volte
✅ Script verbale

---

## 📚 FASE 3 — Preparazione orale teoria (~15-20h)

L'orale **vale 70%** e copre **tutto il programma**, NON solo RC.

### Step 3.1 — Studio sistematico (10-12h)
Segui il `study_method/STUDY_PLAN.md` Step 1:

- [ ] **Lezione 1-3** (Intro, Interpreti vs Compilatori, Struttura): 1.5h
- [ ] **Lezione 4-5** (Analisi lessicale, Flex): 2h
- [ ] **Lezione 6** (Analisi dipendente da contesto, TinyP): 2h
- [ ] **Lezione 7** (IR, LLVM, procedure abstraction, code shape, **GC con RC**): 2-3h
- [ ] **Lezione 8** (Middle end, dataflow, ottimizzazioni): 2-3h
- [ ] **Lezione 9** (Interpretazione astratta): 1.5h

Per ogni lezione:
1. Leggi il `RIASSUNTO.md`
2. Apri il PDF del docente
3. Risp.ondi a voce alle "Domande chiave per l'orale"

### Step 3.2 — Memorizzazione cheatsheet (3h)
- [ ] Equazioni dataflow (Live, Reaching, Available)
- [ ] Algoritmo LVN
- [ ] Algoritmo Hopcroft
- [ ] Domini astratti (sign, interval, congruence)

### Step 3.3 — Domande tipo (2-3h)
- [ ] Tutte le 44 in `notes/04_domande_tipo.md` a voce, cronometrate
- [ ] Sul tuo seminario il prof scaverà su: "perché RC e non tracing?", "atomic vs non-atomic increment", "ARC optimization passes in LLVM"

### Step 3.4 — Domande "incrocio" (2-3h)
- "Spiegami pipeline completa di compilazione"
- "Come funziona internamente uno `shared_ptr`?"
- "Cosa cambia tra il middle-end di Clang e l'interpretazione astratta?"
- "Come emetterebbe un compilatore C++ il codice per `x = y` se entrambi sono `shared_ptr`?"

### Step 3.5 — Ripasso finale (2h, giorno -1)
- [ ] "Punti chiave per l'orale" dei 9 RIASSUNTO
- [ ] Glossario
- [ ] Niente di nuovo

### Deliverable Fase 3
✅ Studio completo 9 capitoli
✅ 44 domande tipo affrontate
✅ Cheatsheet memorizzato

---

## 📅 Timeline suggerita

| Settimana | Attività |
|---|---|
| **Sett. -5** | Mail al prof, appuntamento Teams |
| **Sett. -4** | Conferma argomento + studio teoria RC (Fase 1.2) |
| **Sett. -3** | Esempi multi-linguaggio (Fase 1.3-1.7) |
| **Sett. -2** | Slide + diagrammi + demo (Fase 2.1-2.4) |
| **Sett. -1** | Prove + studio orale parallelo (Fase 2.5-2.6 + Fase 3.1) |
| **Giorni -3/-1** | **SEMINARIO** + studio orale intensivo (Fase 3.2-3.4) |
| **Giorno -1** | Ripasso leggero (Fase 3.5) |
| **Giorno 0** | **ORALE** |

⚠️ Buffer: 5-7 giorni per imprevisti.

## 💡 Tips specifici per Reference Counting

- **Il prof è italiano e accademico**: meno demo flashy, più rigore concettuale. Cura le slide della parte teorica.
- **Nel programma il punto è specifico**: "uso di reference count per la garbage collection" → resta ancorato a quello, non perderti in mille tracing GC algorithms
- **Q&A trabocchetto:**
  - "Quanto costa un retain/release in Swift in termini di cicli CPU?" → ~5-10 cicli (lookup in metadata + atomic increment)
  - "Perché Java non usa RC?" → throughput più alto con tracing per allocazioni frequenti, no costo per assegnamenti
  - "Esiste un linguaggio mainstream che usa RC ma senza cycle collector?" → Sì: **Rust** con `Rc<T>` non gestisce cicli, è responsabilità del programmatore (con `Weak<T>`)
- **Idea bonus:** mostra una slide con il decoratore in Obj-C `__weak`/`__strong`/`__autoreleasing` come confronto
