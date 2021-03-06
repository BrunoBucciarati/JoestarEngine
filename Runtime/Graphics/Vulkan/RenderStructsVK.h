#pragma once
#include "../../IO/Log.h"
#include "../../Platform/Platform.h"
#include "../../Container/Vector.h"
#include "../GraphicDefines.h"
#include "../Shader/ShaderReflection.h"
#include "RenderEnumsVK.h"
namespace Joestar {

#define VK_CHECK(f) \
	if (VK_SUCCESS != f) {LOGERROR("[VK_ERROR:%d]%s\n", f, #f)};

    U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& device);

    class DescriptorSetLayoutVK;
    class StagingBufferVK;
    class CommandBufferVK;

    class BufferVK
    {
    public:
        void SetDevice(VkDevice& dv, VkPhysicalDevice& pd)
        {
            device = dv;
            physicalDevice = pd;
        }
        Vector<VkDeviceMemory> memorys;
        Vector<VkBuffer> buffers;
        VkDeviceSize size;
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties{ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
        VkMemoryRequirements requirements;
        U32 memoryTypeIdx;
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        U32 count{ 1 };
        U32 index{ 0 };

        void SetFrame(U32 idx);

        void CreateBuffer()
        {
            buffers.Resize(count);
            memorys.Resize(count);

            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            for (U32 i = 0; i < count; ++i)
                VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &buffers[i]));

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(device, buffers[0], &memRequirements);
            memoryTypeIdx = FindMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = memoryTypeIdx;

            for (U32 i = 0; i < count; ++i)
            {
                VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &memorys[i]));
                VK_CHECK(vkBindBufferMemory(device, buffers[i], memorys[i], 0));
            }

        }

        VkBuffer GetBuffer()
        {
            return buffers[index];
        }

        void SetSize(U32 sz)
        {
            size = sz;
        }

        void SetData(U8* cpuData)
        {
            void* data;
            vkMapMemory(device, memorys[index], 0, size, 0, &data);
            memcpy(data, cpuData, size);
            vkUnmapMemory(device, memorys[index]);
        }

        void CopyBuffer(VkCommandBuffer cb);
        void CreateStagingBuffer(U32 size, U8* data);
        void UpdateStagingBuffer(U32 size, U8* data);
    private:
        UniquePtr<StagingBufferVK> stagingBuffer;
    };

    class VertexBufferVK : public BufferVK
    {
    public:
        VertexBufferVK()
        {
            usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        void Create(U32 sz, U32 ct, PODVector<VertexElement>& elements);

        void SetVertexElements(PODVector<VertexElement>& elements);

        VkPipelineVertexInputStateCreateInfo& GetInputStateCreateInfo()
        {
            return createInfo;
        }

    private:
        Vector<VkVertexInputAttributeDescription> attributeDescriptions;
        VkVertexInputBindingDescription bindingDescription;
        VkPipelineVertexInputStateCreateInfo createInfo{};
        U32 count;
        U32 binding{ 0 };
    };

    class IndexBufferVK : public BufferVK
    {
    public:
        IndexBufferVK()
        {
            usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        void Create(U32 sz, U32 ct)
        {
            count = ct;
            if (sz == 2)
                type = VK_INDEX_TYPE_UINT16;
            else
                type = VK_INDEX_TYPE_UINT32;
            size = sz * ct;
            CreateBuffer();
        }
        VkIndexType GetIndexType()
        {
            return type;
        }
    private:
        U32 count;
        VkIndexType type;
    };

    class UniformBufferVK : public BufferVK
    {
    public:
        UniformBufferVK()
        {
            usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        void Create(U32 sz)
        {
            size = sz;
            CreateBuffer();

            bufferInfos.Resize(count);
            for (U32 i = 0; i < count; ++i) {
                bufferInfos[i].buffer = buffers[i];
                bufferInfos[i].offset = 0;
                bufferInfos[i].range = size;
            }
        }

        VkDescriptorBufferInfo& GetDescriptorBufferInfo(U32 idx) {
            return bufferInfos[idx];
        }
        
        Vector<VkDescriptorBufferInfo> bufferInfos{};
        //Vector<VkDescriptorImageInfo> imageInfos{};
    };

    class StagingBufferVK : public BufferVK
    {
    public:
        StagingBufferVK()
        {
            usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }
        void Create(U8* data)
        {
            CreateBuffer();
            if (data)
                SetData(data);
        }
    };

    class CommandPoolVK
    {
    public:
        VkCommandPool pool;
    };

	class CommandBufferVK
	{
	public:
        static VkCommandBufferAllocateInfo GetDefaultAllocInfo()
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;
            return allocInfo;
        }
        void Create(VkDevice& inDevice, VkCommandBufferAllocateInfo& allocInfo, U32 num = 1);

		void Begin(U32 idx = 0)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(commandBuffers[idx], &beginInfo) != VK_SUCCESS) {
				LOGERROR("failed to begin recording command buffer!");
			}
            SetIndex(idx); 
		}

		void End(U32 idx = 0)
		{
			if (vkEndCommandBuffer(commandBuffers[idx]) != VK_SUCCESS) {
				LOGERROR("failed to record command buffer!");
			}
		}

        void Submit(VkPipelineStageFlags waitMask = 0, VkFence fence = VK_NULL_HANDLE) {
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pWaitDstStageMask = &waitMask;
            submitInfo.pCommandBuffers = &GetCommandBuffer(index);

            vkQueueSubmit(queue, 1, &submitInfo, fence);
            vkQueueWaitIdle(queue);
            if (bTemp)
                vkFreeCommandBuffers(device, pool, 1, &GetCommandBuffer(index));
        }

        VkCommandBuffer& GetCommandBuffer(U32 idx = 0)
        {
            return commandBuffers[idx];
        }

        void SetQueue(VkQueue& q)
        {
            queue = q;
        }

        void SetIndex(U32 idx)
        {
            index = idx;
        }

        void SetTemp(bool temp)
        {
            bTemp = temp;
        }

	private:
		Vector<VkCommandBuffer> commandBuffers;
		bool bCreated{ false };
        VkDevice device;
        VkCommandPool pool;
        VkQueue queue;
        U32 index{ 0 };
        bool bTemp{ false };
	};

	class ImageVK
	{
	public:
        void Create(VkDevice& device, VkImageCreateInfo& imageInfo, U32 num = 1);

        void AllocMemory(VkDevice& device, U32 memoryTypeIdx);

		VkImage& GetImage(U32 idx = 0)
		{
			return images[idx];
		}
		VkMemoryRequirements memRequirements;

		void SetRawImages(Vector<VkImage>& imgs)
		{
			images = imgs;
		}
        void SetStagingBuffer(StagingBufferVK* buffer)
        {
            stagingBuffer = buffer;
        }
        void CopyBufferToImage(VkCommandBuffer& cb, ImageLayout layout);
        void TransitionImageLayout(VkCommandBuffer& cb, ImageLayout oldLayout, ImageLayout newLayout, U32 aspectFlags);
        void TransitionImageLayout(VkCommandBuffer& cb, VkImageLayout oldLayout, VkImageLayout newLayout, U32 aspectFlags);

	private:
        Vector<VkImage> images;
        Vector<VkDeviceMemory> imageMemorys{};
        UniquePtr<StagingBufferVK> stagingBuffer;
        U32 layers;
        U32 mipLevels;
        VkExtent3D extent;
        VkImageLayout finalLayout;
        VkFormat format;
	};

	class ImageViewVK
	{
	public:
		void Create(VkDevice& device, VkImageViewCreateInfo& viewInfo, U32 num = 1)
		{
			imageViews.Resize(num);
			for (U32 i = 0; i < num; ++i)
			{
                viewInfo.image = image->GetImage(i);
				if (vkCreateImageView(device, &viewInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
					LOGERROR("failed to create texture image view!");
				}
			}
		}

        VkImageView GetImageView(U32 idx = 0)
        {
            return imageViews[idx];
        }

        void SetRawImageViews(Vector<VkImageView>& imgViews)
        {
            imageViews = imgViews;
        }
        ImageVK* GetImage()
        {
            return image;
        }
        void SetImage(ImageVK* img)
        {
            image = img;
        }
        void CreateRawImage()
        {
            image = JOJO_NEW(ImageVK, MEMORY_GFX_STRUCT);
        }
        VkImageLayout GetImageLayout()
        {
            return imageLayout;
        }
    private:
        ImageVK* image;
        VkImageLayout imageLayout{VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        Vector<VkImageView> imageViews;
	};

	class RenderPassVK
	{
	public:
		VkRenderPass renderPass;
	};

	class FrameBufferVK
	{
	public:
		Vector<ImageViewVK*> colorAttachments;
		ImageViewVK* depthStencilAttachment;
		Vector<VkFramebuffer> frameBuffers;
		U32 msaaSamples{ 1 };
		void SetRawImages(Vector<VkImage>& images, U32 idx = 0)
		{
            if (!colorAttachments[idx]->GetImage())
                colorAttachments[idx]->CreateRawImage();
			colorAttachments[idx]->GetImage()->SetRawImages(images);
		}
		void SetRawImageViews(Vector<VkImageView>& imageViews, U32 idx = 0)
		{
			colorAttachments[idx]->SetRawImageViews(imageViews);
		}
        VkFramebuffer GetFrameBuffer(U32 idx = 0)
        {
            return frameBuffers[idx];
        }
		RenderPassVK* renderPass;
	};

	class SwapChainVK
	{
	public:
		VkFormat format;
		VkSwapchainKHR swapChain;
		VkExtent2D extent;
		Vector<VkImage> images;
		Vector<VkImageView> imageViews;
		FrameBufferVK* frameBuffer{nullptr};
		U32 GetImageCount()
		{
			return images.Size();
		}
	};


    class ShaderVK
    {
    public:
        void Create(VkDevice& device, GPUShaderCreateInfo& shader);
        VkShaderModule shaderModule;
        VkShaderStageFlagBits flagBits;
        String entryName = "main";
    };

    class PipelineLayoutVK
    {
    public:
        void Create(VkDevice& device, PODVector<VkDescriptorSetLayout>& setLayouts);
        VkPipelineLayout layout;
        VkPipelineBindPoint GetBindPoint()
        {
            return bindPoint;
        }
    private:
        VkPipelineBindPoint bindPoint{ VK_PIPELINE_BIND_POINT_GRAPHICS };
    };

    class GraphicsPipelineStateVK
    {
    public:
        void CreateIAState();

        void CreateShaderModule();

        void CreateViewportState(Viewport& vp);

        void CreateRasterizationState(GPURasterizationStateCreateInfo& createInfo);

        void CreateMultiSampleState(GPUMultiSampleStateCreateInfo& createInfo);

        void CreateDepthStencilState(GPUDepthStencilStateCreateInfo& createInfo);

        void CreateColorBlendState(GPUColorBlendStateCreateInfo& createInfo);

        void SetPipelineLayout(VkPipelineLayout layout);

        void CreateShaderStages(PODVector<ShaderVK*>& shaders);

        void CreateVertexInputInfo(PODVector<InputBinding>& bindings, PODVector<InputAttribute>& attributes);
        void CreateTessellationState(U32 controlPoints = 0);

        void SetRenderPass(RenderPassVK* rp)
        {
            renderPass = rp->renderPass;
        }

        void Create(VkDevice&);

        VkPipeline GetPipeline()
        {
            return pipeline;
        }
    private:
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        //Color Blend State
        Vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        Vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        Vector<VkVertexInputBindingDescription> vertexInputBindings{};
        Vector<VkVertexInputAttributeDescription> vertexInputAttributes{};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        VkPipelineTessellationStateCreateInfo tessellationInfo{};
        VkRenderPass renderPass{};
        Vector<VkDescriptorSetLayout> setLayouts;
        //Viewport
        VkViewport viewport{};
        VkRect2D scissor{};
        VkPipelineViewportStateCreateInfo viewportState{};
        VkPipelineLayout pipelineLayout{};
        VkPipeline pipeline{};
    };

    class ComputePipelineStateVK
    {
    public:
        void Create(GPUComputePipelineStateCreateInfo& info)
        {

        }

        VkPipeline GetPipeline()
        {
            return pipeline;
        }
    private:
        VkPipeline pipeline{};
    };

    class DescriptorSetLayoutVK
    {
    public:
        void Create(VkDevice&, PODVector<GPUDescriptorSetLayoutBinding>& bindings);
        VkDescriptorSetLayout setLayout;
    };

    class DescriptorSetsVK
    {
    public:
        void Create(VkDevice&, VkDescriptorPool&, PODVector<VkDescriptorSetLayout>& layout);
        VkDescriptorSet& GetDescriptorSets(U32 idx)
        {
            return sets[idx];
        }
        U32 Size()
        {
            return sets.Size();
        }
    private:
        Vector<VkDescriptorSet> sets;
    };

    class SamplerVK
    {
    public:
        void Create(VkDevice&, GPUSamplerCreateInfo&);
        VkSampler GetSampler()
        {
            return sampler;
        }
    private:
        VkSampler sampler;
    };

    class TextureVK
    {
    public:
        void Create(ImageViewVK* imageView, SamplerVK* sampler);
        VkDescriptorImageInfo& GetDescriptorImageInfo(U32 idx = 0)
        {
            return imageInfo;
        }
    private:
        VkDescriptorImageInfo imageInfo{};
    };
}