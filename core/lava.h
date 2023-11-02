#pragma once

#include <string>
#include <cstdint>

#include "lava_cfg.h"
#include "lava_disp.h"
#include "lava_proc.h"

class Lava
{
public:
    enum bool_t {
        True  = 0xffffffff,
        False = 0x00000000,
    };

    enum key_t {
        KeyUp    = 20,
        KeyDown  = 21,
        KeyRight = 22,
        KeyLeft  = 23,
        KeyF1    = 28,
        KeyF2    = 29,
        KeyEnter = 13,
        KeySpace = 20,
        KeyHelp  = 25,
        KeyEsc   = 27,
    };

    Lava();

    bool load(const std::vector<uint8_t> &source);
    void loadLvmBin(const std::vector<uint8_t> &data) {disp.loadLvmBin(data);}

    LavaProc::proc_req_t &run() {return proc.run();}

    const std::vector<uint8_t> &inspectRam() {return proc.inspectRam();}

    uint8_t *getFramebuffer() {return disp.getFramebuffer();}
    void framebufferSwap() {disp.framebufferSwap();}
    uint16_t getFramebufferWidth() {return disp.getFramebufferWidth();}
    uint16_t getFramebufferHeight() {return disp.getFramebufferHeight();}
    uint16_t getFramebufferStride() {return disp.getFramebufferStride();}

    const std::string &getErrorMsg() {return serror;}

private:
    bool error(const std::string &msg)
    {
        serror = msg;
        return false;
    }

    std::string serror;

    LavaDisp disp;
    LavaProc proc;
};
