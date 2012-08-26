AC_PREREQ([2.60])

# The package (a.k.a. release) version is based on semantic versioning 2.0.0 of
# the API. All changes in experimental modules are treated as
# backwards-compatible and therefore at most increase the minor version.
define(_PKG_VERSION_MAJOR, 0)
define(_PKG_VERSION_MINOR, 5)
define(_PKG_VERSION_PATCH, 1)
define(_PKG_VERSION_IS_RELEASE, true)

# The library version is based on libtool versioning of the ABI. The set of
# rules for updating the version can be found here:
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
# All changes in experimental modules are treated as if they don't affect the
# interface and therefore only increase the revision.
define(_LIB_VERSION_CURRENT, 4)
define(_LIB_VERSION_REVISION, 1)
define(_LIB_VERSION_AGE, 2)

AC_INIT([libsecp256k1],m4_join([.], _PKG_VERSION_MAJOR, _PKG_VERSION_MINOR, _PKG_VERSION_PATCH)m4_if(_PKG_VERSION_IS_RELEASE, [true], [], [-dev]),[https://github.com/bitcoin-core/secp256k1/issues],[libsecp256k1],[https://github.com/bitcoin-core/secp256k1])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AC_CANONICAL_HOST

# Require Automake 1.11.2 for AM_PROG_AR
AM_INIT_AUTOMAKE([1.11.2 foreign subdir-objects])

# Make the compilation flags quiet unless V=1 is used.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

if test "${CFLAGS+set}" = "set"; then
  CFLAGS_overridden=yes
else
  CFLAGS_overridden=no
fi
AC_PROG_CC
AM_PROG_AS
AM_PROG_AR

# Clear some cache variables as a workaround for a bug that appears due to a bad
# interaction between AM_PROG_AR and LT_INIT when combining MSVC's archiver lib.exe.
# https://debbugs.gnu.org/cgi/bugreport.cgi?bug=54421
AS_UNSET(ac_cv_prog_AR)
AS_UNSET(ac_cv_prog_ac_ct_AR)
LT_INIT([win32-dll])

build_windows=no

case $host_os in
