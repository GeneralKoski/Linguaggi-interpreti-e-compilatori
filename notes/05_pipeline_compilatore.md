# Pipeline completa: dalla sorgente al binario

Tracciamento end-to-end della compilazione di un piccolo programma C con Clang/LLVM.

## Esempio
```c
// main.c
int square(int x) { return x * x; }

int main(int argc, char** argv) {
    return square(argc);
}
```

## Stadi

### 1. Preprocessing (CPP)
```
clang -E main.c -o main.i
```
- Espansione di `#include`, `#define`, `#ifdef`
- Rimozione commenti
- Output: una **unità di traduzione** (translation unit)

### 2. Front end (Clang front-end)
Suddiviso in:

#### 2a. Analisi lessicale (lexing)
- Input: caratteri di `main.i`
- Tool concettuale: DFA generato da specifica RE (in Clang è scritto a mano)
- Output: stream di token `⟨IDENT, square⟩, ⟨LPAREN, (⟩, ⟨KWD, int⟩, ...`

#### 2b. Analisi sintattica (parsing)
- Input: token
- Tool: parser recursive-descent in Clang (scritto a mano, hand-written)
- Output: parse tree → AST (in Clang costruito direttamente)
- Visualizzabile: `clang -Xclang -ast-dump -fsyntax-only main.c`

#### 2c. Analisi di semantica statica (Sema)
- Costruisce/arricchisce AST con tipi, scope, conversioni implicite
- Risolve overloading, template (in C++)
- Genera diagnostiche

### 3. Generazione IR (CodeGen)
- AST arricchito → **LLVM IR**
- Comando: `clang -S -emit-llvm main.c -o main.ll`
- Esempio output:
```llvm
define i32 @square(i32 %x) {
  %1 = mul nsw i32 %x, %x
  ret i32 %1
}
define i32 @main(i32 %argc, i8** %argv) {
  %1 = call i32 @square(i32 %argc)
  ret i32 %1
}
```

### 4. Middle end (LLVM `opt`)
- Sequenza di **passi di analisi e trasformazione** sull'IR
- Comando manuale: `opt -O2 main.ll -S -o main.opt.ll`
- Passi tipici (ordine importa):
  1. **mem2reg** (promozione di alloca a registri SSA)
  2. **instcombine** (peephole su istruzioni IR)
  3. **simplifycfg** (rimozione branch ridondanti, BB vuoti)
  4. **GVN** (Global Value Numbering)
  5. **LICM** (Loop Invariant Code Motion)
  6. **loop-unroll**
  7. **inline** (function inlining, gated by costo/beneficio)
  8. **dce / dse** (dead-code/dead-store elimination)
  9. **constant propagation/folding**
  10. ... molti altri (decine di passi)
- Tutti operano su LLVM IR (input e output) → idempotenti su semantica

### 5. Back end (LLVM `llc`)
- IR ottimizzata → assembly target
- Comando: `llc main.opt.ll -o main.s`
- Sotto-passi:
  1. **Instruction selection** (DAG → SDAG → MachineInstr) — pattern matching
  2. **Register allocation** (graph coloring + heuristics; LLVM usa "greedy" di default)
  3. **Instruction scheduling** (pre-RA e post-RA)
  4. **Prologue/epilogue insertion** (gestione stack frame, save/restore registri)
  5. **Peephole** finale

### 6. Assembler
```
as main.s -o main.o
```
Produce un file oggetto **rilocabile** (ELF/Mach-O/COFF) con:
- Sezioni: `.text` (codice), `.data`, `.bss`, `.rodata`
- Tabella simboli, relocation entries

### 7. Linker
```
ld main.o -lc -o main
```
- Risolve simboli esterni (`printf` → libc)
- Concatena sezioni di tutti gli oggetti
- Riempie le **rilocazioni** (indirizzi assoluti/relativi finali)
- Output: eseguibile (statico o dinamico)

### 8. Loader (a run-time)
- L'OS mappa l'eseguibile in memoria
- Risolve i simboli del **dynamic linker** (`ld.so` su Linux)
- Salta a `_start` → `__libc_start_main` → `main`

---

## Comandi di debug utili

| Comando | Cosa mostra |
|---|---|
| `clang -E main.c` | output del preprocessore |
| `clang -Xclang -ast-dump -fsyntax-only main.c` | AST |
| `clang -S -emit-llvm main.c` | LLVM IR (senza ottimizzazioni) |
| `clang -O2 -S -emit-llvm main.c` | LLVM IR ottimizzato |
| `opt -O2 -print-after-all main.ll` | IR dopo ogni passo |
| `clang -S main.c` | assembly |
| `objdump -d main.o` | disassemblato del .o |
| `nm main.o` | simboli |
| `readelf -a main` | metadati ELF |

## Connessione con il corso
- **Stadi 2a-2c**: capitoli 2-4 di EaC, slide 4-6 del corso
- **Stadio 3 (CodeGen IR)**: capitolo 5-7 di EaC, slide 7
- **Stadio 4 (middle end)**: capitolo 8-10 di EaC, slide 8
- **Stadio 5 (back end)**: capitolo 11-13 di EaC (non centrale nel corso)
- **Stadi 6-8**: tipicamente fuori scope; assemblaggio/linking studiato in altri corsi (Sistemi Operativi, Architettura)
