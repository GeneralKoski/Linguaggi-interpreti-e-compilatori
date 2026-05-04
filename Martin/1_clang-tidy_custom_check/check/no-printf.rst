.. title:: clang-tidy - misc-no-printf

misc-no-printf
==============

Diagnoses calls to the libc ``::printf`` function in C++ code and suggests
replacing them with ``std::print`` (C++23, ``<print>``).

The check uses an AST matcher anchored at global scope (``hasName("::printf")``),
so it correctly distinguishes between the libc ``printf`` and user-defined
functions named ``printf`` in other namespaces. Calls inside system headers are
ignored via ``isExpansionInSystemHeader()``.

Example
-------

.. code-block:: c++

  extern "C" int printf(const char *, ...);

  void f() {
    printf("hello\n");          // warning: replace with std::print
  }

  namespace mylib {
    int printf(const char *);
  }

  void g() {
    mylib::printf("ok");        // OK: not ::printf
  }

The check provides a fix-it that replaces only the callee token, leaving the
arguments untouched:

.. code-block:: c++

  // before
  printf("hello\n");
  // after applying --fix
  std::print("hello\n");

Notes
-----

* ``std::print`` requires ``#include <print>`` and a C++23 toolchain. The check
  does not insert the include automatically; consider running it together with
  an include-fixer step.
* Calls reached through macros are also reported: the AST is matched after
  preprocessor expansion, so ``LOG(...)`` whose body contains ``printf`` will
  trigger the check at the macro call site.
* Templates: both the generic template body and each instantiation are
  reported. This is intentional — a single template body can produce many
  call sites that need fix-it.

Limitations
-----------

* The check does not handle ``fprintf``, ``sprintf``, ``vprintf`` or other
  printf-family functions. Extending the matcher to a list of names is
  straightforward but out of scope for this educational check.
* No automatic insertion of ``#include <print>``.
