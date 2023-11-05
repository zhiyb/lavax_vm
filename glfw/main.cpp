#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "lava.h"

#define SAVE_STATE_FILE "lava.sav"

// Debug features

// Press F8 key to reset
#define RESET_KEY_F8        1
// Shorter delay for debugging
#define DEBUG_SHORT_DELAY   0

// Lava stuff

static Lava lava;

static struct lava_state_t {
    bool run = true;
    uint32_t ret_code = 0;
    std::queue<int> key_seq;
    std::unordered_set<int> key_state;
    std::unordered_map<int, std::fstream> file_map;
    std::string save;
} lava_state;

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
    double tick = 0;
};

static Callback cb;

static void key_map_init();

static int lava_init(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    key_map_init();
    lava.setCallbacks(&cb);

    // Load LVM.bin
    std::ifstream f_lvm_bin;
    f_lvm_bin.open("LVM.bin", std::ios::binary | std::ios::in);
    if (!f_lvm_bin)
        return 1;

    std::vector<uint8_t> f_lvm_data(1 * 1024 * 1024);
    f_lvm_data.resize(f_lvm_bin.rdbuf()->sgetn(reinterpret_cast<char *>(f_lvm_data.data()), f_lvm_data.size()));
    std::cout << "Read " << f_lvm_data.size() << " bytes" << std::endl;
    lava.loadLvmBin(f_lvm_data);

    // Load .lav file
    std::ifstream flava;
    flava.open(argv[1], std::ios::binary | std::ios::in);
    if (!flava)
        return 1;

    std::vector<uint8_t> fdata(1 * 1024 * 1024);
    fdata.resize(flava.rdbuf()->sgetn(reinterpret_cast<char *>(fdata.data()), fdata.size()));
    std::cout << "Read " << fdata.size() << " bytes" << std::endl;

    try {
        lava.load(fdata);

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;

        lava_state.run = false;
        lava_state.ret_code = -1;
    }

    return 0;
}

static void lava_reset()
{
    std::cerr << "LAVA reset" << std::endl;
    lava.reset();
    while (!lava_state.key_seq.empty())
        lava_state.key_seq.pop();
    lava_state.key_state.clear();
    lava_state.file_map.clear();
    lava_state.run = true;
}

void lava_dump()
{
    // Dump RAM content
    auto const &ram = lava.inspectRam();
    std::ofstream f_ram_dump;
    f_ram_dump.open("ram.bin", std::ios::binary | std::ios::out);
    f_ram_dump.write(reinterpret_cast<const char *>(ram.data()), ram.size());
    f_ram_dump.close();
    std::cerr << "RAM dump: ram.bin" << std::endl;

    // Dump both frame buffers
    std::ofstream f_fb_dump;
    f_fb_dump.open("fb.bin", std::ios::binary | std::ios::out);
    f_fb_dump.write(reinterpret_cast<const char *>(lava.getFramebuffer()),
                    lava.getFramebufferStride() * lava.getFramebufferHeight());
    f_fb_dump.write(reinterpret_cast<const char *>(lava.getWorkingFramebuffer()),
                    lava.getFramebufferStride() * lava.getFramebufferHeight());
    f_fb_dump.close();
    std::cerr << "FB dump: fb.bin" << std::endl;

    cb.refresh(lava.getFramebuffer());
}

static void lava_save_state()
{
    std::ostringstream ss;
    lava.saveState(ss);
    lava_state.save = ss.str();
    std::ofstream fs(SAVE_STATE_FILE, std::ios::binary | std::ios::out | std::ios::trunc);
    fs.write(lava_state.save.data(), lava_state.save.size());
    fs.close();
    std::cerr << "State saved: " << SAVE_STATE_FILE << std::endl;
}

static void lava_restore_state()
{
    if (lava_state.save.empty()) {
        std::ifstream fs(SAVE_STATE_FILE, std::ios::binary | std::ios::in);
        if (!fs) {
            std::cerr << "Save state file not found: " << SAVE_STATE_FILE << std::endl;
            return;
        }
        lava_state.save.resize(1 * 1024 * 1024);
        lava_state.save.resize(fs.rdbuf()->sgetn(lava_state.save.data(), lava_state.save.size()));
    }
    std::istringstream ss(lava_state.save);
    lava.restoreState(ss);
    std::cerr << "State restored" << std::endl;
}

