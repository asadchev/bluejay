import os, sys
import unittest
from bluejay_chem import transform, Molecule

oh = {
  "O" : [ { "s" : [ 1, 3 ] }, { "p" : [ 4, 5 ] }, ],
  "H" : [ { "s" : [ .5, 1 ] } ],
  }

class TransformTest(unittest.TestCase):

  def setUp(self): pass

  def molecule(self):
    geom = [
      ( "H", [-1,0,0] ),
      ( "H", [+1,0,0] ),
      ( "O", [ 0,0,0] ),
    ]
    return Molecule(geom, basis=oh)

  def test_transform(self):
    #transform("t(i,j,r,s) = v(p,q,r,s)*C(p,i)*C(q,j)", molecule=self.molecule(), t=None, v=None, C=None)
    #transform("t(i,j,r,s) = (pq|rs) C(p,i)*C(q,j)", molecule=self.molecule(), t=None, C=None)
    #transform("t(i,j,r,s) = v(p,q,r,s)*C(p,i)*C(q,j)", basis=None, t=None, v=None)
    transform("t(i,j,r,s) = v(p,q,r,s)", molecule=self.molecule(), t=None, v=None)
    transform("t(i,j,r,s) = (pq|rs)", molecule=self.molecule(), t=None)
    transform("t(i,j,r,s) = (pq|rs)*v(p,q,r,s)", molecule=self.molecule(), t=None, v=None)
    transform("t(i,j,r,s) = C(p,i)*C(q,j)", molecule=self.molecule(), t=None, C=None)
    transform("t(i,j,r,s) = C(p,i) (p|r)", molecule=self.molecule(), t=None, C=None)

if __name__ == '__main__':
  unittest.main(exit=False, verbosity=2)
