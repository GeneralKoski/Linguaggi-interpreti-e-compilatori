//===--- NoPrintfCheck.h - clang-tidy ---------------------------*- C++ -*-===//
//
// Custom check (seminario): segnala chiamate a printf/fprintf(stdout, ...)
// e suggerisce la sostituzione con std::print (C++23).
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_NOPRINTFCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_NOPRINTFCHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::misc {

class NoPrintfCheck : public ClangTidyCheck {
public:
  NoPrintfCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::misc

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_NOPRINTFCHECK_H
