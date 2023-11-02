#include "lava_ram.h"
#include "lava_cfg.h"

void LavaRam::init(uint32_t rambits)
{
    this->ram_bits = rambits;
    this->ram_mask = rambits <= 16 ? 0x0000ffff : 0x00ffffff;
    if (rambits <= 16)
        ram.resize(1ul << 16);
    else
        ram.resize(1ul << 24);
    std::fill(ram.begin(), ram.end(), 0);

    stack          = LAVA_STACK_OFFSET;
    stack_string   = LAVA_STRING_OFFSET;
    stack_local    = 0;
    stack_local_bp = 0;
}

uint32_t LavaRam::pushString(const std::vector<uint8_t> &str)
{
    uint32_t a = stack_string;
    std::copy(str.begin(), str.end(), ram.begin() + a);
    stack_string += str.size();
    return a;
}
