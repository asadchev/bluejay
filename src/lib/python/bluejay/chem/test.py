import bluejay.chem

ANGSTROM = 1.0/0.52917721092

def test_dir():
  from os import path
  return path.dirname(__file__) + '/test/'

def read_pubchem(url):
  """"
  read PubChem compound from url
  return compound as [ (Symbol, (x,y,z)) ] in angstroms
  """
  import urllib.request, json
  with urllib.request.urlopen(url) as src:
    pc = json.loads(src.read().decode('utf-8'))
    assert "PC_Compounds" in pc
    compound = pc["PC_Compounds"][0]
    elements = compound['atoms']['element']
    coords = compound['coords']
    conformer = coords[0]['conformers'][0]
    assert('z' in conformer)
    x,y,z = ( conformer[r] for r in "xyx" )
    return [ (a[0], a[1:]) for a in zip(elements, x, y, z) ]

def molecule(geom, basis, units=ANGSTROM):
  if units == "": factor
  for (symbol,center) in geom:
    for q in (0,1,2):
      center[q] *= units
  return bluejay.chem.Molecule(geom, basis)

def water(basis):
  geom = [
    ("O", [  0.00000, -0.07579, 0.00000 ]),
    ("H", [  0.86681,  0.60144, 0.00000 ]),
    ("H", [ -0.86681,  0.60144, 0.00000 ]),
  ]
  return molecule(geom, basis)

def ethanol(basis):
  geom = [
    ("H", (-2.0801425360,  0.4329727646,  0.0722817289)),
    ("C", (-1.2129704155, -0.2295285634, -0.0097156258)),
    ("H", (-1.2655910941, -0.9539857247,  0.8097953440)),
    ("C", ( 0.0849758188,  0.5590385475,  0.0510545434)),
    ("O", ( 1.2322305822, -0.2731895077, -0.1276123902)),
    ("H", ( 0.1506137362,  1.1200249874,  0.9943015309)),
    ("H", ( 1.2473876659, -0.8998737590,  0.6150681570)),
    ("H", ( 0.1316093068,  1.2841805400, -0.7645223601)),
    ("H", (-1.2737541560, -0.7748626513, -0.9540587845)),
  ]
  return molecule(geom, basis)
