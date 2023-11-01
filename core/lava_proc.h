#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "lava_ram.h"
#include "lava_cfg.h"

class LavaDisp;

class LavaProc
{
public:
    LavaProc() {}

    void setDisp(LavaDisp *disp) {this->disp = disp;}

    bool load(const std::vector<uint8_t> &source, uint32_t rambits, bool pen_input);

    std::string param_string(const uint8_t *data) {return std::string();}

private:
#include "lava_op_defs.h"

    LavaDisp *disp;
    LavaRam ram;
    std::vector<uint8_t> source;

    uint32_t rambits;
    bool pen_input;

    uint32_t pc;
    uint32_t flagv;
};

#include "lava_op_code.h"
