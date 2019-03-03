import bluejay.chem

class TransformExpression:

  @staticmethod
  def parse(expr):
    print ('TransformExpression.parse(%r)' % expr)
    import pyparsing as pp
    pp_version = ([ int(s) for s in pp.__version__.split(".") ])
    assert [2,2,0] <= pp_version < [2, 3, 0]
    #pp.ParserElement.setDefaultWhitespaceChars(' \t')
    #symbol = pp.Word(pp.alphas)('symbol')
    symbol = pp.pyparsing_common.identifier
    index = pp.Word(pp.alphas, exact=1)
    index_list = pp.delimitedList(index, ',')
    braket = '(' + index_list('bra') + '|' + index_list('ket') + ')'
    tensor = (symbol('symbol') + '(' + index_list('index_list') + ')')
    transform = tensor('transforms*')
    lhs = tensor("lhs")
    parser = lhs + '=' \
             + (pp.ZeroOrMore(transform + '*')) \
             + braket \
             + (pp.ZeroOrMore('*' + transform))
    result = parser.parseString(expr)
    #print (result.dump())
    bra = list(result.bra)
    ket = list(result.ket)
    from collections import namedtuple
    symbol_index_tuple = namedtuple('symbol_index_tuple', 'symbol index_list')
    args = [
      symbol_index_tuple(a.symbol, list(a.index_list))
      for a in [ result.lhs, *result.transforms ]
    ]
    #print (result.lhs.dump())
    return (bra,ket,args)

  @staticmethod
  def assert_unique(seq,msg):
    dup = set([ s for s in seq if seq.count(s) > 1 ])
    if dup: raise Exception(msg.format(dup))


  def __init__(self, expr, **kwargs):

    self._expr = expr
    self._transforms = []

    (bra,ket,args) = TransformExpression.parse(self._expr)
    # print ('bra,ket = %s %s' % (bra, ket))
    # print ('args = %s' % (args))

    symbols = set([ symbol for (symbol,index) in args ])
    if symbols != kwargs.keys():
      raise Exception(
        'Expression symbols %s do not match kwargs %s'
        % (symbols, set(kwargs.keys()))
      )

    braket = bra+ket
    self.assert_unique(braket, 'Duplicate braket indices: {0}')

    index_mapping = { label : index for (index,label) in enumerate(sorted(braket)) }
    #print ('index mapping:', index_mapping)
    self._braket = [ index_mapping[idx] for idx in braket ]

    free_indices = []
    transformed_indices = []

    for (symbol,index_list) in args[1:]:
      free = [ idx for idx in index_list if idx not in braket ]
      dummy = [ idx for idx in index_list if idx in braket ]
      if not (free and dummy):
        raise Exception(
          'Invalid transform %s(%s): free=%s, dummy=%s' %
          (symbol, ','.join(index_list), free, dummy)
        )
      #print ('transform: ', symbol, free, dummy)
      free_indices += free
      transformed_indices += dummy
      transform = (kwargs[symbol],) + tuple((index_list.index(idx), index_mapping[idx]) for idx in dummy)
      #print ("transform:", transform)
      self._transforms.append(transform)

    self.assert_unique((free_indices+transformed_indices), 'Duplicate rhs indices: {0}')
    self.assert_unique(args[0].index_list, 'Duplicate rhs indices: {0}')

    lhs = args[0].index_list
    rhs = free_indices + [ idx for idx in braket if idx not in transformed_indices ]
    if sorted(lhs) != sorted(rhs):
      raise Exception(
        'Left-hand side indices %s are not equivalent to right-hand side %s',
        lhs, rhs
      );

    self._lhs = kwargs[args[0].symbol]
    self._order = [ rhs.index(idx) for idx in lhs]



class IntegralTransform:

  def __init__(self, basis, integral_engine=None):
    integral_engine = integral_engine or bluejay.chem.integral_engine()
    self._basis = basis
    self._integral_engine_ = integral_engine
    # self._transform = bluejay_chem.IntegralTransform(basis, integral_engine)
    # self._transforms = []

  def compute(self, expr, **kwargs):
    import bluejay_chem.integral, bluejay_core
    expr = TransformExpression(expr, **kwargs)
    # print (
    #   "bluejay_chem.integral.transform: braket=%s, lhs=%s, order=%s, %s"
    #   % (expr._braket, expr._lhs, expr._order, expr._transforms)
    # )

    t = bluejay_chem.integral.IndexTransform(
      #bluejay_core.Array(expr._lhs),
      expr._lhs,
      expr._order, *expr._transforms
    )

    bluejay_chem.integral.transform(
      self._basis,
      self._integral_engine_.coulomb(),
      expr._braket,
      t
    )

    #   bluejay_chem.integral.IndexTransform(
    #     #bluejay_core.Array(expr._lhs),
    #     expr._lhs,
    #     expr._order, *expr._transforms)
    # )
