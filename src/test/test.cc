
#include <cjit/c-jit.hh>

#include <iostream>

int main()
{
    //cjit::JitCompiler *compiler = cjit::JitCompiler::create();
    cjit::JitCompiler *compiler = cjit::create<cjit::MirCompiler>();

    const char *c_code = "int func_add(int a, int b) { return a + b; } \n";
    cjit::CompiledInfo info = compiler->compile(c_code);

    typedef int (*fun_p)(int, int);
    fun_p fun = (fun_p)info.binary;
    int res = fun(1, 2);
    std::cout << "cjit result=" << res << std::endl;
}
