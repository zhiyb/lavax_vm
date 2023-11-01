#include "lava_ram.h"
#include "lava_cfg.h"

void LavaRam::init(uint32_t rambits)
{
    if (rambits <= 16)
        ram.resize(1ul << 16);
    else
        ram.resize(1ul << 24);

    stack        = LAVA_STACK_OFFSET;
    stack_string = LAVA_STRING_OFFSET;
}
