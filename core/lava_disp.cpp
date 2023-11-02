#include <algorithm>
#include <iostream>

#include "lava_disp.h"

#if LAVA_DOUBLE_BUFFER
#define FB_ACTIVE   (1 - fb_disp)
#else
#define FB_ACTIVE   (0)
#endif

#define LCD_WIDTH  320
#define LCD_HEIGHT 240
#define SCROLL_CON (LCD_WIDTH * (LCD_HEIGHT + 8))

LavaDisp::LavaDisp()
{
    graphic_mode = GraphicInvalid;
    fb_disp = 0;
    bg_colour = 0;
    fg_colour = 1;
    refresh = 0;

    framebufferClear();
    framebufferSwap();
}

void LavaDisp::clearScreen()
{
    framebufferClear();
    framebufferSwap();
}

void LavaDisp::drawHLine(uint16_t x0, uint16_t x1, uint16_t y0, uint8_t cmd, uint8_t no_buf)
{
    // Assert: y0 < height, x0 < width, x1 < width, x0 <= x1
    uint32_t fb_active = FB_ACTIVE;
    //std::cerr << x0 << ", " << x1 << ", " << y0 << ", " << (uint32_t)cmd << ", " << (uint32_t)no_buf << std::endl;

	//uint8_t *p;
	uint16_t yy = y0;
	uint16_t xx = x0;

    uint8_t bgc = 0;
    uint8_t fgc = 1;
    uint8_t mask = 1;

    switch (graphic_mode) {
    case Graphic4:
        bgc = bg_colour;
        fgc = fg_colour;
        mask = 0x0f;
        break;
    case Graphic256:
        bgc = bg_colour;
        fgc = fg_colour;
        mask = 0xff;
        break;
    }

	//uint16_t width = x1 - x0 + 1;
    switch (cmd) {
    case 0: // Fill with background colour
        std::fill(&framebuffer[fb_active][y0][x0],
                  &framebuffer[fb_active][y0][x1 + 1],
                  bgc);
        break;
    case 1: // Fill with foreground colour
        std::fill(&framebuffer[fb_active][y0][x0],
                  &framebuffer[fb_active][y0][x1 + 1],
                  fgc);
        break;
    case 2: // Invert colours
        std::transform(&framebuffer[fb_active][y0][x0],
                       &framebuffer[fb_active][y0][x1 + 1],
                       &framebuffer[fb_active][y0][x0],
                       [](uint8_t c) {return c ^ 0xff;});
        break;
    }
}

void LavaDisp::drawBlock(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint8_t cmd, uint8_t no_buf)
{
	if (y0 > y1)
        std::swap(y0, y1);
	if (x0 > x1)
        std::swap(x0, x1);

    x0 = std::min(x0, (uint16_t)(width - 1));
    x1 = std::min(x1, (uint16_t)(width - 1));
    y0 = std::min(y0, (uint16_t)(height - 1));
    y1 = std::min(y1, (uint16_t)(height - 1));

	uint16_t t;
	t = y1 - y0 + 1;
	while (t) {
        drawHLine(x0, x1, y0, cmd, no_buf);
		y0++;
		t--;
	}

    //framebufferSwap();
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
