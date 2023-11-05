#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#include "lava_ram.h"
#include "lava_cfg.h"

class LavaCallback;
class LavaDisp;

class LavaProc
{
public:
    LavaProc() {}
    void setCallbacks(LavaCallback *cb) {this->cb = cb;}
    void setDisp(LavaDisp *disp) {this->disp = disp;}

    void load(const std::vector<uint8_t> &source, uint32_t rambits, bool pen_input);

    const std::vector<uint8_t> &inspectRam() {return ram.data();}

    void reset();
    void run();

    void saveState(std::vector<uint8_t> &data);
    uint32_t restoreState(const std::vector<uint8_t> &data, uint32_t offset);

private:
    uint32_t parse(uint32_t ofs);
    std::string to_string(const std::vector<uint8_t> &data)
    {
        return std::string(reinterpret_cast<const char *>(data.data()), data.size() - 1);
    }

    std::vector<uint8_t> lava_printf(const std::vector<uint8_t> &fmt,
                                     const std::vector<uint32_t> &params);

#include "lava_op_defs.h"

    // Opcode length including params
    enum op_len_t {
        // Constant param length
        OpParamBaseMask = 0xff,
        OpParam0        = 1 + 0,
        OpParam1        = 1 + 1,
        OpParam2        = 1 + 2,
        OpParam3        = 1 + 3,
        OpParam4        = 1 + 4,
        // Special param length
        OpParamAddr     = 0x0100,  // Add 2 or 3 bytes based on rambits
        OpParamStr      = 0x0200,
        OpParamPreset   = 0x0400,
    };

    // Map from opcode to functions
    struct op_info_t {
        std::string name;
        op_len_t len;
        std::function<void(LavaProc *, const std::vector<uint8_t> &)> func;
    };
    static std::unordered_map<uint8_t, op_info_t> op_info;

    // Map from code address to execution functions
    struct op_t {
        std::function<void()> func;
    };
    std::unordered_map<uint32_t, op_t> op_exec;
    std::vector<uint8_t> source;

    // Stallable callback functions
    struct {
        bool stack;
        std::function<int()> func;
        // Save PC and stack pointer before current instruction for save state support
        uint32_t pc, sp;
    } cb_func;

    // Other systems
    LavaCallback *cb;
    LavaDisp *disp;
    LavaRam ram;

    uint32_t rambits;
    bool pen_input;

    uint32_t pc;
    uint32_t flagv;     // Flag value for conditional jumps
    int32_t seed;
};
