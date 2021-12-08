
#include <cjit/c-jit.hh>
#include <string.h>
#include <stdio.h>

#include <utest/utest.h>

namespace {
struct JitC {
    cjit::JitCompiler *compiler;
};
}

#define JITC (utest_fixture->compiler)

UTEST_F_SETUP(JitC) {
    JITC = cjit::create<cjit::MirCompiler>();
}
UTEST_F_TEARDOWN(JitC) {
    delete JITC;
}
UTEST_F(JitC, simple_add) {
    auto *compiler = JITC;
    const char *c_code = "int func_add(int a, int b) { return a + b; } \n";
    cjit::CompiledInfo info = compiler->compile(c_code);

    typedef int (*fun_p)(int, int);
    fun_p fun = (fun_p)info.binary;

    ASSERT_EQ(3, fun(1, 2));
}

