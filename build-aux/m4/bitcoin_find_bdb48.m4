dnl Copyright (c) 2013-2015 The Bitcoin Core developers
dnl Distributed under the MIT software license, see the accompanying
dnl file COPYING or http://www.opensource.org/licenses/mit-license.php.

AC_DEFUN([BITCOIN_FIND_BDB48],[
  AC_ARG_VAR([BDB_CFLAGS], [C compiler flags for BerkeleyDB, bypasses autodetection])
  AC_ARG_VAR([BDB_LIBS], [Linker flags for BerkeleyDB, bypasses autodetection])

  if test "$use_bdb" = "no"; then
