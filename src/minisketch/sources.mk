# - All variables are namespaced with MINISKETCH_ to avoid colliding with
#     downstream makefiles.
# - All Variables ending in _HEADERS or _SOURCES confuse automake, so the
#     _INT postfix is applied.
# - Convenience variables, for example a MINISKETCH_FIELDS_DIR should not be used
