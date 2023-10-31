#include "lava_proc.h"

LavaProc::LavaProc()
{
}

bool LavaProc::load(const std::vector<uint8_t> &source, uint32_t rambits, bool pen_input)
{
    exec = source;
    this->rambits = rambits;
    this->pen_input = pen_input;

    pc = 16;

    return true;
}
