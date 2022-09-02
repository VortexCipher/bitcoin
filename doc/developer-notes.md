Developer Notes
===============

<!-- markdown-toc start -->
**Table of Contents**

- [Developer Notes](#developer-notes)
    - [Coding Style (General)](#coding-style-general)
    - [Coding Style (C++)](#coding-style-c)
    - [Coding Style (Python)](#coding-style-python)
    - [Coding Style (Doxygen-compatible comments)](#coding-style-doxygen-compatible-comments)
      - [Generating Documentation](#generating-documentation)
    - [Development tips and tricks](#development-tips-and-tricks)
        - [Compiling for debugging](#compiling-for-debugging)
        - [Show sources in debugging](#show-sources-in-debugging)
        - [`debug.log`](#debuglog)
        - [Signet, testnet, and regtest modes](#signet-testnet-and-regtest-modes)
        - [DEBUG_LOCKORDER](#debug_lockorder)
        - [DEBUG_LOCKCONTENTION](#debug_lockcontention)
        - [Valgrind suppressions file](#valgrind-suppressions-file)
        - [Compiling for test coverage](#compiling-for-test-coverage)
        - [Performance profiling with perf](#performance-profiling-with-perf)
        - [Sanitizers](#sanitizers)
    - [Locking/mutex usage notes](#lockingmutex-usage-notes)
    - [Threads](#threads)
    - [Ignoring IDE/editor files](#ignoring-ideeditor-files)
- [Development guidelines](#development-guidelines)
    - [General Bitcoin Core](#general-bitcoin-core)
    - [Wallet](#wallet)
    - [General C++](#general-c)
    - [C++ data structures](#c-data-structures)
    - [Strings and formatting](#strings-and-formatting)
    - [Shadowing](#shadowing)
    - [Lifetimebound](#lifetimebound)
    - [Threads and synchronization](#threads-and-synchronization)
    - [Scripts](#scripts)
        - [Shebang](#shebang)
    - [Source code organization](#source-code-organization)
    - [GUI](#gui)
    - [Subtrees](#subtrees)
    - [Upgrading LevelDB](#upgrading-leveldb)
      - [File Descriptor Counts](#file-descriptor-counts)
      - [Consensus Compatibility](#consensus-compatibility)
    - [Scripted diffs](#scripted-diffs)
        - [Suggestions and examples](#suggestions-and-examples)
    - [Release notes](#release-notes)
    - [RPC interface guidelines](#rpc-interface-guidelines)
    - [Internal interface guidelines](#internal-interface-guidelines)

<!-- markdown-toc end -->

Coding Style (General)
----------------------

Various coding styles have been used during the history of the codebase,
and the result is not very consistent. However, we're now trying to converge to
a single style, which is specified below. When writing patches, favor the new
style over attempting to mimic the surrounding style, except for move-only
commits.

Do not submit patches solely to modify the style of existing code.

Coding Style (C++)
------------------

- **Indentation and whitespace rules** as specified in
[src/.clang-format](/src/.clang-format). You can use the provided
[clang-format-diff script](/contrib/devtools/README.md#clang-format-diffpy)
tool to clean up patches automatically before submission.
  - Braces on new lines for classes, functions, methods.
  - Braces on the same line for everything else.
  - 4 space indentation (no tabs) for every block except namespaces.
  - No indentation for `public`/`protected`/`private` or for `namespace`.
  - No extra spaces inside parenthesis; don't do `( this )`.
  - No space after function names; one space after `if`, `for` and `while`.
  - If an `if` only has a single-statement `then`-clause, it can appear
    on the same line as the `if`, without braces. In every other case,
    braces are required, and the `then` and `else` clauses must appear
    correctly indented on a new line.
  - There's no hard limit on line width, but prefer to keep lines to <100
    characters if doing so does not decrease readability. Break up long
    function declarations over multiple lines using the Clang Format
    [AlignAfterOpenBracket](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
    style option.

- **Symbol naming conventions**. These are preferred in new code, but are not
required when doing so would need changes to significant pieces of existing
code.
  - Variable (including function arguments) and namespace names are all lowercase and may use `_` to
    separate words (snake_case).
    - Class member variables have a `m_` prefix.
    - Global variables have a `g_` prefix.
  - Constant names are all uppercase, and use `_` to separate words.
  - Enumerator constants may be `snake_case`, `PascalCase` or `ALL_CAPS`.
    This is a more tolerant policy than the [C++ Core
    Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Renum-caps),
    which recommend using `snake_case`.  Please use what seems appropriate.
  - Class names, function names, and method names are UpperCamelCase
    (PascalCase). Do not prefix class names with `C`. See [Internal interface
    naming style](#internal-interface-naming-style) for an exception to this
    convention.

  - Test suite naming convention: The Boost test suite in file
    `src/test/foo_tests.cpp` should be named `foo_tests`. Test suite names
    must be unique.

- **Miscellaneous**
  - `++i` is preferred over `i++`.
  - `nullptr` is preferred over `NULL` or `(void*)0`.
  - `static_assert` is preferred over `assert` where possible. Generally; compile-time checking is preferred over run-time checking.
  - Use a named cast or functional cast, not a C-Style cast. When casting
    between integer types, use functional casts such as `int(x)` or `int{x}`
    instead of `(int) x`. When casting between more complex types, use `static_cast`.
    Use `reinterpret_cast` and `const_cast` as appropriate.
  - Prefer [`list initialization ({})`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-list) where possible.
    For example `int x{0};` instead of `int x = 0;` or `int x(0);`
  - Recursion is checked by clang-tidy and thus must be made explicit. Use
    `NOLINTNEXTLINE(misc-no-recursion)` to suppress the check.

For function calls a namespace should be specified explicitly, unless such functions have been declared within it.
Otherwise, [argument-dependent lookup](https://en.cppreference.com/w/cpp/language/adl), also known as ADL, could be
triggered that makes code harder to maintain and reason about:
```c++
#include <filesystem>

namespace fs {
class path : public std::filesystem::path
{
};
// The intention is to disallow this function.
bool exists(const fs::path& p) = delete;
} // namespace fs

int main()
{
    //fs::path p; // error
    std::filesystem::path p; // compiled
    exists(p); // ADL being used for unqualified name lookup
}
```

Block style example:
```c++
int g_count{0};

namespace foo {
class Class
{
    std::string m_name;

public:
    bool Function(const std::string& s, int n)
    {
        // Comment summarising what this section of code does
        for (int i = 0; i < n; ++i) {
            int total_sum{0};
