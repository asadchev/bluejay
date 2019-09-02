import bluejay.hf, bluejay.chem.test
import unittest

class HFTest(unittest.TestCase):

  def setUp(self):
    basis = bluejay.chem.test.water("6-31+G").basis
    self.hf = bluejay.hf.HartreeFock(basis)

  def test_HF(self):
    self.assertTrue(self.hf.converge(30))

if __name__ == '__main__':
  unittest.main(verbosity=2)
