# Seminario clang-tidy custom check — bozza slide

Tempo: 15 min (1' hook + 9' contenuto/demo + 4-5' Q&A).
Slide totali: 13. Codice ≤ 15 righe per slide.

---

## Slide 1 — Cover

**Un check `clang-tidy` da zero**
Costruzione, test e integrazione di un linter C++ basato su AST.

Martin Trajkovski — Linguaggi, Interpreti e Compilatori — A.A. 2025/26

---

## Slide 2 — Il problema

> "In code review continuo a vedere `printf` in C++ moderno.
> Vorrei che il CI me lo segnali da solo, con il fix pronto."

- Pattern ripetitivi → review automatizzata
- Strumenti testuali (grep, regex) → falsi positivi su scope/template/macro
- Servirebbe un'analisi che capisce **tipi e scope**

---

## Slide 3 — Architettura clang-tidy

```
codice.cpp ──► clang frontend ──► AST + sema ──► AST matchers ──► check ──► diagnostics + fix-it
```

- `clang-tidy` = libreria di check + driver
- ogni check è una classe C++ che estende `ClangTidyCheck`
- usa l'**AST arricchito** (tipi, overload, template istantiati)

---

## Slide 4 — L'AST di Clang in 6 righe

```bash
echo 'int x = (int)3.14;' | clang -x c++ -Xclang -ast-dump -fsyntax-only -
```

Estratto:
```
TranslationUnitDecl
└── VarDecl 'x' 'int'
    └── CStyleCastExpr 'int' <FloatingToIntegral>
        └── FloatingLiteral 'double' 3.14
```

L'AST è il *contratto* su cui ragionano i check.

---

## Slide 5 — AST Matchers DSL

Linguaggio dichiarativo per descrivere pattern:

```cpp
// "ogni chiamata a una funzione di nome ::printf"
callExpr(callee(functionDecl(hasName("::printf"))))

// "ogni metodo virtuale senza override"
cxxMethodDecl(isVirtual(), unless(isOverride()))
```

Più conciso del `RecursiveASTVisitor` per pattern semplici.

---

## Slide 6 — Anatomia di un check

```cpp
class NoPrintfCheck : public ClangTidyCheck {
public:
  NoPrintfCheck(StringRef N, ClangTidyContext *C)
    : ClangTidyCheck(N, C) {}
  void registerMatchers(MatchFinder *F) override;
  void check(const MatchFinder::MatchResult &R) override;
};
```

Due hook: registra pattern, reagisce ai match.

---

## Slide 7 — Il nostro check: `misc-no-printf`

Cosa fa:
- segnala chiamate a `::printf` (libc)
- **non** segnala `mylib::printf` (scope diverso)
- emette un fix-it: sostituisce il nome con `std::print`

Perché didatticamente:
- matcher minimale ma "non triviale" (scope, macro, template)
- fix-it preciso (solo callee, non l'intera call)

---

## Slide 8 — `registerMatchers`

```cpp
void NoPrintfCheck::registerMatchers(MatchFinder *F) {
  F->addMatcher(
    callExpr(
      callee(functionDecl(hasName("::printf"))),
      unless(isExpansionInSystemHeader())
    ).bind("printfCall"),
    this);
}
```

`hasName("::printf")` aggancia lo scope globale → niente collisioni.

---

## Slide 9 — `check` + diagnostic + fix-it

```cpp
void NoPrintfCheck::check(const MatchFinder::MatchResult &R) {
  const auto *Call = R.Nodes.getNodeAs<CallExpr>("printfCall");
  const Expr *Callee = Call->getCallee()->IgnoreImpCasts();
  diag(Callee->getBeginLoc(),
       "uso di 'printf' sconsigliato; usare 'std::print'")
    << FixItHint::CreateReplacement(
         SourceRange(Callee->getBeginLoc(), Callee->getEndLoc()),
         "std::print");
}
```

---

## Slide 10 — Demo (parte 1): esempi piccoli + LLM

[TERMINALE]

```bash
clang-tidy -checks='-*,misc-no-printf' snippets/02_namespace_collision.cpp
# → segnala SOLO ::printf, ignora mylib::printf

clang-tidy -checks='-*,misc-no-printf' snippets/03_macro_indirection.cpp
# → segnala anche printf nascosto da macro LOG(x)
```

Stesso input a un LLM: confronto live falsi positivi/negativi.
[Tabella di confronto da `comparison/COMPARISON.md`]

---

## Slide 11 — Demo (parte 2): progetto reale

**Caso A — `bear_demo` (Make):**
```bash
bear -- make            # produce compile_commands.json
clang-tidy -p . main.cpp logger.cpp net.cpp util.cpp
# → 5/5 hit, 0 falsi positivi
```

**Caso B — `tinyxml2` (CMake, su macOS Bear non basta):**
```bash
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
clang-tidy -p build --extra-arg=-isysroot $(xcrun --show-sdk-path) \
  tinyxml2.cpp xmltest.cpp
# → 28 hit in 0.2s
```

Punto chiave: **`grep printf` → 42 / clang-tidy AST → 28**. I 14 di scarto sono commenti, stringhe, `vfprintf`/`snprintf`, macro. È esattamente il valore dell'analisi AST.

---

## Slide 12 — Test, doc, CI

- Test in stile lit: `RUN: %check_clang_tidy %s misc-no-printf %t`
- Doc: file `.rst` in `docs/clang-tidy/checks/misc/`
- Integrazione CI: `clang-tidy --warnings-as-errors='*'` in pipeline
- Distribuzione: si compila come parte di `clang-tools-extra`

---

## Slide 13 — Take-away

**Quando scrivere un check custom:**
- pattern interno aziendale ricorrente
- vincolo deterministico, applicabile a ogni PR
- fix-it riapplicabile in CI

**Quando NO:**
- regola fluida → meglio code review umana o LLM
- check di stile generico → c'è già un check ufficiale

**Risorse:**
- `clang-tidy/Contributing.html`
- `LibASTMatchersReference.html`
- repo: `Martin/1_clang-tidy_custom_check/` (questo lavoro)

---

## Note operative

- font monospace: JetBrains Mono / Fira Code
- evidenzia in colore le righe chiave dei matcher
- piano B: video da 2'30" della demo registrato in anticipo
- lingua: italiano
