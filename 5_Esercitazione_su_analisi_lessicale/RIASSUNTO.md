# 5. Esercitazione su analisi lessicale — Lexer per C++ (Flex)

## Obiettivo
Costruire con Flex un lexer per un sottoinsieme significativo del C++ (basato su una bozza dello standard).

## File contenuti
- `lexer-cpp.ll` — sorgente Flex (~277 righe)
- `Makefile` — regola di build
- `note.txt` — note

## Note di scope (dal `note.txt`)
- Specifica = bozza del nuovo standard C++
- **Non gestiti:** direttive del preprocessore (#include, #ifdef, macro), raw string literals, user-defined literals

## Build
```bash
make
# equivale a:
flex -o lexer-cpp.cc lexer-cpp.ll
g++ -Wall -Wextra -O3 -o lexer-cpp lexer-cpp.cc
```

## Struttura del sorgente Flex

### Definition section (`%{ ... %}` + pattern con nome)
- Funzioni inline `print_token(...)` per stampare la categoria (keyword, identifier, integer, floating, character, string, punctuator, comment) e il lessema (`yytext`, `yylineno`)
- `print_error()` invoca `yy_fatal_error`
- Opzioni: `noyywrap`, `nounput`, `nodefault`, `yylineno`
- Start state esclusivo: `%x comment_mode`
- Pattern con nome principali:
  - `DIGIT [0-9]`, `NONZERO_DIGIT [1-9]`, `BIN_DIGIT [01]`, `OCT_DIGIT [0-7]`, `HEX_DIGIT [0-9a-fA-F]`
  - `HEX_QUAD` = 4 hex; `UCN` = `\uXXXX` o `\UXXXXXXXX` (universal char names)
  - `NONDIGIT [a-zA-Z_]`; `IDENT_NONDIGIT = NONDIGIT | UCN`
  - Prefissi numerici `BIN_PREFIX 0b|0B`, `HEX_PREFIX 0x|0X`
  - `SEP_OPT "'"?` — supporta i digit separator C++14 (`1'000'000`)
  - Sequenze: `BIN_SEQ`, `DEC_SEQ`, `HEX_SEQ` (digit con separatore opzionale)
  - Letterali: `BIN_LITERAL`, `OCT_LITERAL` (`0` seguito da OCT_DIGIT), `DEC_LITERAL`, `HEX_LITERAL`
  - Suffissi int: `U`, `L`, `LL` con tutte le combinazioni → `INT_SUFFIX`
  - `ENC_PREFIX u8|u|U|L` per char/string
  - Escape: `SIMPLE_ESCAPE [\\]['"\?\\abfnrtv]`, `OCT_ESCAPE`, `HEX_ESCAPE`
  - `C_CHAR`/`S_CHAR` (caratteri ammessi in char/string literal)
  - Float: `FRACT`, `EXP`, `HEX_FRACT`, `BIN_EXP` (esponente binario per hex floats), `FLOAT_SUFFIX [flFL]`

### Rule section
1. **Keywords** — lunghissima alternanza di tutte le parole chiave C++ (`alignas`, `alignof`, ..., `while`) → `print_keyword()`. Notare l'uso di `|` come azione "vai alla regola successiva".
2. **Punctuators** — operatori e punteggiatura, comprese le forme alternative C++ (`<%` per `{`, `compl` per `~`, `not` per `!`, `bitand` per `&`, `xor` per `^`, ...) → `print_punctuator()`.
3. **Identifiers** — `IDENT_NONDIGIT(IDENT_NONDIGIT|DIGIT)*`. Va dopo le keyword: la priorità per ordine garantisce che `for` sia keyword, non identifier.
4. **Literals**
   - Integer: bin/oct/dec/hex con suffisso opzionale
   - Char: `ENC_PREFIX? '...'`
   - Float: tre forme (decimale con/senza esponente, hex con `p`/`P`)
   - String: `ENC_PREFIX? "..."`
5. **Commenti**
   - `//.*` per single-line
   - Multiline gestito con start state `comment_mode` (entra su `/*`, esce su `*/`, scarta tutto il resto). Approccio raccomandato dalle slide rispetto alla RE monolitica.
6. **Whitespace** — scartati: `[ \f\n\t\v]` e line-continuation `\\\n`
7. **Catch-all errore** — `.` invoca `print_error()` e ritorna 0.

### User code section
```c++
int main() {
  while (yylex() != 0);
  return 0;
}
```

## Lezioni operative dal codice
- **Ordine = priorità:** keyword prima di identifier; multi-char operatori prima dei singoli (`<<=` prima di `<<` prima di `<`)
- **Lessema più lungo:** già garantito da Flex, ma le definizioni `INT_SUFFIX` mettono prima `LL` poi `L` per evitare match troncati
- **Pattern composabili:** spezzando la spec in tante definizioni (digit → seq → literal → letterale-con-suffisso) si ottiene leggibilità
- **Start state per i commenti:** evita la regex monolitica `/\*([^*]|\*+[^/*])*\*+/` e non rischia overflow di buffer
- **`<<EOF>>` non gestito esplicitamente nel comment_mode in questo file:** un commento aperto e mai chiuso causerebbe scanning silenzioso fino a fine file (variante più robusta nelle slide)

## Punti chiave per l'orale / esame
- Spiegare la struttura .ll e cosa fa ciascuna sezione
- Saper giustificare le scelte: ordine delle regole, uso di start state per i commenti, uso di pattern con nome per la leggibilità
- Saper aggiungere/modificare una regola (es. raw string literal, oppure preprocessor directive)
