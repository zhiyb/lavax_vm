#include <algorithm>

#include "lava_disp.h"

#if LAVA_DOUBLE_BUFFER
#define FB_ACTIVE   (1 - fb_disp)
#else
#define FB_ACTIVE   (0)
#endif

LavaDisp::LavaDisp()
{
    graphic_mode = GraphicInvalid;
    fb_disp = 0;

    framebufferClear();
    framebufferSwap();
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
    framebufferFill(0);
}

#if LAVA_DOUBLE_BUFFER
void LavaDisp::framebufferSwap()
{
    uint32_t fb_active = FB_ACTIVE;
    std::copy(&framebuffer[fb_active][0][0],
              &framebuffer[fb_active][0][0] + sizeof(framebuffer[0]),
              &framebuffer[fb_disp][0][0]);
    fb_disp = fb_active;
}
#endif
