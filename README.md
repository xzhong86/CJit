# C-Jit Project

  A simple C programming language JIT project. Dynamicaly generate code from c code.
  
  Currently I only have [MIR](https://github.com/vnmakarov/mir) backend for generating binary code.

## Build

  1. init sub modules: `git submodule update --init --recursive`
  1. create build folder: `mkdir build && cd build/`.
  1. run cmake: `cmake .. && make`

## Simple Example

```c
    auto *compiler = cjit::create<cjit::MirCompiler>();

    const char *c_code = "int func_add(int a, int b) { return a + b; } \n";
    cjit::CompiledInfo info = compiler->compile(c_code);

    typedef int (*fun_p)(int, int);
    fun_p fun = (fun_p)info.binary;

    ASSERT_EQ(3, fun(1, 2));
```
