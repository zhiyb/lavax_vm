#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
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

// Debug features

// Press F8 key to reset
#define RESET_KEY_F8        1
// Shorter delay for debugging
#define DEBUG_SHORT_DELAY   0

// Lava stuff

static Lava lava;

static struct {
    bool run;
    uint32_t ret_code;
    std::queue<int> key_seq;
    std::unordered_set<int> key_state;
    std::unordered_map<int, std::fstream> file_map;
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

    virtual void refresh(uint8_t *framebuffer);

    virtual uint8_t fopen(std::string path, std::string mode);
    virtual void fclose(uint8_t fd);
    virtual std::vector<uint8_t> fread(uint8_t fd, uint32_t size);
    virtual int32_t fwrite(uint8_t fd, const std::vector<uint8_t> &data);
    virtual int32_t fseek(uint8_t fd, int32_t ofs, fseek_mode_t mode);
    virtual int32_t remove(std::string path);

    virtual time_t getTime();

private:
    bool delay_pending = false;
    int tick = -1;
};

static Callback cb;

static void key_map_init();

static int lava_init(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    key_map_init();
    lava.setCallbacks(&cb);
    lava_state.run = true;

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
        //glutLeaveMainLoop();
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

    // Force a framebuffer swap then dump
    //lava.framebufferSwap();
    auto fb = lava.getFramebuffer();
    std::ofstream f_fb_dump;
    f_fb_dump.open("fb.bin", std::ios::binary | std::ios::out);
    f_fb_dump.write(reinterpret_cast<const char *>(fb),
                    LAVA_MAX_HEIGHT * LAVA_MAX_WIDTH);
    f_fb_dump.close();
    std::cerr << "FB dump: fb.bin" << std::endl;
    cb.refresh(lava.getFramebuffer());
}

// LAVA miscellaneous

int32_t Callback::delayMs(uint32_t delay)
{
    if (!delay_pending) {
        tick = glutGet(GLUT_ELAPSED_TIME);
        delay_pending = true;
        return -1;
    }

    int delta = glutGet(GLUT_ELAPSED_TIME) - tick;
#if DEBUG_SHORT_DELAY
    if (delta * 10 < delay)
        return -1;
#else
    if (delta < delay)
        return -1;
#endif

    delay_pending = false;
    return 0;
}

