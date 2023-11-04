#pragma once

#include <cstdint>
#include <vector>

#include "lava_cfg.h"

class LavaDisp
{
public:
    LavaDisp();
    void loadLvmBin(const std::vector<uint8_t> &data) {this->lvm_data = data;}

    enum mode_t {
        GraphicMono    = 1,
        Graphic16      = 4,
        Graphic256     = 8,
    };

    void setSize(uint16_t w, uint16_t h) {width = w; height = h;}
    void setMode(mode_t mode);
    mode_t getMode() {return graphic_mode;}

    uint8_t refreshRequest()
    {
        uint8_t v = refresh;
        refresh = 0;
        return v;
    }

    void clearScreen();
    void clearActive();

    // Draw solid rectangle block
    void drawBlock(int16_t x0, int16_t x1, int16_t y0, int16_t y1, uint8_t cfg);
    // Draw block
    void drawBlock(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t cfg,
                   const std::vector<uint8_t> &data);
    // Draw rectangle
    void drawRectangle(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint8_t cfg);
    // Draw text
    void drawText(const std::vector<uint8_t> &str, int16_t x, int16_t y, uint8_t cfg);
    // xdraw
    void xdraw(uint8_t cmd);

    // Read framebuffer data
    std::vector<uint8_t> getBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t cfg);

    void setForegroundColour(uint8_t c) {fg_colour = c & colour_mask;}
    void setBackgroundColour(uint8_t c) {bg_colour = c & colour_mask;}

#if LAVA_DOUBLE_BUFFER
    uint8_t *getFramebuffer() {return &framebuffer[1 - fb_active][0][0];}
    void framebufferSwap();
#else
    uint8_t *getFramebuffer() {return &framebuffer[0][0][0];}
    void framebufferSwap() {refresh = 1;}
#endif
    uint16_t getFramebufferWidth() {return width;}
    uint16_t getFramebufferHeight() {return height;}
    uint16_t getFramebufferStride() {return LAVA_MAX_WIDTH;}

private:
    // Draw a horizontal line
    void drawHLine(int16_t x, int16_t y, uint16_t w, uint8_t cfg);
    // Draw a vertical line
    void drawVLine(int16_t x, int16_t y, uint16_t h, uint8_t cfg);
    // Draw a character
    void drawCharacter(uint16_t c, int16_t x, int16_t y, uint8_t cfg);
    // Draw block, with 1 bbp source data
    void drawBlockMono(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t cfg,
                       const std::vector<uint8_t> &data);

    void framebufferFill(uint8_t c);
    void framebufferClear();

    bool in_range(int32_t x, int32_t y) {return x >= 0 && y >= 0 && x < width && y < height;}

    std::vector<uint8_t> lvm_data;

    uint8_t framebuffer[LAVA_DOUBLE_BUFFER + 1][LAVA_MAX_HEIGHT][LAVA_MAX_WIDTH];
    uint32_t fb_active;     // Current working buffer
    uint16_t width, height;
    mode_t graphic_mode;
    uint8_t fg_colour, bg_colour, colour_mask;
    uint8_t refresh;
};
