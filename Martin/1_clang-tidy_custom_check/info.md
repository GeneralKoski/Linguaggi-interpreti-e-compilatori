# 🥇 #1 — Scrivere un check custom per `clang-tidy`

## Cos'è
`clang-tidy` è un linter basato sull'AST di Clang. Ti permette di scrivere check personalizzati che fanno **analisi statica** sul tuo codice C/C++ usando l'AST arricchito (con tipi, scope, conversioni).

## Perché è figo
È **il caso più pratico in assoluto** del corso: prendi il front-end di Clang (lexer + parser + checker già fatti per te), navighi l'AST con un visitor e segnali pattern. È un lavoro che fanno i team di tooling nelle aziende grandi.

## Scaletta seminario (15 min, ~12 slide)
1. **Il problema**: code review automatico sui pattern del progetto (1 slide)
2. **Architettura clang-tidy**: AST matchers + check (1 slide)
3. **L'AST di Clang** ricapitolato (con `clang -ast-dump`) (1 slide)
4. **AST matchers DSL**: `cxxMethodDecl(isVirtual())`, `callExpr(callee(...))` (2 slide)
5. **Implementazione step-by-step di un check** (4-5 slide con codice C++)
6. **Demo live**: il check trova il bug in un file di test (2-3 slide)
7. **Wrap-up**: come si distribuisce, integrazione CI (1 slide)

## Idea di check da implementare
- **"Vieta `printf` in C++ moderno"** — segnala uso di `printf` e suggerisce `std::print`/`fmt`
- **"Magic number"** — segnala letterali numerici hardcoded che non sono 0, 1, -1
- **"Logger senza controllo livello"** — pattern interno aziendale

## Demo live (4-5 min)
```bash
# costruisci il check
cd llvm-project/clang-tools-extra
ninja clang-tidy
# applicalo
clang-tidy -checks='-*,my-noprintf' test.cc
```

## Q&A che probabilmente arriva
- "Cosa distingue clang-tidy da un grep evoluto?"
- "Differenza AST matcher vs RecursiveASTVisitor?"
- "Come gestisci i template? L'AST è specializzato o generico?"
- "Falso positivo/falso negativo del tuo check: come li gestisci?"
- "Cosa succede a livello di scoping quando matchi un identifier?"

## Pitch al prof
> Buongiorno Professore,
> avrei pensato di estendere l'esercitazione su clang-tidy che vedremo a lezione: il seminario consisterebbe nella **scrittura di un check custom passo per passo**, partendo dall'AST di Clang e usando gli AST matcher. Mi piacerebbe in particolare mostrare come si naviga un AST arricchito di informazioni semantiche per fare analisi non-locali. Posso scegliere un check semplice ma significativo (es. uso di pattern interni proibiti). Le va come argomento?

---

## ⚠️ Considerazioni pratiche

### Pro
- Massima soddisfazione tecnica per un senior dev
- Ti porti a casa una skill che puoi usare in ufficio (linting custom è oro nei team grossi)
- Mostri "ho costruito qualcosa di mio"
- Q&A circoscritto (parli di codice tuo, è territorio che padroneggi)

### Contro
- **È quello che richiede più prep**: devi scaricare e buildare `llvm-project` con `clang-tools-extra` (~10 GB, ~30-60 min di build su Mac M-series, di più su Intel)
- 15 min sono *pochi* per: AST matchers + struttura di un check + codice + demo. Rischi di correre
- Se capita un bug nella demo live, è imbarazzante

### Tempo di preparazione realistico
**30-40h** (build LLVM + scrivere check + tirar fuori esempi + slide)

### Quando sceglierlo
Hai **>1 mese** di tempo motivato dedicabile, sei pronto a buildare LLVM in locale, vuoi portare a casa una skill spendibile.
