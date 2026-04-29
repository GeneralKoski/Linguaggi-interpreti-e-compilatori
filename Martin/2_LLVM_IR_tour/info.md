# 🥈 #2 — Tour di LLVM IR + ottimizzazioni con `opt`

## Cos'è
LLVM IR è la "lingua intermedia" su cui Clang traduce il C/C++ prima di generare assembly. È un assembly virtuale tipizzato in forma SSA. Lo strumento `opt` applica analisi e trasformazioni sull'IR.

## Perché è figo
È il **cuore reale** della compilazione moderna: ogni linguaggio serio (Rust, Swift, Julia, Zig) si appoggia a LLVM. Padroneggiarlo significa capire come "pensa" un compilatore industriale.

## Scaletta seminario (15 min, ~12 slide)
1. **Cos'è una IR e perché serve** (1 slide)
2. **Posizionamento di LLVM IR** nel pipeline Clang (1 slide)
3. **Tour della sintassi**: tipi, identificatori, struttura modulo (2 slide)
4. **Una funzione completa** in IR: `factorial(n)` letta riga per riga (2 slide)
5. **SSA e funzione `phi`** con esempio if/else (1 slide)
6. **Memoria: `alloca/load/store/GEP`** (1 slide)
7. **`opt` in azione**: prima/dopo `mem2reg`, `instcombine`, `gvn`, `licm` (3 slide con diff)
8. **Wrap-up**: perché LLVM ha vinto (1 slide)

## Demo live (3-4 min al centro della presentazione)
```bash
clang -O0 -S -emit-llvm fact.c -o fact.ll       # mostra IR ingenuo
opt -passes=mem2reg fact.ll -S -o fact.mem.ll    # ora SSA pulito
opt -O2 fact.ll -S -o fact.opt.ll                # tutto ottimizzato
diff fact.ll fact.opt.ll                          # boom
```

## Q&A che probabilmente arriva
- "Cosa fa di preciso la `phi`?"
- "Perché `mem2reg` non sempre riesce a promuovere tutto?"
- "Differenza tra `nsw`/`nuw` flag su una `add`?"
- "Cosa significa che LLVM IR è 'tipizzato'? Type system?"
- "Differenza tra `call` e `invoke`?"

## Pitch al prof
> Buongiorno Professore,
> per il seminario avevo pensato a un **tour applicato di LLVM IR**: partirei dalla struttura del linguaggio (tipi, SSA, BB, terminatori) e poi mostrerei dal vivo come `opt` trasforma una funzione semplice attraverso i pass principali (`mem2reg`, `instcombine`, `gvn`, `licm`). L'idea è di tenere un taglio molto pratico, perché credo che vedere l'IR cambiare passo dopo passo aiuti a capire cosa fa davvero un middle-end moderno. Le sembra appropriato?

---

## ⚠️ Considerazioni pratiche

### Pro
- **Zero infrastruttura**: hai già Clang installato, basta `clang` e `opt`
- Demo cleanissima e impressionante (basta `diff` di IR)
- Argomento largo → puoi adattare in profondità o estensione
- Q&A facile da preparare (sono tutte cose nelle slide del prof)

### Contro
- Meno "wow factor" per un senior dev → sembra più "racconto un manuale"
- Per fare bella figura devi davvero **possedere** l'IR riga per riga (Q&A può scavare)

### Tempo di preparazione realistico
**15-20h** (lettura LangRef + esperimenti con opt + slide)

### Quando sceglierlo
Hai **2-4 settimane**, vuoi un sweet spot tra effort e risultato, vuoi un argomento sicuro e ben coperto dalle slide del prof.
