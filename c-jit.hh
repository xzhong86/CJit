// -*- C++ -*-

#progma once

#include <string>

namespace cjit {

struct CompiledInfo {
    long *id;
    void *binary;
};

class JitCompiler {
public:
    virtual ~JitCompiler() {}

    virtual CompiledInfo compile(const std::string &c_code) = 0;

    static JitCompiler * create(void);
};

} // namespace

