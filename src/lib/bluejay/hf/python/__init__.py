import bluejay.chem
import numpy

numpy.set_printoptions(3)

class HartreeFock:

  def __init__(self, basis, integral_engine=None):
    from .bluejay_hf import FockEngine
    self._basis = basis
    self._integral_engine = integral_engine or bluejay.chem.integral_engine(basis)
    self._fock = FockEngine(basis, self._integral_engine)


  class EigenSolver:
    def __init__(self,S):
      (s,U) = numpy.linalg.eigh(S)
      s = [ v**-0.5 if v > 0 else 0 for v in s ]
      X = U*numpy.diag(s)
      self.X = X
    def __call__(self,F):
      X = self.X
      XFX = numpy.matmul(X, F)
      XFX = numpy.matmul(XFX, X.T)
      e,C = numpy.linalg.eigh(XFX)
      return (C,e)

  @staticmethod
  def make_density_matrix(F, eigs, occ):
    C,e = eigs(F)
    D = numpy.matmul(C[:,occ], C[:,occ].T)
    # print (D)
    return (D,C,e)

  def converge(self, max_iters=10, etol=1e-5):

    basis = self._basis

    enuc = 0
    ne = 0
    for i in range(len(basis.atoms)):
      ai = basis.atoms[i]
      ne += ai.Z
      for j in range(0,i):
        aj = basis.atoms[j]
        r = numpy.linalg.norm(ai.r-aj.r)
        enuc += (ai.Z*aj.Z)/r

    print ("Nuclear repulsion energy = %f" % enuc)

    occ = slice(0,ne//2)

    S = self._fock.overlap()
    T = self._fock.kinetic()
    V = self._fock.nuclear()
    H = T+V

    # print ("Overlap:",S)
    # print ("H core:", H)

    eigs = HartreeFock.EigenSolver(S)

    # Hcore guess
    D = HartreeFock.make_density_matrix(H, eigs, occ)[0]

    last_e = 0

    for i in range(max_iters):
      print ("SCF Iteration %i: " % i, end='')

      F = self._fock.fock2e(D)
      F = H+F

      #print (F)

      D,C,e = HartreeFock.make_density_matrix(F, eigs, occ)

      ehf = numpy.sum(numpy.multiply(D,F+H))
      E = enuc+ehf
      print ("E=%f, Ediff=%f" % (E, E-last_e))

      self.E = E
      self.C = C
      self.e = e

      if abs(E - last_e) < etol:
        print ("Converged after %i iterations" % i)
        return True
      last_e = E
