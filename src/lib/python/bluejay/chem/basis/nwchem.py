import bluejay.chem.basis

@bluejay.chem.basis.parser("nw")
def parse_basis(file):
  
  # for (i,line) in enumerate(file):
  #   line = line[0:line.find('#')].strip()
  #   tokens = line.split()
  #   if not tokens: continue
  #   if tokens[0] == "BASIS":
  #     break
  #   raise Exception("Line '%s' is invalid" % line)

  expect = None

  import pyparsing as pp
  pp.ParserElement.setDefaultWhitespaceChars(' \t')
  atom = pp.Word(pp.alphas)("atom")
  function = pp.Word(pp.alphas)("shell")
  number = pp.pyparsing_common.number()
  gto = number("exponent") + pp.OneOrMore(number)("coefficients")
  parser = (atom + function) | (gto)

  import shlex, re
  end = re.compile("^\w*end#*$", re.IGNORECASE)
  keyword = re.compile("[a-zA-Z]+", re.IGNORECASE)

  from collections import namedtuple
  Shell = namedtuple("Shell", "tag type gto")
  shell = None
  

  basis = None
  shell = None

  shell_types = [ "s", "p", "d", "f", "g" ] + [ "sp" ]

  def parse_shell(tokens):
    expected = "expected SHELL TYPE"
    if len(tokens) != 2:
      raise Exception(expected)
    tag,shell_type = tokens
    errors = []
    if not re.match("\A[a-zA-Z_]\w*$", tag):
      errors += [ "Invalid SHELL %r" % tag ]
    if not shell_type.lower() in shell_types:
      errors += [ "Invalid TYPE %r" % shell_type ]
    if errors:
      raise Exception(expected + "\n" + "\n".join(errors))
    shell = Shell(tag,shell_type,[])
    return shell
      
  def parse_shell_gto(tokens, shell):
    nc = len(shell.type)
    try:
      assert len(tokens) == nc+1
      exp = float(tokens[0])
      coeffs = [ float(t) for t in tokens[1:] ]
      shell.gto.append((exp,coeffs))
    except Exception,e:
      raise Exception("EXPONENT %s" % " ".join(["COEFFICIENT"]*nc))

  def lexer(file):
    stream = enumerate(file)
    while True:
      (index,line) = next(stream)
      tokens = list(shlex.split(line, comments='#'))
      #print "tokens:", tokens
      if tokens: yield ((index,line),tokens)
    
  lex = lexer(file)

  class ParseException(Exception):
    def __init__(self, line, error):
      Exception.__init__(
        self,
        "\nInvalid line %i: %r" % line +
        "\n" + error
      )
  
  for (line,tokens) in lex:
    if not re.match("basis", tokens[0], re.IGNORECASE):
      raise ParseException(line, "Expected BASIS statement")
    break

  basis = []
  
  #while True:
  #  line = next(lex)
  for (line,tokens) in lex:

    if re.match(end, tokens[0]):
      if len(tokens) != 1:
        raise ParseException("text after END statement", line)
      return basis

    if not shell:
      try:
        shell = parse_shell(tokens)
        basis += shell
        line,tokens = next(lex)
        parse_shell_gto(tokens, shell)
        continue
      except Exception,e :
        raise ParseException(line, e)

    errors = []
    try:
      parse_shell_gto(tokens, shell)
      continue
    except Exception,e:
      errors += e

    try:
      shell = parse_shell(tokens)
      continue
    except Exception,e:
      errors += e

    if errors:
      raise ParseException(line, "Expected SHELL TYPE | EXPONENT COEFFICIENTS") # + " or ".join(errors))
    # except Exception, e:
    #   # re.match(keyword, tokens[0]):
    #   assert len(tokens) == 2
    #   #if shell: assert shell.gto

      #print basis
      
      #return basis

@bluejay.chem.basis.parser("nw")
def parse_basis_lark(file):
  import lark
  grammar = """

  %import common.SIGNED_NUMBER -> NUMBER
  %import common.CNAME -> NAME
  %ignore /[\t \f]+/  // WS
  %ignore COMMENT

  COMMENT: /#[^\\n]*/
  _NL: ( /\\r?\\n[\\t ]*/ | COMMENT )+

  _BASIS: /BASIS\s+.*/i
  _END.10: /END/i

  TAG: NAME
  TYPE: NAME
  gto: NUMBER NUMBER+ _NL

  shell.0: TAG TYPE _NL gto+

  start: _NL* _BASIS _NL \
         shell+ \
         _END _NL?

  """


  from collections import namedtuple
  Shell = namedtuple("Shell", "tag type gto")

  class Transformer(lark.Transformer):
    def __init__(self):
      self._basis = []
    def shell(self, args):
      tag,type = [ v.value for v in args[0:2] ]
      self._basis.append(Shell(tag, type, []))
      print tag,type
    def gto(self, args):
      print args
      
  parser = lark.Lark(grammar, parser='lalr', transformer=Transformer())
  #parser = lark.Lark(grammar)

  parser.parse(file.read())

