#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdint>

#define INIT_WINDOW_SCALE   4

struct {
    struct {
        GLuint fb;
    } texture;
} gl_data;

struct {
    uint32_t w, h;
    uint8_t v[320*240];
} framebuffer;

void framebuffer_init()
{
    framebuffer.w = 320;
    framebuffer.h = 240;
    uint8_t i = 0;
    for (auto &c: framebuffer.v)
        c = i++;
}

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
    colour = vec4(texture(tex, vMap).r, 0., 0., 1.);
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
        glm::vec2(0, 0),
        glm::vec2(0, 1),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
    };

    glNamedBufferData(buffer[0], sizeof(vertex), vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
    glEnableVertexAttribArray(0);

    // Texture sampler
    glGenTextures(1, &gl_data.texture.fb);
    glBindTexture(GL_TEXTURE_2D, gl_data.texture.fb);
    glTextureStorage2D(gl_data.texture.fb, 1, GL_R8, framebuffer.w, framebuffer.h);

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
    glTextureSubImage2D(gl_data.texture.fb, 0,
                        0, 0, framebuffer.w, framebuffer.h,
                        GL_RED, GL_UNSIGNED_BYTE, framebuffer.v);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glutSwapBuffers();
}

void gl_reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char *argv[])
{
    framebuffer_init();

    glutInit(&argc, argv);
    glutInitContextVersion(4, 0);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(framebuffer.w * INIT_WINDOW_SCALE, framebuffer.h * INIT_WINDOW_SCALE);

    int window = glutCreateWindow("lava_glut");
    glutDisplayFunc(&gl_display);
    glutReshapeFunc(&gl_reshape);

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
