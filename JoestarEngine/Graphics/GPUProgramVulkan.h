#pragma once
#include "GPUProgram.h"
#include "VulkanHeader.h"
#include "../IO/FileSystem.h"
#include "../IO/File.h"
#include <array>
#include "../Graphics/VertexData.h"

namespace Joestar {
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
    };
    class GPUProgramVulkan :public GPUProgram {
    public:
        VkPipelineVertexInputStateCreateInfo* GetVertexInputInfo();
        void SetDevice(VulkanContext* ptr) { vkContextPtr = ptr; }
        void SetShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        inline VkBuffer* GetVertexBuffer() { return &vertexBuffer; }
        inline VkBuffer* GetIndexBuffer() { return &indexBuffer; }
        inline uint32_t GetIndexSize() { return indices.size(); }
        inline uint32_t GetVertexSize() { return vertices.size(); }
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
        VulkanContext* vkContextPtr;
        VkPipelineShaderStageCreateInfo mShaderStage[2];
        VkShaderModule vertShaderModule{}, fragShaderModule{};
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBufferCreateInfo bufferInfo{};
        VkMemoryRequirements memRequirements;
        VkMemoryAllocateInfo allocInfo{};
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        VkVertexInputBindingDescription bindingDescription{};
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
    };

}