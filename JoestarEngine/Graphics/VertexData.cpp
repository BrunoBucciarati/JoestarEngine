#include "VertexData.h"
namespace Joestar {
	void VertexBuffer::AppendVertexAttr(VERTEX_ATTRIBUTE v) {
		flags = flags | 1 << v;
		attrs.push_back(v);
	}

	VkVertexInputBindingDescription VertexBuffer::GetVKBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		uint32_t stride = 0;
		for (int i = 0; i < attrs.size(); ++i) {
			stride += VERTEX_ATTRIBUTE_SIZE[attrs[i]] * sizeof(float);
		}
		bindingDescription.stride = stride;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	std::vector<VkVertexInputAttributeDescription> VertexBuffer::GetVKAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.resize(attrs.size());

		uint32_t vaSize = 0, offset = 0;
		for (int i = 0; i < attrs.size(); ++i) {
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].location = i;
			attributeDescriptions[i].offset = offset;
			vaSize = VERTEX_ATTRIBUTE_SIZE[attrs[i]];
			switch (vaSize) {
			case 3: attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT; offset += 12; break;
			case 2: attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT; offset += 8;  break;
			default:break;
			}
		}

		return attributeDescriptions;
	}
	VkPipelineVertexInputStateCreateInfo* VertexBuffer::GetVKVertexInputInfo() {
		//bindingDescription = Vertex::GetBindingDescription();
		//attributeDescriptions = Vertex::GetAttributeDescriptions();
		//std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		//VkVertexInputBindingDescription bindingDescription{};
		bindingDescription = GetVKBindingDescription();
		attributeDescriptions = GetVKAttributeDescriptions();
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		return &vertexInputInfo;
	}

}