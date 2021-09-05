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
#include "../Misc/Application.h"

//ez hash, i guess that's ok
#define REGISTER_HASH\
    U32 hash = 0;\
    void HashInsert(U32 i) {\
        hash *= 10;\
        hash += i;\
    }

namespace Joestar {
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& device);

    struct BufferVK {
        VulkanContext* ctx;
        VkDeviceSize size = 0;
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
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
                LOGERROR("failed to create buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(ctx->device, buffer, &memRequirements);
            memoryTypeIdx = FindMemoryType(memRequirements.memoryTypeBits, properties, ctx->physicalDevice);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = memoryTypeIdx;

            if (vkAllocateMemory(ctx->device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
                LOGERROR("failed to allocate buffer memory!");
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
        ~BufferVK() {
            Clean();
        }
    };

    struct CommandBufferVK {
        VulkanContext* ctx;
        VkCommandPool pool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer;
        void Begin() {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = pool == VK_NULL_HANDLE ? ctx->commandPool : pool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            if (vkAllocateCommandBuffers(ctx->device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
                LOGERROR("failed to allocate sub command buffer!\n");
            }
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

            vkFreeCommandBuffers(ctx->device, ctx->commandPool, 1, &commandBuffer);
        }
    };

    struct ShaderVK {
        explicit ShaderVK(Shader* s, VulkanContext* c) : shader(s), ctx(c) {
            for (auto& uniform : shader->info.uniforms) {
                if (uniform.dataType == ShaderDataTypePushConst) {
                    continue;
                } else if (uniform.IsSampler()) {
                    ubs.push_back(uniform);
                } else if (uniform.dataType == ShaderDataTypeBuffer) {
                    uniform.id = hashString(uniform.name.c_str());
                    ubs.push_back(uniform);
                } else {
                    uniform.id = hashString(uniform.name.c_str());
                    ubs.push_back(uniform);
                }
            }
            std::sort(ubs.begin(), ubs.end(), [&](UniformDef& a, UniformDef& b) {
                return a.binding < b.binding;
            });
        }
        std::string& GetName() { return shader->GetName(); }
        U16 GetUniformBindingByName(std::string& name) {
            return shader->GetUniformBindingByName(name);
        }
        U16 GetUniformBindingByHash(U32 hash) {
            return shader->GetUniformBindingByHash(hash);
        }
        U16 GetSamplerBinding(int count) {
            return shader->GetSamplerBinding(count);
        }
        U32 ID() { return shader->id; }
        std::string GetPushConsts() { return shader->GetPushConsts(); }
        Shader* shader;
        std::vector <VkShaderModule> shaderModules;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStage;
        bool operator ==(ShaderVK& s2) {
            return GetName() == s2.GetName();
        }
        void Clean(VkDevice& dev) {
            for (auto& modu : shaderModules)
                vkDestroyShaderModule(dev, modu, nullptr);
        }
        bool HasStage(ShaderStage stage) {
            return shader->flag & stage;
        }
        VkShaderModule CreateShaderModule(File* file) {
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            size_t codeSize = file->Size();
            createInfo.codeSize = codeSize;
            createInfo.pCode = reinterpret_cast<const uint32_t*>(file->GetBuffer());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(ctx->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                LOGERROR("failed to create shader module!");
            }
            return shaderModule;
        }

#define CHECK_ADD_SHADER_STAGE(STAGE, SUFFIX, BIT)\
        if (HasStage(STAGE)) {\
            std::string spvPath = std::string(GetName()) + SUFFIX + ".spv";\
            std::string compileSpvCmd = path + "glslc.exe " + (path + GetName() + "." + SUFFIX) + " -o " + (path + spvPath);\
            system(compileSpvCmd.c_str());\
            File* shaderCode = fs->GetShaderCodeFile(spvPath.c_str());\
            shaderModules.push_back(CreateShaderModule(shaderCode));\
            shaderStage.push_back(VkPipelineShaderStageCreateInfo{});\
            shaderStage.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;\
            shaderStage.back().stage = BIT;\
            shaderStage.back().module = shaderModules.back();\
            shaderStage.back().pName = "main";\
        }

        void Prepare() {
            Application* app = Application::GetApplication();
            FileSystem* fs = app->GetSubSystem<FileSystem>();
            std::string path = fs->GetShaderDirAbsolute();

            CHECK_ADD_SHADER_STAGE(kVertexShader, "vert", VK_SHADER_STAGE_VERTEX_BIT)
            CHECK_ADD_SHADER_STAGE(kFragmentShader, "frag", VK_SHADER_STAGE_FRAGMENT_BIT)
            CHECK_ADD_SHADER_STAGE(kComputeShader, "comp", VK_SHADER_STAGE_COMPUTE_BIT)
        }
        std::vector<UniformDef> ubs;
        std::vector<U32> textures;
        VulkanContext* ctx;
    };


    struct ImageVK {
        VulkanContext* ctx;
        U32 width;
        U32 height;
        U32 mipLevels = 1;
        VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
        VkImageType imageType = VK_IMAGE_TYPE_2D;

        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        U32 memoryTypeIdx;

        void Clean() {
            vkDestroyImageView(ctx->device, imageView, nullptr);
            vkDestroyImage(ctx->device, image, nullptr);
            vkFreeMemory(ctx->device, imageMemory, nullptr);
        }

        U32 GetLayerCount() {
            return viewType == VK_IMAGE_VIEW_TYPE_CUBE ? 6 : 1;
        }

        void Create() {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = imageType;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = mipLevels;
            imageInfo.arrayLayers = GetLayerCount();
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = numSamples;
            imageInfo.flags = viewType == VK_IMAGE_VIEW_TYPE_CUBE ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0; // Optional  
            // Cube faces count as array layers in Vulkan
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
         
        void GenerateMipmaps(CommandBufferVK& cb) {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(ctx->physicalDevice, format, &formatProperties);

            if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
                LOGERROR("texture image format does not support linear blitting!");
            }

            U32 faces = GetLayerCount();
            int mipWidth = width;
            int mipHeight = height;

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = faces;
            barrier.subresourceRange.levelCount = 1;
            for (U32 i = 1; i < mipLevels; i++) {
                barrier.subresourceRange.baseMipLevel = i - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(cb.commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrier);

                VkImageBlit blit{};
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = faces;
                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = faces;

                vkCmdBlitImage(cb.commandBuffer,
                    image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit,
                    VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(cb.commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier);
                if (mipWidth > 1) mipWidth /= 2;
                if (mipHeight > 1) mipHeight /= 2;
            }
            barrier.subresourceRange.baseMipLevel = mipLevels - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(cb.commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);
        }

        void CopyBufferToImage(BufferVK& buffer, CommandBufferVK& cb) {
            U32 face = GetLayerCount();
            VkBufferImageCopy region{};
            U32 offset = 0;
            
            region = {};
            region.bufferOffset = offset;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = face;

            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = {
                width,
                height,
                1
            };

            vkCmdCopyBufferToImage(cb.commandBuffer,buffer.buffer,image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1, &region);
        }

        bool HasStencilComponent(VkFormat format) {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        }

        void CreateImageView(VkImageAspectFlags aspectFlags, CommandBufferVK& cb) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = image;
            viewInfo.viewType = viewType;
            viewInfo.format = format;
            viewInfo.subresourceRange.aspectMask = aspectFlags;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = GetLayerCount();
            if (vkCreateImageView(ctx->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
                LOGERROR("failed to create texture image view!");
            }
        }

        void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, CommandBufferVK& cb) {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = mipLevels;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = GetLayerCount();

            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;
            if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

                if (HasStencilComponent(format)) {
                    barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
            }
            else {
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            }

            if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            }
            else {
                LOGERROR("unsupported layout transition!");
            }

            vkCmdPipelineBarrier(
                cb.commandBuffer,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );
        }
    };


    struct FrameBufferVK {
        VulkanContext* ctx;
        ImageVK* depthImage;
        ImageVK* colorImage;
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

    class VertexBufferVK {
    public:
        explicit VertexBufferVK(VertexBuffer* b, VulkanContext* ctx) {
            vb = b; buffer.ctx = ctx; instance = b->instanceCount > 1; binding = instance ? 1 : 0;
        }
        VertexBuffer* vb;
        U32 ID() {
            return vb->id;
        }
        U32 GetSize() {
            return vb->GetSize();
        }
        U8* GetBuffer() {
            return vb->GetBuffer();
        }
        U32 GetVertexCount() { return vb->GetVertexCount(); }
        U32 GetInstanceCount() { return static_cast<InstanceBuffer*>(vb)->GetCount(); }
        VkVertexInputBindingDescription GetBindingDescription();
        void Clean() {
            buffer.Clean();
        }
        BufferVK buffer;
        bool instance = false;
        U32 binding = 0;

    private:
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        VkVertexInputBindingDescription bindingDescription;

    };

    class IndexBufferVK {
    public:
        explicit IndexBufferVK(IndexBuffer* b, VulkanContext* ctx) { ib = b; buffer.ctx = ctx;}
        U32 GetIndexCount() { return ib->GetIndexCount(); }
        IndexBuffer* ib;
        U32 ID() { return ib->id; }
        U32 GetSize() { return ib->GetSize(); }
        U8* GetBuffer() { return ib->GetBuffer(); }
        BufferVK buffer;
        void Clean() {
            buffer.Clean();
        }
    };

    struct UniformBufferVK {
        std::vector<BufferVK> buffers;
        U32 size;
        void* data;
        U32 texID = 0;
        UniformDef def;
        U32 id = 0;

        U32 ID() {
            return id;
        }

        void Clean(VkDevice& dev) {
            for (auto& buf: buffers) {
                vkDestroyBuffer(dev, buf.buffer, nullptr);
                vkFreeMemory(dev, buf.memory, nullptr);
            }
            buffers.clear();
        }

        bool IsSampler() {
            return def.IsSampler();
        }

        bool IsBuffer() {
            return def.IsBuffer();
        }

        bool IsUniform() {
            return def.IsUniform();
        }


        VkDescriptorType GetDescriptorType() {
            if (IsSampler()) return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            if (IsBuffer()) return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }

        VkShaderStageFlags GetStageFlags() {
            VkShaderStageFlags flags;
            if (def.stageFlag & kVertexShader) {
                flags |= VK_SHADER_STAGE_VERTEX_BIT;
            }
            if (def.stageFlag & kFragmentShader) {
                flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            }
            if (def.stageFlag & kComputeShader) {
                flags |= VK_SHADER_STAGE_COMPUTE_BIT;
            }
            return flags;
        }
    };

    struct PipelineStateVK {
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        REGISTER_HASH
    };

    struct PushConstsVK : PushConsts {
        bool operator ==(PushConstsVK& p2) { return model == p2.model; }
        bool operator !=(PushConstsVK& p2) { return !(*this == p2); }
    };

    struct DrawCallVK {
        std::vector<VertexBufferVK*> vbs{1};
        IndexBufferVK* ib = nullptr;
        ShaderVK* shader;
        MeshTopology topology;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        //PipelineStateVK* pso;
        PushConstsVK* pc = nullptr;
        VkCompareOp depthOp = VK_COMPARE_OP_LESS;
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSets;
        VkDescriptorSetLayout descriptorSetLayout;
        VkSampler textureSampler;
        U32 instanceCount = 0;
        REGISTER_HASH

        VkPipelineVertexInputStateCreateInfo& GetVertexInputInfo() {
            bindingDescriptions.resize(vbs.size());

            U32 vaSize = 0, idx = 0;
            for (int i = 0; i < vbs.size(); ++i) {
                U32 offset = 0;
                bindingDescriptions[i] = vbs[i]->GetBindingDescription();
                attributeDescriptions.resize(vbs[i]->vb->attrs.size() + attributeDescriptions.size());
                for (auto& attr : vbs[i]->vb->attrs) {
                    attributeDescriptions[idx].binding = i;
                    attributeDescriptions[idx].location = idx;
                    attributeDescriptions[idx].offset = offset;
                    vaSize = VERTEX_ATTRIBUTE_SIZE[attr];
                    switch (vaSize) {
                    case 3: attributeDescriptions[idx].format = VK_FORMAT_R32G32B32_SFLOAT; offset += 12; break;
                    case 2: attributeDescriptions[idx].format = VK_FORMAT_R32G32_SFLOAT; offset += 8;  break;
                    default:break;
                    }
                    ++idx;
                }
            }
            	
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<U32>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
            return vertexInputInfo;
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
        REGISTER_HASH
    };

    struct ComputeContextVK {
        VulkanContext* ctx;
        VkQueue queue;
        VkCommandPool commandPool;
        CommandBufferVK commandBuffer;
        VkSemaphore semaphore;
        ComputeContextVK(VulkanContext* c) : ctx(c) {
            queue = c->computeQueue;

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.queueFamilyIndex = ctx->queueFamilyIndices.computeFamily;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
            //create sub command pool
            if (vkCreateCommandPool(ctx->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                LOGERROR("failed to create command pool!");
            }
            commandBuffer = { c, commandPool };
        }
    };

    struct ComputeBufferVK : UniformBufferVK {
        ComputeBuffer* computeBuffer;
        ComputeBufferVK(ComputeBuffer* cb) : computeBuffer(cb) {

        }
    };

    struct ComputePipelineVK {
        ComputeContextVK* ctx;
        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;
        VkDescriptorPool descriptorPool;
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;
        ShaderVK* shader;
        VkSampler textureSampler;
        std::vector<UniformBufferVK*> computeBuffers;
        REGISTER_HASH

        void CreatePipeline() {
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1; // Optional
            VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
            pipelineLayoutInfo.pSetLayouts = layouts; // Optional


        }
    };

    class GPUProgramVulkan : public GPUProgram {
    public:
        GPUProgramVulkan();
        //VkPipelineVertexInputStateCreateInfo* GetVertexInputInfo();
        void SetDevice(VulkanContext* ctx) { vkCtxPtr = ctx; }
        void CreateVertexBuffer(VertexBufferVK* vb);
        void CreateIndexBuffer(IndexBufferVK* ib);
        void CopyBuffer(BufferVK& srcBuffer, BufferVK& dstBuffer, VkDeviceSize size);
        void CreateTextureImage(TextureVK* tex);
        void CreateTextureSampler(DrawCallVK*);

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void Clean();
        void CreateRenderPass(RenderPassVK* pass);
        void CreateGraphicsPipeline(RenderPassVK* pass, int i);
        void GetPipeline(RenderPassVK* pass, int i);
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat FindDepthFormat();
        void CleanupSwapChain();
        void CreateDepthResources(RenderPassVK* pass);
        void CreateColorResources(RenderPassVK* pass);
        void CreateUniformBuffers(UniformBufferVK* ub);
        void CreateComputeBuffers(ComputeBufferVK* ub);
        void CreateFrameBuffers(RenderPassVK* pass);
        void CreateDescriptorPool(DrawCallVK* dc);

        template <class T>
        void CreateDescriptorPool(T* call);
        template <class T>
        void CreateDescriptorSets(T* call);
        template <class T>
        void UpdateDescriptorSets(T* call);
        template <class T>
        void CreateDescriptorSetLayout(T* call);
        void UpdateUniformBuffer(uint32_t currentImage);
        void RecordCommandBuffer(std::vector<RenderPassVK*>&);
        bool ExecuteRenderCommand(std::vector<RenderCommand>& cmdBuffer, uint16_t cmdIdx, U16 imageIdx);
        bool ExecuteComputeCommand(std::vector<ComputeCommand>& cmdBuffer, uint16_t cmdIdx);
        void CreateCommandBuffers();
        void RenderCmdUpdateUniformBuffer(RenderCommand cmd, DrawCallVK* dc);
        void RenderCmdUpdateUniformBufferObject(RenderCommand& cmd);
        void RecordRenderPass(RenderPassVK* pass, int i);
        CommandBufferVK* GetCommandBuffer(bool dynamic = false);
        void PushConstants(std::vector<RenderPassVK*>& passes);
        void PrepareCompute(ComputePipelineVK* compute);
        template<class T>
        void CreatePipelineLayout(T* call);

        VkCommandPool subCommandPool;
    private:
        VulkanContext* vkCtxPtr;
        ComputeContextVK* computeCtx;
        bool dynamicCommandBuffer;
        bool hasCompute = false;

        VkBufferCreateInfo bufferInfo{};
        VkMemoryRequirements memRequirements;
        VkMemoryAllocateInfo allocInfo{};
        U32 mipLevels;
        std::map<U32, TextureVK*> textureVKs;
        std::map<U32, TextureVK*> pendingTextureVKs;
        std::map<U32, ShaderVK*> shaderVKs;
        std::map<U32, UniformBufferVK*> uniformVKs;
        std::map<U32, VertexBufferVK*> vbs;
        std::map<U32, IndexBufferVK*> ibs;
        std::map<U32, FrameBufferVK*> fbs;
        std::map<U32, ComputeBufferVK*> cbs;
        std::map<U32, ComputePipelineVK*> computePipelines;
        //std::map<U32, PushConstsVK*> fbs;

        std::vector<RenderPassVK*> renderPassList;
        std::vector<RenderPassVK*> lastRenderPassList;

        std::vector<CommandBufferVK*> subCommandBuffers;

        U16 curImageIdx = 0;
        bool firstRecord;
    };

    template <class T>
    void GPUProgramVulkan::CreateDescriptorPool(T* call) {
        //already exist
        if (call->descriptorPool != VK_NULL_HANDLE) return;
        std::vector<VkDescriptorPoolSize> poolSizes;
        poolSizes.resize(call->shader->ubs.size());
        for (int i = 0; i < call->shader->ubs.size(); ++i) {
            UniformBufferVK* ubvk = uniformVKs[call->shader->ubs[i].id];
            poolSizes[i].type = ubvk->GetDescriptorType();
            poolSizes[i].descriptorCount = static_cast<U32>(vkCtxPtr->swapChainImages.size());
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<U32>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<U32>(vkCtxPtr->swapChainImages.size());

        if (vkCreateDescriptorPool(vkCtxPtr->device, &poolInfo, nullptr, &call->descriptorPool) != VK_SUCCESS) {
            LOGERROR("failed to create descriptor pool!");
        }
    }

    template <class T>
    void GPUProgramVulkan::CreateDescriptorSets(T* call) {
        std::vector<VkDescriptorSetLayout> layouts(vkCtxPtr->swapChainImages.size(), call->descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = call->descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(vkCtxPtr->swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        call->descriptorSets.resize(vkCtxPtr->swapChainImages.size());
        if (vkAllocateDescriptorSets(vkCtxPtr->device, &allocInfo, call->descriptorSets.data()) != VK_SUCCESS) {
            LOGERROR("failed to allocate descriptor sets!");
        }

        UpdateDescriptorSets(call);
    }

    template <class T>
    void GPUProgramVulkan::UpdateDescriptorSets(T* call) {
        for (size_t i = 0; i < vkCtxPtr->swapChainImages.size(); ++i) {
            std::vector<VkWriteDescriptorSet> descriptorWrites{};
            descriptorWrites.resize(call->shader->ubs.size());
            int samplerCount = 0;
            for (int j = 0; j < call->shader->ubs.size(); ++j) {
                UniformBufferVK* ub = uniformVKs[call->shader->ubs[j].id];
                if (ub->def.IsSampler()) {
                    VkDescriptorImageInfo imageInfo{};
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    std::map<uint32_t, TextureVK*>::iterator it = textureVKs.find(call->shader->textures[0]);
                    TextureVK* tex;
                    if (it == textureVKs.end()) {
                        it = pendingTextureVKs.find(call->shader->textures[0]);
                        if (it == pendingTextureVKs.end()) {
                            LOGERROR("this texture didn't call Graphics::UpdateTexture!!!");
                        }
                        CreateTextureImage(it->second);
                        textureVKs[it->first] = it->second;
                        tex = it->second;
                        pendingTextureVKs.erase(it);
                    }
                    else {
                        tex = it->second;
                    }
                    imageInfo.imageView = tex->image->imageView;
                    imageInfo.sampler = call->textureSampler;

                    descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrites[j].dstSet = call->descriptorSets[i];
                    descriptorWrites[j].dstBinding = call->shader->GetSamplerBinding(samplerCount++);
                    descriptorWrites[j].dstArrayElement = 0;
                    descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrites[j].descriptorCount = 1;
                    descriptorWrites[j].pImageInfo = &imageInfo;
                } else {
                    VkDescriptorBufferInfo bufferInfo{};
                    bufferInfo.buffer = ub->buffers[i].buffer;
                    bufferInfo.offset = 0;
                    bufferInfo.range = ub->size;// sizeof(UniformBufferObject);

                    descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrites[j].dstSet = call->descriptorSets[i];
                    descriptorWrites[j].dstBinding = call->shader->GetUniformBindingByName(ub->def.name);
                    descriptorWrites[j].dstArrayElement = 0;
                    descriptorWrites[j].descriptorType = ub->IsBuffer() ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    descriptorWrites[j].descriptorCount = 1;
                    descriptorWrites[j].pBufferInfo = &bufferInfo;
                }
            }
            vkUpdateDescriptorSets(vkCtxPtr->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
            //descriptorWrites.clear();
        }
    }

    template <class T>
    void GPUProgramVulkan::CreateDescriptorSetLayout(T* call) {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(call->shader->ubs.size());
        for (int i = 0; i < call->shader->ubs.size(); ++i) {
            UniformBufferVK* ubvk = uniformVKs[call->shader->ubs[i].id];
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = i;
            layoutBinding.descriptorType = ubvk->GetDescriptorType();// VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                //need to know shader stage in shader parser, temp write, --todo
            layoutBinding.stageFlags = ubvk->GetStageFlags();// VK_SHADER_STAGE_FRAGMENT_BIT;
            layoutBinding.descriptorCount = 1;
            layoutBinding.pImmutableSamplers = nullptr;

            bindings.push_back(layoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(vkCtxPtr->device, &layoutInfo, nullptr, &(call->descriptorSetLayout)) != VK_SUCCESS) {
            LOGERROR("failed to create descriptor set layout!");
        }
    }

    template<class T>
    void GPUProgramVulkan::CreatePipelineLayout(T* call) {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1; // Optional
        VkDescriptorSetLayout layouts[] = { call->descriptorSetLayout };
        pipelineLayoutInfo.pSetLayouts = layouts; // Optional

        std::string pushConstsName = call->shader->GetPushConsts();
        if (!pushConstsName.empty()) {
            VkPushConstantRange pushConstantRange{};
            pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            pushConstantRange.offset = 0;
            pushConstantRange.size = sizeof(PushConsts);
            pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
            pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Optional
            if (vkCreatePipelineLayout(vkCtxPtr->device, &pipelineLayoutInfo, nullptr, &(call->pipelineLayout)) != VK_SUCCESS) {
                LOGERROR("failed to create pipeline layout!");
            }
        }
        else {
            if (vkCreatePipelineLayout(vkCtxPtr->device, &pipelineLayoutInfo, nullptr, &(call->pipelineLayout)) != VK_SUCCESS) {
                LOGERROR("failed to create pipeline layout!");
            }
        }
    }
}