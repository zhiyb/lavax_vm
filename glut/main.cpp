#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>

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

#include "lava.h"

struct {
    struct {
        GLuint fb;
    } texture;
} gl_data;

// Lava stuff

Lava lava;

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
    virtual int32_t delay_ms(uint32_t delay);
    virtual void exit(uint32_t code);

    virtual int32_t getchar();
    virtual int32_t check_key(uint8_t key);

    virtual void refresh(uint8_t *framebuffer);

    virtual uint8_t fopen(std::string path, std::string mode);
    virtual void fclose(uint8_t fd);
    virtual std::vector<uint8_t> fread(uint8_t fd, uint32_t size);
    virtual int32_t fwrite(uint8_t fd, const std::vector<uint8_t> &data);

private:
    bool delay_pending = false;
    int tick = -1;
};

Callback cb;

#define SPECIAL_KEY 0x8000

static std::unordered_map<int, uint8_t> key_map = {
    {SPECIAL_KEY | GLUT_KEY_UP,    Lava::KeyUp},
    {SPECIAL_KEY | GLUT_KEY_DOWN,  Lava::KeyDown},
    {SPECIAL_KEY | GLUT_KEY_LEFT,  Lava::KeyLeft},
    {SPECIAL_KEY | GLUT_KEY_RIGHT, Lava::KeyRight},
    {SPECIAL_KEY | GLUT_KEY_F1,    Lava::KeyF1},
    {SPECIAL_KEY | GLUT_KEY_F2,    Lava::KeyF2},
    {SPECIAL_KEY | GLUT_KEY_F3,    Lava::KeyHelp},
    {27,                           Lava::KeyEsc},
    {0x0d,                         Lava::KeyEnter},
    {' ',                          Lava::KeySpace},
};

void key_map_init()
{
    for (char c = 'a'; c <= 'z'; c++)
        key_map[c] = c;
    for (char c = 'A'; c <= 'Z'; c++)
        key_map[c] = c;
}

int lava_init(int argc, char *argv[])
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
    if (!lava.load(fdata)) {
        std::cerr << "Error: " << lava.getErrorMsg() << std::endl;
        return 1;
    }

    return 0;
}

// LAVA miscellaneous

int Callback::delay_ms(uint32_t delay)
{
    if (!delay_pending) {
        tick = glutGet(GLUT_ELAPSED_TIME);
        delay_pending = true;
        return -1;
    }

    int delta = glutGet(GLUT_ELAPSED_TIME) - tick;
#if 1
    // Shorter delay for debugging
    if (delta * 10 < delay)
        return -1;
#else
    if (delta < delay)
        return -1;
#endif

    delay_pending = false;
    return 0;
}

void Callback::exit(uint32_t code)
{
    lava_state.ret_code = code;
    lava_state.run = false;
    glutLeaveMainLoop();
}

void Callback::refresh(uint8_t *framebuffer)
{
    glTextureSubImage2D(gl_data.texture.fb, 0,
                        0, 0, lava.getFramebufferWidth(), lava.getFramebufferHeight(),
                        GL_RED, GL_UNSIGNED_BYTE, lava.getFramebuffer());
    glutPostRedisplay();
}

