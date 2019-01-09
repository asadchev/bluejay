from bluejay.core import Matrix
from bluejay.ga import GlobalArray
import numpy as np
import unittest, random

class GaTest(unittest.TestCase):

  def setUp(self): pass

  def test_global_arrays(self):
    g = GlobalArray([4,4])
    self.assertEqual(g.shape, [4,4])

    for i in range(20):
      idx = [ random.randint(0,n-1) for n in g.shape ]
      v = random.random()
      g[idx] = v
      self.assertEqual(g[idx], v)

    b = Matrix(3,3)
    a = np.zeros(g.shape)
    for i in range(b.rows):
      for j in range(b.cols):
        a[i,j] = int(i + j*3)
        b[i,j] = int(i + j*3)

    # print a
    # print "b=",b

    g[0:3,0:3] = a[0:3,0:3]
    self.assertEqual((g[0:3,0:3] == a[0:3,0:3]).all(), True)
    #print g[:,:]

    g[1:4,0:3] = b
    self.assertEqual((g[1:4,0:3] == b).all(), True)
    #print g[:,:]

if __name__ == '__main__':
    unittest.main(verbosity=2).result
