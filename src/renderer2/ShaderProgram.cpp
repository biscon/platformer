//
// Created by bison on 04-09-23.
//

#include <cstring>
#include <stdexcept>
#include <SDL_log.h>
#include <glm/ext.hpp>
#include "ShaderProgram.h"

extern "C" {
#include <glad.h>
}

namespace Renderer {
    static char* loadFile(const char *filename) {
        char *buffer = nullptr;
        long length;
        FILE *f = fopen(filename, "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            length = ftell(f);
            fseek(f, 0, SEEK_SET);
            buffer = (char*) malloc((size_t) length + 1);
            memset(buffer, 0, (size_t) length + 1);
            if (buffer) {
                fread(buffer, 1, (size_t) length, f);
            }
            fclose(f);
        } else {
            throw std::runtime_error("Could not open file");
        }
        return buffer;
    }

    static void checkCompileErrors(GLuint shader, const char *type) {
        GLint success;
        GLchar infoLog[1024];
        if (strcmp("PROGRAM", type) != 0) {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                SDL_Log("Shader compilation error: %s\n%s", type, infoLog);
                throw std::runtime_error("Shader compilation error");
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                SDL_Log("Shader program link error: %s\n%s", type, infoLog);
                throw std::runtime_error("Shader link error");
            }
        }
    }

    static u32 createShaderProgram(const char *vertexsrc, const char *fragmentsrc) {
        u32 vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexsrc, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentsrc, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // shader Program
        GLuint id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        checkCompileErrors(id, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return id;
    }

    ShaderProgram::ShaderProgram(const std::string& vertFilename, const std::string& fragFilename) {
        char *vertSrc = loadFile(vertFilename.c_str());
        char *fragSrc = loadFile(fragFilename.c_str());
        id = createShaderProgram(vertSrc, fragSrc);
        free(vertSrc);
        free(fragSrc);
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(id);
    }

    void ShaderProgram::use() const {
        glUseProgram(id);
    }

    void ShaderProgram::setupUniform(const std::string &name) {
        if(uniforms.count(name) == 0) {
            uniforms[name] = glGetUniformLocation(id, name.c_str());
        }
    }

    void ShaderProgram::setUniform(const std::string &name, float value) {
        glUniform1f(uniforms[name], value);
    }

    void ShaderProgram::setUniform(const std::string &name, glm::mat3 value) {
        glUniformMatrix3fv(uniforms[name], 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const std::string &name, glm::mat4 value) {
        glUniformMatrix4fv(uniforms[name], 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const std::string &name, int32_t value) {
        glUniform1i(uniforms[name], value);
    }

    void ShaderProgram::setUniform(const std::string &name, glm::vec3 value) {
        glUniform3fv(uniforms[name], 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const std::string &name, glm::vec4 value) {
        glUniform4fv(uniforms[name], 1, glm::value_ptr(value));
    }
}


