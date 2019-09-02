from bluejay.chem import basis
import bluejay.libint2 as libint2
import unittest

class Libint2Test(unittest.TestCase):

  def setUp(self):
      libint2.initialize()
      self.shell = (
        basis.GaussianShell("s", [ ( 0.5, 0.1 ) ]), # gaussian
        [ 0.0, 0.0, 0.0 ] # center
        )

  def test_libint(self):
    libint = libint2.integral_engine()
    eri = libint.coulomb()
    s = self.shell
    #print "eri.test ok: ", eri.test(s, q)
    q = eri.compute(s, s, s, s)
    print (q[0,0,0,0])
    #libint.finalized()

if __name__ == '__main__':
    unittest.main(verbosity=2)
