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
        explicit ShaderVK(Shader* _shader) : shader(_shader) {
            for (auto& uniform : shader->info.uniforms) {
                if (uniform.dataType > ShaderDataTypeSampler) {
                    ubs.push_back(uniform.binding);
                } else {
                    ubs.push_back(hashString(uniform.name.c_str()));
                }
            }
        }
        std::string& GetName() { return shader->GetName(); }
        U16 GetUniformBindingByName(std::string& name) {
            return shader->GetUniformBindingByName(name);
        }
        U16 GetSamplerBinding(int count) {
            return shader->GetSamplerBinding(count);
        }
        U32 ID() { return shader->id; }
        Shader* shader;
        VkShaderModule vertShaderModule{}, fragShaderModule{};
        VkPipelineShaderStageCreateInfo shaderStage[2];
        bool operator ==(ShaderVK& s2) {
            return GetName() == s2.GetName();
        }
        void Clean(VkDevice& dev) {
            vkDestroyShaderModule(dev, vertShaderModule, nullptr);
            vkDestroyShaderModule(dev, fragShaderModule, nullptr);
        }
        std::vector<U32> ubs;
        std::vector<U32> textures;
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

    class BufferVK {
    public:
        VkBuffer buffer;
        VkDeviceMemory memory;

        void Clean(VkDevice& dev) {
            vkDestroyBuffer(dev, buffer, nullptr);
            vkFreeMemory(dev, memory, nullptr);
        }
    };

    class VertexBufferVK : public BufferVK {
    public:
        explicit VertexBufferVK(VertexBuffer* b) { vb = b; }
        VertexBuffer* vb;
        U32 ID() { return vb->id; }
        U32 GetSize() { return vb->GetSize(); }
        U8* GetBuffer() { return vb->GetBuffer(); }
        VkPipelineVertexInputStateCreateInfo* GetVertexInputInfo();
        VkVertexInputBindingDescription GetBindingDescription();
        std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

    private:
        std::vector<VkVertexInputAttributeDescription>attributeDescriptions;
        VkVertexInputBindingDescription bindingDescription;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    };

    class IndexBufferVK : public BufferVK {
    public:
        explicit IndexBufferVK(IndexBuffer* b) { ib = b; }
        U32 GetIndexCount() { return ib->GetIndexCount(); }
        IndexBuffer* ib;
        U32 ID() { return ib->id; }
        U32 GetSize() { return ib->GetSize(); }
        U8* GetBuffer() { return ib->GetBuffer(); }
    };

    struct UniformBufferVK {
        std::vector<BufferVK> buffers;
        //std::vector<VkBuffer> uniformBuffers;
        //std::vector<VkDeviceMemory> uniformBuffersMemory;
        uint32_t size;
        void* data;
        uint32_t id;
        std::string name;
        uint32_t texID = 0;

        void Clean(VkDevice& dev) {
            for (auto& buf: buffers) {
                vkDestroyBuffer(dev, buf.buffer, nullptr);
                vkFreeMemory(dev, buf.memory, nullptr);
            }
            buffers.clear();
        }
    };

    struct PipelineStateVK {
        //bool clear = false;
        //bool msaa = false;
        //Vector4f clearColor;
        //UniformBufferObject ubo;
        //ShaderVK* shader;
        std::vector<UniformBufferVK*> ubs;

        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        //PipelineVK* pipeline;

        //VkBuffer indexBuffer;
        //VkDeviceMemory indexBufferMemory;
        //VkBuffer vertexBuffer;
        //VkDeviceMemory vertexBufferMemory;
        VkSampler textureSampler;

        std::vector<uint32_t> textures;

        bool operator== (PipelineStateVK& p2) {
            if (textures.size() != p2.textures.size()) return false;
            for (int i = 0; i < textures.size(); ++i) {
                if (textures[i] != p2.textures[i]) return false;
            }
            if (ubs.size() != p2.ubs.size()) return false;
            for (int i = 0; i < ubs.size(); ++i) {
                if (ubs[i] != p2.ubs[i]) return false;
            }
            return true;// clearColor == p2.clearColor && shader == p2.shader && clear == p2.clear;
        }

        bool operator!= (PipelineStateVK& p2) {
            return !(*this == p2);
        }
    };

    struct DrawCallVK {
        VertexBufferVK* vb;
        IndexBufferVK* ib;
        ShaderVK* shader;
        MeshTopology topology;
        PipelineStateVK* pso;
    };

    struct RenderPassVK {
        VkRenderPass renderPass;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        FrameBufferVK* fb;
        bool clear = false;
        bool msaa = false;
        Vector4f clearColor;
        const char* name;
        std::vector<DrawCallVK*> dcs;
    };


    class GPUProgramVulkan : public GPUProgram {
    public:
        GPUProgramVulkan();
        //VkPipelineVertexInputStateCreateInfo* GetVertexInputInfo();
        void SetDevice(VulkanContext* ctx) { vkCtxPtr = ctx; }
        void SetShader(ShaderVK* shader);
        void CreateVertexBuffer(VertexBufferVK* vb);
        void CreateIndexBuffer(IndexBufferVK* ib);
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CreateTextureImage(TextureVK* tex);
        void CreateTextureImageView(TextureVK* tex);
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, uint32_t mipLevels = 1);
        void CreateTextureSampler(RenderPassVK* pass, int i);
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
        void CreateRenderPass(RenderPassVK* pass);
        void CreateGraphicsPipeline(RenderPassVK* pass, int i);
        void CreateDescriptorSetLayout(RenderPassVK* pass, int i);
        void GetPipeline(RenderPassVK* pass, int i);
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat FindDepthFormat();
        //void CreateDescriptorSets();
        void CleanupSwapChain();
        void CreateDepthResources(RenderPassVK* pass);
        void CreateColorResources(RenderPassVK* pass);
        void CreateUniformBuffers(UniformBufferVK* ub);
        void CreateFrameBuffers(RenderPassVK* pass);
        void CreateDescriptorPool();
        void CreateDescriptorSets(DrawCallVK* pso);
        void UpdateDescriptorSets(DrawCallVK* pso);
        void UpdateUniformBuffer(uint32_t currentImage);
        void RecordCommandBuffer(std::vector<RenderPassVK*>&);
        void ExecuteRenderCommand(std::vector<RenderCommand>& cmdBuffer, uint16_t cmdIdx);
        //void RenderCmdUpdateUniformBuffer(RenderCommand cmd, PipelineState& pso);
        void RenderCmdUpdateUniformBufferObject(RenderCommand& cmd);
        void RecordRenderPass(RenderPassVK* pass, int i);

    private:
        VulkanContext* vkCtxPtr;
        bool dynamicCommandBuffer;

        VkBufferCreateInfo bufferInfo{};
        VkMemoryRequirements memRequirements;
        VkMemoryAllocateInfo allocInfo{};
        uint32_t mipLevels;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_2_BIT;
        //PipelineStateVK currentPSO;
        //std::map<uint32_t, PipelineStateVK> allPSOs;
        //pending texture, will upload during UpdateUniform
        //all resources
        std::map<uint32_t, TextureVK*> textureVKs;
        std::map<uint32_t, TextureVK*> pendingTextureVKs;
        std::map<uint32_t, ShaderVK*> shaderVKs;
        std::map<uint32_t, UniformBufferVK*> uniformVKs;
        std::map<uint32_t, VertexBufferVK*> vbs;
        std::map<uint32_t, IndexBufferVK*> ibs;
        std::map<uint32_t, FrameBufferVK*> fbs;
        //std::vector<PipelineStateVK*> psoChain;

        std::vector<RenderPassVK*> renderPassList;
    };

}