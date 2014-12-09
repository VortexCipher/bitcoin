# Unit tests

The sources in this directory are unit test cases. Boost includes a
unit testing framework, and since Bitcoin Core already uses Boost, it makes
sense to simply use this framework rather than require developers to
configure some other framework (we want as few impediments to creating
unit tests as possible).

The build system is set up to compile an executable called `test_bitcoin`
that runs all of the unit tests. The main source file for the test library is found in
`util/setup_common.cpp`.

### Compiling/running unit tests

Unit tests will be automatically compiled if dependencies were met in `./configure`
