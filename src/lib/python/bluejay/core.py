from bluejay_core import *

import numpy

class Matrix(numpy.core.numeric.ndarray):

  __array_priority__ = 10.0

  def __new__(subtype, *args, dtype=None, order='F'):
    N = numpy.core.numeric
    buffer=None
    shape=None
    if len(args) == 2:
      shape = args
    else:
      buffer, = args
      shape = buffer.shape
    return N.ndarray.__new__(subtype, shape=shape, dtype=dtype, order=order, buffer=buffer)



  @property
  def rows(self):
    return self.shape[0]

  @property
  def cols(self):
    return self.shape[1]
