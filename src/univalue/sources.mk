# - All Variables ending in _HEADERS or _SOURCES confuse automake, so the
#     _INT postfix is applied.
# - Convenience variables, for example a UNIVALUE_TEST_DIR should not be used
#     as they interfere with automatic dependency generation
# - The %reldir% is the relative path from the Makefile.am.

UNIVALUE_INCLUDE_DIR_INT = %reldir%/include