@bluejay.chem.basis.parser("nw")  
def parse_basis_pyparsing(stream):
  import pyparsing as pp

  basis = pp.Literal('BASIS')
  end = pp.Literal('END')
  tag = pp.Word(pp.alphas)("tag")
  type = pp.Word(pp.alphas)("type")
  shell = (tag + type)("shell")
  exp = pp.pyparsing_common.number("exp")
  coeffs = pp.OneOrMore(pp.pyparsing_common.number())("coeffs")
  gto = (exp + coeffs)('gto')
  statement = ( basis | shell | gto | end | pp.Optional(pp.White()) )
  statement.ignore('#' + pp.restOfLine())
  for line in stream:
    statement.parseString(line)
  pass

#@bluejay.chem.basis.parser("nw")
def parse_basis_pyparsing_file(file):

  import pyparsing as pp

  pp.ParserElement.setDefaultWhitespaceChars(' \t')

  nl = pp.LineEnd().suppress()

  atom = pp.Word(pp.alphas)("atom")
  function = pp.Word(pp.alphas)("shell")
  number = pp.pyparsing_common.number()
  primitive = number("exponent") + pp.OneOrMore(number)("coefficients")
  shell = pp.Group(
    atom + function + nl +
    pp.OneOrMore(
      pp.Group(
        primitive + nl
      )
    )
    ("primitives")
  )
  #shell.setParseAction(lambda t: basis_set[t[0]] = None )
  shells = pp.OneOrMore(shell | nl)

  basis = (pp.Literal('BASIS') + pp.Regex('(".*")?') + pp.restOfLine).suppress()
  end = (pp.Literal("END")).suppress()

  parser = (
    #basis + nl + shells + end
    pp.ZeroOrMore(nl) + basis + nl + shells + end + nl
  )

  comment = '#' + pp.restOfLine()
      #parser.ignore('#' + pp.restOfLine()  + pp.LineEnd())
      #parser.ignore(pp.Optional(pp.White(" \t")) + pp.Optional(comment) + pp.LineEnd())
  parser.ignore(comment)
      
  results = parser.parseFile(file, parseAll=True)

  from bluejay.chem.basis import Shell
  from bluejay.chem.basis import BasisSet
  
  # basis_set = dict()
  # for r in results:
  #   #print r
  #   if r.atom not in basis_set:
  #     basis_set[r.atom] = []
  #   #print r.shell, r.primitives
  #   for k in range(len(r.shell)):
  #     sk = r.shell[k]
  #     pk = [(p.exponent, p.coefficients[k]) for p in r.primitives]
  #     basis_set[r.atom].append(Shell(sk, pk))

  # return BasisSet(basis_set)

@bluejay.chem.basis.parser("nw")
def parse_basis_lark_stream(stream):
  import lark
  grammar = """

  %import common.SIGNED_NUMBER -> NUMBER
  %import common.CNAME -> NAME
  %ignore /[\t \f\\n]+/  // WS
  %ignore COMMENT

  COMMENT: /#[^\\n]*/
  _NL: ( /\\r?\\n[\\t ]*/ | COMMENT )+

  BASIS.10: /BASIS\s+.*/i
  END.10: /END/i

  gto: NUMBER NUMBER+
  shell: NAME NAME

  start: ( BASIS | shell | gto | END )?
  """


  from collections import namedtuple
  Shell = namedtuple("Shell", "tag type gto")

  class Transformer(lark.Transformer):
    def __init__(self):
      self._basis = []
    def shell(self, args):
      tag,type = [ v.value for v in args[0:2] ]
      self._basis.append(Shell(tag, type, []))
      #print tag,type
      return Shell(tag, type, [])
    def gto(self, args):
      return [ float(v.value) for v in args ]
    
  parser = lark.Lark(grammar, parser='lalr', transformer=Transformer())
  #parser = lark.Lark(grammar)

  basis = None
  shell = None
  end = None

  def parse_next_line(stream, parser):
    for (i,line) in enumerate(stream):
      p = parser.parse(line)
      if not p.children: continue
      yield (i,line,p.children[0])

  import re

  for (i,line,token) in parse_next_line(stream,parser):
    #print line
    if isinstance(token, Shell):
      if basis is None:
        raise Exception(
          "Invalid line %i: %r\n"
          "SHELL statement before BASIS statement"
          % (i,line)
        )
      basis.append(token)
      continue
    if isinstance(token, list):
      if not basis:
        raise Exception(
          "Invalid line %i: %r\n"
          "GTO statement before SHELL statement"
          % (i,line)
        )
      gto = (token[0], token[1:])
      basis[-1].gto.append(gto)
      continue
    if re.match("BASIS\s+.*", token, re.IGNORECASE):
      basis = []
      continue
    if re.match("END", token, re.IGNORECASE):
      break
    
      
  #print basis
    
      
    
