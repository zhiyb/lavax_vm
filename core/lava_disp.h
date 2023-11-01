#pragma once

#include <cstdint>

#include "lava_cfg.h"

class LavaDisp
{
public:
    LavaDisp();

    enum mode_t {
        GraphicInvalid = 0,
        GraphicMono    = 1,
        Graphic4       = 4,
        Graphic256     = 8,
    };

    void setSize(uint32_t w, uint32_t h) {width = w; height = h;}
    void setMode(mode_t mode) {graphic_mode = mode;}
    mode_t getMode() {return graphic_mode;}

#if LAVA_DOUBLE_BUFFER
    uint8_t *getFramebuffer() {return &framebuffer[fb_disp][0][0];}
#else
    uint8_t *getFramebuffer() {return &framebuffer[0][0][0];}
#endif
    uint32_t getFramebufferWidth() {return width;}
    uint32_t getFramebufferHeight() {return height;}

private:
    void framebufferFill(uint8_t c);
    void framebufferClear();

#if LAVA_DOUBLE_BUFFER
    void framebufferSwap();
#else
    void framebufferSwap() {}
#endif

    uint8_t framebuffer[LAVA_DOUBLE_BUFFER + 1][LAVA_MAX_HEIGHT][LAVA_MAX_WIDTH];
    uint32_t fb_disp;
    uint32_t width, height;
    mode_t graphic_mode;
};
