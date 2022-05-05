""" Generates the constants used in secp256k1_scalar_split_lambda.

See the comments for secp256k1_scalar_split_lambda in src/scalar_impl.h for detailed explanations.
"""

load("secp256k1_params.sage")

def inf_norm(v):
    """Returns the infinity norm of a vector."""
    return max(map(abs, v))

