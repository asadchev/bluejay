from bluejay.chem import basis
from bluejay.core import Tensor4d, Vector3
import bluejay.libint2 as libint2
import unittest

class Libint2Test(unittest.TestCase):

  def setUp(self):
      libint2.initialize()
      self.shell = (
        basis.Shell("s", [ ( 0.5, 0.1 ) ]), # gaussian
        Vector3(0.0, 0.0, 0.0) # center
        )

  def test_libint(self):
    libint = libint2.integral_engine(10)
    eri = libint.coulomb()
    s = self.shell
    q = Tensor4d()
    #print "eri.test ok: ", eri.test(s, q)
    eri.compute(s, s, s, s, q)
    #print q[0,0,0,0]
    #libint.finalized()

if __name__ == '__main__':
    unittest.main(verbosity=2).result
