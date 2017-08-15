# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_boost_base.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_BASE([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the Boost C++ headers of a particular version (or newer)
#
#   If no path to the installed boost library is given the macro searchs
#   under /usr, /usr/local, /opt, /opt/local and /opt/homebrew and evaluates
#   the $BOOST_ROOT environment variable. Further documentation is available
#   at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_CPPFLAGS)
#
#   And sets:
#
#     HAVE_BOOST
#
