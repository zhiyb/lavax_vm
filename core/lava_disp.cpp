#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <functional>

#include "lava_disp.h"

#define TODO(err) throw std::runtime_error(std::string("TODO ") + __FUNCSIG__ + ": " + (err))

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
    graphic_mode = GraphicMono;
    refresh = 0;

    bg_colour = 0;
    fg_colour = 0;
    setSize(LAVA_MAX_WIDTH, LAVA_MAX_HEIGHT);
    clearScreen();
}

void LavaDisp::setSize(uint16_t w, uint16_t h)
{
    width = w;
    height = h;
    fb_disp.resize(w * h);
    fb_working.resize(w * h);
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

    fg_colour = colour_mask;
    bg_colour = 0;
}

void LavaDisp::saveState(std::vector<uint8_t> &data)
{
    data.push_back(width  >> 0);
    data.push_back(width  >> 8);
    data.push_back(height >> 0);
    data.push_back(height >> 8);
    data.push_back(graphic_mode);
    data.insert(data.end(), fb_disp.cbegin(), fb_disp.cend());
    data.insert(data.end(), fb_working.cbegin(), fb_working.cend());
    data.push_back(fg_colour);
    data.push_back(bg_colour);
}

uint32_t LavaDisp::restoreState(const std::vector<uint8_t> &data, uint32_t offset)
{
    width   = data[offset + 0] << 0;
    width  |= data[offset + 1] << 8;
    height  = data[offset + 2] << 0;
    height |= data[offset + 3] << 8;
    setSize(width, height);
    graphic_mode = (mode_t)data[offset + 4];
    setMode(graphic_mode);
    offset += 5;

    std::copy(data.cbegin() + offset, data.cbegin() + offset + fb_disp.size(), fb_disp.begin());
    offset += fb_disp.size();
    std::copy(data.cbegin() + offset, data.cbegin() + offset + fb_working.size(), fb_working.begin());
    offset += fb_working.size();

    fg_colour = data[offset + 0];
    bg_colour = data[offset + 1];
    offset += 2;

    refresh = 1;

    return offset;
}

void LavaDisp::clearScreen()
{
    clearWorking();
    framebufferFlush();
}

void LavaDisp::clearWorking()
{
    std::fill(fb_working.begin(), fb_working.end(), bg_colour);
}

void LavaDisp::drawHLine(int16_t x, int16_t y, uint16_t w, uint8_t cfg)
{
    // Boundary checking
    if (y < 0 || y >= height)
        return;
    int16_t ww = w;
    if (x < 0) {
        ww -= -x;
        x = 0;
    } else if (x >= width) {
        return;
    }
    ww = std::min(ww, (int16_t)(width - x));
    if (ww <= 0)
        return;

    uint8_t no_buf = cfg & 0x40;
    uint8_t cmd    = cfg & 0x03;

    bool fb = no_buf ? false : true;
    switch (cmd) {
    case 0: // Fill with background colour
        std::fill(fbData(fb, y, x),
                  fbData(fb, y, x + ww),
                  bg_colour);
        break;
    case 1: // Fill with foreground colour
        std::fill(fbData(fb, y, x),
                  fbData(fb, y, x + ww),
                  fg_colour);
        break;
    case 2: // Invert colours
        std::transform(fbData(fb, y, x),
                       fbData(fb, y, x + ww),
                       fbData(fb, y, x),
                       [=](uint8_t c) {return c ^ colour_mask;});
        break;
    default:
        TODO("Unknown cmd: " + std::to_string(cmd));
        break;
    }

    if (no_buf)
        refresh = 1;
}

void LavaDisp::drawVLine(int16_t x, int16_t y, uint16_t h, uint8_t cfg)
{
    // Boundry checking
    if (x < 0 || x >= width)
        return;
    int16_t hh = h;
    if (y < 0) {
        hh -= -y;
        y = 0;
    } else if (y >= height) {
        return;
    }
    hh = std::min(hh, (int16_t)(height - y));
    if (hh <= 0)
        return;

    uint8_t no_buf = cfg & 0x40;
    uint8_t cmd    = cfg & 0x03;

    uint8_t c = cmd == 0 ? bg_colour : fg_colour;
    bool fb = no_buf ? false : true;
    if (cmd == 2) {
        for (int dy = 0; dy < hh; dy++)
            *fbData(fb, y + dy, x) ^= colour_mask;
    } else {
        for (int dy = 0; dy < hh; dy++)
            *fbData(fb, y + dy, x) = c;
    }

    if (no_buf)
        refresh = 1;
}

