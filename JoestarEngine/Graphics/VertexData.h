#pragma once
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "VulkanHeader.h"
#include "GraphicDefines.h"
#include <vector>

namespace Joestar {
    class VertexBuffer {
    public:
        inline void SetFlag(uint32_t f) { flags = f; }
        uint32_t GetFlag() { return flags; }
        inline void SetBuffer(uint8_t* data) { buffer = data; }
        inline uint8_t* GetBuffer() { return buffer; }
        inline void SetSize(uint32_t s) { size = s; buffer = new uint8_t[size]; }
        inline uint32_t GetSize() { return size; }
        void AppendVertexAttr(VERTEX_ATTRIBUTE v);
        ~VertexBuffer() {
            delete buffer;
        }
        VkPipelineVertexInputStateCreateInfo* GetVKVertexInputInfo();
        VkVertexInputBindingDescription GetVKBindingDescription();
        std::vector<VkVertexInputAttributeDescription> GetVKAttributeDescriptions();
    private:
        uint8_t* buffer; //raw data
        uint32_t flags; //flag of vertex channel
        uint32_t size; //size of data
        std::vector<VERTEX_ATTRIBUTE> attrs; //size of vertex channels
        std::vector<VkVertexInputAttributeDescription>attributeDescriptions;
        VkVertexInputBindingDescription bindingDescription;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    };

    class IndexBuffer {
    public:
        inline void SetBuffer(uint8_t* data) { buffer = data; }
        inline uint8_t* GetBuffer() { return buffer; }
        inline void SetSize(uint32_t s) { size = s; buffer = new uint8_t[size]; }
        inline uint32_t GetSize() { return size; }
        inline uint32_t GetIndexCount() { return size / 2; }
        ~IndexBuffer() {
            delete buffer;
        }
    private:
        uint8_t* buffer;
        uint32_t size; //size of data
    };
}