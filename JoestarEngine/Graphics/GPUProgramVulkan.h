#pragma once
#include "GPUProgram.h"
#include "VulkanHeader.h"
#include "../IO/FileSystem.h"
#include "../IO/File.h"
#include <array>
#include "../Graphics/VertexData.h"
#include "../Math/Matrix4x4.h"
#include "Mesh.h"

namespace Joestar {
    struct UniformBufferObject {
        Matrix4x4f model;
        Matrix4x4f view;
        Matrix4x4f proj;
    };
    class GPUProgramVulkan :public GPUProgram {
    public:
        GPUProgramVulkan();
        VkPipelineVertexInputStateCreateInfo* GetVertexInputInfo();
        void SetDevice(VulkanContext* ctx) { vkCtxPtr = ctx; }
        void SetShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CreateTextureImage();
        void CreateTextureImageView();
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, uint32_t mipLevels = 1);
        void CreateTextureSampler();
        void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        inline VkBuffer* GetVertexBuffer() { return &vertexBuffer; }
        inline VkBuffer* GetIndexBuffer() { return &indexBuffer; }
        inline uint32_t GetIndexSize() { return mesh->GetIB()->GetSize(); }
        inline uint32_t GetIndexCount() { return mesh->GetIB()->GetIndexCount(); }
        //inline uint32_t GetVertexSize() { return vertices.size(); }
        void Clean();        
        inline VkPipelineShaderStageCreateInfo* GetShaderStage() { return mShaderStage; };
        VkShaderModule CreateShaderModule(File* code);
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        void CreateRenderPass(VKPipelineContext* pipelineContext);
        void CreateGraphicsPipeline(VKPipelineContext* pipelineContext);
        void CreateDescriptorSetLayout(VKPipelineContext* pipelineContext);
        VKPipelineContext* GetPipelineContext();
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat FindDepthFormat();
        void CreateDescriptorSets();
        void CleanupSwapChain();

        VkImageView textureImageView;
        VkSampler textureSampler;
    private:
        VulkanContext* vkCtxPtr;
        VKPipelineContext* pipelineCtx;
        VkPipelineShaderStageCreateInfo mShaderStage[2];
        VkShaderModule vertShaderModule{}, fragShaderModule{};
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBufferCreateInfo bufferInfo{};
        VkMemoryRequirements memRequirements;
        VkMemoryAllocateInfo allocInfo{};
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        //VertexBuffer* vb;
        Mesh* mesh;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_2_BIT;
    };

}