int32_t Callback::getMs()
{
    return glutGet(GLUT_ELAPSED_TIME);
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
    glutLeaveMainLoop();
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

int32_t Callback::remove(std::string path)
{
    std::cerr << __func__ << ": " << path << std::endl;
    // Return 0 for failure
    return 1;
}

// LAVA keyboard operations

#define SPECIAL_KEY   0x8000
#define CONVERTED_KEY 0x4000

static std::unordered_map<int, uint8_t> key_map = {
    {SPECIAL_KEY | GLUT_KEY_UP,    Lava::KeyUp},
    {SPECIAL_KEY | GLUT_KEY_DOWN,  Lava::KeyDown},
    {SPECIAL_KEY | GLUT_KEY_LEFT,  Lava::KeyLeft},
    {SPECIAL_KEY | GLUT_KEY_RIGHT, Lava::KeyRight},
    {SPECIAL_KEY | GLUT_KEY_F1,    Lava::KeyF1},
    {SPECIAL_KEY | GLUT_KEY_F2,    Lava::KeyF2},
    {SPECIAL_KEY | GLUT_KEY_F3,    Lava::KeyF3},
    {SPECIAL_KEY | GLUT_KEY_F4,    Lava::KeyF4},
    {SPECIAL_KEY | GLUT_KEY_F5,    Lava::KeyHelp},
    //{SPECIAL_KEY | GLUT_KEY_F8,    Lava::KeyHelp},
    {27,                           Lava::KeyEsc},
    {0x0d,                         Lava::KeyEnter},
    {' ',                          Lava::KeySpace},
};

static void key_map_init()
{
    for (char c = 'a'; c <= 'z'; c++)
        key_map[c] = c;
    for (char c = 'A'; c <= 'Z'; c++)
        key_map[c] = c;
}

static void gl_keyboard_keys(uint8_t key, int x, int y)
{
    if (!lava_state.run)
        glutLeaveMainLoop();
    lava_state.key_seq.push(key);
    lava_state.key_state.insert(key);
}

static void gl_keyboard_keys_up(uint8_t key, int x, int y)
{
    lava_state.key_state.erase(key);
}

static void gl_special_keys(int key, int x, int y)
{
#if RESET_KEY_F8
    if (key == GLUT_KEY_F8) {
        lava_reset();
        return;
    }
#endif

    if (!lava_state.run)
        glutLeaveMainLoop();
    key |= SPECIAL_KEY;
    lava_state.key_seq.push(key);
    lava_state.key_state.insert(key);
}

static void gl_special_keys_up(int key, int x, int y)
{
    key |= SPECIAL_KEY;
    lava_state.key_state.erase(key);
}

static uint8_t keycode_conv(int key)
{
    if (key & CONVERTED_KEY)
        return key & 0xff;

    auto const &k = key_map.find(key);
    if (k == key_map.cend()) {
        std::cerr << "Key code not found: " << key << std::endl;
        return 0;
    }
    std::cerr << "Key code found: " << key << " -> " << (uint32_t)k->second << std::endl;
    return k->second;
}

int32_t Callback::getchar()
{
    if (lava_state.key_seq.empty())
        return -1;  // -1 tells LAVA processor to stall waiting
    int v = lava_state.key_seq.front();
    lava_state.key_seq.pop();
    return keycode_conv(v);
}

int32_t Callback::checkKey(uint8_t key)
{
    if (key >= 0x80) {
        if (lava_state.key_state.empty())
            return Lava::False;
        return keycode_conv(*lava_state.key_state.cbegin());
    }

    for (auto const &k: lava_state.key_state) {
        uint32_t kcode = keycode_conv(k);
        if (kcode == key)
            return Lava::True;
    }
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
        uint32_t kcode = keycode_conv(k);
        if (kcode == key) {
            lava_state.key_seq.push(k);
            return;
        }
    }
}

void gl_joystick(unsigned int buttonMask, int x, int y, int z)
{
    const int threshold = 800;
    std::unordered_set<uint8_t> keys;
    if (x >= threshold)
        keys.insert(Lava::KeyRight);
    else if (x <= -threshold)
        keys.insert(Lava::KeyLeft);
    if (y >= threshold)
        keys.insert(Lava::KeyDown);
    else if (y <= -threshold)
        keys.insert(Lava::KeyUp);

    if (buttonMask & 0x01)
        keys.insert(Lava::KeyEnter);
    if (buttonMask & 0x02)
        keys.insert(Lava::KeyEsc);
    if (buttonMask & 0x03)
        keys.insert(Lava::KeyF4);
    if (buttonMask & 0x04)
        keys.insert(Lava::KeyF4);
    if (buttonMask & 0x40)
        keys.insert(Lava::KeyF4);
    if (buttonMask & 0x80)
        keys.insert(Lava::KeyHelp);

    static std::unordered_set<uint8_t> prev_keys;
    bool changed = false;
    for (auto const k: keys) {
        if (prev_keys.find(k) == prev_keys.cend()) {
            // New key pressed
            lava_state.key_seq.push(CONVERTED_KEY | k);
            lava_state.key_state.insert(CONVERTED_KEY | k);
            changed = true;
        }
    }
    for (auto const k: prev_keys) {
        if (keys.find(k) == keys.cend()) {
            // Key released
            lava_state.key_state.erase(CONVERTED_KEY | k);
            changed = true;
        }
    }
    prev_keys = keys;

#if 1
    if (changed)
        std::cerr << __func__ << ": " << buttonMask << ", " << x << ", " << y << ", " << z << std::endl;
#endif
}

// LAVA main loop

static void gl_idle()
{
    try {
        if (lava_state.run)
            lava.run();

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        lava_dump();

        lava_state.run = false;
        lava_state.ret_code = -1;
        //glutLeaveMainLoop();
    }
}


// OpenGL stuff

static struct {
    struct {
        GLuint fb;
        GLuint palette;
    } texture;
    struct {
        GLint mode;
        GLint projection;
    } uniform;

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

layout(location = 0) in vec2 vPosition;
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
    // Invert colour
    //v = float(mode - 1u) / 255. - v;
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

