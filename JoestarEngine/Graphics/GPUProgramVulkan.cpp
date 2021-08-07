#include "GPUProgramVulkan.h"
#include "../IO/Log.h"

namespace Joestar {
	VkShaderModule GPUProgramVulkan::CreateShaderModule(File* file) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		size_t codeSize = file->Size();
		createInfo.codeSize = codeSize;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(file->GetBuffer());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(*mDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			LOGERROR("failed to create shader module!");
		}
		return shaderModule;
	}
	void GPUProgramVulkan::SetShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
		File* vShaderCode = ShaderCodeFile(vertexPath);
		File* fShaderCode = ShaderCodeFile(fragmentPath);

		vertShaderModule = CreateShaderModule(vShaderCode);
		fragShaderModule = CreateShaderModule(fShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		//VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
		mShaderStage[0] = vertShaderStageInfo;
		mShaderStage[1] = fragShaderStageInfo;
	}

	void GPUProgramVulkan::Clean() {
		vkDestroyShaderModule(*mDevice, vertShaderModule, nullptr);
		vkDestroyShaderModule(*mDevice, fragShaderModule, nullptr);
	}
}
//#include "../IO/Log.h"
//#define CREATE_SHADER_GL(HANDLE, CODE)\
//    glShaderSource(HANDLE, 1, &CODE, NULL);\
//    glCompileShader(HANDLE);\
//    glGetShaderiv(HANDLE, GL_COMPILE_STATUS, &success);\
//    if (!success)\
//    {\
//        glGetShaderInfoLog(HANDLE, 512, NULL, infoLog);\
//        LOGERROR("ERROR::GLSHADER::COMPILATION_FAILED\n")\
//    };\
//
//void GPUProgramGL::SetShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
//{
//    const char* vShaderCode = ShaderCodeFromFile(vertexPath);
//    const char* fShaderCode = ShaderCodeFromFile(fragmentPath);
//    GLuint vertex, fragment, geoShader;
//    int success;
//    char infoLog[512];
//    vertex = glCreateShader(GL_VERTEX_SHADER);
//    fragment = glCreateShader(GL_FRAGMENT_SHADER);
//    CREATE_SHADER_GL(vertex, vShaderCode);
//    CREATE_SHADER_GL(fragment, fShaderCode);
//  
//
//    ID = glCreateProgram();
//    glAttachShader(ID, vertex);
//    glAttachShader(ID, fragment);
//    if (geometryPath) {
//        const char* gShaderCode = ShaderCodeFromFile(geometryPath);
//        geoShader = glCreateShader(GL_GEOMETRY_SHADER);
//        CREATE_SHADER_GL(geoShader, gShaderCode);
//        glAttachShader(ID, geoShader);
//    }
//    glLinkProgram(ID);
//    glGetProgramiv(ID, GL_LINK_STATUS, &success);
//    if (!success)
//    {
//        glGetProgramInfoLog(ID, 512, NULL, infoLog);
//        LOGERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
//    }
//
//    glDeleteShader(vertex);
//    glDeleteShader(fragment);
//    if (geometryPath) {
//        glDeleteShader(geoShader);
//    }
//}
//
//void GPUProgramGL::Use()
//{
//    glUseProgram(ID);
//}
//
//void GPUProgramGL::SetBool(const std::string& name, bool value) const
//{
//    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
//}
//void GPUProgramGL::SetInt(const std::string& name, int value) const
//{
//    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
//}
//void GPUProgramGL::SetFloat(const std::string& name, float value) const
//{
//    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
//}
//void GPUProgramGL::SetVec3(const std::string& name, glm::vec3 value) const
//{
//    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
//}
//void GPUProgramGL::SetVec3(const std::string& name, float x, float y, float z) const
//{
//    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
//}
//void GPUProgramGL::SetVec4(const std::string& name, glm::vec4 value) const
//{
//    glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
//}
//
//void GPUProgramGL::SetMat4(const std::string& name, const glm::mat4& mat) const
//{
//    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//}