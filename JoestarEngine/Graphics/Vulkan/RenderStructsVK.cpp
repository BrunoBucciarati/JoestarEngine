#include "RenderStructsVK.h"

namespace Joestar {
    U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& device) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        LOGERROR("failed to find suitable memory type!");
    }
}