# Strumenti: Flex, Bison, Clang/LLVM

## Flex (lexer generator)

### Comandi essenziali
```bash
flex -o lexer.c lexer.ll          # genera lexer.c
gcc -Wall -O2 -o lexer lexer.c    # compila
./lexer < input.txt               # esegue
```

### Struttura file `.ll`
```
%{
   // codice C (literal block, copiato verbatim)
%}

%option noyywrap nodefault yylineno
%x STATO_ESCLUSIVO

NOME_PATTERN  espressione

%%
   /* sezione regole */
pattern   { azione_C; }

%%
   /* sezione codice utente */
int main() { while(yylex()); return 0; }
```

### API runtime
- `int yylex()` — chiama il lexer, ritorna il token
- `char* yytext` — puntatore al lessema
- `int yyleng` — lunghezza
- `int yylineno` — numero riga (con `%option yylineno`)
- `BEGIN STATE;` — entra in uno start state
- `<<EOF>>` — pseudo-pattern di fine file

### Opzioni utili
- `%option noyywrap` (sempre)
- `%option nodefault` (sempre)
- `%option yylineno`
- `%option case-insensitive`
- `%x NOMESTATO` (start state esclusivo)

---

## Bison (parser generator)

### Comandi essenziali
```bash
bison -d -o parser.c parser.yy    # genera parser.c + parser.h
# Oppure (come in TinyP):
bison --report=all --defines=parser.hh -o parser.cc parser.yy
```

### Struttura file `.yy`
```
%{
   // includes, dichiarazioni
%}

%token TOK_INT TOK_PLUS ...
%left '+' '-'
%left '*' '/'

%%
   /* sezione regole grammaticali */
prog: stmt_list
    ;
stmt: ID '=' expr ';'   { $$ = make_assign($1, $3); }
    ;
expr: expr '+' expr     { $$ = make_add($1, $3); }
    | NUM               { $$ = make_num($1); }
    ;

%%
   /* sezione utente */
```

### Pseudo-variabili nelle azioni
- `$$` — valore semantico del non-terminale a sinistra
- `$1`, `$2`, ..., `$n` — valori dei simboli a destra (left-to-right)
- `@$`, `@1`, ... — informazioni di posizione (file/line)

### Tipi e attributi
```
%union {
   int int_val;
   char* str_val;
   Node* node;
}
%token <int_val> NUM
%token <str_val> ID
%type <node> expr stmt
```

---

## Clang/LLVM

### Pipeline
```
.c → clang → .ll (LLVM IR testo) → opt → .ll' → llc → .s → as → .o → ld → eseguibile
```

### Comandi utili
```bash
# AST dump
clang -Xclang -ast-dump -fsyntax-only main.c

# IR generato dal frontend (no ottimizzazioni)
clang -O0 -S -emit-llvm main.c -o main.ll
clang -O0 -S -emit-llvm -fno-discard-value-names main.c -o main.ll  # nomi leggibili

# IR con ottimizzazioni
clang -O2 -S -emit-llvm main.c -o main.opt.ll

# Bitcode binario
clang -c -emit-llvm main.c -o main.bc
llvm-dis main.bc -o main.ll       # bitcode → testo
llvm-as main.ll -o main.bc        # testo → bitcode

# Applicare passi specifici
opt -passes=mem2reg,instcombine,gvn,dce main.ll -S -o main.opt.ll

# Vedere passi disponibili
opt --print-passes

# Compilare IR a assembly
llc main.opt.ll -o main.s

# Disassemblare un .o
objdump -d main.o
llvm-objdump -d main.o
```

### Costrutti LLVM IR essenziali
```llvm
; Variabili locali in stack
%x = alloca i32, align 4
store i32 42, i32* %x, align 4
%v = load i32, i32* %x, align 4

; Aritmetica
%sum = add nsw i32 %a, %b
%prod = mul nsw i32 %a, %b
%div = sdiv i32 %a, %b

; Confronti e branch
%cmp = icmp slt i32 %a, %b
br i1 %cmp, label %then, label %else

; Phi (SSA)
%v = phi i32 [ %x, %bb1 ], [ %y, %bb2 ]

; Chiamate
%r = call i32 @foo(i32 %a, i32 %b)
ret i32 %r

; Indirizzi (GEP)
%p = getelementptr inbounds [10 x i32], [10 x i32]* %arr, i32 0, i32 %i
```

### Tipi LLVM
- `void`, `iN` (interi N-bit), `half/float/double/fp128`
- Puntatori: `T*`
- Aggregati: `[N x T]` array, `<N x T>` vector, `{T1, T2, ...}` struct
- Funzioni: `RetT (ArgT, ...)`

### Linkage
- `external` (default per dichiarazioni)
- `internal` (file-local, simile a `static` C)
- `weak`, `linkonce` (deduplicazione tra TU)
- `private` (mai esportato neanche al linker)
- `dso_local` (visibile via DSO locale)

---

## Make / build

Pattern tipico per un mini-compiler (vedi TinyP):
```makefile
CXX = clang++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2

lexer.cc: lexer.ll parser.hh
	flex -o lexer.cc lexer.ll

parser.cc parser.hh: parser.yy
	bison --defines=parser.hh -o parser.cc parser.yy

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

mycompiler: lexer.o parser.o main.o
	$(CXX) $^ -o $@
```

## Risorse
- *Engineering a Compiler*, 2nd ed. — Cooper & Torczon (libro adottato)
- *flex & bison* — John Levine, O'Reilly 2009
- LLVM Language Reference: https://llvm.org/docs/LangRef.html
- Clang internals: https://clang.llvm.org/docs/InternalsManual.html