// LAVA miscellaneous

int32_t Callback::delayMs(uint32_t delay)
{
    if (!delay_pending) {
        tick = glfwGetTime();
        delay_pending = true;
        return -1;
    }

    double delta = glfwGetTime() - tick;
#if DEBUG_SHORT_DELAY
    if (delta * 1000. * 10. < delay)
        return -1;
#else
    if (delta * 1000. < delay)
        return -1;
#endif

    delay_pending = false;
    return 0;
}

int32_t Callback::getMs()
{
    return glfwGetTime() * 1000.;
}

LavaCallback::time_t Callback::getTime()
{
    std::cerr << __func__ << std::endl;
    return time_t();
}

void Callback::exit(uint32_t code)
{
    lava_state.ret_code = code;
    lava_state.run = false;
    lava_dump();
}

// LAVA file operations

uint8_t Callback::fopen(std::string path, std::string mode)
{
    std::filesystem::path fpath(".");
    fpath += path;
    //std::cerr << __func__ << "(" << fpath << ", \"" << mode << "\")" << std::endl;
    std::ios::openmode fmode = 0;
    // Always use binary mode to avoid issues with line return characters
    fmode |= std::ios::binary;
    for (auto c: mode) {
        switch (c) {
        case 'r':
            fmode |= std::ios::in;
            break;
        case 'w':
            fmode |= std::ios::out | std::ios::trunc;
            break;
        case '+':
            fmode |= std::ios::in | std::ios::out;
            break;
        case 'b':
            fmode |= std::ios::binary;
            break;
        default:
            throw std::runtime_error("Unknown file mode: \"" + mode + "\": " + path);
            break;
        }
    }
    for (int i = 1; i <= 255; i++) {
        if (lava_state.file_map.find(i) == lava_state.file_map.end()) {
            auto &fstream = lava_state.file_map[i];
            fstream.open(fpath, fmode);
            if (fstream)
                return i;
            else
                return 0;
        }
    }
    return 0;
}

std::vector<uint8_t> Callback::fread(uint8_t fd, uint32_t size)
{
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return std::vector<uint8_t>();
    std::vector<uint8_t> data;
    data.resize(size);
    data.resize(it->second.rdbuf()->sgetn(reinterpret_cast<char *>(data.data()), size));
    return data;
}

int32_t Callback::fwrite(uint8_t fd, const std::vector<uint8_t> &data)
{
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return 0;
    it->second.write(reinterpret_cast<const char *>(data.data()), data.size());
    return data.size();
}

void Callback::fclose(uint8_t fd)
{
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return;
    it->second.close();
    lava_state.file_map.erase(fd);
}

int32_t Callback::fseek(uint8_t fd, int32_t ofs, fseek_mode_t mode)
{
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return -1;
    auto &fstream = it->second;
    std::ios::seekdir way = std::ios::beg;
    if (mode == SeekCur)
        way = std::ios::cur;
    else if (mode == SeekEnd)
        way = std::ios::end;
    return fstream.seekg(ofs, way) ? 0 : -1;
}

int32_t Callback::ftell(uint8_t fd)
{
    auto it = lava_state.file_map.find(fd);
    if (it == lava_state.file_map.end())
        return -1;
    auto &fstream = it->second;
    return fstream.tellg();
}

bool Callback::frestore(uint8_t fd, std::string path, std::string mode, int32_t offset)
{
    std::filesystem::path fpath(".");
    fpath += path;
    //std::cerr << __func__ << "(\"" << path << "\", \"" << mode << "\", " << offset << ")" << std::endl;
    std::ios::openmode fmode = 0;
    // Always use binary mode to avoid issues with line return characters
    fmode |= std::ios::binary;
    for (auto c: mode) {
        switch (c) {
        case 'r':
            fmode |= std::ios::in;
            break;
        case 'w':
            // No trunc when restoring file state
            fmode |= std::ios::out /*| std::ios::trunc*/;
            break;
        case '+':
            fmode |= std::ios::in | std::ios::out;
            break;
        case 'b':
            fmode |= std::ios::binary;
            break;
        default:
            throw std::runtime_error("Unknown file mode: \"" + mode + "\": " + path);
            break;
        }
    }

    // File map should have been erased by fclose when restoring
    // Safe to override without checking
    auto &fstream = lava_state.file_map[fd];
    fstream.open(fpath, fmode);
    if (!fstream)
        return false;
    fstream.seekg(offset, std::ios::beg);
    return true;
}

