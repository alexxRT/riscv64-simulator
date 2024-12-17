#include "basic_block.hpp"
#include "instruction.hpp"
#include "hart.hpp"

BasicBlock bbs_arr[BB_arr_mask+1] = {};

size_t BasicBlock::construct(const instT *arr) {
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
