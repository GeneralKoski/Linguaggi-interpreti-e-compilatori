# 🥉 #3 — Reference counting per garbage collection

## Cos'è
Tecnica di gestione automatica della memoria dove ogni oggetto tiene un **contatore** di chi lo referenzia. Quando il contatore va a zero, l'oggetto si dealloca. È la GC di **Swift, Obj-C, PHP, CPython, Rust (Rc/Arc)**.

## Perché è figo
È il punto del syllabus che dice testualmente "uso di reference count per la garbage collection". Sei senior dev → sai già perché fa la differenza tra un `Rc<T>` e un `Box<T>`. Da fuori-corso porti il **cycle problem** e il **weak reference**, che è lì la sfida vera.

## Scaletta seminario (15 min, ~12 slide)
1. **Problema gestione memoria**: stack vs heap, manuale vs automatica (1 slide)
2. **Tre famiglie di GC**: tracing (mark-sweep), copying, **reference counting** (1 slide)
3. **RC: idea base** con counter increment/decrement (2 slide con codice)
4. **Implementazione in IR**: cosa emette il compilatore alle assegnazioni (2 slide)
5. **Il cycle problem**: perché RC fallisce con cicli (2 slide con esempio)
6. **Soluzioni**: weak references (Swift), cycle collector concorrente (PHP, CPython) (2 slide)
7. **Trade-off RC vs tracing GC**: latency, throughput, predicibilità (2 slide)
8. **Wrap-up** (1 slide)

## Demo
- Mostra in C++ un `shared_ptr` che leakka per ciclo, poi risolto con `weak_ptr`
- Oppure in Swift: `var owner: Person?` vs `weak var owner: Person?`
- Disassembly: `objdump` di un'assegnazione Swift mostra la chiamata a `swift_retain/swift_release`

## Q&A che probabilmente arriva
- "Perché Swift usa RC e Java usa tracing?"
- "Costo amortizzato di un retain/release vs un mark-sweep stop-the-world?"
- "Atomic vs non-atomic increment in Rust (Rc vs Arc)?"
- "Come fa un compilatore a inserire automaticamente le retain/release? (ARC in Obj-C)"
- "Se hai un grafo di oggetti molto grande con un solo ciclo, RC che fa?"

## Pitch al prof
> Buongiorno Professore,
> dato che nel programma è citato l'uso di reference counting come strategia di garbage collection, mi piacerebbe svilupparlo come argomento di seminario. L'idea è partire dall'implementazione base del retain/release a livello di IR generato dal compilatore, mostrare il **cycle problem** e poi le soluzioni reali adottate da Swift (weak reference) e PHP/CPython (cycle collector). Mi piacerebbe portare anche un confronto con la GC tracing per evidenziare i trade-off. Le sembra un taglio adeguato?

---

## ⚠️ Considerazioni pratiche

### Pro
- **Zero codice da scrivere** se vuoi (puoi fare 100% talk + esempi presi da Swift/PHP)
- Topic universale, ogni dev OO lo capisce
- Cycle problem è il "twist" che lo rende interessante e ti dà materiale per Q&A

### Contro
- Più "talky" e meno demo-heavy → rischio di sembrare meno tecnico
- Se non sei a tuo agio con le slide concettuali, può sembrare più scolastico
- Q&A può deviare verso "compara con tracing GC" che è un pelo fuori syllabus

### Tempo di preparazione realistico
**10-15h** (slide concettuali + ricerca Swift ARC, PHP gc, Python gc)

### Quando sceglierlo
Hai **<2 settimane**, vuoi meno preparazione possibile, ti senti a tuo agio con presentazioni concettuali e con un Q&A più aperto.