int32_t Callback::remove(std::string path)
{
    std::cerr << __func__ << ": " << path << std::endl;
    // Return 0 for failure
    return 1;
}

// LAVA keyboard operations

static std::unordered_map<uint32_t, uint8_t> key_map = {
    {GLFW_KEY_UP,       Lava::KeyUp},
    {GLFW_KEY_DOWN,     Lava::KeyDown},
    {GLFW_KEY_LEFT,     Lava::KeyLeft},
    {GLFW_KEY_RIGHT,    Lava::KeyRight},
    {GLFW_KEY_F1,       Lava::KeyF1},
    {GLFW_KEY_F2,       Lava::KeyF2},
    {GLFW_KEY_F3,       Lava::KeyF3},
    {GLFW_KEY_F4,       Lava::KeyF4},
    {GLFW_KEY_F5,       Lava::KeyHelp},
    {GLFW_KEY_F8,       Lava::KeyHelp},
    {GLFW_KEY_ESCAPE,   Lava::KeyEsc},
    {GLFW_KEY_ENTER,    Lava::KeyEnter},
    {GLFW_KEY_SPACE,    Lava::KeySpace},
};

static void key_map_init()
{
    for (int c = GLFW_KEY_A; c <= GLFW_KEY_Z; c++)
        key_map[c] = 'a' + c - GLFW_KEY_A;
    for (int c = GLFW_KEY_0; c <= GLFW_KEY_9; c++)
        key_map[c] = '0' + c - GLFW_KEY_0;
}

static uint8_t keycode_conv(int key)
{
    auto const &k = key_map.find(key);
    if (k == key_map.cend()) {
        std::cerr << "Key code not found: " << key << std::endl;
        return 0;
    }
    std::cerr << "Key code found: " << key << " -> " << (uint32_t)k->second << std::endl;
    return k->second;
}

static void refresh_working_fb();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        // Special functions
        if (key == GLFW_KEY_DELETE)
            lava_reset();
        else if (key == GLFW_KEY_F9)
            lava_save_state();
        else if (key == GLFW_KEY_F10)
            lava_restore_state();
        else if (key == GLFW_KEY_F11)
            refresh_working_fb();
        else if (key == GLFW_KEY_F12)
            lava_dump();

        // Lava key code conversion
        uint8_t k = keycode_conv(key);
        if (k == 0)
            return;
        lava_state.key_seq.push(k);
        lava_state.key_state.insert(k);

    } else if (action == GLFW_RELEASE) {
        uint8_t k = keycode_conv(key);
        if (k == 0)
            return;
        lava_state.key_state.erase(k);
    }
}

int32_t Callback::getchar()
{
    if (lava_state.key_seq.empty())
        return -1;  // -1 tells LAVA processor to stall waiting
    int v = lava_state.key_seq.front();
    lava_state.key_seq.pop();
    return v;
}

int32_t Callback::checkKey(uint8_t key)
{
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
    int32_t v = getchar();
    if (v < 0)
        return 0;
    return v;
}

void Callback::releaseKey(uint8_t key)
{
    // If the specified key is currently pressed, generate a new key event
    for (auto const &k: lava_state.key_state) {
        if (k == key) {
            lava_state.key_seq.push(k);
            return;
        }
    }
}

