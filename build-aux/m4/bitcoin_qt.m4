dnl Copyright (c) 2013-2016 The Bitcoin Core developers
dnl Distributed under the MIT software license, see the accompanying
dnl file COPYING or http://www.opensource.org/licenses/mit-license.php.

dnl Helper for cases where a qt dependency is not met.
dnl Output: If qt version is auto, set bitcoin_enable_qt to false. Else, exit.
AC_DEFUN([BITCOIN_QT_FAIL],[
  if test "$bitcoin_qt_want_version" = "auto" && test "$bitcoin_qt_force" != "yes"; then
    if test "$bitcoin_enable_qt" != "no"; then
      AC_MSG_WARN([$1; bitcoin-qt frontend will not be built])
    fi
    bitcoin_enable_qt=no
    bitcoin_enable_qt_test=no
  else
    AC_MSG_ERROR([$1])
  fi
])

AC_DEFUN([BITCOIN_QT_CHECK],[
  if test "$bitcoin_enable_qt" != "no" && test "$bitcoin_qt_want_version" != "no"; then
    true
    $1
  else
    true
    $2
  fi
])

dnl BITCOIN_QT_PATH_PROGS([FOO], [foo foo2], [/path/to/search/first], [continue if missing])
dnl Helper for finding the path of programs needed for Qt.
dnl Inputs: $1: Variable to be set
dnl Inputs: $2: List of programs to search for
dnl Inputs: $3: Look for $2 here before $PATH
dnl Inputs: $4: If "yes", don't fail if $2 is not found.
dnl Output: $1 is set to the path of $2 if found. $2 are searched in order.
AC_DEFUN([BITCOIN_QT_PATH_PROGS],[
  BITCOIN_QT_CHECK([
