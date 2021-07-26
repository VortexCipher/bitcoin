# Prover implementation for Weierstrass curves of the form
# y^2 = x^3 + A * x + B, specifically with a = 0 and b = 7, with group laws
# operating on affine and Jacobian coordinates, including the point at infinity
# represented by a 4th variable in coordinates.

load("group_prover.sage")


class affinepoint:
  def __init__(self, x, y, infinity=0):
    self.x = x
    self.y = y
    self.infinity = infinity
  def __str__(self):
    return "affinepoint(x=%s,y=%s,inf=%s)" % (self.x, self.y, self.infinity)


class jacobianpoint:
  def __init__(self, x, y, z, infinity=0):
    self.X = x
    self.Y = y
    self.Z = z
    self.Infinity = infinity
  def __str__(self):
    return "jacobianpoint(X=%s,Y=%s,Z=%s,inf=%s)" % (self.X, self.Y, self.Z, self.Infinity)


