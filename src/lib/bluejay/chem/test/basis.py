import os, sys
import unittest
import bluejay.chem.basis as basis
from bluejay.chem.basis import BasisSet, GaussianShell

class BasisTest(unittest.TestCase):

  def setUp(self): pass

  def test_basis_set(self):
    path = os.path.dirname(__file__)

    b = BasisSet("STO-3G")
    b = basis.read(os.path.join(path, "..", "basis", "STO-3G.json"))

    b = BasisSet("6-31+G*") # , path=os.path.join(path, "../basis/"))
    b = basis.read(os.path.join(path, "..", "basis", "6-31+G*.json"))

    b = dict()
    b["H"] = [
      GaussianShell("s", [ ( 0.1, 1.0) ]),
      GaussianShell("s", [ ( 0.01, 1.0) ]),
      GaussianShell("p", [ ( 1e-2, 1.0) ]),
    ]
    b = BasisSet(b)
    for e,shells in b:
      print (e, shells)

  def test_basis_set_nwchem_parser(self):
    import bluejay.chem.basis.nwchem
    path = os.path.dirname(__file__)
    path = os.path.join(path, "3-21G.nw")
    b = basis.read(path)
    #print b
    # for e,shells in b:
    #   print e, shells

if __name__ == '__main__':
  unittest.main(verbosity=2)
