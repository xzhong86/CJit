
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
        MIR_gen_init (ctx, 1);  // gens_num=1
    }
    ~RealCompiler() {
        MIT_finish(ctx_);
        MIR_gen_finish (ctx);
    }

    CompiledInfo compile(const std::string &c_code) {
        StrBuf buf(c_code);
        CompiledInfo info;

        c2mir_init(ctx_);
        c2mir_compile(ctx_, &c2m_opt_, cjit_getc, &buf, "cjit", NULL);
        c2mir_finish(ctx_);

        std::vector<MIR_item_t> funcs;
        MIR_module_t m = DLIST_TAIL (MIR_module_t, *MIR_get_module_list (ctx_));
        for (MIR_item_t item = DLIST_HEAD (MIR_item_t, m->items); item != NULL;
             item = DLIST_NEXT(MIR_item_t, item)) {
            if (item->item_type == MIR_func_item)
                funcs.push_back(item);
        }
        MIR_item_t func = funcs.front(); // TBD: support multi-functions

        MIR_gen_set_optimize_level (ctx, 0, 2); // gen_num=0, level=2
        if (debug_) {
            MIR_gen_set_debug_file (ctx, 0, stdout);
            MIR_gen_set_debug_level (ctx, 0, 1);
        }
        MIR_link (ctx, MIR_set_gen_interface, NULL);

        info.id = NULL;
        info.binary = MIR_gen (ctx, 0, func);

        return info;
    }
};

JitCompiler *JitCompiler::create()
{
    return new RealCompiler();
}

} // namespace
