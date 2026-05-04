# Script verbale del seminario

Tempo target: 15 minuti totali (1' apertura + 9-10' contenuto/demo + 4-5' Q&A).
Tono: tecnico ma colloquiale. Non leggere parola per parola, sono **frasi pivot** per ogni slide.
Italiano, voce calma, ritmo naturale.

---

## Apertura forte (~15 secondi, prima della Slide 1)

> "Vi mostro una cosa strana. Ho dato lo stesso pezzo di codice C++ a Claude Opus 4.7, in due chat completamente separate, con lo stesso prompt. Mi ha dato due risposte tecniche **opposte**. Lo stesso codice, lo stesso modello. Oggi vi spiego come ho costruito uno strumento che invece dà sempre la stessa risposta — e perché questo conta."

*Pausa breve, click su slide 1.*

---

## Slide 1 — Cover (~10s)

> "Buongiorno. Mi chiamo Martin Trajkovski, e nelle prossime cinque minuti vi mostro come si costruisce un check personalizzato di clang-tidy da zero, lo si testa, e lo si confronta con un LLM su casi reali."

**Transizione:** *"Partiamo dal problema concreto."*

---

## Slide 2 — Il problema (~30s)

> "Immaginate di essere in code review e di vedere `printf` in C++ moderno per l'ennesima volta. Vorreste che il CI ve lo segnali da solo, con il fix pronto. Il problema è che gli strumenti testuali — grep, regex — falliscono appena entrano in gioco scope, template, macro: o vi danno troppi falsi positivi o si perdono casi reali. Serve un'analisi che capisca **tipi e scope**, non solo testo."

**Punto da non saltare:** "scope, template, macro" — sono i tre casi che torneranno nella demo.

**Transizione:** *"E uno strumento del genere esiste già: clang-tidy. Vediamo come è fatto dentro."*

---

## Slide 3 — Architettura clang-tidy (~45s)

> "Questo è il flusso. Il sorgente entra nel frontend di Clang che produce un **AST arricchito di semantica** — cioè con tipi risolti, overload selezionati, template istanziati. Sopra l'AST gira il sistema di **AST matchers**, una specie di linguaggio dichiarativo per descrivere pattern. Il **check** vero e proprio è una classe C++ che reagisce ai match e produce diagnostica e fix-it.
>
> I due pezzi in rosso — matchers e check — sono quello che scriviamo noi. Tutto il resto è infrastruttura Clang, gratis. È esattamente la potenza dell'avere il compilatore come libreria."

**Punto da non saltare:** "AST arricchito di semantica" — è la differenza che fa funzionare tutto.

**Transizione:** *"Vediamo veramente cos'è questo AST."*

---

## Slide 4 — L'AST di Clang (~30s)

> "Sei righe per vedere l'AST di un cast `(int)3.14`. Sotto al `VarDecl` c'è il `CStyleCastExpr` con il tipo della conversione, e sotto ancora il `FloatingLiteral`. Notate: il cast non è una stringa, è un **nodo tipato** con la conversione esplicitata.
>
> Questo è il contratto su cui ragionano i check. Tutto il lavoro di parsing, type checking, name resolution è già fatto. Noi attacchiamo qui."

**Transizione:** *"Per matchare nodi su questo AST esiste un piccolo DSL."*

---

## Slide 5 — AST Matchers DSL (~30s)

> "Lettura libera: `callExpr` — qualsiasi chiamata di funzione — `callee` — di cui il chiamato è — `functionDecl` — una dichiarazione di funzione — `hasName "::printf"` — di nome `::printf` nel namespace globale.
>
> Si compone come un albero, è dichiarativo, e — punto importante — il `hasName "::printf"` lavora sul **nome qualificato post-resolution**. Un metodo `Logger::printf` non ha questo nome, quindi non matcha. L'avete già intuito: questo è il pezzo che la regex non può fare."

**Transizione:** *"Vediamo la struttura di un check vera e propria."*

---

## Slide 6 — Anatomia di un check (~25s)

> "Ogni check è una classe che eredita da `ClangTidyCheck` e implementa due hook: `registerMatchers`, dove dichiariamo i pattern, e `check`, che viene chiamato a ogni match. Tutto qui. È piccolo perché tutta la complessità è gestita dal driver."

**Transizione:** *"Ed ecco il check che ho scritto."*

---

## Slide 7 — `misc-no-printf` (~30s)

> "Ho chiamato il mio check `misc-no-printf`. Cosa fa: segnala chiamate a `::printf` di libc, **non** segnala `mylib::printf` (è user-defined, scope diverso), e produce un fix-it che sostituisce solo il nome con `std::print` di C++23.
>
> L'ho scelto come argomento didattico perché è minimale ma non banale: tocca scope, macro, template — i tre casi che fanno fallire un'analisi testuale."

**Transizione:** *"Vediamo le venti righe di codice del matcher."*

---

## Slide 8 — `registerMatchers` (~30s)

> "Questo è il matcher completo. Otto righe. La cosa importante è il `hasName "::printf"`: aggancia il nome **globale**, qualificato. Niente collisioni con namespace utente.
>
> L'`unless(isExpansionInSystemHeader())` esclude rumore dagli header di sistema, perché altrimenti ogni `#include <cstdio>` ci darebbe diagnostica fuori dal nostro codice."

**Transizione:** *"E la funzione `check` che produce diagnostica e fix-it."*

---

## Slide 9 — `check` + diagnostic + fix-it (~30s)

> "Estraggo il `CallExpr` matchato, recupero la posizione del callee — cioè il token `printf` — e produco una diagnostica che include un `FixItHint::CreateReplacement` che sostituisce solo quell'intervallo con la stringa `std::print`. Niente di più, niente di meno.
>
> Il fix-it è **idempotente** e applicabile in CI: clang-tidy lo può applicare in batch su mille file e rilanciarlo a ogni PR."

**Transizione:** *"Tempo di vederlo all'opera. E qui arriva la parte interessante: il confronto con un LLM."*

---

## Slide 10 — Demo + confronto LLM (~75s, è la slide più densa)

> "Ho preparato sei snippet di esempio. Lancio clang-tidy: sul caso namespace, **un solo hit**, `mylib::printf` ignorato. Sul caso macro: vede dentro l'espansione, due hit. Sul caso più tosto — quello con `using logging::printf` e overload resolution — il check sa che `::printf("...", 1)` con argomento `int` risolve in realtà al template `logging::printf`, non a libc. Quindi un solo hit, dentro il wrapper.
>
> Stesso codice dato a Claude Opus 4.7 e ChatGPT 5.5. Ho fatto **24 chat fresche** in totale — sei snippet, due modelli, due esecuzioni indipendenti per testare anche il determinismo.
>
> Risultati. Determinismo: clang-tidy 100%, Claude alto su 5 snippet su 6, ChatGPT medio. Il fix raccomandato — `std::print` di C++23 — Claude lo propone in 4 casi su 6, ChatGPT mai come fix primario. Lo snippet 6 è quello che mi ha sorpreso di più: lì entrambi i modelli, in due chat fresche dello stesso modello, danno **interpretazioni opposte** dell'overload resolution. Uno dice 'ah, qui `::printf` risolve al wrapper'; l'altro dice 'no, è libc'. Stesso codice, stesso modello.
>
> Ma c'è anche l'altro lato della medaglia. Sullo snippet 4 entrambi i modelli pescano un bug che il mio check non vede: `static_cast<int>(2.5)` tronca a `2`, perdita silenziosa di precisione. È un bug semantico fuori dal mandato del check. Quindi non sono in competizione: fanno cose diverse."

**Punti da non saltare:**
- "24 chat fresche" — la cifra che dà credibilità al confronto
- "interpretazioni opposte sullo stesso modello" — è il wow moment
- "bug che il check non vede" — onestà del confronto

**Transizione:** *"Stessa storia su un progetto reale, non più toy example."*

---

## Slide 11 — Demo progetto reale (~45s)

> "Due casi. Prima `bear_demo`: un piccolo progetto Make. Bear intercetta la build, produce `compile_commands.json`, clang-tidy gira: cinque hit su cinque, zero falsi positivi. Pattern realistico: un team eredita un progetto con build proprio e ci attacca un linter senza riscriverlo.
>
> Secondo caso, `tinyxml2` da GitHub. CMake, due unit di compilazione, ventotto hit in zero virgola due secondi. Il dato che vi voglio lasciare è questo: **`grep printf` su quel codice trova quarantadue match. clang-tidy ne segnala ventotto. I quattordici di scarto sono commenti, stringhe, `vfprintf`, `snprintf`, macro che `printf` non lo chiamano.** Quei quattordici sono esattamente i falsi positivi che un linter testuale produrrebbe. È il valore numerico misurabile dell'analisi AST."

**Punto da non saltare:** "42 → 28, i 14 di scarto sono il valore dell'AST" — il numero che si ricorda.

**Transizione:** *"Per chiudere il cerchio, come si integra in produzione."*

---

## Slide 12 — Test, doc, CI (~25s)

> "Quattro pezzi. Test in stile lit, lo standard di LLVM: `RUN: %check_clang_tidy` e funziona dentro la build farm ufficiale. Documentazione in `.rst` come tutti i check ufficiali. Integrazione CI: `--warnings-as-errors='*'` blocca il merge. E distribuzione: il check si compila come parte di `clang-tools-extra`, quindi può anche essere proposto upstream."

**Transizione:** *"Take-away."*

---

## Slide 13 — Take-away (~40s)

> "Tre cose. Quando scrivere un check custom: pattern aziendale ricorrente, vincolo deterministico, fix riapplicabile. Quando NO: regola fluida — meglio review umana o LLM — oppure stile generico, dove esiste già un check ufficiale.
>
> E sul confronto con gli LLM, la cosa che mi premeva di più: **non sono sostituti**. Clang-tidy è preciso e deterministico dove conta — scope, tipi, overload resolution. Gli LLM trovano bug fuori dal mandato del check, come il troncamento `2.5` in `2`. Sono **complementari**. Un LLM può aiutare a scrivere un nuovo check; clang-tidy poi lo applica per sempre, in modo idempotente, su milioni di righe."

---

## Chiusura forte (~15 secondi, dopo Slide 13)

> "Quindi, a domanda 'meglio clang-tidy o un LLM' la risposta onesta è: **dipende da che domanda fate al codice**. Se la domanda ha una risposta deterministica, vince clang-tidy. Se la domanda richiede comprensione del contesto, vince l'LLM. Il confronto che ho fatto vi mostra dove sta il confine. Grazie."

*Pausa. Aspetta domande.*

---

## Q&A — domande probabili e risposte preparate

### Q: "Quanto ci hai messo a scriverlo?"
> "Il check in sé è meno di 50 righe. Il grosso del tempo è andato sul **build di LLVM da sorgente** (~30 minuti la prima volta), il **lit test ufficiale** integrato nella build farm, e le run sui progetti reali. Diciamo 25-30 ore totali, ma se rifacessi un secondo check sarebbe 1-2 ore."

### Q: "Su grossi codebase è scalabile?"
> "Sì, e questo è uno dei punti forti. clang-tidy è parallelizzabile per file, idempotente, e ha un costo per file dell'ordine dei millisecondi una volta che `compile_commands.json` esiste. Cosa che con un LLM è impraticabile: rate limit, costi token, non determinismo. Su 10⁵ file c'è una sola scelta sensata."

### Q: "Perché hai scelto `std::print` come fix e non `std::cout`?"
> "Perché `std::print` (C++23) è il rimpiazzo type-safe più diretto di `printf`. Mantiene la format string come idea, ma con controllo a compile time. `std::cout` è il fallback C++20, ma cambia stile (operatore `<<`). Volevo un fix-it che fosse drop-in il più possibile."

### Q: "Hai contato anche i falsi negativi del check?"
> "Su `tinyxml2` zero, perché `printf` libc è la cosa che cerchiamo specificamente. Su un caso più sofisticato — chiamata via puntatore a funzione — il check li **deliberatamente** ignora perché non sono `callee(functionDecl)` matchabili. È la trade-off: precisione contro recall. Ho preferito massimizzare la precisione per stare a zero falsi positivi."

### Q: "Il confronto LLM è statisticamente significativo? Solo 24 run."
> "Onestissima la critica: 24 run è un sample piccolo. Il pattern 'ChatGPT non propone mai `std::print` come fix primario' è 0 su 6 snippet — è un risultato qualitativo, non statistico. Per averlo statisticamente significativo servirebbero centinaia di run su decine di snippet. Quello che mostro è un'**indicazione qualitativa** robusta in due aspetti: i due LLM testati hanno risposte qualitativamente diverse tra loro, e lo stesso LLM è incoerente con sé stesso tra chat fresche."

### Q: "L'avresti potuto fare con `grep -P` e basta?"
> "Lasciatemi rispondere col numero: `grep printf` su `tinyxml2` trova **42 match**. Di quei 42, **14 sono falsi positivi** evidenti: commenti, stringhe, `vfprintf`, `snprintf`, macro. Quindi `grep` ti dà 33% di rumore. Su un progetto da 1000 file, 33% di falsi positivi è ingestibile in code review. Per quello esiste l'AST."

---

## Note pratiche per la presentazione

1. **Velocità di lettura:** la slide 10 è densa, **NON** leggere la tabella riga per riga. Indica con il puntatore "ecco i 24 run, ecco lo 0 su 6 di ChatGPT, ecco il 50/50 dello snippet 6".
2. **Demo viva:** se hai tempo per la demo (slide 10 e 11), batti i comandi a schermo. Se sei in ritardo, salta direttamente al video di backup di 2'30".
3. **Pause:** dopo "due risposte tecniche opposte" nell'apertura, e dopo "interpretazioni opposte" nello slide 10, fai una pausa di 1-2 secondi. È il momento da far atterrare.
4. **Q&A:** se non sai una risposta, dilla: "non ho misurato esattamente quel caso, ma proverei prima X e Y per capire". Onestà > finta sicurezza.
5. **Tempo:** se vai lungo, taglia slide 4 (AST dump) — è la più sacrificabile. Se sei velocissimo, espandi slide 11 con il grep vs AST.

---

## Cose da NON dire

- Non dire "questo è un argomento difficile" o "magari non avete capito". Tono assertivo.
- Non scusarti per la durata. Hai 15 minuti, prendili.
- Non dire "ovviamente". Quello che è ovvio per te magari non lo è per chi ascolta.
- Non leggere le slide. Le slide sono lì per chi vuole rivederle dopo. Tu parli con chi ascolta.
