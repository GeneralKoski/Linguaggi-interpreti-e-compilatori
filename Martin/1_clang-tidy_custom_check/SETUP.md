# Setup ambiente — fatto il 2026-05-04

Toolchain installata e LLVM clonato. La build di `clang-tidy` è in corso.

## Comandi eseguiti

```bash
# 1. Prereq
brew install ninja bear        # cmake già presente
# verificato:
#   ninja 1.13.2
#   bear  4.1.2
#   clang Apple (sistema)

# 2. Sorgenti
cd ~
git clone --depth 1 https://github.com/llvm/llvm-project.git
# clone fatto in ~/llvm-project (~3 GB)

# 3. Configurazione build
cmake -S ~/llvm-project/llvm -B ~/llvm-project/build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
  -DLLVM_TARGETS_TO_BUILD="AArch64" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DLLVM_OPTIMIZED_TABLEGEN=ON \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++

# 4. Build (long running, 30-60 min su M-series)
ninja -C ~/llvm-project/build clang-tidy
```

## Note di setup

- `LLVM_TARGETS_TO_BUILD="AArch64"` (solo Apple Silicon, taglia ~25% del tempo)
- `LLVM_ENABLE_ASSERTIONS=OFF` per build più veloce
- `LLVM_OPTIMIZED_TABLEGEN=ON` velocizza la generazione di codice
- Spazio richiesto: ~10-12 GB nella cartella `build/`
- Dopo la build, eseguibile in `~/llvm-project/build/bin/clang-tidy`

## Struttura repo

I sorgenti del nostro check vivono in `check/` (questo repo) e vengono **copiati/linkati** dentro `~/llvm-project/clang-tools-extra/clang-tidy/misc/`. Lo script `install_check.sh` automatizza l'operazione.
