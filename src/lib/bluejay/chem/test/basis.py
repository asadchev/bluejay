import os, sys
import unittest
import bluejay.chem.basis as basis
from bluejay.chem.basis import BasisSet

class BasisTest(unittest.TestCase):

  def setUp(self): pass

  # def test_basis_set_nwchem_parser(self):
  #   import timeit
  #   print timeit.timeit(self._test_basis_set_nwchem_parser, number=1)
  
  def test_basis_set_nwchem_parser(self):
    import bluejay.chem.basis.nwchem
    path = os.path.dirname(__file__)
    path = os.path.join(path, "3-21G.nw")
    b = basis.read(path)
    #print b
    # for e,shells in b:
    #   print e, shells

  def test_basis_set(self):
    path = os.path.dirname(__file__)
    b = basis.BasisSet("6-31+G*") # , path=os.path.join(path, "../basis/"))
    b = basis.read(os.path.join(path, "..", "basis", "6-31+G*.json"))
    b = dict()
    b["H"] = [
      BasisSet.Shell("s", [ ( 0.1, 1.0) ]),
      BasisSet.Shell("s", [ ( 0.01, 1.0) ]),
      BasisSet.Shell("p", [ ( 1e-2, 1.0) ]),
    ]
    b = basis.BasisSet(b)
    for e,shells in b:
      print (e, shells)

# # b = basis.BasisSet(oh)
# # print(b["O"])
# # print(len(b["O"]))
# # print(b["O"][1].L)
# # print(b["O"][1].data)

# # print(b["H"])

# # b = basis.load("basis.json")
# # for (e,shells) in b:
# #     for f in shells:
# #         for c in f:
# #             print(c)

class MoleculeTest(unittest.TestCase):
  def setUp(self): pass
  def test_molecule(self):
    from bluejay.chem import Molecule
    m = Molecule([], BasisSet("6-31+G*"))
    # Molecule([ ( "H", [0,0,0] ) ], basis=oh)


if __name__ == '__main__':
  unittest.main(exit=False, verbosity=2)
