from bluejay_chem.basis import *

__registered_parsers__ = dict()

def parse(fh, fmt):
  p = __registered_parsers__.get(fmt, None)
  if not p:
    raise Exception("Unsupposed basis format '%s'" % fmt)
  return p(fh)

def parser(fmt):
  # print ("registering basis set parser fmt=%s" % fmt)
  def decorator(parser):
    __registered_parsers__[fmt] = parser
  return decorator

@parser("json")
def parse_json(f):
  return parse_basis_set_json(f)


def read(filename, fmt=None):
  if not fmt:
    from os.path import splitext
    fmt = splitext(filename)[1][1:]
  with open(filename) as fh:
    return parse(fh, fmt)
