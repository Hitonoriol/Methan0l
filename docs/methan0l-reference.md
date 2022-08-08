---
header-includes:
    - \usepackage{hyperref}

geometry:
  - top=20mm
  - bottom=20mm
  - left=20mm
  - right=20mm
  - heightrounded

hyperrefoptions:
  - linktoc=all
  - pdfwindowui
  - pdfpagemode=FullScreen

papersize: a4
fontsize: 13pt
listings-no-page-break: true
disable-header-and-footer: true
colorlinks: true
linkcolor: gray
urlcolor: gray
links-as-notes: true
toc-own-page: true
...

# Introduction

Methan0l is a general-purpose interpreted programming language.  

Every Methan0l program is a **Unit** - a block of expressions that can return a **Value**.  

Expressions can be either separated by a semicolon / newline or just a space (semicolons are not always required to write multiple expressions on one line):  

```
foo = 2; bar = 5

/* Valid too */
calc = foo * bar bar += 123

%% calc
...
```

\
Expressions can be either **evaluated** or **executed**.  

**Evaluation** of an expression returns some kind of value, while **execution** doesn't return anything. **Execution** and **evaluation** are equivalent by default for all expressions, but may behave differently for some (for example: if identifier is used inside other expression (**evaluation**), its value is extracted from the nearest local scope, while if it's used by itself (**execution**) the value is extracted and then executed).  

Every parentless expression is **executed** by the interpreter while expressions that have parents get **evaluated** for the result to be used in the parent expression.  

## Aliases used in this document

* `expr` -- any expression.
* `idfr` -- identifier, alphanumeric name (underscores can be a part of an idfr too) associated with a **Value**.  

# Data Types

Values in methan0l are represented by the `mtl::Value` class, which is a wrapper around std::variant under the hood.  
On amd64 machines size of a single methan0l value is **24 bytes** regardless of its type.  

## Primitives & heap-stored values

There are two types of values:  

* Primitives, which are stored entirely within value instances.
* Heap-stored, for which only pointers to the actual objects are stored within value instances.  

Because of this, copying of values is also separated into two types:  

* Partial copy: only the value instance itself is copied (this is equivalent to full copy for primitive types / only the pointer to an object is copied for heap-stored types)
* Full copy: a new copy of the heap-stored object is allocated along with a new value that contains a pointer to this object  

## Type names

Inbuilt type names for now are reserved identifiers and are replaced by Integer literals during lexing. This can be used inside `lhs instanceof: rhs` statements or just for comarison with `typeid: expr`s of some other expressions.

## List of standard data types

### Primitive types

* **Nil**  
  This is the value type of `nil` and `void` reserved identifiers as well as the return type of a non-returning function. 

* **Integer**  
  64-bit integral type. Supports hex (`0x123`) and binary (`0b0110101`) literal formats. Digits of numbers can be delimited with spaces for readability (e.g. `x = 1 000 000`)  

* **Float**  
  64-bit floating point type.

* **Boolean**  
  Logical type.

* **Character**  
  Literal format: `'c'`, escape sequences are supported: `'\n'`, ...

* **Reference**  
  Points at another value and is always evaluated to it. References must always be assigned to variables and returned from functions explicitly (by using the `**expr` operator), otherwise even if `expr` evaluates to a reference, it will be unwrapped into a copy of the referenced value.

### Heap-stored types

* **String**  
  String type. Escape sequences are supported inside literals (e.g. `"a, b \n c\td"`).

### Data Structures

* **List**  
  Array-like type, can be defined via the `$(...)` operator (e.g. `list = $(1, 2, 3)`).  

* **Set**  
  A list without duplicate elements. Can be defined via the `defset $(...)` keyword or the conversion of an existing list: `list.convert(Set)`.  

* **Map**  
  Associative key-value container. Can be defined via the `@(key1 => value1, key2 => value2, ...)` syntax where keys are unevaluated, so no expressions can be used in the LHS of the `=>` operator, only string literals or identifiers (that are not evaluated too, instead they are treated as literal strings).

