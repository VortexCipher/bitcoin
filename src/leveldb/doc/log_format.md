leveldb Log format
==================
The log file contents are a sequence of 32KB blocks.  The only exception is that
the tail of the file may contain a partial block.

Each block consists of a sequence of records:

