#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

#include "lava.h"

struct {
    struct {
        GLuint fb;
    } texture;
} gl_data;

// Lava stuff

Lava lava;

int lava_init(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

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

void gl_idle()
{
    static bool lava_run = true;
    try {
        LavaProc::proc_req_t req = LavaProc::ReqNone;
        if (lava_run)
            req = lava.run();

        if (req & LavaProc::ReqRefresh) {
            glTextureSubImage2D(gl_data.texture.fb, 0,
                                0, 0, lava.getFramebufferWidth(), lava.getFramebufferHeight(),
                                GL_RED, GL_UNSIGNED_BYTE, lava.getFramebuffer());
            glutPostRedisplay();
        }

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;

        auto const &ram = lava.inspectRam();
        std::ofstream f_ram_dump;
        f_ram_dump.open("ram.bin", std::ios::binary | std::ios::out);
        f_ram_dump.write(reinterpret_cast<const char *>(ram.data()), ram.size());
        f_ram_dump.close();
        std::cerr << "RAM dump: ram.bin" << std::endl;

        auto fb = lava.getFramebuffer();
        std::ofstream f_fb_dump;
        f_fb_dump.open("fb.bin", std::ios::binary | std::ios::out);
        f_fb_dump.write(reinterpret_cast<const char *>(fb),
                        LAVA_MAX_HEIGHT * LAVA_MAX_WIDTH);
        f_fb_dump.close();
        std::cerr << "FB dump: fb.bin" << std::endl;

        lava_run = false;
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
    v = v * 40.;
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
    uint32_t s = std::max(1u, std::min(scnw / w, scnh / h) / 2u);
    glutInitWindowSize(w * s, h * s);

    int window = glutCreateWindow("lava_glut");
    glutDisplayFunc(&gl_display);
    glutReshapeFunc(&gl_reshape);
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
    return 0;
}
