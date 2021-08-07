#pragma once
#include "GPUProgram.h"
#include "../IO/FileSystem.h"
#include <vulkan/vulkan.h>
#include "../IO/File.h"

namespace Joestar {
    class GPUProgramVulkan :public GPUProgram {
    public:
        void SetDevice(VkDevice* dev) { mDevice = dev; }
        void SetShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
        void Clean();
        inline VkPipelineShaderStageCreateInfo* GetShaderStage() { return mShaderStage; };
        //void Use();
        //void SetBool(const std::string& name, bool value) const;
        //void SetInt(const std::string& name, int value) const;
        //void SetFloat(const std::string& name, float value) const;
        //void SetVec3(const std::string& name, glm::vec3 value) const;
        //void SetVec3(const std::string& name, float x, float y, float z) const;
        //void SetVec4(const std::string& name, glm::vec4 value) const;
        //void SetMat4(const std::string& name, const glm::mat4& value) const;
        VkShaderModule CreateShaderModule(File* code);
    private:
        VkDevice* mDevice;
        VkPipelineShaderStageCreateInfo mShaderStage[2];
        VkShaderModule vertShaderModule, fragShaderModule;

    };

}