# This code supports verifying group implementations which have branches
# or conditional statements (like cmovs), by allowing each execution path
# to independently set assumptions on input or intermediary variables.
#
# The general approach is:
# * A constraint is a tuple of two sets of symbolic expressions:
#   the first of which are required to evaluate to zero, the second of which
#   are required to evaluate to nonzero.
#   - A constraint is said to be conflicting if any of its nonzero expressions
#     is in the ideal with basis the zero expressions (in other words: when the
#     zero expressions imply that one of the nonzero expressions are zero).
# * There is a list of laws that describe the intended behaviour, including
#   laws for addition and doubling. Each law is called with the symbolic point
#   coordinates as arguments, and returns:
#   - A constraint describing the assumptions under which it is applicable,
#     called "assumeLaw"
#   - A constraint describing the requirements of the law, called "require"
# * Implementations are transliterated into functions that operate as well on
#   algebraic input points, and are called once per combination of branches
#   executed. Each execution returns:
#   - A constraint describing the assumptions this implementation requires
#     (such as Z1=1), called "assumeFormula"
#   - A constraint describing the assumptions this specific branch requires,
#     but which is by construction guaranteed to cover the entire space by
#     merging the results from all branches, called "assumeBranch"
#   - The result of the computation
# * All combinations of laws with implementation branches are tried, and:
#   - If the combination of assumeLaw, assumeFormula, and assumeBranch results
#     in a conflict, it means this law does not apply to this branch, and it is
#     skipped.
#   - For others, we try to prove the require constraints hold, assuming the
#     information in assumeLaw + assumeFormula + assumeBranch, and if this does
#     not succeed, we fail.
#     + To prove an expression is zero, we check whether it belongs to the
#       ideal with the assumed zero expressions as basis. This test is exact.
