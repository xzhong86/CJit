
#include "c-jit.hh"

extern "C" {
#include "c2mir/c2mir.h"
#include "mir-gen.h"
}

#include <vector>

namespace cjit {

using std::string;

class StrBuf {
    string str;
    int len, pos = 0;
public:
    StrBuf(const string &str) : str(str), len(str.length()) { }
    int getc() {
        return pos >= len ? EOF : str[pos++];
    }
};

static int cjit_getc(void *data) {
    return static_cast<StrBuf*>(data)->getc();
}

class MirCompiler : public JitCompiler {
    MIR_context_t ctx_;
    c2mir_options c2m_opt_;
    bool debug_ = false;

    void initCompilerOptions() {
        memset(&c2m_opt_, 0, sizeof(c2m_opt_));
        if (debug_) {
            c2m_opt_.message_file = stdout; // for debug
        }
    }
public:
    MirCompiler() {
        ctx_ = MIR_init();
        initCompilerOptions();
        MIR_gen_init (ctx_, 1);  // gens_num=1
    }
    ~MirCompiler() {
        MIR_gen_finish (ctx_);
        MIR_finish(ctx_);
    }

    void importSymbol(const std::string &name, void *p) override {
        MIR_load_external(ctx_, name.c_str(), p);
    }
    CompiledInfo compile(const std::string &c_code) override {
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

        MIR_load_module(ctx_, m);
        MIR_gen_set_optimize_level (ctx_, 0, 2); // gen_num=0, level=2
        if (debug_) {
            MIR_gen_set_debug_file (ctx_, 0, stdout);
            MIR_gen_set_debug_level (ctx_, 0, 3);
        }
        MIR_link (ctx_, MIR_set_gen_interface, NULL);

        info.id = NULL;
        info.binary = MIR_gen (ctx_, 0, func);

        return info;
    }
};

template <>
JitCompiler *create<MirCompiler>()
{
    return new MirCompiler();
}

} // namespace
