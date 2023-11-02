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

    void setSize(uint16_t w, uint16_t h) {width = w; height = h;}
    void setMode(mode_t mode) {graphic_mode = mode;}
    mode_t getMode() {return graphic_mode;}

    uint8_t refreshRequest()
    {
        uint8_t v = refresh;
        refresh = 0;
        return v;
    }

    void clearScreen();

    // Draw horizontal line
    void drawHLine(uint16_t x0, uint16_t x1, uint16_t y0, uint8_t cmd, uint8_t no_buf);
    // Draw solid rectangle block
    void drawBlock(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint8_t cmd, uint8_t no_buf);

    void setForegroundColour(uint8_t c) {fg_colour = c;}
    void setBackgroundColour(uint8_t c) {bg_colour = c;}

#if LAVA_DOUBLE_BUFFER
    uint8_t *getFramebuffer() {return &framebuffer[fb_disp][0][0];}
#else
    uint8_t *getFramebuffer() {return &framebuffer[0][0][0];}
#endif
    uint16_t getFramebufferWidth() {return width;}
    uint16_t getFramebufferHeight() {return height;}
    uint16_t getFramebufferStride() {return LAVA_MAX_WIDTH;}

private:
    void framebufferFill(uint8_t c);
    void framebufferClear();

#if LAVA_DOUBLE_BUFFER
    void framebufferSwap();
#else
    void framebufferSwap() {refresh = 1;}
#endif

    uint8_t framebuffer[LAVA_DOUBLE_BUFFER + 1][LAVA_MAX_HEIGHT][LAVA_MAX_WIDTH];
    uint32_t fb_disp;
    uint16_t width, height;
    mode_t graphic_mode;
    uint8_t fg_colour, bg_colour;
    uint8_t refresh;
};