void joystick_process()
{
    const float axis_threshold = 0.7;

    static const std::vector<uint8_t> btn_map = {
        // A B X Y L R Menu_L Menu_R L3 R3
        Lava::KeyEnter, Lava::KeyEsc, Lava::KeyF3, Lava::KeyF4,
        Lava::KeyF1, Lava::KeyF2, Lava::KeyHelp, Lava::KeySpace,
    };

    // Joystick debug dump buttons
    bool debug = false;
    // Joystick key states
    std::unordered_set<uint8_t> keys;

    for (int js = GLFW_JOYSTICK_1; js <= GLFW_JOYSTICK_LAST; js++) {
        if (!glfwJoystickPresent(js))
            continue;

        int naxis = 0;
        const float *paxis = glfwGetJoystickAxes(js, &naxis);
        for (int i = 0; i < std::min(4, naxis); i++) {
            float v = paxis[i];
            if (v >= axis_threshold)
                keys.insert(i % 2 ? Lava::KeyDown : Lava::KeyRight);
            if (v <= -axis_threshold)
                keys.insert(i % 2 ? Lava::KeyUp : Lava::KeyLeft);
        }

        int nhats = 0;
        const uint8_t *phat = glfwGetJoystickHats(js, &nhats);
        for (int i = 0; i < nhats; i++) {
            uint8_t v = phat[i];
            if (v & GLFW_HAT_UP)
                keys.insert(Lava::KeyUp);
            if (v & GLFW_HAT_DOWN)
                keys.insert(Lava::KeyDown);
            if (v & GLFW_HAT_LEFT)
                keys.insert(Lava::KeyLeft);
            if (v & GLFW_HAT_RIGHT)
                keys.insert(Lava::KeyRight);
        }

        int nbtns = 0;
        const uint8_t *pbtn = glfwGetJoystickButtons(js, &nbtns);
        for (int i = 0; i < nbtns; i++) {
            uint8_t v = pbtn[i];
            if (v && i < btn_map.size() && btn_map[i] != 0)
                keys.insert(btn_map[i]);
            if (v && i >= btn_map.size())
                debug = true;
        }
    }

    // Extract pressed and released keys
    static std::unordered_set<uint8_t> prev_keys;
    bool changed = false;
    for (auto const k: keys) {
        if (prev_keys.find(k) == prev_keys.cend()) {
            // New key pressed
            lava_state.key_seq.push(k);
            lava_state.key_state.insert(k);
            changed = true;
        }
    }
    for (auto const k: prev_keys) {
        if (keys.find(k) == keys.cend()) {
            // Key released
            lava_state.key_state.erase(k);
            changed = true;
        }
    }
    prev_keys = keys;

    // Trigger debug dumps
    static bool prev_debug = false;
    if (debug && !prev_debug)
        lava_dump();
    prev_debug = debug;
}

// LAVA main loop

static void lava_loop()
{
    try {
        if (lava_state.run)
            lava.run();

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        lava_dump();

        lava_state.run = false;
        lava_state.ret_code = -1;
    }
}


// OpenGL stuff

static struct {
    GLuint vao;
    GLuint program;
    struct {
        GLuint fb;
        GLuint palette;
    } texture;
    struct {
        GLint mode;
        GLint projection;
    } uniform;

    GLFWwindow *window;

    bool refresh = true;
    LavaDisp::mode_t mode = (LavaDisp::mode_t)0;    // Invalid mode
} gl_data;

static std::string gl_get_shader_info_log(GLuint shader)
{
    GLchar log[512];
    glGetShaderInfoLog(shader, sizeof(log), NULL, log);
    uint32_t slen = strnlen(log, sizeof(log));
    return std::string(log, slen);
}

static std::string gl_get_program_info_log(GLuint program)
{
    GLchar log[512];
    glGetProgramInfoLog(program, sizeof(log), NULL, log);
    uint32_t slen = strnlen(log, sizeof(log));
    return std::string(log, slen);
}

static GLuint gl_compile_shader(GLenum type, GLchar *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint ret;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
    if (ret != GL_TRUE)
        throw std::runtime_error("Failed to compile vertex shader: " + gl_get_shader_info_log(shader));
    return shader;
}

struct {
    std::vector<uint8_t> c2, c16, c256;
} palette;

