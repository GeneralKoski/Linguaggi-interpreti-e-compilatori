---
marp: true
theme: default
paginate: true
size: 16:9
header: 'clang-tidy custom check'
footer: 'Martin Trajkovski — Linguaggi, Interpreti e Compilatori — A.A. 2025/26'
style: |
  section {
    font-family: "Helvetica Neue", "Arial", sans-serif;
    font-size: 22px;
    padding: 40px 55px 50px 55px;
  }
  h1 { color: #1a4d7a; font-weight: 600; margin-bottom: 0.3em; }
  h2 { color: #2e6da4; font-weight: 600; margin-bottom: 0.3em; }
  h3 { color: #2e6da4; margin-bottom: 0.2em; }
  ul, ol { margin: 0.3em 0; }
  li { margin: 0.15em 0; }
  p { margin: 0.4em 0; }
  code, pre {
    font-family: "JetBrains Mono", "Fira Code", "Menlo", monospace;
  }
  pre {
    font-size: 0.72em;
    line-height: 1.35;
    background: #f6f8fa;
    border-left: 3px solid #2e6da4;
    padding: 0.5em 0.9em;
    border-radius: 4px;
    margin: 0.4em 0;
  }
  code {
    background: #eef2f7;
    padding: 0.1em 0.3em;
    border-radius: 3px;
  }
  blockquote {
    border-left: 4px solid #d9534f;
    color: #444;
    font-style: italic;
    padding-left: 0.8em;
    margin: 0.4em 0;
  }
  table {
    font-size: 0.78em;
    border-collapse: collapse;
    margin: 0.4em 0;
  }
  th { background: #2e6da4; color: white; padding: 0.35em 0.7em; }
  td { border: 1px solid #ddd; padding: 0.25em 0.55em; }
  section.cover {
    background: linear-gradient(135deg, #1a4d7a 0%, #2e6da4 100%);
    color: white;
    text-align: center;
  }
  section.cover h1 { color: white; font-size: 1.8em; }
  section.cover h2 { color: #cfe2f3; font-weight: 400; font-size: 1em; }
  section.cover code {
    background: rgba(255, 255, 255, 0.18);
    color: white;
    padding: 0.05em 0.35em;
    border-radius: 4px;
  }
  section.cover header, section.cover footer { color: #cfe2f3; }
  header { font-size: 0.55em; color: #888; }
  footer { font-size: 0.55em; color: #888; }
  .flow {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 8px;
    font-family: "JetBrains Mono", "Fira Code", "Menlo", monospace;
    font-size: 14px;
    margin: 18px 0 8px 0;
    flex-wrap: nowrap;
  }
  .flow .box {
    border: 2px solid #888;
    background: #f6f8fa;
    padding: 8px 10px;
    border-radius: 6px;
    text-align: center;
    line-height: 1.25;
    white-space: nowrap;
  }
  .flow .box.ours {
    border-color: #d9534f;
    background: #fdf2f2;
    font-weight: 600;
  }
  .flow .box.out {
    border-color: #5cb85c;
    background: #f2f9f2;
  }
  .flow .arr {
    color: #2e6da4;
    font-size: 1.4em;
    font-weight: 600;
  }
  .flow-caption {
    text-align: center;
    font-size: 0.78em;
    color: #666;
    margin: 0 0 12px 0;
  }
---

<!-- _class: cover -->

# Un check `clang-tidy` da zero

## Costruzione, test e integrazione di un linter C++ basato su AST

Martin Trajkovski
*Linguaggi, Interpreti e Compilatori — A.A. 2025/26*

---

## Il problema

> "In code review continuo a vedere `printf` in C++ moderno.
> Vorrei che il CI me lo segnali da solo, con il fix pronto."

- Pattern ripetitivi → review automatizzata
- Strumenti testuali (grep, regex) → falsi positivi su scope/template/macro
- Servirebbe un'analisi che capisce **tipi e scope**

---

## Architettura clang-tidy

<div class="flow"><div class="box">codice.cpp</div><div class="arr">→</div><div class="box">clang<br>frontend</div><div class="arr">→</div><div class="box">AST<br>+ sema</div><div class="arr">→</div><div class="box ours">AST<br>matchers</div><div class="arr">→</div><div class="box ours">check</div><div class="arr">→</div><div class="box out">diagnostics<br>+ fix-it</div></div>

<div class="flow-caption">in <span style="color:#d9534f"><b>rosso</b></span>: i due pezzi che scriviamo noi</div>

- `clang-tidy` = libreria di check + driver, ogni check estende `ClangTidyCheck`
- usa l'**AST arricchito**: tipi, overload, template istantiati

---

## L'AST di Clang in 6 righe

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

## AST Matchers DSL

Linguaggio dichiarativo per descrivere pattern:

```cpp
// "ogni chiamata a una funzione di nome ::printf"
callExpr(callee(functionDecl(hasName("::printf"))))

// "ogni metodo virtuale senza override"
cxxMethodDecl(isVirtual(), unless(isOverride()))
```

Più conciso del `RecursiveASTVisitor` per pattern semplici.

---

## Anatomia di un check

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

## Il nostro check: `misc-no-printf`

**Cosa fa:**
- segnala chiamate a `::printf` (libc)
- **non** segnala `mylib::printf` (scope diverso)
- emette un fix-it: sostituisce il nome con `std::print`

**Perché didatticamente:**
- matcher minimale ma "non triviale" (scope, macro, template)
- fix-it preciso (solo callee, non l'intera call)

---

## `registerMatchers`

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

## `check` + diagnostic + fix-it

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

## Demo (parte 1): esempi piccoli + confronto LLM

```bash
clang-tidy -checks='-*,misc-no-printf' snippets/02_namespace_collision.cpp
# → 1 hit: SOLO ::printf, ignora mylib::printf

clang-tidy -checks='-*,misc-no-printf' snippets/06_using_template_alias.cpp
# → 1 hit: overload resolution dirotta `::printf("...", int)` al template
```

Confronto reale: **6 snippet × {Claude Opus 4.7, ChatGPT 5.5} × 2 chat fresche = 24 run**

| | clang-tidy | Claude | ChatGPT |
|---|---|---|---|
| Determinismo | 100% | alto su 5/6 | medio |
| Fix primaria = `std::print` | sì | 4/6 | **0/6** |
| Snippet 06 (overload res.) | sempre 1 hit | 1ª ✓ / 2ª ✗ | 1ª ✗ / 2ª ✓ |
| Bug fuori scope (`2.5→2`) | non visto | trovato ⭐ | trovato ⭐ |

**Wow moment:** stesso modello, due chat fresche → **interpretazioni opposte** dell'overload resolution.

---

## Demo (parte 2): progetto reale

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

Punto chiave: **`grep printf` → 42 / clang-tidy AST → 28**. I 14 di scarto sono commenti, stringhe, `vfprintf`/`snprintf`, macro.

---

## Test, doc, CI

- **Test in stile lit**: `RUN: %check_clang_tidy %s misc-no-printf %t`
- **Doc**: file `.rst` in `docs/clang-tidy/checks/misc/`
- **Integrazione CI**: `clang-tidy --warnings-as-errors='*'` in pipeline
- **Distribuzione**: si compila come parte di `clang-tools-extra`

---

## Take-away

**Quando scrivere un check custom:** pattern interno ricorrente · vincolo deterministico · fix-it riapplicabile in CI

**Quando NO:** regola fluida (meglio review umana / LLM) · stile generico (esiste già un check ufficiale)

**LLM e clang-tidy sono complementari, non sostituibili:**
- clang-tidy = deterministico sul suo mandato
- LLM = trova bug fuori scope (`static_cast<int>(2.5) → 2`)
- ma su precisione AST gli LLM **fluttuano tra esecuzioni**

**Risorse:** `clang-tidy/Contributing.html` · `LibASTMatchersReference.html`
repo: `Martin/1_clang-tidy_custom_check/`
