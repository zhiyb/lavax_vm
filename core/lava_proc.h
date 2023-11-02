#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <functional>

#include "lava_ram.h"
#include "lava_cfg.h"

class LavaDisp;

class LavaProc
{
public:
    LavaProc() {}

    void setDisp(LavaDisp *disp) {this->disp = disp;}

    bool load(const std::vector<uint8_t> &source, uint32_t rambits, bool pen_input);
    uint32_t parse(uint32_t ofs);

    const std::vector<uint8_t> &inspectRam() {return ram.data();}

    enum proc_req_t {
        ReqNone    = 0x00,
        ReqRefresh = 0x01,
    };

    proc_req_t run();

private:
#include "lava_op_defs.h"

    // Opcode length including params
    enum op_len_t {
        OpParam0    = 1 + 0,
        OpParam1    = 1 + 1,
        OpParam2    = 1 + 2,
        OpParam3    = 1 + 3,
        OpParam4    = 1 + 4,
        OpParamAddr = 0x0100,  // Add 2 or 3 bytes based on rambits
        OpParamStr  = 0x0200,
    };

    struct op_info_t {
        std::string name;
        op_len_t len;
        std::function<void(LavaProc *, const std::vector<uint8_t> &)> func;
    };
    static std::map<uint8_t, op_info_t> op_info;

    struct op_t {
        std::function<void(LavaProc *, const std::vector<uint8_t> &)> func;
        std::vector<uint8_t> data;
        uint8_t opcode;
    };
    std::map<uint32_t, op_t> op_exec;

    LavaDisp *disp;
    LavaRam ram;
    std::vector<uint8_t> source;

    uint32_t rambits;
    bool pen_input;

    uint32_t pc;
    uint32_t flagv;
};
