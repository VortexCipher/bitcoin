# Test libsecp256k1' group operation implementations using prover.sage

import sys

load("group_prover.sage")
load("weierstrass_prover.sage")

def formula_secp256k1_gej_double_var(a):
  """libsecp256k1's secp256k1_gej_double_var, used by various addition functions"""
  rz = a.Z * a.Y
  s = a.Y^2
  l = a.X^2
  l = l * 3
  l = l / 2
  t = -s
  t = t * a.X
  rx = l^2
  rx = rx + t
  rx = rx + t
  s = s^2
  t = t + rx
  ry = t * l
  ry = ry + s
  ry = -ry
  return jacobianpoint(rx, ry, rz)

def formula_secp256k1_gej_add_var(branch, a, b):
  """libsecp256k1's secp256k1_gej_add_var"""
