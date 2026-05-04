#!/usr/bin/env bash
# Installa NoPrintfCheck dentro llvm-project e ribuilda clang-tidy.
# Idempotente: salta i passi già fatti.

set -euo pipefail

REPO="$(cd "$(dirname "$0")" && pwd)"
LLVM="${LLVM_PROJECT:-$HOME/llvm-project}"
MISC="$LLVM/clang-tools-extra/clang-tidy/misc"

if [[ ! -d "$MISC" ]]; then
  echo "ERRORE: $MISC non esiste. Imposta LLVM_PROJECT o clona llvm-project in ~/." >&2
  exit 1
fi

# 1. Copia i sorgenti del check
cp -v "$REPO/check/NoPrintfCheck.h"   "$MISC/"
cp -v "$REPO/check/NoPrintfCheck.cpp" "$MISC/"

# 2. Patch CMakeLists.txt — aggiunge il file alla lista delle source
CMAKE="$MISC/CMakeLists.txt"
if ! grep -q "NoPrintfCheck.cpp" "$CMAKE"; then
  # inseriamo prima della riga vuota che separa la lista dai LINK_LIBS
  awk '
    /UseInternalLinkageCheck.cpp/ && !done {
      print
      print "  NoPrintfCheck.cpp"
      done = 1
      next
    }
    { print }
  ' "$CMAKE" > "$CMAKE.tmp" && mv "$CMAKE.tmp" "$CMAKE"
  echo "patched $CMAKE"
else
  echo "$CMAKE: NoPrintfCheck.cpp già presente, skip"
fi

# 3. Patch MiscTidyModule.cpp — include + registerCheck
MOD="$MISC/MiscTidyModule.cpp"
if ! grep -q "NoPrintfCheck.h" "$MOD"; then
  awk '
    /^#include "NewDeleteOverloadsCheck\.h"$/ && !done {
      print
      print "#include \"NoPrintfCheck.h\""
      done = 1
      next
    }
    { print }
  ' "$MOD" > "$MOD.tmp" && mv "$MOD.tmp" "$MOD"
  echo "patched $MOD include"
fi

if ! grep -q "misc-no-printf" "$MOD"; then
  awk '
    /registerCheck<NoRecursionCheck>/ && !done {
      print
      print "    CheckFactories.registerCheck<NoPrintfCheck>(\"misc-no-printf\");"
      done = 1
      next
    }
    { print }
  ' "$MOD" > "$MOD.tmp" && mv "$MOD.tmp" "$MOD"
  echo "patched $MOD registerCheck"
fi

# 4. Documentazione (.rst)
DOCS="$LLVM/clang-tools-extra/docs/clang-tidy/checks/misc"
LIST="$LLVM/clang-tools-extra/docs/clang-tidy/checks/list.rst"
cp -v "$REPO/check/no-printf.rst" "$DOCS/"
if ! grep -q "misc-no-printf <misc/no-printf>" "$LIST"; then
  awk '
    /misc-no-recursion <misc\/no-recursion>/ && !done {
      print "   :doc:`misc-no-printf <misc/no-printf>`, \"Yes\""
      done = 1
    }
    { print }
  ' "$LIST" > "$LIST.tmp" && mv "$LIST.tmp" "$LIST"
  echo "patched $LIST"
fi

# 5. Build
echo "==> ninja clang-tidy"
ninja -C "$LLVM/build" clang-tidy
