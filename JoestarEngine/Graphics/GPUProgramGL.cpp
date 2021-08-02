#include "GPUProgramGL.h"
#include "../IO/Log.h"
#define CREATE_SHADER_GL(HANDLE, CODE)\
    glShaderSource(HANDLE, 1, &CODE, NULL);\
    glCompileShader(HANDLE);\
    glGetShaderiv(HANDLE, GL_COMPILE_STATUS, &success);\
    if (!success)\
    {\
        glGetShaderInfoLog(HANDLE, 512, NULL, infoLog);\
        LOGERROR("ERROR::GLSHADER::COMPILATION_FAILED\n")\
    };\

void GPUProgramGL::SetShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    const char* vShaderCode = ShaderCodeFromFile(vertexPath);
    const char* fShaderCode = ShaderCodeFromFile(fragmentPath);
    GLuint vertex, fragment, geoShader;
    int success;
    char infoLog[512];
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    CREATE_SHADER_GL(vertex, vShaderCode);
    CREATE_SHADER_GL(fragment, fShaderCode);
  

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath) {
        const char* gShaderCode = ShaderCodeFromFile(geometryPath);
        geoShader = glCreateShader(GL_GEOMETRY_SHADER);
        CREATE_SHADER_GL(geoShader, gShaderCode);
        glAttachShader(ID, geoShader);
    }
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        LOGERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath) {
        glDeleteShader(geoShader);
    }
}

void GPUProgramGL::Use()
{
    glUseProgram(ID);
}

void GPUProgramGL::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void GPUProgramGL::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void GPUProgramGL::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void GPUProgramGL::SetVec3(const std::string& name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}
void GPUProgramGL::SetVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void GPUProgramGL::SetVec4(const std::string& name, glm::vec4 value) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void GPUProgramGL::SetMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}