static void load_palettes()
{
    // Default palettes
    palette.c2 = {
        0xff, 0xff, 0xff,
        0x00, 0x00, 0x00,
    };

    for (int i = 15; i >= 0; i--) {
        uint8_t c = i * 255 / 15;
        palette.c16.push_back(c);
        palette.c16.push_back(c);
        palette.c16.push_back(c);
    }

    for (int c = 255; c >= 0; c--) {
        palette.c256.push_back(c);
        palette.c256.push_back(c);
        palette.c256.push_back(c);
    }

    // Optional palette images
    int x = 0, y = 0;
    uint8_t *pdata = nullptr;

    pdata = stbi_load("palette2.png", &x, &y, nullptr, 3);
    if (pdata && x >= 2) {
        palette.c2 = std::vector<uint8_t>(pdata, pdata + 2 * 3);
        stbi_image_free(pdata);
    }

    pdata = stbi_load("palette16.png", &x, &y, nullptr, 3);
    if (pdata && x >= 16) {
        palette.c16 = std::vector<uint8_t>(pdata, pdata + 16 * 3);
        stbi_image_free(pdata);
    }

    pdata = stbi_load("palette256.png", &x, &y, nullptr, 3);
    if (pdata && x >= 256) {
        palette.c256 = std::vector<uint8_t>(pdata, pdata + 256 * 3);
        stbi_image_free(pdata);
    }
}

