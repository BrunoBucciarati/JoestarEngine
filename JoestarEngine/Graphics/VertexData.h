#pragma once
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "VulkanHeader.h"
#include "GraphicDefines.h"
#include <vector>

namespace Joestar {
    class VertexBuffer {
    public:
        inline uint8_t* GetBuffer() { return buffer; }
        inline void SetFlag(uint32_t f) { flags = f; }
        inline void SetBuffer(uint8_t* data) { buffer = data; }
        inline void SetSize(uint32_t s) { size = s; }
        inline uint32_t GetSize() { return size; }
        void AppendVertexAttr(VERTEX_ATTRIBUTE v);
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
    struct Vertex {
        Vector3f pos;
        Vector3f color;
        Vector2f texCoord;
        static VkVertexInputBindingDescription GetBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        //static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
        //    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        //    attributeDescriptions[0].binding = 0;
        //    attributeDescriptions[0].location = 0;
        //    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        //    attributeDescriptions[0].offset = offsetof(Vertex, pos);

        //    attributeDescriptions[1].binding = 0;
        //    attributeDescriptions[1].location = 1;
        //    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        //    attributeDescriptions[1].offset = offsetof(Vertex, color);

        //    attributeDescriptions[2].binding = 0;
        //    attributeDescriptions[2].location = 2;
        //    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        //    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        //    return attributeDescriptions;
        //}
    };
}