void LavaDisp::drawBlock(int16_t x0, int16_t x1, int16_t y0, int16_t y1, uint8_t cfg)
{
    if (y0 > y1)
        std::swap(y0, y1);
    if (x0 > x1)
        std::swap(x0, x1);

    uint16_t w = x1 - x0 + 1;
    for (int16_t y = y0; y <= y1; y++)
        drawHLine(x0, y, w, cfg);
}

void LavaDisp::drawBlock(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t cfg,
                         const std::vector<uint8_t> &data)
{
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
    auto const &op_default = [=](uint8_t cfb, uint8_t cptn) {return cptn;};
    auto const &op_inv     = [=](uint8_t cfb, uint8_t cptn) {return colour_mask ^ cptn;};
    const std::function<uint8_t (uint8_t, uint8_t)> op[8] = {
        /* 0x00 */ op_default,
        /* 0x01 */ op_default,
        /* 0x02 */ op_inv,
        /* 0x03 */ [=](uint8_t cfb, uint8_t cptn) {return cfb | cptn;},
        /* 0x04 */ [=](uint8_t cfb, uint8_t cptn) {return cfb & cptn;},
        /* 0x05 */ [=](uint8_t cfb, uint8_t cptn) {return cfb ^ cptn;},
        /* 0x06 */ [=](uint8_t cfb, uint8_t cptn) {return cptn ? cptn : cfb;},
        /* 0x07 */ op_default,
    };
    auto const &func = op[cmd & 7];

    // Packed data pattern
    uint8_t bits = graphic_mode;
    uint8_t mask = colour_mask;
    uint8_t inv  = cmd & 0x08 ? mask : 0;

    // Draw to frame buffer
    bool fb = no_buf ? false : true;
    uint32_t ofs = 0;
    for (uint32_t dy = 0; dy < h; dy++) {
        for (uint32_t dx = 0; dx < w; dx++) {
            int32_t xx = x + (hflip ? w - 1 - dx : dx);
            int32_t yy = y + dy;
            if (inRange(xx, yy)) {
                uint8_t cptn = ((data[ofs / 8] >> (8 - bits - (ofs % 8))) & mask) ^ inv;
                uint8_t &cfb = *fbData(fb, yy, xx);
                cfb = func(cfb, cptn);
            }
            ofs += bits;
        }
        // Align to 8-bit
        ofs += (8 - (ofs % 8)) % 8;
    }

    if (no_buf)
        refresh = 1;
}

void LavaDisp::drawBlockMono(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t cfg,
                             const std::vector<uint8_t> &data)
{
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
    uint8_t inv  = (cmd == 2) || (cmd & 0x08) ? colour_mask : 0;

    // Draw to frame buffer
    bool fb = no_buf ? false : true;
    uint32_t ofs = 0;
    for (uint32_t dy = 0; dy < h; dy++) {
        for (uint32_t dx = 0; dx < w; dx++) {
            int32_t xx = x + (hflip ? w - 1 - dx : dx);
            int32_t yy = y + dy;
            if (inRange(xx, yy)) {
                uint8_t v = data[ofs / 8] & (0x80 >> (ofs % 8)) ? fg_colour : bg_colour;
                v = v ^ inv;
                *fbData(fb, yy, xx) = v;
            }
            ofs += 1;
        }
        // Align to 8-bit
        ofs += (8 - (ofs % 8)) % 8;
    }

    if (no_buf)
        refresh = 1;
}

void LavaDisp::drawRectangle(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint8_t cfg)
{
    uint8_t cmd    = cfg & 0x03;

    if (y0 > y1)
        std::swap(y0, y1);
    if (x0 > x1)
        std::swap(x0, x1);

    uint16_t w = x1 - x0 + 1;
    uint16_t h = y1 - y0 + 1;

    drawHLine(x0, y0, w, cfg);
    drawHLine(x0, y1, w, cfg);
    drawVLine(x0, y0, h, cfg);
    drawVLine(x1, y0, h, cfg);
}

void LavaDisp::drawText(const std::vector<uint8_t> &str, int16_t x, int16_t y, uint8_t cfg)
{
    // Font width
    bool large = cfg & 0x80;
    uint32_t w = large ? 8 : 6;

    for (uint32_t i = 0; i < str.size(); i++) {
        uint16_t c = str.at(i);
        if (c == '\0')
            break;
        int16_t sx = x;
        if (c >= 0x80) {
            c |= str.at(++i) << 8;
            // Chinese characters have double width
            x += w;
        }
        drawCharacter(c, sx, y, cfg);
        x += w;
    }
}