void gl_refresh()
{
    cb.refresh(lava.getFramebuffer());
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

// LAVA keyboard process

void gl_keyboard_keys(uint8_t key, int x, int y)
{
    if (!lava_state.run)
        glutLeaveMainLoop();
    lava_state.key_seq.push(key);
    lava_state.key_state.insert(key);
}

void gl_keyboard_keys_up(uint8_t key, int x, int y)
{
    lava_state.key_state.erase(key);
}

void gl_special_keys(int key, int x, int y)
{
    if (!lava_state.run)
        glutLeaveMainLoop();
    key |= SPECIAL_KEY;
    lava_state.key_seq.push(key);
    lava_state.key_state.insert(key);
}

void gl_special_keys_up(int key, int x, int y)
{
    key |= SPECIAL_KEY;
    lava_state.key_state.erase(key);
}

uint8_t keycode_conv(int key)
{
    auto const &k = key_map.find(key);
    if (k == key_map.cend()) {
        std::cerr << "Key code not found: " << key << std::endl;
        return 0;
    }
    std::cerr << "Key code found: " << key << std::endl;
    return k->second;
}

int32_t Callback::getchar()
{
    if (lava_state.key_seq.empty())
        return -1;
    int v = lava_state.key_seq.front();
    lava_state.key_seq.pop();
    return keycode_conv(v);
}

int32_t Callback::check_key(uint8_t key)
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

// LAVA main loop

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
    lava.framebufferSwap();
    auto fb = lava.getFramebuffer();
    std::ofstream f_fb_dump;
    f_fb_dump.open("fb.bin", std::ios::binary | std::ios::out);
    f_fb_dump.write(reinterpret_cast<const char *>(fb),
                    LAVA_MAX_HEIGHT * LAVA_MAX_WIDTH);
    f_fb_dump.close();
    std::cerr << "FB dump: fb.bin" << std::endl;
    gl_refresh();
}

void gl_idle()
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

std::string gl_get_shader_info_log(GLuint shader)
{
    GLchar log[512];
    glGetShaderInfoLog(shader, sizeof(log), NULL, log);
    uint32_t slen = strnlen(log, sizeof(log));
    return std::string(log, slen);
}

std::string gl_get_program_info_log(GLuint program)
{
    GLchar log[512];
    glGetProgramInfoLog(program, sizeof(log), NULL, log);
    uint32_t slen = strnlen(log, sizeof(log));
    return std::string(log, slen);
}

GLuint gl_compile_shader(GLenum type, GLchar *source)
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

void gl_init()
{
    // Shader program
    GLuint program = glCreateProgram();

    glAttachShader(program, gl_compile_shader(GL_VERTEX_SHADER, (GLchar *)R"(
#version 330 core

layout(location = 0) in vec2 vPosition;
out vec2 vMap;

void main()
{
    vMap = vPosition;
    gl_Position = vec4(vec2(1., -1.) * (vPosition * 2. - vec2(1., 1.)), 0., 1.);
}
    )"));

    glAttachShader(program, gl_compile_shader(GL_FRAGMENT_SHADER, (GLchar *)R"(
#version 330 core

uniform sampler2D tex;

in vec2 vMap;
out vec4 colour;

void main()
{
    float v = texture(tex, vMap).r;
    v = 1. - (v * 16.);
    //v = v * 16.;
    colour = vec4(v, v, v, 1.);
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

    // Texture and sampler
    glGenTextures(1, &gl_data.texture.fb);
    glBindTexture(GL_TEXTURE_2D, gl_data.texture.fb);
    glTextureStorage2D(gl_data.texture.fb, 1, GL_R8, lava.getFramebufferWidth(), lava.getFramebufferHeight());
    glPixelStorei(GL_UNPACK_ROW_LENGTH, lava.getFramebufferStride());
    glTextureSubImage2D(gl_data.texture.fb, 0,
                        0, 0, lava.getFramebufferWidth(), lava.getFramebufferHeight(),
                        GL_RED, GL_UNSIGNED_BYTE, lava.getFramebuffer());

    const size_t nsamplers = 1;
    GLuint sampler[nsamplers];
    glGenSamplers(nsamplers, sampler);
    glSamplerParameteri(sampler[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindSampler(0, sampler[0]);

    // Clear screen
    glClearColor(0.4, 0.8, 1., 1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gl_display()
{
#if 0
    glTextureSubImage2D(gl_data.texture.fb, 0,
                        0, 0, lava.getFramebufferWidth(), lava.getFramebufferHeight(),
                        GL_RED, GL_UNSIGNED_BYTE, lava.getFramebuffer());
#endif
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glutSwapBuffers();
}

void gl_reshape(int width, int height)
{
    glViewport(0, 0, width, height);
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

    glutMainLoop();
    return lava_state.ret_code;
}
