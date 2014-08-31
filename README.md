expr
====
Tiny library for run-time evaluation of expressions for C++.

The code is just one include file (no .cpp, no dependencies) providing
the class `Expr`.

Usage
-----
You can instantiate an `Expr` object by parsing an expression from a
`const char *`:

    Expr e = Expr::parse("1/(1 + x)", vars);

or also
    
    Expr e("1/(1 + x)", vars);

where `vars` is an `std::map<std::string, double>` instance containing
variables.  Referencing undefined variables is a parse time error.

To evaluate the expression with current variable values just call `e.eval()`.

>>> Warning <<<
---------------
For efficiency reasons the variables are stored in parsed expressions
as `double *`. This is fine for `std::map<std::string, double>`
because the address of an item inside a map is guaranteed to never
change when other items are added or if item value is changed. The map
variable used in the expression however must not be removed when an
`Expr` instance depending on them is stil alive because calling `eval`
in that case would be undefined behavior.

`Expr` can be copied but note that the copy refers to the same
variables in memory.

An `Expr` instance can be initialized with a single `double` value
and in that case is a constant expression returning that value.

Default constructor returns a constant expr evaluating to 0.0 and
an `Expr` instance can also be implicitly converted to a double
(it calls `eval`).

Partial parsing
---------------
It's also possible to parse an expression without giving an error if
extra characters are present. This is done using `Expr::parsePartial`
that requires a `const char *&` instead of a `const char *`. The
character pointer will be on the first character not used for parsing
the expression and not being on the terminating `NUL` is not
considered an error.

Syntax
------
Normal C syntax is used; implemented operators (in order of precedence) are:

    - (unary)
    * /
    + -
    < <= > >= == !=
    &&
    ||

variables are parsed with `[a-zA-Z][a-zA-Z0-9_]*`

Errors
------
Parsing errors throw an instance of `Expr::Error` (that derives from
`std::runtime_error`).