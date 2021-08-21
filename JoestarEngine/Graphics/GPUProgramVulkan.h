#pragma once
#include "GPUProgram.h"
#include "VulkanHeader.h"
#include "../IO/FileSystem.h"
#include "../IO/File.h"
#include <array>
#include "../Graphics/VertexData.h"
#include "../Math/Matrix4x4.h"
#include "Mesh.h"
#include <vector>
#include "RenderCommand.h"
#include <map>
#include <cstring>

namespace Joestar {
    struct UniformBufferObject {
        Matrix4x4f model;
        Matrix4x4f view;
        Matrix4x4f proj;
        bool operator== (UniformBufferObject& ub) {
            return model == ub.model && view == ub.view && proj == ub.proj;
        }
    };

    struct PipelineState {
        Vector4f clearColor;
        UniformBufferObject ubo;
        VertexBuffer* vb;
        IndexBuffer* ib;
        std::string shader;

        VKPipelineContext* pipelineCtx;
        VKFrameBufferContext* fbCtx;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
        VkPipelineShaderStageCreateInfo shaderStage[2];
        VkShaderModule vertShaderModule{}, fragShaderModule{};

        bool operator== (PipelineState & p2) {
            return (clearColor == p2.clearColor) && shader == p2.shader &&
                (vb == vb) && (ib == ib);
        }
    };
    class GPUProgramVulkan :public GPUProgram {
    public:
        GPUProgramVulkan();
        VkPipelineVertexInputStateCreateInfo* GetVertexInputInfo();
        void SetDevice(VulkanContext* ctx) { vkCtxPtr = ctx; }
        void SetShader(PipelineState& pso, std::string& vertexPath, std::string& fragmentPath, std::string& geometryPath);
        void CreateVertexBuffer(PipelineState& pso, VertexBuffer* vb);
        void CreateIndexBuffer(PipelineState& pso, IndexBuffer* ib);
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CreateTextureImage(PipelineState& pso);
        void CreateTextureImageView(PipelineState& pso);
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, uint32_t mipLevels = 1);
        void CreateTextureSampler(PipelineState& pso);
        void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        //inline VkBuffer* GetVertexBuffer() { return &vertexBuffer; }
        //inline VkBuffer* GetIndexBuffer() { return &indexBuffer; }
        //inline uint32_t GetIndexSize() { return mesh->GetIB()->GetSize(); }
        //inline uint32_t GetIndexCount() { return mesh->GetIB()->GetIndexCount(); }
        //inline uint32_t GetVertexSize() { return vertices.size(); }
        void Clean();        
        //inline VkPipelineShaderStageCreateInfo* GetShaderStage() { return mShaderStage; };
        VkShaderModule CreateShaderModule(File* code);
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        void CreateRenderPass(PipelineState& pso);
        void CreateGraphicsPipeline(PipelineState& pso);
        void CreateDescriptorSetLayout(PipelineState& pso);
        VKPipelineContext* GetPipelineContext(PipelineState& pso);
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat FindDepthFormat();
        //void CreateDescriptorSets();
        void CleanupSwapChain();
        void CreateDepthResources(PipelineState& pso);
        void CreateColorResources(PipelineState& pso);
        void CreateUniformBuffers();
        void CreateFrameBuffers(PipelineState& pso);
        void CreateDescriptorPool();
        void CreateDescriptorSets(PipelineState& pso);
        void UpdateUniformBuffer(uint32_t currentImage);
        void RecordCommandBuffer(PipelineState& pso);
        void ExecuteRenderCommand(std::vector<RenderCommand> cmdBuffer, uint16_t cmdIdx);

    private:
        VulkanContext* vkCtxPtr;
        //VKPipelineContext* pipelineCtx;
        //VKFrameBufferContext* fbCtx;

        VkBufferCreateInfo bufferInfo{};
        VkMemoryRequirements memRequirements;
        VkMemoryAllocateInfo allocInfo{};
        uint32_t mipLevels;
        //VertexBuffer* vb;
        //Mesh* mesh;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_2_BIT;
        PipelineState currentPSO;
        std::vector<PipelineState> allPSO;
    };

}