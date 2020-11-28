leveldb
=======

_Jeff Dean, Sanjay Ghemawat_

The leveldb library provides a persistent key value store. Keys and values are
arbitrary byte arrays.  The keys are ordered within the key value store
according to a user-specified comparator function.

## Opening A Database

A leveldb database has a name which corresponds to a file system directory. All
of the contents of database are stored in this directory. The following example
shows how to open a database, creating it if necessary:

```c++
#include <cassert>
#include "leveldb/db.h"

leveldb::DB* db;
leveldb::Options options;
options.create_if_missing = true;
leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
assert(status.ok());
...
```

If you want to raise an error if the database already exists, add the following
line before the `leveldb::DB::Open` call:

```c++
options.error_if_exists = true;
```

## Status

You may have noticed the `leveldb::Status` type above. Values of this type are
returned by most functions in leveldb that may encounter an error. You can check
if such a result is ok, and also print an associated error message:

```c++
leveldb::Status s = ...;
if (!s.ok()) cerr << s.ToString() << endl;
```

## Closing A Database

When you are done with a database, just delete the database object. Example:

```c++
... open the db as described above ...
... do something with db ...
