//
// Created by bison on 04-09-23.
//

#ifndef PLATFORMER_SHADERPROGRAM_H
#define PLATFORMER_SHADERPROGRAM_H

#include <unordered_map>
#include <memory>
#include <defs.h>
#include <string>
#include <glm/glm.hpp>

namespace Renderer {
    class ShaderProgram {
    public:
        ShaderProgram(const std::string& vertFilename, const std::string& fragFilename);
        ~ShaderProgram();
        void use() const;
        void setupUniform(const std::string& name);
        void setUniform(const std::string& name, float value);
        void setUniform(const std::string& name, i32 value);
        void setUniform(const std::string& name, glm::mat3 value);
        void setUniform(const std::string& name, glm::mat4 value);
        void setUniform(const std::string& name, glm::vec3 value);
        void setUniform(const std::string& name, glm::vec4 value);

    private:
        u32 id = 0;
        std::unordered_map<std::string, i32> uniforms;
    };

}



#endif //PLATFORMER_SHADERPROGRAM_H
