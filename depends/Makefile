.NOTPARALLEL :

# Pattern rule to print variables, e.g. make print-top_srcdir
print-%: FORCE
	@echo '$*'='$($*)'

# When invoking a sub-make, keep only the command line variable definitions
# matching the pattern in the filter function.
#
# e.g. invoking:
#   $ make A=1 C=1 print-MAKEOVERRIDES print-MAKEFLAGS
#
# with the following in the Makefile:
#   MAKEOVERRIDES := $(filter A=% B=%,$(MAKEOVERRIDES))
#
# will print:
#   MAKEOVERRIDES = A=1
#   MAKEFLAGS = -- A=1
#
# this is because as the GNU make manual says:
#   The command line variable definitions really appear in the variable
#   MAKEOVERRIDES, and MAKEFLAGS contains a reference to this variable.
#
# and since the GNU make manual also says:
#   variables defined on the command line are passed to the sub-make through
#   MAKEFLAGS
#
# this means that sub-makes will be invoked as if:
#   $(MAKE) A=1 blah blah
