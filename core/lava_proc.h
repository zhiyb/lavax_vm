#pragma once

#include <cstdint>
#include <vector>

#include "lava_cfg.h"

class LavaProc
{
public:
    LavaProc();

    bool load(const std::vector<uint8_t> &source, uint32_t rambits, bool pen_input);

private:
    std::vector<uint8_t> exec;

    uint32_t rambits;
    bool pen_input;

    uint32_t pc;
};
