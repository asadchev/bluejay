import os, sys
import unittest
import bluejay.chem.basis as basis
from bluejay.chem.basis import BasisSet, GaussianShell

class BasisTest(unittest.TestCase):

  def setUp(self): pass

  def test_basis_set(self):

    b = dict()
    b["H"] = [
      GaussianShell("s", [ ( 0.1, 1.0) ]),
      GaussianShell("s", [ ( 0.01, 1.0) ]),
      GaussianShell("p", [ ( 1e-2, 1.0) ]),
    ]
    b = BasisSet(b)
    for e,shells in b:
      print (e, shells)

if __name__ == '__main__':
  unittest.main(verbosity=2)
