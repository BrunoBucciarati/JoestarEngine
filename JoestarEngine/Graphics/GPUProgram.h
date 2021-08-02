#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class GPUProgram
{
public:
    unsigned int ID;
    enum ShaderType {
        kVertexShader = 0,
        kFragmentShader,
        kGeometryShader,
        kComputeShader
    };


    GPUProgram() {}
    virtual void SetShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr){}
    const char* ShaderCodeFromFile(const char* file);
    virtual void SetShader(const char* shaderPath, ShaderType typ){}
    virtual const char* GetShaderDirectory() { return ""; }
    virtual void Use() {}
    virtual void SetBool(const std::string& name, bool value) const {}
    virtual void SetInt(const std::string& name, int value) const {}
    virtual void SetFloat(const std::string& name, float value) const {}
    virtual void SetVec3(const std::string& name, glm::vec3 value) const {}
    virtual void SetVec3(const std::string& name, float x, float y, float z) const {}
    virtual void SetVec4(const std::string& name, glm::vec4 value) const {}
    virtual void SetMat4(const std::string& name, const glm::mat4& value) const {}
    static GPUProgram* CreateProgram();
};

#endif