static void gl_init()
{
    load_palettes();

    // Shader program
    GLuint program = glCreateProgram();

    glAttachShader(program, gl_compile_shader(GL_VERTEX_SHADER, (GLchar *)R"(
#version 330 core

uniform mat4 projection;

layout (location = 0) in vec2 vPosition;
out vec2 vMap;

void main()
{
    vMap = vPosition;
    gl_Position = projection * vec4(vPosition, 0., 1.);
}
    )"));

    glAttachShader(program, gl_compile_shader(GL_FRAGMENT_SHADER, (GLchar *)R"(
#version 330 core

uniform uint mode;

uniform sampler2D tex;
uniform sampler1D palette;

in vec2 vMap;
out vec4 colour;

void main()
{
    float v = texture(tex, vMap).r;
    // Scale to palette texture pixel centre
    const float delta = 1. / 256.;
    v = v * (1. - delta) + delta / 2.;
    // Map palette
    vec3 plt = texture(palette, v).rgb;
    colour = vec4(plt.rgb, 1.);
}
    )"));

    glLinkProgram(program);
    GLint ret;
    glGetProgramiv(program, GL_LINK_STATUS, &ret);
    if (ret != GL_TRUE)
        throw std::runtime_error("Failed to link shader program: " + gl_get_program_info_log(program));

    glUseProgram(program);
    gl_data.program = program;

    // Vertex buffer for program
    glGenVertexArrays(1, &gl_data.vao);
    glBindVertexArray(gl_data.vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);

    glm::vec2 vertices[] = {
        glm::vec2(0., 0.),
        glm::vec2(1., 0.),
        glm::vec2(0., 1.),
        glm::vec2(1., 1.),
    };

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glNamedBufferData(buffer, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Textures and samplers
    GLuint tex[2];
    glGenTextures(2, &tex[0]);
    GLuint sampler[2];
    glGenSamplers(2, sampler);

    glSamplerParameteri(sampler[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindSampler(0, sampler[0]);

    // Texture: frame buffer
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glTextureStorage2D(tex[0], 1, GL_R8, lava.getFramebufferWidth(), lava.getFramebufferHeight());
    glPixelStorei(GL_UNPACK_ROW_LENGTH, lava.getFramebufferStride());
    glTextureParameteri(tex[0], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(tex[0], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureSubImage2D(tex[0], 0, 0, 0,
                        lava.getFramebufferWidth(), lava.getFramebufferHeight(),
                        GL_RED, GL_UNSIGNED_BYTE, lava.getFramebuffer());
    glUniform1i(glGetUniformLocation(program, "tex"), 0);
    gl_data.texture.fb = tex[0];

    // Texture: palette
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, tex[1]);
    glTextureStorage1D(tex[1], 1, GL_RGB8, 256);
    glTextureParameteri(tex[1], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureSubImage1D(tex[1], 0, 0, 256,
                        GL_RGB, GL_UNSIGNED_BYTE, palette.c256.data());
    glUniform1i(glGetUniformLocation(program, "palette"), 1);
    gl_data.texture.palette = tex[1];

    // Global uniform data
    gl_data.uniform.mode = glGetUniformLocation(program, "mode");
    gl_data.uniform.projection = glGetUniformLocation(program, "projection");
    glUniform1ui(gl_data.uniform.mode, 256);
    glUniformMatrix4fv(gl_data.uniform.projection, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // Clear screen
    glClearColor(0.0, 0.0, 0.0, 1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl_data.refresh = true;
}

void Callback::refresh(const uint8_t *framebuffer)
{
    if (lava.getGraphicMode() != gl_data.mode) {
        gl_data.mode = lava.getGraphicMode();
        uint8_t gl_mode = 1u << gl_data.mode;
        glUniform1ui(gl_data.uniform.mode, gl_mode);

        const uint8_t *plt = palette.c2.data();
        if (gl_mode == 16)
            plt = palette.c16.data();
        else if (gl_mode == 256)
            plt = palette.c256.data();

        GLuint tex = gl_data.texture.palette;
        glTextureSubImage1D(tex, 0, 0, gl_mode,
                            GL_RGB, GL_UNSIGNED_BYTE, plt);
    }

    glTextureSubImage2D(gl_data.texture.fb, 0,
                        0, 0, lava.getFramebufferWidth(), lava.getFramebufferHeight(),
                        GL_RED, GL_UNSIGNED_BYTE, lava.getFramebuffer());
    gl_data.refresh = true;
}

static void refresh_working_fb()
{
    glTextureSubImage2D(gl_data.texture.fb, 0,
                        0, 0, lava.getFramebufferWidth(), lava.getFramebufferHeight(),
                        GL_RED, GL_UNSIGNED_BYTE, lava.getWorkingFramebuffer());
    gl_data.refresh = true;
}

static void gl_refresh()
{
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glfwSwapBuffers(gl_data.window);
}

static void refresh_callback(GLFWwindow* window)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
    int lavaw = lava.getFramebufferWidth();
    int lavah = lava.getFramebufferHeight();
    int xscale = width / lavaw;
    int yscale = height / lavah;
    int scale = std::max(1, std::min(xscale, yscale));
    // Scaled width & height pixels
    int w = lavaw * scale;
    int h = lavah * scale;
    // Screen size in content space
    double dw = double(width) / double(w);
    double dh = double(height) / double(h);
    // x and y pixel offsets
    int idx = (width - w) / 2;
    int idy = (height - h) / 2;
    // Offsets in content space
    double dx = dw * double(idx) / double(width);
    double dy = dh * double(idy) / double(height);

    glViewport(0, 0, width, height);
    glm::mat4 projection = glm::ortho(-dx, dw - dx, dh - dy, -dy);
    glUniformMatrix4fv(gl_data.uniform.projection, 1, GL_FALSE, glm::value_ptr(projection));

    glClearColor(0.0, 0.0, 0.0, 1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_refresh();
}

void glfw_error(int error, const char *desc)
{
	std::cerr << "glfw error: " << error << std::endl;
	std::cerr << desc << std::endl;
}

int main(int argc, char *argv[])
{
    int e = lava_init(argc, argv);
    if (e != 0)
        return e;


    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
	if (!glfwInit()) {
		std::cerr << "glfwInit failed" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwSetErrorCallback(&glfw_error);

    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    uint32_t scnw = mode->width;
    uint32_t scnh = mode->height;
    uint32_t w = lava.getFramebufferWidth();
    uint32_t h = lava.getFramebufferHeight();
    uint32_t s = std::max(1u, std::min(scnw / w / 2, scnh / h / 2));
	GLFWwindow *window = glfwCreateWindow(w * s, h * s, "lava_glfw", nullptr, nullptr);
    gl_data.window = window;
	if (!window) {
		std::cerr << "Cannot create glfw Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//glewExperimental = GL_TRUE;
    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::cerr << glewGetErrorString(glew_err) << std::endl;
		glfwTerminate();
		return -1;
	}

    try {
        gl_init();
        refresh_callback(window);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

	glfwSetWindowRefreshCallback(window, &refresh_callback);
    glfwSetKeyCallback(window, &key_callback);

    double js_polling = glfwGetTime();
    double js_polling_rate = 1. / 50.;

	while (!glfwWindowShouldClose(window)) {
        double time = glfwGetTime();
        try {
            lava_loop();
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }

        if (time - js_polling >= js_polling_rate) {
            js_polling = time;
            joystick_process();
        }

        if (gl_data.refresh) {
            gl_data.refresh = false;
            gl_refresh();
        }
		glfwPollEvents();
    }

    return lava_state.ret_code;
}
