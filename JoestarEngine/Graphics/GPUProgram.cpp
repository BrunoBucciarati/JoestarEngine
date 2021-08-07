#include "GPUProgram.h"
#include "../Misc/GlobalConfig.h"
#include "GPUProgramGL.h"
const char* GPUProgram::ShaderCodeFromFile(const char* file) {
	std::string code;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        shaderFile.open(file);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        code = shaderStream.str();
        return code.c_str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    return code.c_str();
}

static GPUProgram* CreateProgram() {
    std::string GFX_API = GetGlobalConfig()->GetConfig<std::string>("GFX_API");
    if (GFX_API.find("OpenGL") != std::string::npos) {
        return new GPUProgramGL;
    }
}