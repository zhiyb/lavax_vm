#include <dirent.h>
#include <stdio.h>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>

#include <unistd.h>
#define TRACE(fmt,...) printf("%s:%d " fmt "\n", __PRETTY_FUNCTION__, __LINE__, ## __VA_ARGS__)

#include <switch.h>

// Lava rom souce
#define LAV_FILE    "LAVA/魔塔.lav"
// #define LAV_FILE    "LAVA/英雄坛说.lav"

// Data files
#define SDMC_BASE   "sdmc:/lava/"
#define ROMFS_BASE  "romfs:/"

// Lava shit
#include "lava.h"
#include "lava_app.h"

static Lava lava;

class Callback: public LavaCallback
{
public:
    virtual int32_t delayMs(uint32_t delay);
    virtual int32_t getMs();

    virtual void exit(uint32_t code);

    virtual int32_t getchar();
    virtual int32_t checkKey(uint8_t key);
    virtual int32_t inKey();
    virtual void releaseKey(uint8_t key);

    void setDebug(int key) {debug = key;}
    virtual int32_t debugContinue();

    virtual void refresh(const uint8_t *framebuffer);

    virtual uint8_t fopen(std::string path, std::string mode);
    virtual void fclose(uint8_t fd);
    virtual std::vector<uint8_t> fread(uint8_t fd, uint32_t size);
    virtual int32_t fwrite(uint8_t fd, const std::vector<uint8_t> &data);
    virtual int32_t fseek(uint8_t fd, int32_t ofs, fseek_mode_t mode);
    virtual int32_t ftell(uint8_t fd);
    virtual bool frestore(uint8_t fd, std::string path, std::string mode, int32_t offset);
    virtual int32_t remove(std::string path);

    virtual time_t getTime();

private:
    bool delay_pending = false;
    u64 delay_tick = 0;
    u32 ms = 0;
    u64 ms_tick = 0;
    int debug = -1;
};

static Callback cb;

struct file_t {
    std::string fpath;
    FILE *f;
};

static struct lava_state_t {
    std::queue<Lava::key_t> key_seq;
    std::unordered_set<Lava::key_t> key_state;
    std::unordered_map<int, file_t> file_map;
    std::string save;

    bool run = false;
    uint32_t ret_code = 0;

    bool refresh = false;
    const uint8_t *fb = 0;

    bool key_check = false;
    u64 pad_state;
} lava_state;

static std::unordered_map<HidNpadButton, Lava::key_t> key_map = {
    {HidNpadButton_Up,      Lava::KeyUp},
    {HidNpadButton_Down,    Lava::KeyDown},
    {HidNpadButton_Left,    Lava::KeyLeft},
    {HidNpadButton_Right,   Lava::KeyRight},
    {HidNpadButton_L,       Lava::KeyPageUp},
    {HidNpadButton_ZL,      Lava::KeyPageDown},
    {HidNpadButton_ZR,      Lava::KeyF3},
    {HidNpadButton_R,       Lava::KeyF4},
    {HidNpadButton_Y,       Lava::KeyHelp},
    {HidNpadButton_B,       Lava::KeyEsc},
    {HidNpadButton_A,       Lava::KeyEnter},
    {HidNpadButton_X,       Lava::KeySpace},
};

static std::vector<std::string> str_split(std::string s, char sp)
{
    std::vector<std::string> split;
    for (;;) {
        size_t ofs = s.find(sp);
        if (ofs == std::string::npos) {
            if (s.size())
                split.push_back(s);
            break;
        }
        if (ofs != 0)
            split.push_back(s.substr(0, ofs));
        s = s.substr(ofs + 1);
    }
    return split;
}

static std::vector<u8> lava_freadall(FILE *f)
{
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::vector<u8> data(size);
    fread(data.data(), 1, size, f);
    return data;
}

