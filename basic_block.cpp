#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"
#include <string>

#include "instruction.hpp"
#include "hart.hpp"
#include "basic_block.hpp"

using namespace llvm;
using namespace llvm::orc;

RVBasicBlock bbs_arr[BB_arr_mask+1] = {};

static llvm::Expected<std::unique_ptr<llvm::orc::LLJIT>> jit = nullptr;
static llvm::LLVMContext ctx;
static llvm::FunctionType *jit_ft;

void RVBasicBlock::init() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    jit = LLJITBuilder().create();
    if (!jit) {
        std::cerr << "Failed to create LLJIT: " << toString(jit.takeError()) << std::endl;
        exit(1);
    }
    jit_ft = FunctionType::get(Type::getVoidTy(ctx), 
       {Type::getInt64PtrTy(ctx), Type::getInt8Ty(ctx), Type::getInt64PtrTy(ctx), Type::getInt1PtrTy(ctx)}, false);
}

size_t RVBasicBlock::construct(const instT *arr) {
    int i = 0;
    for (; i < BB_len; i++) {
        DEB("bb:" << i);
        const instT &instruction = arr[i];
        DEB("bb:" << instruction);
        uint32_t fingerprint = instruction & mask[instruction & 127];
        DEB("bb:" << i);
        fingerprint = FP_HASH(fingerprint);

        DEB("reading...");
        auto dec = decoders[fingerprint];
        DEB("decoding..");
        dec.decod(instrs[i], instruction);
        instrs[i].execute = dec.exec;
        if (!dec.linear) {
            len = i + 1;
            return i;
        }
    }
    DEB("decoded\n");
    instrs[BB_len].execute = Executors::empty_executor;
    len = BB_len+1;
    return BB_len+1;
}

size_t RVBasicBlock::do_jit(const instT *arr) {
    static int cnt = 0;
    cnt++;
    const std::string name = std::to_string(cnt);
    auto module = std::make_unique<Module>(name, ctx);
    Function* fn = Function::Create(jit_ft, Function::ExternalLinkage, name, module.get());
    BasicBlock* block = BasicBlock::Create(ctx, "", fn);
    IRBuilder<> builder(block);

    auto args = fn->args().begin();
    Value* regs = &*args++;
    Value* mem = &*args++;
    Value* pc = &*args++;
    Value* done = &*args;
    Value* new_pc = builder.CreateAlloca(Type::getInt64Ty(ctx), 0, "new_pc");

    int i = 0;
    for (; i < BB_len; i++) {
        DEB("bb:" << i);
        const instT &instruction = arr[i];
        DEB("bb:" << instruction);
        uint32_t fingerprint = instruction & mask[instruction & 127];
        DEB("bb:" << i);
        fingerprint = FP_HASH(fingerprint);

        DEB("reading...");
        auto dec = decoders[fingerprint];
        DEB("decoding..");
        dec.decod(instrs[i], instruction);
        dec.jit(instrs[i], builder, ctx, regs, mem, pc, new_pc, fn, done);
        if (!dec.linear) {
            len = i + 1;
            break;
        }
    }
    if (i==BB_len) {
        DEB("decoded\n");
        instrs[BB_len].execute = Executors::empty_executor;
        len = BB_len+1;
    }
    module->print(outs(), nullptr);
    std::cout << "printed\n";

    bool verif = verifyModule(*module, &outs());
    outs() << "[VERIFICATION] " << (!verif ? "OK\n\n" : "FAIL\n\n");



    if (auto err = jit->get()->addIRModule(ThreadSafeModule(std::move(module), std::make_unique<LLVMContext>()))) {
        std::cerr << "Failed to add module to LLJIT: " << toString(std::move(err)) << std::endl;
        return 1;
    }

    std::cout << jit->get() << " no err\n";
    // Look up the JIT'd function, cast it to a function pointer, then call it.
    auto jit_func_sym = jit->get()->lookup(name);
    std::cout << jit->get() << " no err\n";
    if (!jit_func_sym) {
        std::cerr << "Failed to look up function: " << toString(jit_func_sym.takeError()) << std::endl;
        return 1;
    }

    std::cout << "looked up\n";
    // Cast the symbol's address to a function pointer and call it.
    jitted = jit_func_sym->toPtr<BBFType>();
    std::cout << "casted\n";
    return BB_len+1;
}
