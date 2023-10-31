#include "lava.h"

Lava::Lava()
{
}

bool Lava::load(const std::vector<uint8_t> &source)
{
    // Check 16-byte file header
    const uint8_t *header = source.data();
    if (source.size() < 16)
        return error("File too short");

    // File signature
    if (!(header[0] == 'L' && header[1] == 'A' && header[2] == 'V' && header[3] == 0x12))
        return error("File signature mismatch");

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
        return error("Unknown RAM mode");
    }

    LavaDisp::mode_t gmode;
    switch (cfg & 0x60) {
    case 0x00:
        gmode = LavaDisp::GraphicMono;
        break;
    case 0x40:
        gmode = LavaDisp::Graphic4;
        break;
    case 0x60:
        gmode = LavaDisp::Graphic256;
        break;
    default:
        return error("Unknown graphic mode");
    }

    // Screen size
    uint32_t w = header[9] * 16;
    uint32_t h = header[10] * 16;
    w = w >= 320 ? 320 : w < 160 ? 160 : w;
    h = h >= 240 ? 240 : h <  80 ?  80 : h;

    disp.setMode(gmode);
    disp.setSize(w, h);

    if (!proc.load(source, rambits, pen_input))
        return false;

    return true;
}
