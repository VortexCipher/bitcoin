load("secp256k1_params.sage")

MAX_ORDER = 1000

# Set of (curve) orders we have encountered so far.
orders_done = set()

# Map from (subgroup) orders to [b, int(gen.x), int(gen.y), gen, lambda] for those subgroups.
solutions = {}

# Iterate over curves of the form y^2 = x^3 + B.
for b in range(1, P):
    # There are only 6 curves (up to isomorphism) of the form y^2 = x^3 + B. Stop once we have tried all.
    if len(orders_done) == 6:
        break
