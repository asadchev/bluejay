import sympy
from bluejay.chem.integral import IntegralTransform

def indices(args, cls=sympy.Idx):
  return sympy.symbols([ str(a) for a in args ], cls=cls)

class Int1e(sympy.Indexed):
  def __new__(cls, p, q):
    return sympy.Indexed.__new__(cls, "Int1e", *indices([p,q]))
  def _sympystr(self, printer):
    p,q = self.indices
    return "(%s|%s)" % (p,q)

class Int2e(sympy.Indexed):
  def __new__(cls, p, q, r, s):
    return sympy.Indexed.__new__(cls, "Int2e", *indices([p, q, r, s]))
  def _sympystr(self, printer):
    p,q,r,s = self.indices
    return "(%s,%s|%s,%s)" % (p,q,r,s)

class Tensor(sympy.Indexed):
  def __new__(cls, name, *args):
    return sympy.Indexed.__new__(cls, str(name), *indices(args))

class Expression:
  def __init__(self, lhs, rhs):
    self._lhs = lhs
    self._rhs = rhs
  def __repr__(self):
    return "%r = %r" % (self._lhs, self._rhs)
  @property
  def expr_free_symbols(self):
    s = set()
    s.update(self._lhs.expr_free_symbols)
    s.update(self._lhs.expr_free_symbols)
    return s

def parse(expr):

  """
  parse string expression to sympy expression
  """

  import lark
  grammar = """

  %import common.SIGNED_NUMBER -> NUMBER
  %import common.CNAME -> NAME
  %ignore /[\t \f\\n]+/  // WS
  %ignore COMMENT

  COMMENT: /#[^\\n]*/
  _NL: ( /\\r?\\n[\\t ]*/ | COMMENT )+

  INDEX: /[a-zA-Z]/
  OPERATOR: /[\+\-\*\/]/

  tensor: NAME "(" INDEX ("," INDEX)* ")"
  int1e.10: "(" INDEX "|" INDEX ")"
  int2e.10: "(" INDEX "," INDEX "|" INDEX "," INDEX ")"

  ?expression_op_expression: expression OPERATOR expression
  ?product_expression: expression expression

  //#?expression: atom | product_expression | expression_op_expression

  ?product: atom
          | product atom -> mul
          | product "*" atom  -> mul
          | product "/" atom  -> div

  ?expression: product
             | expression "+" product   -> add
             | expression "-" product   -> sub

  ?atom: NUMBER -> number
       | tensor
       | int1e
       | int2e
       | "(" expression ")"

  start: expression -> expression
       | tensor "=" expression -> assign_expression

"""

  class Transformer(lark.Transformer):

    @staticmethod
    def _indices(args, cls=sympy.Idx):
      return sympy.symbols(args, cls=cls)

    def __init__(self):
      pass

    def number(self, args): return sympy.Number(args[0])

    def int1e(self, args): return Int1e(*args)

    def int2e(self, args): return Int2e(*args)

    def tensor(self, args): return Tensor(*args)

    def add(self, args):
      return args[0] + args[1]

    def sub(self, args):
      return args[0] - args[1]

    def mul(self, args):
      return args[0] * args[1]

    def div(self, args):
      return args[0] / args[1]

    def neg(self, args):
      return -args[0]

    def expression(self, args):
      return args[0]

    def assign_expression(self, args):
      #print args
      #print dir(args[0])
      return Expression(*args)


  #parser = lark.Lark(grammar, parser="lalr", transformer=Transformer())
  parser = lark.Lark(grammar)

  tree = parser.parse(expr)
  tree = Transformer().transform(tree)

  #print "%s\n->\n%s" % (expr, tree)

  return tree

import sys
print(sys.version)

class IntegralExpression:

  def __init__(self, int2e=None):
    self._int2e = int2e

  def evaluate(self, *args, **kwargs):
    exprs = []
    for expr in args:
      expr = parse(expr)
      print (expr)
      for s in expr.expr_free_symbols:
        if not s.is_symbol: continue
        if not str(s.base) in kwargs:
          raise Exception("Missing symbol %s" % s.base)
        #   print s.__class__.__bases__
        #   print s is isinstance(s, sympy.Basic)
        # print [ s.name for s in expr.free_symbols if s is isinstance(s, sympy.Indexed) ]
        # print get_indices(expr)
        # print get_contraction_structure(expr)
        #exprs.append(expr, kwargs)
        if isinstance(expr, Expression):
          self._evaluate_integral_transform(expr, kwargs)

  @staticmethod
  def indices(expr):
    for s in expr.symbols:
      print (type(s))

  def _evaluate_integral_transform(self, expr, symbols):
    from sympy.tensor import get_indices, get_contraction_structure
    rhs,lhs = expr._rhs, expr._lhs
    print (get_contraction_structure(rhs))
    print (get_indices(rhs))
    IntegralTransform().evaluate2(lhs, rhs)
    #bluejay_chem.integral.Transform.evaluate(

def test_parser():
  # parse("- (C(j,q) + C(i,p)) * (p,q|r,s)")
  # parse("(C(i,p) * (p,q|r,s))")
  # parse("(C(i,p) + (p,q|r,s))")
  # parse("1/2*(C(i,p) - (p|r)) + V(k,l)")
  assert parse("C(i,p) C(j,q) + 0.5") == Tensor("C", *"ip") * Tensor("C", *"jq") + 0.5
  assert parse("(p,q|r,s)") == Int2e(*"pqrs")
  parse("v(p,q,r,s) = (p,q|r,s)")

def test_mp2():

  # direct mp2 equation
  parse("v(i,j,a,b) = (i,a|j,b)")
  parse("""
  v(i,j,a,b)*( 2*v(i,j,a,b) - v(j,i,a,b) ) /
  (e(i) + e(j) - e(a) - e(b))
  """)

  V=None

  IntegralExpression().evaluate(
    "v(i,j,a,b) = C(p,i) C(q,j) (p,q|r,s) C(r,a) C(s,b)",
    "v(i,j,a,b)*( 2*v(i,j,a,b) - v(j,i,a,b) ) / (e(i) + e(j) - e(a) - e(b))",
    v=V,
    e=None
  )

def test():
  test_parser()
  test_mp2()
