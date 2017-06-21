ACLOCAL_AMFLAGS = -I build-aux/m4

# AM_CFLAGS will be automatically prepended to CFLAGS by Automake when compiling some foo
# which does not have an explicit foo_CFLAGS variable set.
AM_CFLAGS = $(SECP_CFLAGS)

lib_LTLIBRARIES = libsecp256k1.la
include_HEADERS = include/secp256k1.h
include_HEADERS += include/secp256k1_preallocated.h
noinst_HEADERS =
noinst_HEADERS += src/scalar.h
noinst_HEADERS += src/scalar_4x64.h
noinst_HEADERS += src/scalar_8x32.h
noinst_HEADERS += src/scalar_low.h
noinst_HEADERS += src/scalar_impl.h
noinst_HEADERS += src/scalar_4x64_impl.h
noinst_HEADERS += src/scalar_8x32_impl.h
noinst_HEADERS += src/scalar_low_impl.h
noinst_HEADERS += src/group.h
noinst_HEADERS += src/group_impl.h
noinst_HEADERS += src/ecdsa.h
noinst_HEADERS += src/ecdsa_impl.h
noinst_HEADERS += src/eckey.h
noinst_HEADERS += src/eckey_impl.h
noinst_HEADERS += src/ecmult.h
noinst_HEADERS += src/ecmult_impl.h
noinst_HEADERS += src/ecmult_compute_table.h
noinst_HEADERS += src/ecmult_compute_table_impl.h
