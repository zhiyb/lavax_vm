#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include "lava_disp.h"

#define TODO(err) throw std::runtime_error(std::string("TODO ") + __FUNCSIG__ + ": " + (err))

#if LAVA_DOUBLE_BUFFER
#define FB_ACTIVE   (1 - fb_disp)
#else
#define FB_ACTIVE   (0)
#endif

enum lvm_data_offset_t {
    OffsetAscii     = 0,
    OffsetAscii8    = OffsetAscii + 1536,
    OffsetGbFont    = OffsetAscii8 + 2048,
    OffsetGbFont16  = OffsetGbFont + 81*94*24,
    OffsetScreenKey = OffsetGbFont16 + 81*94*32 + 0x3e,
    OffsetPinyin    = OffsetScreenKey + 6400,
};

LavaDisp::LavaDisp()
{
    graphic_mode = GraphicInvalid;
    fb_disp = 0;
    bg_colour = 0;
    fg_colour = 1;
    refresh = 0;

    clearScreen();
}

void LavaDisp::setMode(mode_t mode)
{
    graphic_mode = mode;
    switch (mode) {
    case Graphic16:
        colour_mask = 0x0f;
        break;
    case Graphic256:
        colour_mask = 0xff;
        break;
    default:
        colour_mask = 1;
        break;
    }
}

void LavaDisp::clearScreen()
{
    framebufferClear();
    framebufferSwap();
}

void LavaDisp::drawHLine(uint16_t x, uint16_t y, uint16_t w, uint8_t cfg)
{
    // Assert: y0 < height, x0 < width, x1 < width, x0 <= x1

    uint8_t no_buf = cfg & 0x40;
    uint8_t cmd    = cfg & 0x03;

    uint32_t fb_active = FB_ACTIVE;
    switch (cmd) {
    case 0: // Fill with background colour
        std::fill(&framebuffer[fb_active][y][x],
                  &framebuffer[fb_active][y][x + w],
                  bg_colour);
        break;
    case 1: // Fill with foreground colour
        std::fill(&framebuffer[fb_active][y][x],
                  &framebuffer[fb_active][y][x + w],
                  fg_colour);
        break;
    case 2: // Invert colours
        std::transform(&framebuffer[fb_active][y][x],
                       &framebuffer[fb_active][y][x + w],
                       &framebuffer[fb_active][y][x],
                       [=](uint8_t c) {return c ^ colour_mask;});
        break;
    default:
        TODO("Unknown cmd: " + std::to_string(cmd));
        break;
    }
}

void LavaDisp::drawVLine(uint16_t x, uint16_t y, uint16_t h, uint8_t cfg)
{
    uint8_t no_buf = cfg & 0x40;
    uint8_t cmd    = cfg & 0x03;

    uint8_t c = cmd == 0 ? bg_colour : fg_colour;
    uint32_t fb_active = FB_ACTIVE;
    if (cmd == 2) {
        for (int dy = 0; dy < h; dy++)
            framebuffer[fb_active][y + dy][x] ^= colour_mask;
    } else {
        for (int dy = 0; dy < h; dy++)
            framebuffer[fb_active][y + dy][x] = c;
    }
}

void LavaDisp::drawBlock(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint8_t cfg)
{
    //std::cerr << __func__ << ": (" << x0 << "," << y0 << "), (" << x1 << "," << y1 << "), " << (uint32_t)cfg << std::endl;

    uint8_t no_buf = cfg & 0x40;
    uint8_t cmd    = cfg & 0x03;

    if (y0 > y1)
        std::swap(y0, y1);
    if (x0 > x1)
        std::swap(x0, x1);

    x0 = std::min(x0, (uint16_t)(width - 1));
    x1 = std::min(x1, (uint16_t)(width - 1));
    y0 = std::min(y0, (uint16_t)(height - 1));
    y1 = std::min(y1, (uint16_t)(height - 1));

    uint16_t w = x1 - x0 + 1;
    for (uint16_t y = y0; y <= y1; y++)
        drawHLine(x0, y, w, cfg);

    if (no_buf)
        framebufferSwap();
}

void LavaDisp::drawBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t cfg,
                         const std::vector<uint8_t> &data)
{
    uint32_t fb_active = FB_ACTIVE;

	bool no_buf = cfg & 0x40;
	bool hflip  = cfg & 0x20;   // Horizontal flip
	uint8_t cmd = cfg & 0x0f;

    // default:    Pattern copy
    // cmd ==  2:  Inverted colour
    // cmd ==  3:  Pattern OR
    // cmd ==  4:  Pattern AND
    // cmd ==  5:  Pattern XOR
    // cmd ==  6:  Transparent background (0)
    // cmd bit 3:  Inverted pattern
    if ((cmd & 7) >= 3 && (cmd & 7) <= 6)
        TODO("Unknown cmd: " + std::to_string((int)cmd));

    // Packed data pattern
    uint8_t bits = graphic_mode;
    uint8_t mask = colour_mask;
    uint8_t inv  = (cmd == 2) || (cmd & 0x80) ? mask : 0;

    // Draw to frame buffer
    uint32_t ofs = 0;
    for (uint32_t dy = 0; dy < h; dy++) {
        for (uint32_t dx = 0; dx < w; dx++) {
            uint8_t v = ((data[ofs / 8] >> (8 - bits - (ofs % 8))) & mask) ^ inv;
            uint32_t xx = hflip ? w - 1 - dx : dx;
            framebuffer[fb_active][y + dy][x + xx] = v;
            ofs += bits;
        }
        // Align to 8-bit
        ofs += (8 - (ofs % 8)) % 8;
    }

    if (no_buf)
        framebufferSwap();
}

