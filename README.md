

# Methan0l

## How to use

#### Build Methan0l

`cd build && make`  
(GCC 7+ is required)  

#### Run Methan0l Programs

**Execute** program and stop: `./methan0l /path/to/src_file.mt0`  
**Interactive mode** (parse & execute expressions from stdin): `./methan0l`  

## Grammar

Every Methan0l program is a **Unit** - a block of expressions that can return a **Value**.  

Expressions can be either delimited by a semicolon or a newline (though semicolons are required to write multiple expressions on one line):  

```
foo = 999; bar = 0.0725
calc = foo * bar + (foo % 3) / bar
%%(calc)
...
```

\
Expressions can be either **evaluated** or **executed**.  

**Evaluation** of an expression returns some kind of value, while **execution** doesn't return anything. **Execution** and **evaluation** are equivalent by default for all expressions, but may behave differently for some (for example: if identifier is used inside other expression (**evaluation**), its value is extracted from the nearest local scope, while if it's used by itself (**execution**) the value is extracted and then executed).  

Every parentless expression is **executed** by the interpreter while expressions that have parents get **evaluated** for the result to be used in the parent expression.  

## Value Types

**Primitives**:
* Integer
* Double
* Boolean
* String

**Data Structures**:
* List

**Callables**:
* Unit

## Syntax

Aliases used in this section:  
`expr` -- any expression, for example `a & ((foo * 1.123) >= bar)`.  
`idfr` -- identifier, alphanumeric name (underscores can be a part of idfr too) associated with a **Value**.  

### Scopes

By default, **Units** can only access idfrs in their local scope (or up to the first **Regular Unit** for **Weak Units**).  
To access an idfr from the global scope, use **#** prefix before it: `foo = #global_idfr`  

### Comments

There's only one type of comments supported: `/* Comment text */`.  
Once Lexer meets the `/*` token, everything  inside this block is ignored until the `*/` token is met.  

### Identifier assignment

Any expression can be assigned to an identifier:  
`idfr = <expr>`  

### Operators

Operator exprs can be grouped using `(` `)`.  

#### Arithmetic

If at least one operand of the expr is Double, the result is also Double (except for the case of `%` -- if applied on a Double value, it's first converted to Integer, then the `%` is applied, then the result is converted to Double). Otherwise Integer arithmetic is performed.  

Binary: `+`, `-`, `*`, `/`, `%`, `^`  
Unary: `++`, `--`, `-`  

#### String

`::` -- inline concatenation operator, converts operands to string and creates a new String **Value** with the result of concatenation.  
Example: `result = "One " :: 2 :: " " :: 3.0`.  

#### Comparison

`==`, `>`, `>=`, `<`, `<=`  

Equals operator `==` works for every type (including Units -- in their case expression lists are compared), while all others are implemented only for numeric types.  

#### Logical

Binary: `&`, `|`  
Unary: `!`  

#### Ternary

`(expr ? expr : expr)`  

The condition is converted to Boolean and if true, the **Then** expression is evaluated and returned; otherwise -- the **Else** expression result is returned.  
In **Methan0l** ternary operator is also used instead of conditional operators (by using **execution syntax**). The **else** branch is optional.  
Example:  

```
/* if-else */
(a == b | !c) ? -> {
	expr1
	expr2
	...
} : -> {
	else_expr1
	else_expr2
}

/* if only */
foo > bar ? -> {
	expr
	...
}

/* if-elseif-...-else */
!x ? -> {
	expr
	...
} : foo == "str" ? -> {
	elseif_expr
	...
} : -> {
	else_expr
	...
}

```

#### Return

`expr!`  

Stops the execution of **Unit** and returns the result of the provided expr evaluation.  

#### Data structure operators

`delete(idfr)` -- delete idfr & the value associated with it  
`size(expr)` -- get size of **string** / **list**  
`type(expr)` -- get **typeid** of an evaluated expr. Can be compared to one of type literals: `type int`, `type double`, `type boolean`, `type string`, `type unit`, `type list`, `type function`, `type map`, `type nil`.  
Example:  

```
%%(type("foo") == type string) /* Prints "true" */
```

#### Unit operators

`load(path_str_expr)` -- load Methan0l source file as a Unit  
`persistent(unit_expr)` -- get a persistent copy of Unit (preserves Unit's data table after the execution)  

#### Execution control operators

`exit()` -- stop program's execution  

### Reserved Words

#### Identifiers

`nil` -- evaluates to empty **Value**  
`newl` -- evaluates to the newline character  
`true` and `false` -- evaluate to Boolean **Value**  

### Input / Output

**Input** operator: `>>idfr` -- get a value from stdin, deduce its type and assign it to the specified identifier in the nearest local scope.  

**Output** operator: `%%expr` -- evaluate the expression, convert it to string and print it out to the stdout.  

### Units

**Regular Unit** can be defined using the following syntax:  

```
unit = {
expr
expr
...
}
```

Exprs between `{` and `}` will be added to the **Unit**'s expr list. If assigned to an idfr, it then can be called either as a **pure expr** (returned value will be discarded in this case):  

```
...

unit	/* <-- calls the unit defined above*/

```

or using the **invocation syntax**:  

```
...

result = unit$() /* <-- unit's return will be captured, if exists */

```

### Weak Units

Return from a **Weak Unit** also causes all subsequent **Weak Units** stop their execution & carry the returned value up to the first **Regular Unit**. Typical use case for this is loop / conditional operator body.  
**Weak Units** can also access idfrs from the scopes above the current one up to the first **Regular Unit** without using the **#** prefix (for convenience in loops & conditional oprs).  
Definition:  

```
-> {
	expr1
	expr2
	...
}
```

### Box Units

**Box Unit** preserves its data table after the execution and is executed automatically on definition. Fields inside it can then be accessed by using '.' operator.  
Definition:  

```
module = box {
	some_field = "Blah blah blah"
	foo = func def @(x) {
		(x^2)!
	}
}

%% module.foo$(5)	/* Prints 25 */
some_field	/* Prints contents of the field */
```

### Execution Syntax

Execution syntax can be used with any expression:  

**Literals** are printed out:  

```
"Some string"
123
0.456
true
```

\
**Identifiers** are evaluated and then executed:  

```
some_number = (foo + bar) * c / 1.5
some_number	/* <-- Execution syntax, the result will be printed out */

unit = {%%("Hello!");}
unit	/* <-- Execution syntax, unit will be executed */
```

\
**Ternary Operators** are evaluated and the result is executed:  

```
a = 100; b = -1337
(a > b ? -> {"a is greater than b";} : -> {"b is greater or equals a";})
/* ^^^ Execution syntax, one of the string literals will be executed after the evaluation */
```

\
For the expressions not listed here **Execution Syntax** behavior is identical to **Evaluation**.  

### Loops

#### For loop

```
do $(i = 0, i < 10, ++i) -> {
	...
}
```

#### While loop

```
do $(#i < 10) -> {
	...
	i++
}
```

#### Foreach loop

```
do $(as_elem, list) -> {
	...
}
```

### Functions

Definition:  

```
foo = func def @(arg1, arg2, arg3 => def_value, ...) {
expr1
expr2
expr3
}
```  

Can be called using **invocation syntax**: `result = foo$(expr1, expr2, ...)`  

### Lists

Definition syntax: `list = $(expr, expr, expr, ...)`  
Append a new element: `list[] = "New element"`  
Access an element: `list[expr]`  
Remove element: `list[~expr]`  

### Maps (wip)

Currently can only be used as part of function definition.  
Definition syntax: `map = @(key1 => val_expr1, key2 => val_expr2, ...)`  