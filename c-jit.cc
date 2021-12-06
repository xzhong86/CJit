
#include "c-jit.hh"

extern "C" {
#include "c2mir.h"
#include "mir-gen.h"
}

namespace cjit {

class RealCompiler : public JitCompiler {
    MIR_context_t ctx_;
    c2mir_options c2m_opt_;
    bool debug_ = false;

    void initCompilerOptions() {
        memset(&c2m_opt_, 0, sizeof(c2m_opt_));
        if (debug_) {
            c2m_opt_.messaeg_file = stdout; // for debug
        }
    }
public:
    RealCompiler() {
        ctx_ = MIR_init();
        initCompilerOptions();
    }
    ~RealCompiler() {
        MIT_finish(ctx_);
    }

    CompiledInfo compile(const std::string &c_code) {
        StrBuf buf(c_code);
        CompiledInfo info;

        c2mir_init(ctx_);
        c2mir_compile(ctx_, &c2m_opt_, cjit_getc, &buf, "cjit", NULL);
        c2mir_finish(ctx_);

        MIR_gen_init (ctx, 1);  // gens_num=1
        MIR_gen_set_optimize_level (ctx, 0, 2); // gen_num=0, level=2
        if (debug_) {
            MIR_gen_set_debug_file (ctx, 0, stdout);
            MIR_gen_set_debug_level (ctx, 0, 1);
        }
        MIR_link (ctx, MIR_set_gen_interface, NULL);

        void *binary = MIR_gen (ctx, 0, func);

        MIR_gen_finish (ctx);
    }
};

JitCompiler *JitCompiler::create()
{
    return new RealCompiler();
}

} // namespace
