#pragma once

#include <string>
#include <cstdint>

#include "lava_cfg.h"
#include "lava_disp.h"
#include "lava_proc.h"

// Callback functions to be implemented by platform
class LavaCallback {
public:
    // Return -1 to wait until next lava.run()
    virtual int32_t delay_ms(uint32_t delay) = 0;
    virtual int32_t get_ms() = 0;

    virtual void exit(uint32_t code) = 0;

    // Return -1 to wait until next lava.run()
    virtual int32_t getchar() = 0;

    virtual int32_t check_key(uint8_t key) = 0;
    virtual int32_t in_key() = 0;

    virtual void refresh(uint8_t *framebuffer) = 0;

    enum fseek_mode_t {
        SeekSet = 0,
        SeekCur = 1,
        SeekEnd = 2,
    };

    virtual uint8_t fopen(std::string path, std::string mode) = 0;
    virtual void fclose(uint8_t fd) = 0;
    virtual std::vector<uint8_t> fread(uint8_t fd, uint32_t size) = 0;
    virtual int32_t fwrite(uint8_t fd, const std::vector<uint8_t> &data) = 0;
    virtual int32_t fseek(uint8_t fd, int32_t ofs, fseek_mode_t mode) = 0;
};

class Lava
{
public:
    enum bool_t {
        True  = 0xffffffff,
        False = 0x00000000,
    };

    enum key_t {
        KeyUp    = 20,
        KeyDown  = 21,
        KeyRight = 22,
        KeyLeft  = 23,
        KeyF1    = 28,
        KeyF2    = 29,
        KeyF3    = 30,
        KeyF4    = 31,
        KeyEnter = 13,
        KeySpace = 20,
        KeyHelp  = 25,
        KeyEsc   = 27,
    };

    Lava();
    void setCallbacks(LavaCallback *cb) {proc.setCallbacks(cb);}

    void load(const std::vector<uint8_t> &source);
    void loadLvmBin(const std::vector<uint8_t> &data) {disp.loadLvmBin(data);}

    void reset();
    void run() {proc.run();}

    const std::vector<uint8_t> &inspectRam() {return proc.inspectRam();}

    LavaDisp::mode_t getGraphicMode() {return disp.getMode();}
    uint8_t *getFramebuffer() {return disp.getFramebuffer();}
    void framebufferSwap() {disp.framebufferSwap();}
    uint16_t getFramebufferWidth() {return disp.getFramebufferWidth();}
    uint16_t getFramebufferHeight() {return disp.getFramebufferHeight();}
    uint16_t getFramebufferStride() {return disp.getFramebufferStride();}

private:
    LavaDisp disp;
    LavaProc proc;
};
