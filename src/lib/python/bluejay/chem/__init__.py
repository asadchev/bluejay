from bluejay_chem.molecule import Molecule

__all__ = [
    "basis",
    "integral",
]

_default_integral_engine = None

def integral_engine(basis):
  if not _default_integral_engine:
    import bluejay.libint2 as libint
    libint.initialize()
    return libint.integral_engine()
  return _default_integral_engine
