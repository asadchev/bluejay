import os, sys
import unittest
import bluejay.chem
from bluejay.core import Matrix, Tensor4d
import numpy as np

class IntegralTransformTest(unittest.TestCase):

  def setUp(self):

    import bluejay.chem.test
    import numpy

    m = bluejay.chem.test.water("6-31+G*")
    basis = m.basis

    self.basis = basis
    self.integral_engine = bluejay.chem.integral_engine(basis)
    self.C = numpy.random.rand(basis.nbf, basis.nbf)

  @staticmethod
  def _test_transform_C1C2(basis, int2e, braket, C1, C2, order=None):
    import bluejay_chem.integral
    order = order or [0,1,2,3]
    ni = C1[0].rows
    nj = C2[0].rows
    dims = [ ni, nj, basis.nbf, basis.nbf ]
    dims = [ dims[idx] for idx in order ]
    t2 = Tensor4d(*dims)
    bluejay_chem.integral.transform(
      basis,
      int2e,
      braket,
      [ C1, C2 ],
      t2,
      order
    )
    print ("t2=%r" % t2.shape())

  def _test_transform(self):

    basis = self.basis
    int2e = self.integral_engine.coulomb()
    p,q,r,s = [0,1,2,3]

    C1 = Matrix(self.C[:10,:])
    C2 = Matrix(self.C[:12,:])
    _test_transform_C1C2 = IntegralTransformTest._test_transform_C1C2

    _test_transform_C1C2(basis, int2e, [ p, q, r, s ], (C1, (1, p)), (C2, [1, q]))
    _test_transform_C1C2(basis, int2e, [ p, q, r, s ], (C1, (1, q)), (C2, [1, p]))
    _test_transform_C1C2(basis, int2e, [ p, q, r, s ], (C1, (1, s)), (C2, [1, r]))
    _test_transform_C1C2(basis, int2e, [ p, q, r, s ], (C2, (1, s)), (C1, [1, r]))

    _test_transform_C1C2(basis, int2e, [ p, q, r, s ], (C1, (1, p)), (C2, [1, q]), order=[1,0,3,2])

    _test_transform_C1C2(basis, int2e, [ p, r, q, s ], (C1, (1, p)), (C2, [1, r]))
    _test_transform_C1C2(basis, int2e, [ s, r, q, p ], (C1, (1, q)), (C2, [1, s]), order=[3,2,1,0])

  @staticmethod
  def _test_transform_expression(transform, expr, **kwargs):
    nbf = transform._basis.nbf
    t = np.ndarray([10,12,nbf,nbf]) # [C1.rows, C2.rows, nbf, nbf])
    #t = Tensor4d(C1.rows, C2.rows, nbf, nbf)
    transform.compute(expr, t=t, **kwargs)

  def test_expression(self):

    from bluejay.chem.integral.transform import IntegralTransform

    transform = IntegralTransform(self.basis, self.integral_engine)
    nbf = self.basis.nbf

    C = Matrix(nbf,nbf)
    C1 = Matrix(self.C[:10,:])
    C2 = Matrix(self.C[:12,:])

    _test_transform_expression = IntegralTransformTest._test_transform_expression


    _test_transform_expression(transform, "t(i,j,r,s) = C1(i,p)*C2(j,q)*(p,q|r,s)", C1=C1, C2=C2)
    _test_transform_expression(transform, "t(s,r,j,i) = C1(i,p)*C2(j,q)*(p,q|r,s)", C1=C1, C2=C2)
    _test_transform_expression(transform, "t(i,j,r,s) = C1(i,p)*(p,r|q,s)*C2(j,q)", C1=C1, C2=C2)
    _test_transform_expression(transform, "t(i,j,k,s) = C(i,p)*(p,r|q,s)*C(j,q)*C(k,r)", C=C1)

    T2 = np.ndarray([10,12,nbf,nbf])
    _test_transform_expression(transform, "t(i,j,r,s) = t2(i,j,p,q)*(p,r|q,s)", t2=T2)


if __name__ == "__main__":
  unittest.main(verbosity=2)
