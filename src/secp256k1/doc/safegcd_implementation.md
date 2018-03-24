# The safegcd implementation in libsecp256k1 explained

This document explains the modular inverse and Jacobi symbol implementations in the `src/modinv*.h` files.
It is based on the paper
["Fast constant-time gcd computation and modular inversion"](https://gcd.cr.yp.to/papers.html#safegcd)
by Daniel J. Bernstein and Bo-Yin Yang. The references below are for the Date: 2019.04.13 version.

The actual implementation is in C of course, but for demonstration purposes Python3 is used here.
Most implementation aspects and optimizations are explained, except those that depend on the specific
number representation used in the C code.

## 1. Computing the Greatest Common Divisor (GCD) using divsteps

The algorithm from the paper (section 11), at a very high level, is this:

```python
def gcd(f, g):
    """Compute the GCD of an odd integer f and another integer g."""
    assert f & 1  # require f to be odd
    delta = 1     # additional state variable
    while g != 0:
        assert f & 1  # f will be odd in every iteration
        if delta > 0 and g & 1:
            delta, f, g = 1 - delta, g, (g - f) // 2
        elif g & 1:
            delta, f, g = 1 + delta, f, (g + f) // 2
        else:
            delta, f, g = 1 + delta, f, (g    ) // 2
    return abs(f)
```

It computes the greatest common divisor of an odd integer *f* and any integer *g*. Its inner loop
keeps rewriting the variables *f* and *g* alongside a state variable *&delta;* that starts at *1*, until
*g=0* is reached. At that point, *|f|* gives the GCD. Each of the transitions in the loop is called a
"division step" (referred to as divstep in what follows).

For example, *gcd(21, 14)* would be computed as:
- Start with *&delta;=1 f=21 g=14*
- Take the third branch: *&delta;=2 f=21 g=7*
- Take the first branch: *&delta;=-1 f=7 g=-7*
- Take the second branch: *&delta;=0 f=7 g=0*
- The answer *|f| = 7*.

Why it works:
- Divsteps can be decomposed into two steps (see paragraph 8.2 in the paper):
  - (a) If *g* is odd, replace *(f,g)* with *(g,g-f)* or (f,g+f), resulting in an even *g*.
  - (b) Replace *(f,g)* with *(f,g/2)* (where *g* is guaranteed to be even).
- Neither of those two operations change the GCD:
  - For (a), assume *gcd(f,g)=c*, then it must be the case that *f=a&thinsp;c* and *g=b&thinsp;c* for some integers *a*
    and *b*. As *(g,g-f)=(b&thinsp;c,(b-a)c)* and *(f,f+g)=(a&thinsp;c,(a+b)c)*, the result clearly still has
    common factor *c*. Reasoning in the other direction shows that no common factor can be added by
    doing so either.
  - For (b), we know that *f* is odd, so *gcd(f,g)* clearly has no factor *2*, and we can remove
    it from *g*.
- The algorithm will eventually converge to *g=0*. This is proven in the paper (see theorem G.3).
- It follows that eventually we find a final value *f'* for which *gcd(f,g) = gcd(f',0)*. As the
  gcd of *f'* and *0* is *|f'|* by definition, that is our answer.

Compared to more [traditional GCD algorithms](https://en.wikipedia.org/wiki/Euclidean_algorithm), this one has the property of only ever looking at
the low-order bits of the variables to decide the next steps, and being easy to make
constant-time (in more low-level languages than Python). The *&delta;* parameter is necessary to
guide the algorithm towards shrinking the numbers' magnitudes without explicitly needing to look
at high order bits.

Properties that will become important later:
- Performing more divsteps than needed is not a problem, as *f* does not change anymore after *g=0*.
- Only even numbers are divided by *2*. This means that when reasoning about it algebraically we
  do not need to worry about rounding.
- At every point during the algorithm's execution the next *N* steps only depend on the bottom *N*
  bits of *f* and *g*, and on *&delta;*.


## 2. From GCDs to modular inverses

We want an algorithm to compute the inverse *a* of *x* modulo *M*, i.e. the number a such that *a&thinsp;x=1
mod M*. This inverse only exists if the GCD of *x* and *M* is *1*, but that is always the case if *M* is
prime and *0 < x < M*. In what follows, assume that the modular inverse exists.
It turns out this inverse can be computed as a side effect of computing the GCD by keeping track
of how the internal variables can be written as linear combinations of the inputs at every step
(see the [extended Euclidean algorithm](https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm)).
Since the GCD is *1*, such an algorithm will compute numbers *a* and *b* such that a&thinsp;x + b&thinsp;M = 1*.