void LavaDisp::drawCharacter(uint16_t c, int16_t x, int16_t y, uint8_t cfg)
{
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
    case OffsetGbFont16:
        if (c1 < 0xb0)
            offset += ((c1 - 0xa1) * 94 + (c2 - 0xa1)) * 32;
        else
            offset += ((c1 - 0xa7) * 94 + (c2 - 0xa1)) * 32;
        break;
    default:
        std::cerr << "Unknown font type: " << font_type << std::endl;
        break;
    }

    // Draw graphic block
    uint32_t size = h * ((w + 7) / 8);
    if (lvm_data.size() < offset + size) {
        std::cerr << "Error: Font offset out of range" << std::endl;
        return;
    }
    std::vector<uint8_t> data(lvm_data.cbegin() + offset, lvm_data.cbegin() + offset + size);
    drawBlockMono(x, y, w, h, cfg, data);
}

void LavaDisp::xdraw(uint8_t cmd)
{
    bool fb = true;

    if (cmd == 0) {
        // Move 1 pixel to the left
        for (uint32_t y = 0; y < width; y++) {
            std::move(fbData(fb, y, 1), fbData(fb, y, width), fbData(fb, y, 0));
            *fbData(fb, y, width - 1) = bg_colour;
        }

    } else if (cmd == 1) {
        // Move 1 pixel to the right
        for (uint32_t y = 0; y < width; y++) {
            std::move(fbData(fb, y, 0), fbData(fb, y, width - 1), fbData(fb, y, 1));
            *fbData(fb, y, 0) = bg_colour;
        }

    } else if (cmd == 2) {
        // Move 1 line to the top
        for (uint32_t y = 0; y < width - 1; y++)
            std::move(fbData(fb, y + 1, 0), fbData(fb, y + 1, width), fbData(fb, y, 0));
        uint32_t y = width - 1;
        std::fill(fbData(fb, y, 0), fbData(fb, y, width), bg_colour);

    } else if (cmd == 3) {
        // Move 1 line to the bottom
        for (int32_t y = width - 2; y >= 0; y--)
            std::move(fbData(fb, y, 0), fbData(fb, y, width), fbData(fb, y + 1, 0));
        uint32_t y = 0;
        std::fill(fbData(fb, y, 0), fbData(fb, y, width), bg_colour);

    } else if (cmd == 4) {
        // Horizontal flip
        for (uint32_t y = 0; y < width; y++)
            std::reverse(fbData(fb, y, 0), fbData(fb, y, width));

    } else if (cmd == 5) {
        // Vertical flip
        for (uint32_t y = 0; y < width / 2; y++)
            std::swap_ranges(fbData(fb, y, 0), fbData(fb, y, width),
                             fbData(fb, width - 1 - y, 0));

    } else if (cmd == 6) {
        // Copy display buffer to working buffer
        fb_working = fb_disp;
#if 0
        std::copy(fbData(1 - fb, 0, 0),
                  fbData(1 - fb, 0, 0) + fb_working.size(),
                  fbData(fb, 0, 0));
#endif
    }
}

std::vector<uint8_t> LavaDisp::getBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t cfg)
{
    std::vector<uint8_t> data;

    // Align x and w to 8 pixels
    if (graphic_mode != Graphic256) {
        x = x & 0xfff8;
        w = w & 0xfff8;
    }

    // Boundary checking
    x = std::min(x, width);
    y = std::min(y, height);
    w = std::min((int)w, width - x);
    h = std::min((int)h, height - y);

    // Active working frame buffer
    bool no_buf = cfg & 0x40;
    bool fb = no_buf ? false : true;

    uint8_t bits = graphic_mode;
    uint32_t ofs = 0;
    for (uint32_t dy = 0; dy < h; dy++) {
        uint8_t v = 0;
        for (uint32_t dx = 0; dx < w; dx++) {
            v <<= bits;
            v |= *fbData(fb, y + dy, x + dx);
            ofs += bits;
            if (ofs % 8 == 0) {
                data.push_back(v);
                v = 0;
            }
        }
    }

    return data;
}

void LavaDisp::framebufferFlush()
{
    //std::copy(fb_working.cbegin(), fb_working.cend(), fb_disp.begin());
    fb_disp = fb_working;
    refresh = 1;
}