static FILE *lava_fopen(std::string fpath, std::string fmode)
{
    // Sanitise file mode
    // Always use binary mode to avoid issues with line return characters
    std::string mode = "b";
    bool write = false;
    for (auto c: fmode) {
        switch (c) {
        case 'w':
            write = true;
            // fall-through
        case 'r':
            mode = c + mode;
            break;
        case '+':
            write = true;
            mode.push_back(c);
            break;
        }
    }

    // Remove duplicated slashes
    auto path_split = str_split(fpath, '/');
    fpath = std::string();
    for (auto &split: path_split) {
        if (fpath.size())
            fpath.push_back('/');
        fpath += split;
    }
    // TRACE("fpath = %s", fpath.data());

    // Try SDMC first
    std::string path = SDMC_BASE + fpath;
    FILE *f = fopen(path.data(), mode.data());
    if (f)
        return f;
    // TRACE("Failed to open: %s mode %s", path.data(), mode.data());

    // Try ROMFS next
    path = ROMFS_BASE + fpath;
    if (write) {
        // If file exists in ROMFS, copy to SDMC first so that it can be opened for write
        f = fopen(path.data(), "rb");
        if (f) {
            auto data = lava_freadall(f);
            fclose(f);

            // Create parent directories
            std::string dpath;
            for (u32 i = 0; i < path_split.size() - 1; i++) {
                if (dpath.size())
                    dpath.push_back('/');
                dpath += path_split[i];
                std::string path(SDMC_BASE + dpath);
                // TRACE("mkdir(%s)", path.data());
                mkdir(path.data(), 0755);
            }

            path = SDMC_BASE + fpath;
            f = fopen(path.data(), "wb");
            if (f) {
                fwrite(data.data(), 1, data.size(), f);
                fclose(f);
            }
        }
    }
    f = fopen(path.data(), mode.data());
    if (f)
        return f;
    // TRACE("Failed to open: %s mode %s", path.data(), mode.data());

    TRACE("Failed to open: %s mode %s", fpath.data(), mode.data());
    return f;
}

int lava_init()
{
    lava.setCallbacks(&cb);

    // Make sure sdmc directory exists
    mkdir(SDMC_BASE, 0755);

    // Load LVM.bin
    FILE *f = lava_fopen("LVM.bin", "rb");
    if (!f)
        return -1;
    auto lvm_bin_data = lava_freadall(f);
    fclose(f);
    lava.loadLvmBin(lvm_bin_data);

    // Load .lav file
    f = lava_fopen(LAV_FILE, "rb");
    if (!f)
        return -1;
    auto lav_data = lava_freadall(f);
    TRACE("%s: Read %ld bytes", LAV_FILE, lav_data.size());
    lava.load(lav_data);

    lava_state.run = true;
    lava_state.refresh = false; //true
    TRACE("Loaded");
    return 0;
}

lava_app_op_t lava_loop()
{
    if (!lava_state.run)
        return LavaAppStopped; //lava_state.ret_code;

    lava.run();

    if (lava_state.refresh) {
        lava_state.refresh = false;
        return LavaAppRefresh;
    }

    if (lava_state.key_check) {
        lava_state.key_check = false;
        return LavaAppCheckKey;
    }

    return LavaAppNop;
}

Lava &lava_inst()
{
    return lava;
}

void lava_update_pad(u64 key)
{
    if (lava_state.pad_state == key)
        return;

    u64 update = lava_state.pad_state ^ key;
    lava_state.pad_state = key;
    for (u64 mask = 1; update != 0; mask <<= 1) {
        if (update & mask) {
            update &= ~mask;
            auto it = key_map.find((HidNpadButton)mask);
            if (it != key_map.end()) {
                Lava::key_t k = it->second;
                if (key & mask) {
                    // Button pressed
                    // TRACE("Pressed: 0x%018lx", mask);
                    lava_state.key_seq.push(k);
                    lava_state.key_state.insert(k);
                } else {
                    // Button released
                    // TRACE("Released: 0x%018lx", mask);
                    lava_state.key_state.erase(k);
                }
            }
        }
    }
}

int32_t Callback::delayMs(uint32_t delay)
{
    // TRACE("%u, %lu", delay, delay_tick);
    if (!delay_pending) {
        delay_tick = armGetSystemTick();
        delay_pending = true;
        return -1;
    }

    s64 delta = armGetSystemTick() - delay_tick;
    if (armTicksToNs(delta) < delay * 1000000)
        return -1;

    delay_pending = false;
    // TRACE("%u, %lu", delay, armTicksToNs(delta));
    return 0;
}

int32_t Callback::getMs()
{
    u64 tick = armGetSystemTick();
    u64 delta = tick - ms_tick;
    u64 ms_delta = armTicksToNs(delta) / 1000000;
    ms_tick += armNsToTicks(ms_delta * 1000000);
    ms += ms_delta;
    // TRACE("%d", ms);
    return ms;
}

