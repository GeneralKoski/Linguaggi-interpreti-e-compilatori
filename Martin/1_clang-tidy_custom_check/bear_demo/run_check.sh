#!/usr/bin/env bash
# Demo: usa Bear per generare compile_commands.json e poi lancia clang-tidy.
set -euo pipefail

DIR="$(cd "$(dirname "$0")" && pwd)"
LLVM="${LLVM_PROJECT:-$HOME/llvm-project}"
TIDY="$LLVM/build/bin/clang-tidy"

cd "$DIR"

echo "==> make clean"
make clean >/dev/null

echo "==> bear -- make"
bear -- make

echo
echo "==> compile_commands.json prodotto:"
ls -la compile_commands.json

echo
echo "==> run-clang-tidy con misc-no-printf"
"$LLVM/build/bin/run-clang-tidy" -p . -checks='-*,misc-no-printf' \
  -clang-tidy-binary "$TIDY" \
  main.cpp logger.cpp net.cpp util.cpp || true

echo
echo "==> --fix in dry-run (mostra solo le modifiche)"
"$TIDY" -p . -checks='-*,misc-no-printf' --export-fixes=fixes.yaml main.cpp logger.cpp net.cpp || true
echo "fix esportati in fixes.yaml"