void LavaDisp::drawRectangle(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint8_t cfg)
{
    uint8_t no_buf = cfg & 0x40;
    uint8_t cmd    = cfg & 0x03;

    if (y0 > y1)
        std::swap(y0, y1);
    if (x0 > x1)
        std::swap(x0, x1);

    x0 = std::min(x0, (uint16_t)(width - 1));
    x1 = std::min(x1, (uint16_t)(width - 1));
    y0 = std::min(y0, (uint16_t)(height - 1));
    y1 = std::min(y1, (uint16_t)(height - 1));

    uint16_t w = x1 - x0 + 1;
    uint16_t h = y1 - y0 + 1;

    drawHLine(x0, y0, w, cfg);
    drawHLine(x0, y1, w, cfg);
    drawVLine(x0, y0, h, cfg);
    drawVLine(x1, y0, h, cfg);

    if (no_buf)
        framebufferSwap();
}

void LavaDisp::drawText(const std::vector<uint8_t> &str, uint16_t x, uint16_t y, uint8_t cfg)
{
    // Font width
    bool large  = cfg & 0x80;
    uint32_t w = large ? 8 : 6;

    for (uint32_t i = 0; i < str.size(); i++) {
        uint16_t c = str.at(i);
        if (c == '\0')
            break;
        uint16_t sx = x;
        if (c >= 0x80) {
            c |= str.at(++i) << 8;
            // Chinese characters have double width
            x += w;
        }
        drawCharacter(c, sx, y, cfg);
        x += w;
    }
}

void LavaDisp::drawCharacter(uint16_t c, uint16_t x, uint16_t y, uint8_t cfg)
{
    uint32_t fb_active = FB_ACTIVE;

    uint8_t c1 = c;
    uint8_t c2 = c >> 8;
    bool zh_cn    = c1 >= 0x80;
    bool large    = cfg & 0x80;
    bool no_buf   = cfg & 0x40;
    bool negative = cfg & 0x20;
    uint8_t cmd   = cfg & 0x0f;

    lvm_data_offset_t font_type;
    if (large)
        font_type = zh_cn ? OffsetGbFont16 : OffsetAscii8;
    else
        font_type = zh_cn ? OffsetGbFont : OffsetAscii;

    // Font size
    uint8_t w = large ? 8 : 6;
    uint8_t h = w * 2;
    w *= zh_cn ? 2 : 1;

    // Find font graphic offset
    uint32_t offset = font_type;
    switch (font_type) {
    case OffsetAscii:
        offset += c1 * 12;
        break;
    case OffsetAscii8:
        offset += c1 * 16;
        break;
    case OffsetGbFont:  // font_12x12
        if (c1 < 0xb0)
            offset += ((c1 - 0xa1) * 94 + (c2 - 0xa1)) * 24;
        else
            offset += ((c1 - 0xa7) * 94 + (c2 - 0xa1)) * 24;
        break;
    default:
        std::cerr << "Unknown font type: " << font_type << std::endl;
        break;
    }

    // Draw graphic block
    uint8_t fgc = graphic_mode == GraphicMono ? 1 : fg_colour;
    uint8_t bgc = graphic_mode == GraphicMono ? 0 : bg_colour;
    uint8_t *p = &lvm_data.at(offset);
    uint32_t ofs = 0;   // Offset in bits
    for (uint32_t yy = 0; yy < h; yy++) {
        for (uint32_t xx = 0; xx < w; xx++) {
            uint8_t v = p[ofs / 8] & (0x80 >> (ofs % 8)) ? fgc : bgc;
            framebuffer[fb_active][y + yy][x + xx] = v;
            ofs++;
        }
        // Align to 8-bit
        ofs += (8 - (ofs % 8)) % 8;
    }
}

void LavaDisp::framebufferFill(uint8_t c)
{
    uint32_t fb_active = FB_ACTIVE;
    std::fill(&framebuffer[fb_active][0][0],
              &framebuffer[fb_active][0][0] + sizeof(framebuffer[0]),
              c);
}

void LavaDisp::framebufferClear()
{
    framebufferFill(bg_colour);
}

#if LAVA_DOUBLE_BUFFER
void LavaDisp::framebufferSwap()
{
    uint32_t fb_active = FB_ACTIVE;
    std::copy(&framebuffer[fb_active][0][0],
              &framebuffer[fb_active][0][0] + sizeof(framebuffer[0]),
              &framebuffer[fb_disp][0][0]);
    fb_disp = fb_active;
    refresh = 1;
}
#endif
