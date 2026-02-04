
# Built-in Functions

Note that every function returns something; this is a rule for the language.

### import
- Takes one argument, which should be a string filepath. Imports the code and runs it here. Returns true on success and false on failure.

### define
- Takes two arguments: a symbol name, and any value. That symbol is now a global variable, and can be used anywhere.
- This can also be used to redefine (e.g. "set") already defined variables.
- Returns the value stored in the variable.

### do
- Execute a sequence of operations. The result of the final operation will be returned.
- Example: (do (define x 1) (define y 2) (+ x y))

### while
- Syntax: (while cond expr1 expr2 ...)
- Evaluates all expressions until cond is true (or does nothing if cond was true from the start).

### +
- Add two or more numbers together. If one is a float, the result will be a float.

### -
- Subtract two numbers. If one is a float, the result will be a float.

### *
- Multiply two or more numbers. If one is a float, the result will be a float.

### /
- Divide two numbers. Always returns a float.

### %
- Modulo on two integers.

### and
- Logic "and" on two or more booleans.

### or
- Logic "or" on two or more booleans.

### not
- Logic "not" on one boolean.

### print
- Print the value of the argument to the console. Argument type does not matter. Returns the printed value.

### println
- Print the value of the argument to the console and add a newline. Argument type does not matter. Returns the printed value.

### concat
- Concatenate two or more strings.

### substr
- Takes three arguments: string, index, length. Produces a substring of string starting at index. The first character is index 0.

### =
- Check equality of two numbers. Returns true or false. Comparing integers and floats is allowed.

### ==
- Exactly the same as `=`.

### !=
- Check inequality of two numbers. Returns true or false. Comparing integers and floats is allowed.

### <
- Check if first number < second number. Returns true or false. Comparing integers and floats is allowed.

### <=
- Check if first number <= second number. Returns true or false. Comparing integers and floats is allowed.

### >
- Check if first number > second number. Returns true or false. Comparing integers and floats is allowed.

### >=
- Check if first number >= second number. Returns true or false. Comparing integers and floats is allowed.

### str=
- Check two strings are equivalent. Returns true or false.

### strlen
- Outputs the integer length of given string.

### int2float
- Converts an integer to a float.

### float2int
- Converts a float to an integer, rounding down.
