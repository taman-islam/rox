# Functions

Functions in ROX are **first-class citizens**, meaning they can be assigned to variables, passed as arguments, and returned from other functions.

## Function Declaration

Functions are declared using the `function` keyword, followed by the function name, parameters, and return type.

```rox
function add(num a, num b) -> num {
    return a + b;
}
```

**Note**: Function names (except `main`) are automatically namespaced by the compiler (prefixed with `roxv26_`) to avoid collisions. Users cannot start function or variable names with `roxv26_`.

### Parameters

Parameters must have explicit types.

```rox
function greet(string name) -> none {
    print("Hello, ", name, "!\n");
}
```

### Return Type

The return type must be explicitly specified after `->`. If a function does not return a value, use `none`.

```rox
function do_nothing() -> none {
    return;
}
```

## Functions as Values

Since functions are first-class, you can use them just like any other value.

### Type Syntax

Function types are written as `function(ParamType1, ParamType2, ...) -> ReturnType`.

### Assigning to Variables

You can assign a function to a variable with a matching function type.

```rox
function square(num x) -> num {
    return x * x;
}

// Declare a variable 'op' that holds a function taking a num and returning a num
function(num) -> num op = square;

print(op(5)); // Output: 25
```

### Passing as Arguments

Functions can be passed to other functions (higher-order functions).

```rox
function apply(function(num) -> num f, num val) -> num {
    return f(val);
}

function double(num x) -> num {
    return x * 2;
}

print(apply(double, 10)); // Output: 20
```

### Returning Functions

Functions can return other functions.

```rox
function log(string msg) -> none {
    print(msg);
}

function get_logger() -> function(string) -> none {
    return log;
}

function(string)->none logger = get_logger();
logger("Log this message\n");
```

## Recursion

Recursive functions are fully supported.

```rox
function factorial(num n) -> num {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
```

## Notes

- **Anonymous Functions**: Anonymous functions (lambdas) are not supported by design. You must define a named function first.
- **Closures**: Functions do not capture variables from their surrounding scope by design. They only have access to their parameters and global constants.
