import bluejay.chem

def molecule(geom, basis):
  return bluejay.chem.Molecule(geom, basis)

def h2o(basis):
  geom = [
    ( "o", [ 0.00000,  0.00000,  0.11779 ] ),
    ( "H", [ 0.00000,  0.75545, -0.47116 ] ),
    ( "H", [ 0.00000, -0.75545, -0.47110 ] ),
  ]
  return molecule(geom, basis)
