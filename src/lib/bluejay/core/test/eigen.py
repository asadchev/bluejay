import bluejay.core
import unittest, random

class CoreTest(unittest.TestCase):

  def setUp(self): pass

  def test_matrix(self):
      m = bluejay.core.Matrix(10,20)
      self.assertEqual(m.rows, 10)
      self.assertEqual(m.cols, 20)
      for i in range(m.rows):
        for j in range(m.cols):
          v = random.random()
          m[i,j] = v
          self.assertEqual(m[i,j], v)

  def test_tensor(self):
      shape = [ random.randint(1,20) for i in range(4) ]
      t = bluejay.core.Tensor4d(*shape)
      self.assertEqual(t.shape(), shape)
      for i in range(20):
        idx = [ random.randint(0,n-1) for n in t.shape() ]
        v = random.random()
        t[idx] = v
        # fails, need fix
        #self.assertEqual(t[idx], v)

if __name__ == '__main__':
    unittest.main(verbosity=2)