void Callback::exit(uint32_t code)
{
    TRACE();
    lava_state.ret_code = code;
    lava_state.run = false;
}

int32_t Callback::getchar()
{
    // TRACE();
    if (lava_state.key_seq.empty()) {
        lava_state.key_check = true;
        return -1;  // -1 tells LAVA processor to stall waiting
    }
    int v = lava_state.key_seq.front();
    lava_state.key_seq.pop();
    return v;
}

int32_t Callback::checkKey(uint8_t key)
{
    // TRACE();
    lava_state.key_check = true;
    if (key >= 0x80) {
        if (lava_state.key_state.empty())
            return Lava::False;
        return *lava_state.key_state.cbegin();
    }

    for (auto const &k: lava_state.key_state)
        if (k == key)
            return Lava::True;
    return Lava::False;
}

int32_t Callback::inKey()
{
    TRACE();
    int32_t v = getchar();
    if (v < 0)
        return 0;
    return v;
}

void Callback::releaseKey(uint8_t key)
{
    TRACE();
    lava_state.key_check = true;
    // If the specified key is currently pressed, generate a new key event
    for (auto const &k: lava_state.key_state) {
        if (k == key) {
            lava_state.key_seq.push(k);
            return;
        }
    }
}

int32_t Callback::debugContinue()
{
    TRACE();
    int dbg = debug;
    debug = -1;
    return dbg;
}

void Callback::refresh(const uint8_t *framebuffer)
{
    // TRACE();
    lava_state.fb = framebuffer;
    lava_state.refresh = true;
}

uint8_t Callback::fopen(std::string path, std::string mode)
{
    TRACE("%s, %s", path.c_str(), mode.c_str());

    // Find next free file descriptor
    int fd = 0;
    for (fd = 0x80; fd <= 0xff; fd++)
        if (lava_state.file_map.find(fd) == lava_state.file_map.end())
            break;
    if (fd > 0xff)
        return 0;   // No available file descriptor

    auto &fdata = lava_state.file_map[fd];
    FILE *f = lava_fopen(path, mode);
    if (!f) {
        lava_state.file_map.erase(fd);
        return 0;
    }

    fdata.fpath = path;
    fdata.f = f;

    TRACE("Opened file: %s, mode %s", path.data(), mode.data());
    return fd;
}

void Callback::fclose(uint8_t fd)
{
    TRACE("%d", (int)fd);
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return;
    ::fclose(it->second.f);
    lava_state.file_map.erase(fd);
}

std::vector<uint8_t> Callback::fread(uint8_t fd, uint32_t size)
{
    // TRACE("%d, %u", (int)fd, size);
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return std::vector<uint8_t>();
    auto &fdata = it->second;

    std::vector<uint8_t> data(size);
    data.resize(::fread(data.data(), 1, size, fdata.f));
    return data;
}

int32_t Callback::fwrite(uint8_t fd, const std::vector<uint8_t> &data)
{
    // TRACE("%d, %lu", (int)fd, data.size());
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return 0;
    return ::fwrite(data.data(), 1, data.size(), it->second.f);
}

int32_t Callback::fseek(uint8_t fd, int32_t ofs, fseek_mode_t mode)
{
    // TRACE();
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return -1;
    int way = SEEK_SET;
    if (mode == SeekCur)
        way = SEEK_CUR;
    else if (mode == SeekEnd)
        way = SEEK_END;
    // If seek failed, skip operation, and restore current offset
    auto &fdata = it->second;
    size_t pos = ::ftell(fdata.f);
    if (::fseek(fdata.f, ofs, way)) {
        ::fseek(fdata.f, pos, SEEK_SET);
        return -1;
    }
    return 0;
}

int32_t Callback::ftell(uint8_t fd)
{
    // TRACE();
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return -1;
    return ::ftell(it->second.f);
}

bool Callback::frestore(uint8_t fd, std::string path, std::string mode, int32_t offset)
{
    TRACE();
    return false;
}

int32_t Callback::remove(std::string path)
{
    TRACE();
    // Return 0 for failure
    return 1;
}

LavaCallback::time_t Callback::getTime()
{
    TRACE();
    return time_t();
}
