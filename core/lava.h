#pragma once

#include <string>
#include <cstdint>

#include "lava_cfg.h"
#include "lava_disp.h"
#include "lava_proc.h"

class Lava
{
public:
    Lava();

    bool load(const std::vector<uint8_t> &source);

#if LAVA_DOUBLE_BUFFER
    uint8_t *getFramebuffer() {return disp.getFramebuffer();}
#endif
    uint32_t getFramebufferWidth() {return disp.getFramebufferWidth();}
    uint32_t getFramebufferHeight() {return disp.getFramebufferHeight();}

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
