#pragma once
#include "GPUProgram.h"
#include "VulkanHeader.h"
#include "../IO/FileSystem.h"
#include "../IO/File.h"
#include <array>
#include "GraphicDefines.h"
#include "../Graphics/VertexData.h"
#include "../Math/Matrix4x4.h"
#include "Mesh.h"
#include <vector>
#include "RenderCommand.h"
#include <map>
#include <string>
#include "Texture.h"
#include "Shader/Shader.h"

namespace Joestar {
    class ShaderVK {
    public:
        explicit ShaderVK(Shader* _shader) : shader(_shader) {}
        std::string& GetName() { return shader->GetName(); }
        uint16_t GetUniformBindingByName(std::string& name) {
            return shader->GetUniformBindingByName(name);
        }
        uint16_t GetSamplerBinding(int count) {
            return shader->GetSamplerBinding(count);
        }
        uint32_t ID() { return shader->id; }
        Shader* shader;
        VkShaderModule vertShaderModule{}, fragShaderModule{};
        VkPipelineShaderStageCreateInfo shaderStage[2];
        bool operator ==(ShaderVK& s2) {
            return GetName() == s2.GetName();
        }
        void Clear(VkDevice& dev) {
            vkDestroyShaderModule(dev, vertShaderModule, nullptr);
            vkDestroyShaderModule(dev, fragShaderModule, nullptr);
        }
    };

    class TextureVK {
    public:
        explicit TextureVK(Texture*);
        inline uint32_t ID() { return texture->id; }
        inline uint32_t GetSize() { return texture->GetSize(); }
        inline bool HasMipmap() { return texture->hasMipMap; }
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
    private:
        Texture* texture;
    };

    class UniformBufferVK {
    public:
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        uint32_t size;
        void* data;
        uint32_t id;
        std::string name;
        uint32_t texID;
    };

    struct PipelineState {
        Vector4f clearColor;
        //UniformBufferObject ubo;
        VertexBuffer* vb;
        IndexBuffer* ib;
        ShaderVK* shader;
        std::vector<UniformBufferVK*> ubs;

        VKPipelineContext* pipelineCtx;
        VKFrameBufferContext* fbCtx;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkSampler textureSampler;

        std::vector<uint32_t> textures;

        bool operator== (PipelineState & p2) {
            if (textures.size() != p2.textures.size()) return false;
            for (int i = 0; i < textures.size(); ++i) {
                if (textures[i] != p2.textures[i]) return false;
            }
            if (ubs.size() != p2.ubs.size()) return false;
            for (int i = 0; i < ubs.size(); ++i) {
                if (ubs[i] != p2.ubs[i]) return false;
            }
            return (clearColor == p2.clearColor) && shader == p2.shader &&
                (vb == vb) && (ib == ib);
        }
    };

    //class VertexBufferVK {
    //    explicit VertexBufferVK(VertexBuffer* v) : vb(v)  {}
    //private:
    //    VertexBuffer* vb;
    //};

    class GPUProgramVulkan :public GPUProgram {
    public:
        GPUProgramVulkan();
        VkPipelineVertexInputStateCreateInfo* GetVertexInputInfo();
        void SetDevice(VulkanContext* ctx) { vkCtxPtr = ctx; }
        void SetShader(PipelineState& pso);
        void CreateVertexBuffer(PipelineState& pso, VertexBuffer* vb);
        void CreateIndexBuffer(PipelineState& pso, IndexBuffer* ib);
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CreateTextureImage(TextureVK* tex);
        void CreateTextureImageView(TextureVK* tex);
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
        void CreateUniformBuffers(UniformBufferVK* ub);
        void CreateFrameBuffers(PipelineState& pso);
        void CreateDescriptorPool();
        void CreateDescriptorSets(PipelineState& pso);
        void UpdateDescriptorSets(PipelineState& pso);
        void UpdateUniformBuffer(uint32_t currentImage);
        void RecordCommandBuffer(PipelineState& pso);
        void ExecuteRenderCommand(std::vector<RenderCommand> cmdBuffer, uint16_t cmdIdx);
        void RenderCmdUpdateUniformBuffer(RenderCommand cmd, PipelineState& pso);
        void RenderCmdUpdateUniformBufferObject(RenderCommand cmd, PipelineState& pso);

    private:
        VulkanContext* vkCtxPtr;
        bool dynamicCommadBuffer;

        VkBufferCreateInfo bufferInfo{};
        VkMemoryRequirements memRequirements;
        VkMemoryAllocateInfo allocInfo{};
        uint32_t mipLevels;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_2_BIT;
        PipelineState currentPSO;
        std::vector<PipelineState> allPSO;
        std::map<uint32_t, TextureVK*> textureVKs;
        //pending texture, will upload during UpdateUniform
        std::map<uint32_t, TextureVK*> pendingTextureVKs;
        std::map<uint32_t, ShaderVK*> shaderVKs;
        std::map<uint32_t, UniformBufferVK*> uniformVKs;
    };

}