// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#define TRACE(fmt,...) printf("%s:%d " fmt "\n", __PRETTY_FUNCTION__, __LINE__, ## __VA_ARGS__)

// Include the main libnx system header, for Switch development
#include <switch.h>

// Lava shit
#include "lava_app.h"

// See also libnx display/framebuffer.h.

// Define the desired framebuffer resolution (here we set it to 720p).
// #define FB_WIDTH  1280
// #define FB_HEIGHT 720

// Remove above and uncomment below for 1080p
#define FB_WIDTH  1920
#define FB_HEIGHT 1080

// Main program entrypoint
int main(int argc, char* argv[])
{
    // Retrieve the default window
    NWindow* win = nwindowGetDefault();

    // Create a linear double-buffered framebuffer
    Framebuffer fb;
    framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
    framebufferMakeLinear(&fb);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    Result rc = romfsInit();
    if (R_FAILED(rc))
        TRACE("romfsInit error: %08X", rc);
    else
        TRACE("romfs init successful");

    bool fbinit = true;
    int err = lava_init();

    // Main loop
    while (appletMainLoop()) {
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        u64 kDown = padGetButtons(&pad);
        if (kDown & HidNpadButton_Plus)
            break; // break in order to return to hbmenu

        // LAVA shit
        if (err)
            break;

        lava_update_pad(kDown);

        for (;;) {
            lava_app_op_t op = lava_loop();
            if (op == LavaAppNop) {
                continue;
            } else if (op == LavaAppStopped) {
                err = 1;
                break;
            }
            break;
        }

        // Retrieve the framebuffer
        u32 stride;
        u32* framebuf = (u32*) framebufferBegin(&fb, &stride);

        // Update framebuffer from Lava
        if (fbinit) {
            for (u32 y = 0; y < FB_HEIGHT; y ++)
                for (u32 x = 0; x < FB_WIDTH; x ++)
                    framebuf[y * stride / sizeof(u32) + x] = 0xffffffff;
            fbinit = false;
        }

        auto &lava = lava_inst();
        u32 sw = lava.getFramebufferWidth();
        u32 sh = lava.getFramebufferHeight();
        u32 ss = lava.getFramebufferStride();
        u32 scale = std::max(1u, std::min(FB_WIDTH / sw, FB_HEIGHT / sh));
        u32 ox = std::max(0u, (FB_WIDTH - scale * sw) / 2);
        u32 oy = std::max(0u, (FB_HEIGHT - scale * sh) / 2);
        // TRACE("%dx%d %d %dx%d", sw, sh, scale, ox, oy);

        static const u32 palette_2[2] = {0xffffffff, 0xff000000};

        static const u32 palette_16[16] = {
            0xffffffff, 0xffeeeeee, 0xffdddddd, 0xffcccccc,
            0xffbbbbbb, 0xffaaaaaa, 0xff999999, 0xff888888,
            0xff777777, 0xff666666, 0xff555555, 0xff444444,
            0xff333333, 0xff222222, 0xff111111, 0xff000000,
        };

        const u32 *palette = &palette_16[0];
        if (lava.getGraphicMode() == LavaDisp::GraphicMono) {
            palette = &palette_2[0];
        } else if (lava.getGraphicMode() == LavaDisp::Graphic16) {
            palette = &palette_16[0];
        } else {
            TRACE("Unknown graphics mode: %d", (int)lava.getGraphicMode());
            err = -1;
        }


        for (u32 y = 0; y < scale * sh; y++) {
            u32 sy = y / scale;
            for (u32 x = 0; x < scale * sw; x++) {
                u32 sx = x / scale;
                u32 fy = oy + y;
                u32 fx = ox + x;
                u32 c = lava.getFramebuffer()[sy * ss + sx];
                framebuf[fy * stride / sizeof(u32) + fx] = palette[c];
            }
        }

        // We're done rendering, so we end the frame here.
        framebufferEnd(&fb);
    }

    framebufferClose(&fb);
    return 0;
}
