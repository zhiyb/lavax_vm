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

    const std::vector<uint8_t> &inspectRam() {return ram.data();}

    struct proc_req_t {
        enum proc_req_type_t {
            ReqNone    = 0,
            ReqRefresh = 1 << 0,
            ReqDelay   = 1 << 1,
            ReqGetchar = 1 << 2,
            ReqExit    = 1 << 3,
        };
        uint32_t req;
        uint32_t req_value;

        enum proc_resp_type_t {
            RespNone    = 0,
            RespGetchar = 1 << 0,
        };
        uint32_t resp;
        uint32_t resp_value;
    };

    proc_req_t &run();

private:
    uint32_t parse(uint32_t ofs);

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
    proc_req_t req;

    uint32_t rambits, ram_mask;
    bool pen_input;

    uint32_t pc;
    uint32_t flagv;
};