    // Vertex buffer for program
    const size_t nvao = 1;
    GLuint vao[nvao];
    glGenVertexArrays(nvao, vao);
    glBindVertexArray(vao[0]);

    const size_t nbuffers = 1;
    GLuint buffer[nbuffers];
    glGenBuffers(nbuffers, buffer);

    glm::vec2 vertex[4] = {
        glm::vec2(0., 0.),
        glm::vec2(0., 1.),
        glm::vec2(1., 0.),
        glm::vec2(1., 1.),
    };

    glNamedBufferData(buffer[0], sizeof(vertex), vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
    glEnableVertexAttribArray(0);

    // Textures and samplers
    GLuint tex;
    const size_t nsamplers = 1;
    GLuint sampler[nsamplers];
    glGenSamplers(nsamplers, sampler);

    glSamplerParameteri(sampler[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindSampler(0, sampler[0]);

    // Texture: frame buffer
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTextureStorage2D(tex, 1, GL_R8, lava.getFramebufferWidth(), lava.getFramebufferHeight());
    glPixelStorei(GL_UNPACK_ROW_LENGTH, lava.getFramebufferStride());
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureSubImage2D(tex, 0, 0, 0,
                        lava.getFramebufferWidth(), lava.getFramebufferHeight(),
                        GL_RED, GL_UNSIGNED_BYTE, lava.getFramebuffer());
    gl_data.texture.fb = tex;

    // Texture: palette
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTextureStorage1D(tex, 1, GL_RGB8, 256);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureSubImage1D(tex, 0, 0, 256,
                        GL_RGB, GL_UNSIGNED_BYTE, palette.c256.data());
    gl_data.texture.palette = tex;

    // Global uniform data
    gl_data.uniform.mode = glGetUniformLocation(program, "mode");
    gl_data.uniform.projection = glGetUniformLocation(program, "projection");
    glUniform1ui(gl_data.uniform.mode, 256);
    glUniformMatrix4fv(gl_data.uniform.projection, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

    // Clear screen
    glClearColor(0.0, 0.0, 0.0, 1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Callback::refresh(uint8_t *framebuffer)
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
    glutPostRedisplay();
}

static void gl_display()
{
#if 0
    glTextureSubImage2D(gl_data.texture.fb, 0,
                        0, 0, lava.getFramebufferWidth(), lava.getFramebufferHeight(),
                        GL_RED, GL_UNSIGNED_BYTE, lava.getFramebuffer());
#endif
    glClearColor(0.0, 0.0, 0.0, 1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glutSwapBuffers();
}

static void gl_reshape(int width, int height)
{
    int lavaw = lava.getFramebufferWidth();
    int lavah = lava.getFramebufferHeight();
    int xscale = width / lavaw;
    int yscale = height / lavah;
    int scale = std::min(xscale, yscale);
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
}

int main(int argc, char *argv[])
{
    int e = lava_init(argc, argv);
    if (e != 0)
        return e;

    glutInit(&argc, argv);
    glutInitContextVersion(4, 0);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

    uint32_t scnw = glutGet(GLUT_SCREEN_WIDTH);
    uint32_t scnh = glutGet(GLUT_SCREEN_HEIGHT);
    uint32_t w = lava.getFramebufferWidth();
    uint32_t h = lava.getFramebufferHeight();
    uint32_t s = std::max(1u, std::min(scnw / w / 2, scnh / h / 2));
    glutInitWindowSize(w * s, h * s);
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    int window = glutCreateWindow("lava_glut");
    glutDisplayFunc(&gl_display);
    glutReshapeFunc(&gl_reshape);
    glutSpecialFunc(&gl_special_keys);
    glutSpecialUpFunc(&gl_special_keys_up);
    glutKeyboardFunc(&gl_keyboard_keys);
    glutKeyboardUpFunc(&gl_keyboard_keys_up);
    glutJoystickFunc(&gl_joystick, 10);
    glutIdleFunc(&gl_idle);

    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::cerr << glewGetErrorString(glew_err) << std::endl;
        return 1;
    }

    try {
        gl_init();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (lava_state.run)
        glutMainLoop();
    return lava_state.ret_code;
}
