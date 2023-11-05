#include <iostream>
#include <stdexcept>

#include "lava.h"

Lava::Lava()
{
    proc.setDisp(&disp);
}

void Lava::load(const std::vector<uint8_t> &source)
{
    // Check 16-byte file header
    const uint8_t *header = source.data();
    if (source.size() < 16)
        throw std::runtime_error("File too short");

    // File signature
    if (!(header[0] == 'L' && header[1] == 'A' && header[2] == 'V' && header[3] == 0x12))
        throw std::runtime_error("File signature mismatch");

    // Configurations
    uint8_t cfg = header[8];

    bool pen_input = cfg & 0x01;

    uint32_t rambits = 0;
    switch (cfg & 0x90) {
    case 0x00:
        rambits = 16;
        break;
    case 0x10:
        rambits = 32;
        break;
    case 0x80:
        rambits = 24;
        break;
    default:
        throw std::runtime_error("Unknown RAM mode: " + std::to_string(cfg));
    }

    LavaDisp::mode_t gmode;
    switch (cfg & 0x60) {
    case 0x00:
        gmode = LavaDisp::GraphicMono;
        break;
    case 0x40:
        gmode = LavaDisp::Graphic16;
        break;
    case 0x60:
        gmode = LavaDisp::Graphic256;
        break;
    default:
        throw std::runtime_error("Unknown graphic mode: " + std::to_string(cfg));
    }

    // Screen size
    uint32_t w = header[9] * 16;
    uint32_t h = header[10] * 16;
    w = w >= 320 ? 320 : w < 160 ? 160 : w;
    h = h >= 240 ? 240 : h <  80 ?  80 : h;
    std::cerr << "Screen size: " << w << "x" << h << std::endl;

    disp.setMode(gmode);
    disp.setSize(w, h);
    disp.clearScreen();

    proc.load(source, rambits, pen_input);
}

void Lava::reset()
{
    disp.setMode(disp.getMode());
    disp.clearScreen();
    proc.reset();
}

std::vector<uint8_t> Lava::saveState()
{
    std::vector<uint8_t> data;

    const uint32_t version = 0;
    data.push_back(version >>  0);
    data.push_back(version >>  8);
    data.push_back(version >> 16);
    data.push_back(version >> 24);

    disp.saveState(data);
    proc.saveState(data);
    return data;
}

void Lava::restoreState(const std::vector<uint8_t> &data)
{
    uint32_t offset = 0;

    uint32_t version = 0;
    version |= data[offset + 0] <<  0;
    version |= data[offset + 1] <<  8;
    version |= data[offset + 2] << 16;
    version |= data[offset + 3] << 24;
    offset += 4;

    offset = disp.restoreState(data, offset);
    offset = proc.restoreState(data, offset);
}