For more detailed usage of container types, see the [container types](#container-types) section.

### Callables

* **Unit**  
  An abstract expression block that may or may not return a value. Can be defined via the `{...}` syntax.

* **Function**  
  Function type. For definition syntax, see the [functions](#functions) section.

### User-defined types

* **Object**  
  Represents an object of some class (or an anonymous class). Objects are created via the `new: Class(...)` operator. Copying rules don't apply to them, so to create a full copy of an object, `objcopy: expr` operator must be used.

* **Fallback**  
  Represents an object of an unknown type. Used for interfacing with modules and library classes.

### Unevaluated expressions
* **Expression**  
  Value type returned by the `noeval: expr` operator. When evaluated, evaluates the underlying expression.

# Variables

Variables are dynamically-typed and are associated with identifiers.  
In methan0l (almost) anything can be assigned to a variable, including functions, headless functions (units) and even unevaluated expressions.
In terms of visibility variables are divided into two categories: **local** and **global**.  

# Visibility scopes

All variables in Methan0l are implicitly local.  
This means that for any **regular unit** (a function or a plain unit) any variable defined outside of its scope is **global** while variables defined inside of its scope are **local**.  
**Weak units** (loops, conditional / try-catch expressions, lambdas) have a different visibility rule: all variables from parent scopes up to the first **regular unit** are local and all the other ones are global.  

## Local variables

**Local** variables can be accessed just by referencing their names:  

```
{
	x = 123
	foo(x) /* Access local variable x */
}
```

There's also a way to define an explicitly local variable inside of the current scope.  
`var: name` - creates a local variable `name` and returns a reference to it.  
Even though multiple variables with the same name inside nested scopes can be created this way, the `#` prefix will be referring only to the first outer variable with the given name effectively shadowing all the other ones.  

\
Example:  

@EXEC(cat ../examples/scopes/explicit-locals.mt0)

Output:  

@EXEC(../build/methan0l ../examples/scopes/explicit-locals.mt0)

## Global variables

**Global** variables can be accessed:  

* either by using the `#` prefix  

  ```
  x = "global value"
  f = func: () {
  	x = "local value"
  	foo(#x) /* Access x from the outer scope */
  }
  ```

* or by importing into the current scope by reference via the `global` operator:
  ```
  global: var1, var2, ...
  var1.foo()
  ```

**Global** variables can also be captured inside function parameter list definitions after all arguments (similar to default argument definition):  

```
f = func: x, y, %glob1, %glob2, ... {
	glob1 = x + y; /* Modifies the global `glob1` by reference */
}
```

## Class visibility rules

As for classes, object fields can be accessed using `this` reference from within the method definition bodies.  
All the other visibility rules are applied without changes.  

Example:  

@EXEC(cat ../examples/scopes/classes.mt0)

Output:  

@EXEC(../build/methan0l ../examples/scopes/classes.mt0)

## Function invocation rules

Scope visibility rules don't apply to invocation expressions (`e.g. foo()`).  
If function being called doesn't exist in the current scope, an additional lookup will be performed until it's found. If it's not defined even in the global scope, an exception will be thrown.  

# Reserved Identifiers

Methan0l has a number of reserved identifiers:  

* `nil` and `void` -- evaluate to an empty value
* `newl` -- evaluates to the newline character
* `true` and `false` -- evaluate to their respective **Boolean** values
* [Type literals](#data-types) -- replaced by **Int** literals during lexing

# Comments

There's only one type of comments supported:  
`/* Comment text */`  

# Operators

Most Methan0l operators have traditional behavior that doesn't differ from any other language.  
Operator expressions can also be grouped using `(` `)`.  

## Operator precedence

The higher the precedence level is, the tighter the operators are bound to their operands.

| Pecedence      | Operators |
| ----------- | ----------- |
|1| `noeval`|
|2| `<%`, `%%`, `%>`|
|3| `+=`, `-=`, `*=`, `/=`, ...|
|4| `=`, `<-`, `:=`|
|5| `::`|
|6| `e1 ? e2 : e3`|
|7| <code>&#124;&#124;</code>|
|8| `&&`|
|9| <code>&#124;</code>|
|10| `^`|
|11| `&`|
|12| `==`, `!=`|
|13| `>`, `<`, `<=`, `>=`|
|14| `>>`, `<<`|
|15| `+`, `-`|
|16| `*`, `/`, `%`|
|17| `-`, `!`, `~` (prefix)|
|18| `++`, `--` (prefix)|
|19| Postfix|
|20| `.`, `@`|
|21| `[]`|
|22| `()`|
|23| `++`, `--` (postfix)|
|24| `var`|

## Assignment operators

### Copy assignment

```
dest_expr = expr
```

This effectively creates a **full copy** of the `expr`'s Value. This means that for heap-stored types a new object containing the copy of the `expr`'s Value is created.  
Copy assignment is evaluated **right-to-left**.  

### Move assignment

```
dest_expr <- expr
```

Creates a temporary **partial copy** of `expr`, removes it from the data table (if `expr` is an idfr) and assigns the **partial copy** to `dest_idfr`.  
Move assignment is evaluated **left-to-right**.  

### Type-keeping assignment

```
dest_expr := expr
```

Evaluates `expr` and converts it to the `dest_expr`'s type before assigning to it.  
Type-keeping assignment is evaluated **right-to-left**.  

## Input / output operators

### Input operator

```
%> expr
```

Gets a value from stdin, deduces its type and assigns it to the specified variable / reference.  

**Input** can also be read as a String using `read_line()` function.  

### Output operators

* `%% expr` -- convert `expr`'s evaluation result to string and print it out to stdout.
* `<% expr`  -- print with a trailing newline.  

## Arithmetic operators

If at least one operand of the expr is **Float**, the result is also **Float** (except for the case of bitwise operators and `%` -- only **Integer**s are supported, no implicit conversion is performed). Otherwise **Integer** arithmetic is performed.  

**Binary:**  
`+`, `-`, `*`, `/`, `%`  
`+=`, `-=`, `*=`, `/=`, `%=`  
`&`, `|`, `^`, `>>`, `<<`  
`&=`, `|=`, `^=`, `>>=`, `<<=`  
\
**Unary:**  
`++`, `--` (both prefix and postfix)  
`-`, `~` (prefix)  

## String operators

* `::` -- concatenation operator, converts operands to string and creates a new String **Value** with the result of concatenation.  
Example: `result = "One " :: 2 :: " " :: 3.0`.  

* `$$` (prefix) -- string conversion operator. Example:
  ```
  x = 123
  <% ($$x).substr(1)
  /* Outputs "23" */
  ```

## String formatter

String formatter can be used in one of two ways:  

* via a formatted string expression: `$"format" arg1, arg2, ...`
* via a call to the `format()` function: `format_str.format(arg1, arg2, ...)`

Formatter syntax:  

* `{}`  is replaced by the next argument in list
* `{n}`, (`n` is an Int) is replaced by the `n`-th argument. Numbering starts with 1.

Each format cell `{...}` can contain the following modifiers:  

0. None at all (this will just be replaced by the next argument from the argument list).
0.5. Argument index (optional): `n`. Must be specified before any other modifiers.  

Other modifiers can be used in no particular order.  

1. Preferred width: `%n`.
2. Alignment (left by default): `-l` for left / `-r` for right / `-c` for center.
3. Floating point precision: `.n`.

## Comparison operators

`==`, `!=`, `>`, `>=`, `<`, `<=`  

Equals operator `==` works for every type (including Units -- in their case expression lists are compared), while all others are implemented only for numeric types.  

## Logical operators

Binary:  
`&&`, `||`, `^^`  
\
Unary:  
`!`  

## Return operator

Return operators stop the execution of a **Unit** and returns the result of the provided expr evaluation.  

**Prefix:**
* `return: expr` -- return by **partial copy** (copies only the pointer for heap-stored types, "full" copy can then be made via the `=` operator).

**Postfix:**
* `expr!` -- return by **partial copy**.  

## Reference operator

Get a reference to the **Value** associated with identifier:  
`**idfr`

Example:  

```
foo = 123
ref = **foo
/* Now <foo> and <ref> point at the same Value */

%% ++ref	<-- Outputs 124
```

\
**!** To assign a reference to a variable it's always required to use the `**` operator before the expression being assigned, even if it evaluates to a reference. Otherwise, a copy of the value behind the reference will be assigned. Example:  

```
f = func: x -> x *= 10 /* Explicit `**` is required only when assigning  */
foo = 123

bar = f(**foo)
/* `bar` is now an Int variable = 1230
 * (f() call evaluated to a copy instead of reference),
 * `foo` is now = 1230 */

baz = **f(**foo)
/* `baz` is now a Reference to `foo` = 12300,
 * so any subsequent changes to `baz` will modify
 * the value of `foo` */
```

## Reference-related operators

* `unwrap: idfr` -- unwraps a named reference, e.g. `x = **y; unwrap: x` -- `x` becomes a copy of `y` instead of the reference to its value. Returns a reference to the unwrapped identifier.
* `is_reference: expr` -- returns true if `expr` evaluates to a Reference
* `expr!!` -- returns a copy of the value behind the reference `expr`

## Conditional (ternary) operator

`condition ? expr_then : expr_else`  

The condition is converted to Boolean and if true, the **Then** expression is evaluated and returned; otherwise -- the **Else** expression result is returned.  

## Range operator

```
start..end[..step]
```

Evaluates to a List of Ints in inclusive range from `start` to `end`.  
Can be used for:
* short form for loops:  `(1..10)[do: @: n -> foo(n)]`  
* container slicing: `list[start..end]` -- evaluates to a new sublist with elements `start` to `end` inclusively  
* any other purpose lists are used for

# If-else statements

If-else operator in **Methan0l** uses the same syntax as ternary operator, but with `if` and `else` words used before the conditions and in between the **then** and **else** branches:

```
/* if-else */
if (condition) ? {
	expr1
	...
} else: {
	else_expr1
	...
}

/* if only */
if (condition) ? {
	expr1
	...
}

/* if-elseif-...-else */
if (condition1) ? {
	expr1
	...
} else: if (condition2) ? {
	elseif_expr1
	...
} else: {
	else_expr1
	...
}

```  

# Loops

## For loop

```
for (i = 0, i < 10, ++i) {
	...
}
```

## While loop

```
while (i < 10) {
	...
	i++
}
```

## Foreach loop

```
for (as_elem, list) {
	...
}
```  

## Break expression

You can interrupt a loop via the break expression:  

```
return: break
```

# Try-catch statement

During a program's execution exceptions may be thrown either by the interpreter itself or native modules or by using the `die(expr)` function, where `expr` can be of any type and can be caught by the try-catch expression:

```
try {
	...
} catch: name {
	...
}
```

# Units

**Regular Unit** can be defined using the following syntax:  

```
unit = {
	expr
	expr
	...
}
```

Exprs between `{` and `}` will be added to the **Unit**'s expr list. If assigned to an idfr, it then can be called either as a **pure expression** (returned value will be discarded in this case):  

```

unit	/* <-- calls the unit defined above */

```

or using the **invocation syntax**:  

```
...

result = unit() /* <-- unit's return will be captured, if exists */

```

\
Function bodies are always **regular** units.

## Weak Units

Return from a **Weak Unit** also causes all subsequent **Weak Units** stop their execution & carry the returned value up to the first **Regular Unit**. Typical use case for this is loop / conditional operator body.  
**Weak Units** can also access idfrs from the scopes above the current one up to the first **Regular Unit** without using the **#** prefix.  
Loop & if-else expression bodies are **Weak Units**.

Definition:  

```
-> {
	expr1
	expr2
	...
}
```

## Box Units

**Box Unit** preserves its data table after the execution and is executed automatically on definition. Fields inside it can then be accessed by using the `.` operator.  
Definition:  

```
module = box {
	some_field = "Blah blah blah"
	foo = func @(x) {
		x.pow(2)!
	}
}

<% module.foo(5)	/* Prints 25 */
<% module.some_field	/* Prints contents of the field */
```  
\
Any Non-persistent Unit can also be converted to **Box Unit** using `unit.make_box()` function.

**Box** **Units** as well as **Non-persistent** ones can also be imported into the current scope:  

```
module.import()
```

This effectively executes all expressions of `module` in the current scope.  


## Pseudo-function invocation

**Units** can also be invoked with a single argument that's also **Unit** (**Init Block**). **Init block** will be executed before the **Unit** being invoked and the  resulting **Data Table** (which stores all identifiers local to the **Unit**) will be shared between them. This syntax can be used to "inject" identifiers into a **Unit** when calling it, for example:  

```
ratio = {
	if (b == 0) ? {"inf"!}
	return: (a / b) * 100.0
}

%% "a is " :: ratio({a = 9.125; b = 13}) :: "% of b" :: newl
```

# Execution Syntax

Execution syntax can be used with any expression:  

**Literals** are converted to string and printed out:  

```
"Some string"
123
0.456
true
```
\
**String concatenation operator** (`::`) expressions are evaluated and printed out.

\
**Identifiers** are evaluated and then executed:  

```
some_number = (foo + bar) * c / 1.5
some_number	/* <-- Execution syntax, the result will be printed out */

unit = {%%("Hello!");}
unit	/* <-- Execution syntax, unit will be executed */
```

\
For the expressions not listed here **Execution Syntax** behavior is identical to **Evaluation**.  

# Functions

Functions accept a list of values and return some value (or **nil** when no return occurs).

## Definition syntax

```
foo = func (arg1, arg2, arg3 => def_value, ...) {
    ...
}
```  

\
Or:
```
foo = func: arg1, arg2, arg3 => def_value, ... {
    ...
}
```

\
If function's body contains only one expression, `{` & `}` braces can be omitted:  

```
foo = func: x
	do_stuff(x)
```


## Implicit return short form

```
foo = func: x, y -> x + y
```

Here the expression after the `->` token is wrapped in a `return` expression, so its result will be returned from the function without needing to explicitly use the `return` operator.

## Multi-expression short form 

In this case no return expression is generated automatically.  

```
bar = func: () -> a = 123, b = 456, out = a * b, out!
```

## Lambdas

Can be defined by using `@:` or `f:` function prefix instead of `func:`:  

```
foo = @: x
	do_stuff(x)
```

Or:  

```
foo = f: x
	return: x.pow(2)
```

Lambdas can also be used with implicit return and multi-expression short form of function body definition. However, they can't use the parenthesized argument definition form (only when a lambda accepts no arguments: `foo = f: () -> do_something()`).  

The key difference between lambdas and regular functions is that lambdas' bodies are **weak units**, which means that it's possible to access variables from their parent scopes without using global access syntax:  

```
glob = "foo bar"
lambda = @: x
	glob[] = x

lambda(" baz") /* Appends " baz" to the global variable `glob` */
```

## Calling functions

Functions can be called using **invocation syntax**:  

```
result = foo(expr1, expr2, ...)
```

\
Functions can also be called by using **pseudo-method** syntax:  
```
value.func(arg1, arg2, ...)
```

The expression above is internally rewritten as `func(value, arg1, arg2, ...)` and can be used even with literals, for example:  

```
42.sqrt()
"foo bar".find("bar")
```

# Unevaluated expressions

Can be created by using the following syntax:  

```
foo = noeval: expr
```
In this case `expr` won't be evaluated until the evaluation of `foo`. When `foo` gets evaluated or executed, a new temporary value containing `expr`'s evaluation result is created and used in place of `foo` while `foo`'s contents remain unevaluated.  

Example:  
```
x = noeval: y * 2
y = 2

/* Equivalent to x = 2 + 2 * 2 */
<% "Result: " :: (x += y)
```
<br>

# Container types

## Generic container operations

* Remove element: `container[~expr]`, where `expr` is element's index for **Lists** / the element itself for **Sets** / key for **Maps**.

* Get size: `size(container)`  or `container.size()`

* Clear: `container.clear()` or `container[~]` -- works for **Strings** as well.

* Test emptiness: `container.is_empty()` -- works for **Strings** as well.

* Iterate over a container functionally (allows element modification):

  ```
  container.for_each(funcexpr)
  ```

  Iterate using bracket-foreach syntax:
  ```
  container[do: funcexpr]
  ```

  Where `funcexpr` is:
    * A 1-arg function for **Lists** and **Sets**.
    * A 2-arg function for **Maps** (where 1st argument holds a copy of the current key and 2nd holds a **reference** to the current **Value**).  

* `a.add_all(b)` -- add all elements from `b` to `a`
* `a.remove_all(b)` -- remove all elements of `a` that are present in `b`
* `a.retain_all(b)` -- remove all elements of `a` that are not present in `b`
* `list.fill(val, [n])` -- fill `list` with copies of `val`. It's filled to its full capacity or resized to `n` elements and then filled.
* `ctr.sum()`
* `ctr.product()`
* `ctr.mean()`
* `ctr.rms()`
* `ctr.deviation()`
* `ctr.join()`

## Lists

Definition:  
`list = $(expr, expr, expr, ...)`  

* Append a new element: `list[] = "New element"`  

* Access an element: `list[expr]`  

* Join as String: `list.join()`

\
Iterate over a list:  

```
do $(elem, list) -> {
    ...
}
```
\
Map into another list:
```
list.map(funcexpr)
```

Where `funcexpr` is a 1-arg function.
This can be used for list filtering: if `funcexpr` doesn't return anything for a given input, no element will be appended to the resulting list.

## Sets

Definition:  
`set = defset $(expr, expr, expr, ...)`  
or  
`set = list.convert(type set)`  

* Insert a new element: `set[->expr]`
* Test element's existence: `set[expr]`

**Set operations:**  

* Union: `a.union(b)`
* Intersection: `a.intersect(b)`
* Difference: `a.diff(b)`
* Symmetric difference: `a.symdiff(b)`

Map into another set:
```
set.map(funcexpr)
```

## Maps

Definition:  
`map = @(key1 => val_expr1, key2 => val_expr2, ...)`  

Keys in map definition can be specified either with or without quotes: unquoted keys won't be evaluated as identifiers. If you want to use spaces / special characters in a key, you must declare it as a string literal.  

* Access existing / Add new element: `map["key"]`  
  When using `[]` operator, expressions inside it are evaluated, so string literals must be used to access string keys, even if defined without quotes.  

* Get key / value list: `map.list_of(keys)` / `map.list_of(values)`.  
  Here `keys` and `values` are reserved words, not identifiers.  

\
Iterate over a map:  

```
do $(key, map.list_of$(keys)) -> {
	val = map[key]
	...
}
```  

# Modules

Modules are just shared libraries that define a special function `void init_methan0l_module(mtl::ExprEvaluator*)` to be able to hook up to the interpreter instance.  
This function can be defined using the `INIT_MODULE` macro that is a part of the methan0l module API (defined in the `methan0l.h` header in the root source directory).  
Modules can optionally define an initialization function via the `LOAD_MODULE {...}` macro.  
Alternatively, functions can be bound to the interpreter even without the `LOAD_MODULE` entry point by using the `FUNCTION(name)` macro inside of the source file, where `name` is a name of an already declared function.  
C++ classes can also be bound to methan0l classes via the `NativeClass<...>` helper or directly using the `ClassBinder<...>` class.  

You can see an example of module API usage (including `ClassBinder`) in `modules/test`.  

## Loading modules

Load a Methan0l source file or a native module as a Box Unit:  

```
load(path_str_expr)
```

Loaded module can be assigned to a variable and its contents can be accessed via the `.` operator, as with regular Box Units:  

```
sys = "modules/system".load()
<% sys.exec("ls -a")
```

Modules can also be imported into the current scope after loading:  

```
sys = "modules/system".load()
sys.import()
```

Or imported in-place via the `using_module` or `import` operator:  

```
import: "modules/system"
using_module: "modules/test"

<% sys.exec("ls -a")
test()
```

# Classes

Definition:  
```
class: ClassName @(
	field1, field2
	field3 => 123
	
	construct => method: arg1, arg2, arg3 {
		this.field1 = arg1 + arg2 + arg3
		this.field2 = "stuff"
	}
	
	foo => method: arg1, arg2 {
		this.field1 = arg1
		this.field2 = arg2
	}
	
	static_method => func: x
		<% $"Static method arg: {}" x
)
```  
\
**Constructor** call syntax:  
```
obj = new: ClassName(arg1, arg2, arg3)
```  
\
**Methods** or **fields** of an object can be accessed by using "dot operator" ( . ):  
```
foo = obj.field
obj.some_method(arg1, arg2, ...)
```  
\
**Static Methods** can be called using the following syntax:  
```
x = ClassName@static_method(foo)
```  

## Operator overloading

Classes also support **operator overloading**:  

```
class: Foo @(
	x, y
	construct => method: x => 0, y => 0 {
		this.x = x
		this.y = y
	}
	
	`+` => @: rhs -> new: Foo(this.x + rhs.x, this.y + rhs.y)
	`*=` => @: () -> <% "Overloaded compound assignment"
)
```

\
**Class id** of an object can be checked by calling `class_id()` method as a static method or as a method of an object itself:  

```
obj = Class.new(a, b, c)
obj.class_id() == Class@class_id()
/*	     ^^ Evaluates to true */
```

## Copying objects

Objects can be **copied** using `objcopy: obj` operator.

## Anonymous objects

Methan0l supports anonymous objects which can be used to pass around multiple named values.  
Anonymous objects also work as "classes" themselves: when used in the RHS of the `new` operator, a deep copy of the object is created and then its constructor (if defined) is invoked (e.g. `new_anon_obj = new: anon_obj(123)`, where `anon_obj` is an anonymous object with defined constructor).  
Methods defined inside anonymous objects (including constructor) must have an explicit `this` argument in their parameter list.  
Generally, defining methods inside such objects is a huge overhead as they are stored inside the object's data table and are copied along with new objects when used inside of `new` or `objcopy` operator.  

\
Example of anonymous object usage:  

```
{
	obj = new: @(
		x => 123
		y => "some text"

		some_func => func: x
			return: x * 10

		foo => method: this, n
			return: this.x * n
	)

	<% "obj.x = " :: obj.x
	<% "Anonymous object method call: " :: obj.foo(2)

	/* For now there's no other way to do this */
	<% "Regular function: " :: obj["some_func"](42)
}
```

Output:  

```
obj.x = 123
Anonymous object method call: 246
Regular function: 420
```

## Reflection

Get all fields / methods of a class:  

```
Class@get_fields()
Class@get_methods()
```

These methods return lists of class' field / method names.  
Specific fields can then be accessed from this class' instance by name via the `[]` operator, e.g:  

```
obj = new: SomeClass(42)
fields = SomeClass@get_fields()
<% $"Some field of `obj`: {}" obj[fields[0]]
```

Specific methods can be invoked by name via the `Class@get_method(name)` static method:  

```
/* Getting a `Class`'s method and calling it for an object `obj` */
methods = SomeClass@get_methods()
SomeClass@get_method(methods[0])(obj, arg1, arg2)
```



# Inbuilt library functions {.unnumbered}

# Input / output operators & functions `[LibIO]`

**Input** operator:  
`%> idfr`

**String input** function:  
`read_line()` -- read string from stdin and return it.  
Usage: `foo = read_line()`

**Output** operators:  
`<% expr`, `%% expr`


# Data structure operators & functions `[LibData]`

## Hashing values of any type

```
hashcode: expr
```
(overloadable for classes)

## Defining a value of type

```
var = defval: typeid
```

Operator `defval` creates a new default-initialized Value of type `typeid`, where `typeid` is one of the [typeid literals](#data-types).  

This can then be used in combination with type-keeping assignment `:=` and type assert `expr1 assert_type: typeid_expr` to ensure that initial type of `var` remains unchanged.

## Deleting values

```
delete: idfr
```

Delete the identifier from the data table (and the value associated with it if it's a primitive or a heap-stored value with refcount == 0 after identifier deletion).  

## Type operators

* `typeid: expr` -- get **typeid** of value.  
Can be compared to one of [typeid literals](#data-types).

  Example:  

  ```
  %% (typeid: "foo") == type string /* Prints "true" */
  ```

* `msg_expr assert: condition_expr`

* `expr type_assert: typeid_expr`

* `expr instanceof: typeid_expr`

## Value conversion

```
expr.convert(typeid)
```

Returns a copy of evaluated expr converted to specified type, example:  

```
bool = "true".convert(Boolean)
str = 1337.convert(String)
dbl = "-1.234".convert(Double)
set = $(123, 234, 345).convert(Set)
```  
  
## Variadic function arguments

```
args = get_args()
```

Can be called from inside a function's body to get a list of all arguments passed to it, including extra arguments not defined in the function's parameter list.

## Command line arguments
```
get_launch_args()
```

Get command line args passed to the currently running Methan0l program.  

## Range

Generate a **List** with Int **Values** in range **[0; n - 1]**:  

```
range(n)
```
\
Range **[start, n - 1]**:  

```
range(start, n)
```
\
Range **{start, start + step, ..., n - 1}**:  

```
range(start, n, step)
```  

## Get environment info  

* `get_os_name()`
* Get path to the currently running interpreter executable / its parent directory: `get_runpath()`, `get_rundir()`

## Heap-related functions

* Heap state getters: `mem_in_use()`, `max_mem()`
* Set max heap capacity: `set_max_mem(Int)`
* Print current heap state: `mem_info()`
* Toggle either to throw an exception when exceeding the max heap capacity or not: `enforce_mem_limit(Boolean)`

## Get methan0l version:  

* `get_version_code()`
* `get_minor_version()`
* `get_release_version()`
* `get_major_version()`
* `get_version()` -- returns version string


## Unit-related functions & operators `[LibUnit]`

`sync_work_dir()` -- change working directory to the location of currently running script

## Persistence

`unit.make_box()` -- get a persistent copy of Unit (preserves Unit's data table after the execution)  

## Check if current Unit is the program's entry point

`is_main_unit()` -- true if and only if unit currently being executed is at the bottom of the execution stack

## Execution control

* `exit()` -- stop program execution.  

* `die(expr)` -- throw an exception `expr` that can be caught by a try-catch statement.

* `err(msg)` -- print `msg` to stderr.

* `pause(msecs)` -- pauses execution for at least `msecs` milliseconds.

* `selfinvoke(...)` -- can be used from inside a function's body to recursively call it.

* `unit.local(action)` -- execute `action` inside `unit`'s scope.  
If `unit` is **non-persistent**, it's executed first and `action` is executed after it. `unit`'s data table is then cleared as if after a regular execution.

## Benchmark

`measure_time(expr)` -- returns time spent on evaluation of `expr` in milliseconds, discarding the result, if any.

## Reflection

**Get a reference to identifier's value by name:**

* `unit.value("idfr_name")` or `unit["idfr_name"]` -- performs lookup in `unit`'s scope.

* `value("idfr_name")` -- global scope lookup.

# Common math functions `[LibMath]`

## Trigonometry

* `rad(deg)` -- degrees to radians
* `deg(rad)` -- radians to degrees
* `sin(rad)`
* `cos(rad)`
* `tan(rad)`
* `acos(x)`
* `asin(x)`
* `atan(x)`
* `atan2(x, y)`

## Power functions

* `pow(x, n)`
* `sqrt(x)`

## Exponential and logarithmic functions

* `exp(x)`
* `log(x)`
* `log10(x)`
* `logn(base, x)`

## Rounding

* `ceil(x)`
* `floor(x)`
* `round(dbl, n)` - round `dbl` to `n` decimal places

## Absolute value

* `abs(x)`


# Common String functions `[LibString]`

## Functions that don't modify String contents

* `str.substr(start, [length])`  -- returns a substring of the specified string, starting from the `start`'th character. If length is not specified, the remainder is used.

* `str.find(substr, [start_pos])` -- returns index of `substr` inside the specified string. If not found, `-1` is returned instead.

* `str.contains(substr)` -- returns `true` if specified string contains `substr` at least once.

* `str.split(delim_expr)` -- returns a **List** of tokens.

* `int_val.to_base(base)` -- returns a **String** representation of the provided **Integer** **Value** converted to base `base`.

* `str.repeat(times)`

## Functions that modify String contents
* `str.erase(start, [length])` -- erases a substring from the specified string, starting from the `start`'th character. If length is not specified, the remainder is used.

* `str.replace(from_str, to_str, [limit])` -- replace `limit` occurrences of `from_str` with `to_str`. If limit is not specified, all occurrences will be replaced.

* `str.insert(pos, substr)` -- insert `substr` after the `pos`'th character of the provided string.  



# Inbuilt Classes {.unnumbered}

# File

## Constructor:

```
file = File.new(path_str)
```  

## Path prefixes

Special prefixes can be used at the beginning of the path string when constructing a File instance / calling File methods statically:  

* `$:` - expands into interpreter run directory
* `#:` - expands into script run directory  

Example: `"$:/modules/ncurses"` becomes: `"/path/to/binary/modules/ncurses"`

## Methods:

### Read / Write operations

* `file.open()` -- open file for reading / writing

* `file.close()` -- close file

* `file.read_line()` -- read one line from file as **String**.
Sets `file.eof` field to **true** if the end of file has been reached.

* `file.write_line(expr)` -- evaluate expr, convert it to string and write to file

* `file.read_contents()` -- returns full contents of the managed file as a **String Value**.  
Doesn't require the `file` to be `open()`'d.

* `file.write_contents(str)` -- writes the whole string `str` to `file`, deleting previous contents, if any.  
Doesn't require the `file` to be `open()`'d.

### Miscellaneous

* `file.set(path)` -- sets the path of the file managed by this object.

* `file.mkdirs()` -- creates all non-existing directories in the managed path **String**.

* `file.cd()` -- changes current working directory to the managed path.

* `file.equivalent(path)` -- returns `true` if managed file is equivalent to the specified file.

* `file.copy_to(path)` -- copies managed file to the specified directory.

* `file.extension()` -- returns the file's extension as **String**

* `file.size()` -- returns file size in bytes as **Integer**

* `file.absolute_path()` -- returns absolute path to the managed file as **String**

* `file.filename()` -- returns filename of the managed file as **String**

* `file.is_dir()` -- returns `true` if this file is a directory

* `file.exists()` -- returns `true` if this file exists

* `file.for_each(action)` -- iterates recursively through all files in directory `file`, calling `action(file_name)` for each one of them.

* `file.rename(new_name)`

* `file.remove()`

## Static methods

* `File@cwd()` -- returns path to current working directory.

## Properties

* `file.eof()` -- **Boolean**. Set only after calling read operations. **True** if the end of the managed file has been reached.  

\
! All **File** methods (that don't modify the state of the object) can be invoked statically.


# Random

## Constructors

```
rnd = Random.new()
rnd = Random.new(seed)
```  

If `seed` is not specified, a random one will be used (provided by `std::random_device`).  

## Methods

### RNG Seeding

* `rnd.get_seed()` -- returns **Integer Value** of seed used by this rng.

* `rnd.reseed(new_seed)` -- re-seeds the rng with specified seed,

### Random Integer

* `rnd.next_int()` -- returns next pseudo-random **Integer Value** in 64-bit range.

* `rnd.next_int(n)` -- returns next pseudo-random **Integer Value** in range **[0; n - 1]**.

* `rnd.next_int(a, b)` -- returns next pseudo-random **Integer Value** in range **[a; b]**.

### Random Float

* `rnd.next_double()` -- returns next pseudo-random **Double Value** in range **[0; 1]**.

* `rnd.next_double(n)` -- returns next pseudo-random **Double Value** in range **[0; n - 1]**.

* `rnd.next_double(a, b)` -- returns next pseudo-random **Double Value** in range **[a; b - 1]**.

### Random Boolean

* `rnd.next_boolean()`	-- returns `true` with  **~50%** probability.

* `rnd.next_boolean(probability)`	-- returns `true` with  probability `probability` in range **[0; 1]**.

# Pair

A binding to `std::pair<Value, Value>`.

## Constructor

```
pair = new: Pair(a, b)
```

## Methods

* `a.swap(b)` -- swaps the contents of the pair `a` with the pair `b`
* `pair.x()`, `pair.y` -- first and second element getters (by reference)
* `pair.swap_contents()` -- swaps first and second elements  



# Inbuilt modules {.unnumbered}

# System

```
import: "modules/system"
```

* `exec(str)` -- execute `str` as a system shell command. Returns a list of `$(stdout_str, return_value)`  

# Ncurses

```
import: "modules/ncurses"
```

Partial libncurses bindings.  
