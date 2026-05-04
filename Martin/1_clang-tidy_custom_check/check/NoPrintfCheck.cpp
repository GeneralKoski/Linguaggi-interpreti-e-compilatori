//===--- NoPrintfCheck.cpp - clang-tidy -----------------------------------===//
//
// Implementazione del check misc-no-printf.
// Matcha le chiamate a printf e produce diagnostica + fix-it
// che sostituisce il nome con std::print.
//
//===----------------------------------------------------------------------===//

#include "NoPrintfCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::misc {

void NoPrintfCheck::registerMatchers(MatchFinder *Finder) {
  // Matcha qualsiasi callExpr la cui callee è una funzione di nome "printf".
  // unless(isExpansionInSystemHeader) evita rumore da header di sistema.
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::printf"))),
               unless(isExpansionInSystemHeader()))
          .bind("printfCall"),
      this);
}

void NoPrintfCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Call = Result.Nodes.getNodeAs<CallExpr>("printfCall");
  if (!Call)
    return;

  // Posizione del callee (il token "printf"): è ciò che vogliamo sostituire.
  const Expr *Callee = Call->getCallee()->IgnoreImpCasts();
  SourceLocation CalleeBegin = Callee->getBeginLoc();
  SourceLocation CalleeEnd = Callee->getEndLoc();

  diag(CalleeBegin,
       "uso di 'printf' sconsigliato in C++ moderno; usare 'std::print' "
       "(richiede #include <print> e C++23)")
      << FixItHint::CreateReplacement(SourceRange(CalleeBegin, CalleeEnd),
                                      "std::print");
}

} // namespace clang::tidy::misc
