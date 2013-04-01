# Functional tests

### Writing Functional Tests

#### Example test

The file [test/functional/example_test.py](example_test.py) is a heavily commented example
of a test case that uses both the RPC and P2P interfaces. If you are writing your first test, copy
that file and modify to fit your needs.

#### Coverage

Running `test/functional/test_runner.py` with the `--coverage` argument tracks which RPCs are
called by the tests and prints a report of uncovered RPCs in the summary. This
can be used (along with the `--extended` argument) to find out which RPCs we
don't have test cases for.

#### Style guidelines

