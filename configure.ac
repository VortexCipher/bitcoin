AC_PREREQ([2.69])
define(_CLIENT_VERSION_MAJOR, 27)
define(_CLIENT_VERSION_MINOR, 99)
define(_CLIENT_VERSION_BUILD, 0)
define(_CLIENT_VERSION_RC, 0)
define(_CLIENT_VERSION_IS_RELEASE, false)
define(_COPYRIGHT_YEAR, 2024)
define(_COPYRIGHT_HOLDERS,[The %s developers])
define(_COPYRIGHT_HOLDERS_SUBSTITUTION,[[Bitcoin Core]])
AC_INIT([Bitcoin Core],m4_join([.], _CLIENT_VERSION_MAJOR, _CLIENT_VERSION_MINOR, _CLIENT_VERSION_BUILD)m4_if(_CLIENT_VERSION_RC, [0], [], [rc]_CLIENT_VERSION_RC),[https://github.com/bitcoin/bitcoin/issues],[bitcoin],[https://bitcoincore.org/])
AC_CONFIG_SRCDIR([src/validation.cpp])
AC_CONFIG_HEADERS([src/config/bitcoin-config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])

m4_ifndef([PKG_PROG_PKG_CONFIG], [m4_fatal([PKG_PROG_PKG_CONFIG macro not found. Please install pkg-config and re-run autogen.sh])])
PKG_PROG_PKG_CONFIG
if test "$PKG_CONFIG" = ""; then
  AC_MSG_ERROR([pkg-config not found])
fi

# When compiling with depends, the `PKG_CONFIG_PATH` and `PKG_CONFIG_LIBDIR` variables,
# being set in a `config.site` file, are not exported to let the `--config-cache` option
# work properly.
if test -n "$PKG_CONFIG_PATH"; then
  PKG_CONFIG="env PKG_CONFIG_PATH=$PKG_CONFIG_PATH $PKG_CONFIG"
fi
if test -n "$PKG_CONFIG_LIBDIR"; then
  PKG_CONFIG="env PKG_CONFIG_LIBDIR=$PKG_CONFIG_LIBDIR $PKG_CONFIG"
fi

BITCOIN_DAEMON_NAME=bitcoind
BITCOIN_GUI_NAME=bitcoin-qt
BITCOIN_TEST_NAME=test_bitcoin
BITCOIN_CLI_NAME=bitcoin-cli
BITCOIN_TX_NAME=bitcoin-tx
BITCOIN_UTIL_NAME=bitcoin-util
BITCOIN_CHAINSTATE_NAME=bitcoin-chainstate
BITCOIN_WALLET_TOOL_NAME=bitcoin-wallet
dnl Multi Process
BITCOIN_MP_NODE_NAME=bitcoin-node
BITCOIN_MP_GUI_NAME=bitcoin-gui

dnl Unless the user specified ARFLAGS, force it to be cr
dnl This is also the default as-of libtool 2.4.7
AC_ARG_VAR([ARFLAGS], [Flags for the archiver, defaults to <cr> if not set])
if test "${ARFLAGS+set}" != "set"; then
  ARFLAGS="cr"
fi

AC_CANONICAL_HOST

AH_TOP([#ifndef BITCOIN_CONFIG_H])
AH_TOP([#define BITCOIN_CONFIG_H])
AH_BOTTOM([#endif //BITCOIN_CONFIG_H])

dnl Automake init set-up and checks
AM_INIT_AUTOMAKE([1.13 no-define subdir-objects foreign])

AM_MAINTAINER_MODE([enable])

dnl make the compilation flags quiet unless V=1 is used
AM_SILENT_RULES([yes])

dnl Compiler checks (here before libtool).
if test "${CXXFLAGS+set}" = "set"; then
  CXXFLAGS_overridden=yes
else
  CXXFLAGS_overridden=no
fi
AC_PROG_CXX

dnl libtool overrides
case $host in
  *mingw*)
     dnl By default, libtool for mingw refuses to link static libs into a dll for
     dnl fear of mixing pic/non-pic objects, and import/export complications. Since
     dnl we have those under control, re-enable that functionality.
     lt_cv_deplibs_check_method="pass_all"

     dnl Remove unwanted -DDLL_EXPORT from these variables.
     dnl We do not use this macro, but system headers may export unwanted symbols
     dnl if it's set.
     lt_cv_prog_compiler_pic="-DPIC"
     lt_cv_prog_compiler_pic_CXX="-DPIC"
  ;;
  *darwin*)
     dnl Because it prints a verbose warning, lld fails the following check
     dnl for "-Wl,-single_module" from libtool.m4:
     dnl   # If there is a non-empty error log, and "single_module"
     dnl   # appears in it, assume the flag caused a linker warning
     dnl "-single_module" works fine on ld64 and lld, so just bypass the test.
     dnl Failure to set this to "yes" causes libtool to use a very broken
     dnl link-line for shared libs.
     lt_cv_apple_cc_single_mod="yes"
  ;;
esac

dnl Require C++20 compiler (no GNU extensions)
AX_CXX_COMPILE_STDCXX([20], [noext], [mandatory])

dnl Unless the user specified OBJCXX, force it to be the same as CXX. This ensures
dnl that we get the same -std flags for both.
m4_ifdef([AC_PROG_OBJCXX],[
if test "${OBJCXX+set}" = ""; then
  OBJCXX="${CXX}"
fi
AC_PROG_OBJCXX
])

dnl OpenBSD ships with 2.4.2
LT_PREREQ([2.4.2])
dnl Libtool init checks.
LT_INIT([pic-only win32-dll])

dnl Check/return PATH for base programs.
AC_PATH_TOOL([AR], [ar])
AC_PATH_TOOL([GCOV], [gcov])
AC_PATH_TOOL([LLVM_COV], [llvm-cov])
AC_PATH_PROG([LCOV], [lcov])
dnl The minimum supported version is specified in .python-version and should be used if available, see doc/dependencies.md
AC_PATH_PROGS([PYTHON], [python3.9 python3.10 python3.11 python3.12 python3 python])
AC_PATH_PROG([GENHTML], [genhtml])
AC_PATH_PROG([GIT], [git])
AC_PATH_PROG([CCACHE], [ccache])
AC_PATH_PROG([XGETTEXT], [xgettext])
AC_PATH_PROG([HEXDUMP], [hexdump])
AC_PATH_TOOL([OBJDUMP], [objdump])
AC_PATH_TOOL([OBJCOPY], [objcopy])
AC_PATH_PROG([DOXYGEN], [doxygen])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

AC_ARG_ENABLE([wallet],
  [AS_HELP_STRING([--disable-wallet],
  [disable wallet (enabled by default)])],
  [enable_wallet=$enableval],
  [enable_wallet=auto])

AC_ARG_WITH([sqlite],
  [AS_HELP_STRING([--with-sqlite=yes|no|auto],
  [enable sqlite wallet support (default: auto, i.e., enabled if wallet is enabled and sqlite is found)])],
  [use_sqlite=$withval],
  [use_sqlite=auto])

AC_ARG_WITH([bdb],
  [AS_HELP_STRING([--without-bdb],
  [disable bdb wallet support (default is enabled if wallet is enabled)])],
  [use_bdb=$withval],
  [use_bdb=auto])

AC_ARG_ENABLE([usdt],
  [AS_HELP_STRING([--enable-usdt],
  [enable tracepoints for Userspace, Statically Defined Tracing (default is yes if sys/sdt.h is found)])],
  [use_usdt=$enableval],
  [use_usdt=yes])

AC_ARG_WITH([miniupnpc],
  [AS_HELP_STRING([--with-miniupnpc],
  [enable UPNP (default is yes if libminiupnpc is found)])],
  [use_upnp=$withval],
  [use_upnp=auto])

AC_ARG_WITH([natpmp],
            [AS_HELP_STRING([--with-natpmp],
                            [enable NAT-PMP (default is yes if libnatpmp is found)])],
            [use_natpmp=$withval],
            [use_natpmp=auto])

AC_ARG_ENABLE(tests,
    AS_HELP_STRING([--disable-tests],[do not compile tests (default is to compile)]),
    [use_tests=$enableval],
    [use_tests=yes])

AC_ARG_ENABLE(gui-tests,
    AS_HELP_STRING([--disable-gui-tests],[do not compile GUI tests (default is to compile if GUI and tests enabled)]),
