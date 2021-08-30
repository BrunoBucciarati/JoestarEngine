#pragma once
#include "GPUProgram.h"
#include "VulkanHeader.h"
#include "../IO/FileSystem.h"
#include "../IO/File.h"
#include "../IO/Log.h"
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
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device);

    struct FrameBufferVK {
        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;
        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;
        void Clean(VkDevice& dev) {
            vkDestroyImageView(dev, colorImageView, nullptr);
            vkDestroyImage(dev, colorImage, nullptr);
            vkFreeMemory(dev, colorImageMemory, nullptr);
            vkDestroyImageView(dev, depthImageView, nullptr);
            vkDestroyImage(dev, depthImage, nullptr);
            vkFreeMemory(dev, depthImageMemory, nullptr);
        }
    };

    class ShaderVK {
    public:
        explicit ShaderVK(Shader* _shader) : shader(_shader) {
            for (auto& uniform : shader->info.uniforms) {
                if (uniform.dataType == ShaderDataTypePushConst) {
                    continue;
                } else if (uniform.dataType > ShaderDataTypeSampler) {
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
        std::string GetPushConsts() { return shader->GetPushConsts(); }
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


    struct ImageVK {
        VulkanContext* ctx;
        U32 width;
        U32 height;
        U32 mipLevels;
        VkSampleCountFlagBits numSamples;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkMemoryPropertyFlags properties;
        VkImageType imageType;
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        U32 memoryTypeIdx;

        void Create() {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = imageType;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = mipLevels;
            imageInfo.arrayLayers = 1;
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = numSamples;
            imageInfo.flags = 0; // Optional  
            if (vkCreateImage(ctx->device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
                LOGERROR("failed to create image!");
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(ctx->device, image, &memRequirements);
            memoryTypeIdx = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ctx->physicalDevice);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = memoryTypeIdx;

            if (vkAllocateMemory(ctx->device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate image memory!");
            }

            vkBindImageMemory(ctx->device, image, imageMemory, 0);
        }
    };

    class TextureVK {
    public:
        explicit TextureVK(Texture*);
        U32 ID() { return texture->id; }
        U32 GetSize() { return texture->GetSize(); }
        U32 GetWidth() { return texture->GetWidth(); }
        U32 GetHeight() { return texture->GetHeight(); }
        void* GetData() { return texture->GetData(); }
        bool HasMipmap() { return texture->hasMipMap; }
        TEXTURE_TYPE Type() { return texture->typ; }
        //VkImage textureImage;
        //VkDeviceMemory textureImageMemory;
        //VkImageView textureImageView;
        ImageVK* image;
    private:
        Texture* texture;
    };

    struct BufferVK {
    public:
        VulkanContext* ctx;
        VkDeviceSize size;
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties;
        VkBuffer buffer;
        VkDeviceMemory memory;
        U32 memoryTypeIdx;

        void Create() {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(ctx->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(ctx->device, buffer, &memRequirements);
            memoryTypeIdx = FindMemoryType(memRequirements.memoryTypeBits, properties, ctx->physicalDevice);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = memoryTypeIdx; 

            if (vkAllocateMemory(ctx->device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate buffer memory!");
            }

            vkBindBufferMemory(ctx->device, buffer, memory, 0);
        }

        void CopyBuffer(U8* cpuData) {
            void* data;
            vkMapMemory(ctx->device, memory, 0, size, 0, &data);
            memcpy(data, cpuData, size);
            vkUnmapMemory(ctx->device, memory);
        }

        void Clean() {
            vkDestroyBuffer(ctx->device, buffer, nullptr);
            vkFreeMemory(ctx->device, memory, nullptr);
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
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        VkSampler textureSampler;
        U32 hash = 0;
        //ez hash, i guess that's ok
        void HashInsert(U32 i) {
            hash *= 10;
            hash += i;
        }
    };

    struct PushConstsVK : PushConsts {
        bool operator ==(PushConstsVK& p2) { return model == p2.model; }
        bool operator !=(PushConstsVK& p2) { return !(*this == p2); }
    };

    struct DrawCallVK {
        VertexBufferVK* vb;
        IndexBufferVK* ib;
        ShaderVK* shader;
        MeshTopology topology;
        PipelineStateVK* pso;
        PushConstsVK* pc = nullptr;
        U32 hash = 0;
        //ez hash, i guess that's ok
        void HashInsert(U32 i) {
            hash *= 10;
            hash += i;
        }
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
        U32 hash = 0;
        //ez hash, i guess that's ok
        void HashInsert(U32 i) {
            hash *= 10;
            hash += i;
        }
    };

    class CommandBufferVK {
    public:
        VkCommandBuffer commandBuffer;
        explicit CommandBufferVK(VkCommandPool* p, VulkanContext* c, bool dynamic = false) {
            pool = p;
            ctx = c;
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = *p;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            if (vkAllocateCommandBuffers(ctx->device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
                LOGERROR("failed to allocate sub command buffer!\n");
            }
        }
        void Begin() {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                LOGERROR("failed to begin recording command buffer!");
            }
        }
        void End() {
            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                LOGERROR("failed to record command buffer!");
            }
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(ctx->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(ctx->graphicsQueue);

            if (dynamic) {
                vkFreeCommandBuffers(ctx->device, *pool, 1, &commandBuffer);
            }
        }

        ~CommandBufferVK() {
            vkFreeCommandBuffers(ctx->device, *pool, 1, &commandBuffer);
        }
    private:
        VkCommandPool* pool;
        VulkanContext* ctx;
        bool dynamic;
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
        //void CreateImage(ImageVK* image);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void Clean();
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
        bool ExecuteRenderCommand(std::vector<RenderCommand>& cmdBuffer, uint16_t cmdIdx, U16 imageIdx);
        void CreateCommandBuffers();
        void RenderCmdUpdateUniformBuffer(RenderCommand cmd, DrawCallVK* dc);
        void RenderCmdUpdateUniformBufferObject(RenderCommand& cmd);
        void RecordRenderPass(RenderPassVK* pass, int i);
        CommandBufferVK* GetCommandBuffer(bool dynamic = false);
        void PushConstants(std::vector<RenderPassVK*>& passes);

        VkCommandPool subCommandPool;
    private:
        VulkanContext* vkCtxPtr;
        bool dynamicCommandBuffer;

        VkBufferCreateInfo bufferInfo{};
        VkMemoryRequirements memRequirements;
        VkMemoryAllocateInfo allocInfo{};
        U32 mipLevels;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_2_BIT;
        std::map<U32, TextureVK*> textureVKs;
        std::map<U32, TextureVK*> pendingTextureVKs;
        std::map<U32, ShaderVK*> shaderVKs;
        std::map<U32, UniformBufferVK*> uniformVKs;
        std::map<U32, VertexBufferVK*> vbs;
        std::map<U32, IndexBufferVK*> ibs;
        std::map<U32, FrameBufferVK*> fbs;
        //std::map<U32, PushConstsVK*> fbs;

        std::vector<RenderPassVK*> renderPassList;
        std::vector<RenderPassVK*> lastRenderPassList;

        std::vector<CommandBufferVK*> subCommandBuffers;

        U16 curImageIdx = 0;
        bool firstRecord;
    };

}