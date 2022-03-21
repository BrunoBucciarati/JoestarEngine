#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include "../IO/File.h"
#include <iostream>
#include "Shader/ShaderDefs.h"
#include "../Container/Str.h"

namespace Joestar {
    class GPUProgram
    {
    public:
        unsigned int ID;

        GPUProgram() {}
        //virtual void SetShader(std::string& vertexPath, std::string& fragmentPath, std::string& geometryPath = nullptr) {}
        File* ShaderCodeFile(const char* file);
        virtual void SetShader(const char* shaderPath, ShaderStage stage) {}
        virtual const char* GetShaderDirectory() { return ""; }
        virtual void Use() {}
        virtual void SetBool(const String& name, bool value) const {}
        virtual void SetInt(const String& name, int value) const {}
        virtual void SetFloat(const String& name, float value) const {}
        virtual void SetVec3(const String& name, glm::vec3 value) const {}
        virtual void SetVec3(const String& name, float x, float y, float z) const {}
        virtual void SetVec4(const String& name, glm::vec4 value) const {}
        virtual void SetMat4(const String& name, const glm::mat4& value) const {}
        static GPUProgram* CreateProgram();
    };
}
#endif