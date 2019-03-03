import bluejay.chem.integral
import bluejay_chem

class IntegralTransform:

  def __init__(self, basis, integral_engine=None):
    integral_engine = integral_engine or bluejay.chem.integral._default_integral_engine
    self._basis = basis
    self._integral_engine_ = integral_engine
    # self._transform = bluejay_chem.IntegralTransform(basis, integral_engine)
    # self._transforms = []

def test_transform(basis, int2e, braket, C1, C2):
  from bluejay.core import Matrix, Tensor4d
  ni = C1[0].rows
  nj = C2[0].rows
  p,q,r,s = braket
  t2 = Tensor4d(ni, nj, basis.nbf, basis.nbf)
  bluejay_chem.integral.transform(
    basis,
    int2e,
    [ p, q, r, s ],
    C1, C2,
    t2, [0, 1, 2, 3]
  )
  #print (t2.dimensions)

def test():

  from bluejay.chem.test import h2o
  from bluejay.core import Matrix, Tensor4d

  m = h2o("6-31+G*")
  basis = m.basis
  integral_engine = bluejay.chem.integral._default_integral_engine

  C1 = Matrix(10, basis.nbf)
  C2 = Matrix(12, basis.nbf)

  int2e = integral_engine.coulomb()
  p,q,r,s = [0,1,2,3]

  test_transform(basis, int2e, [ p, q, r, s ], (C1, (1, p)), (C2, [1, q]))
  test_transform(basis, int2e, [ p, q, r, s ], (C1, (1, q)), (C2, [1, p]))
  test_transform(basis, int2e, [ p, q, r, s ], (C1, (1, s)), (C2, [1, r]))
  test_transform(basis, int2e, [ p, q, r, s ], (C2, (1, s)), (C1, [1, r]))

if __name__ == "__main__":
  test()
