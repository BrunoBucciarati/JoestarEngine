#include "GPUProgramVulkan.h"
#include "../IO/Log.h"
#include "../Misc/Application.h"
#include <array>
#include <direct.h>

namespace Joestar {
	VkShaderModule GPUProgramVulkan::CreateShaderModule(File* file) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		size_t codeSize = file->Size();
		createInfo.codeSize = codeSize;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(file->GetBuffer());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(vkCtxPtr->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			LOGERROR("failed to create shader module!");
		}
		return shaderModule;
	}
	void GPUProgramVulkan::SetShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
		Application* app = Application::GetApplication();
		FileSystem* fs = app->GetSubSystem<FileSystem>();
		std::string path = fs->GetResourceDir();
		path += "Shaders/";
		char workDir[260];
		if (_getcwd(workDir, 260))
			path = workDir + ("/" + path);

		//First Compile To Spir-V
		std::string vertSpvPath = std::string(vertexPath) + ".spv";
		std::string fragSpvPath = std::string(fragmentPath) + ".spv";
		std::string compileVertSpv = path + "glslc.exe " + (path + vertexPath) + " -o " + (path + vertSpvPath);
		std::string compileFragSpv = path + "glslc.exe " + (path + fragmentPath) + " -o " + (path + fragSpvPath);
		system(compileVertSpv.c_str());
		system(compileFragSpv.c_str());
		File* vShaderCode = ShaderCodeFile(vertSpvPath.c_str());
		File* fShaderCode = ShaderCodeFile(fragSpvPath.c_str());

		vertShaderModule = CreateShaderModule(vShaderCode);
		fragShaderModule = CreateShaderModule(fShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		//VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
		mShaderStage[0] = vertShaderStageInfo;
		mShaderStage[1] = fragShaderStageInfo;
	}

	void GPUProgramVulkan::Clean() {
		vkDestroyShaderModule(vkCtxPtr->device, vertShaderModule, nullptr);
		vkDestroyShaderModule(vkCtxPtr->device, fragShaderModule, nullptr);
		vkDestroyBuffer(vkCtxPtr->device, vertexBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, vertexBufferMemory, nullptr);
		vkDestroyBuffer(vkCtxPtr->device, indexBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, indexBufferMemory, nullptr);
	}

	VkPipelineVertexInputStateCreateInfo* GPUProgramVulkan::GetVertexInputInfo() {
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		return &vertexInputInfo;
	}

	void GPUProgramVulkan::CreateVertexBuffer() {

		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(vkCtxPtr->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(vkCtxPtr->device, stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);
		CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(vkCtxPtr->device, stagingBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, stagingBufferMemory, nullptr);
	}

	void GPUProgramVulkan::CreateIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(vkCtxPtr->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(vkCtxPtr->device, stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(vkCtxPtr->device, stagingBuffer, nullptr);
		vkFreeMemory(vkCtxPtr->device, stagingBufferMemory, nullptr);
	}

	uint32_t GPUProgramVulkan::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(vkCtxPtr->physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}


		LOGERROR("failed to find suitable memory type!");
	}

	void GPUProgramVulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(vkCtxPtr->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vkCtxPtr->device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(vkCtxPtr->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(vkCtxPtr->device, buffer, bufferMemory, 0);
	}

	void GPUProgramVulkan::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vkCtxPtr->commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(vkCtxPtr->device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(vkCtxPtr->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(vkCtxPtr->graphicsQueue);
	}
}
//#include "../IO/Log.h"
//#define CREATE_SHADER_GL(HANDLE, CODE)\
//    glShaderSource(HANDLE, 1, &CODE, NULL);\
//    glCompileShader(HANDLE);\
//    glGetShaderiv(HANDLE, GL_COMPILE_STATUS, &success);\
//    if (!success)\
//    {\
//        glGetShaderInfoLog(HANDLE, 512, NULL, infoLog);\
//        LOGERROR("ERROR::GLSHADER::COMPILATION_FAILED\n")\
//    };\
//
//void GPUProgramGL::SetShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
//{
//    const char* vShaderCode = ShaderCodeFromFile(vertexPath);
//    const char* fShaderCode = ShaderCodeFromFile(fragmentPath);
//    GLuint vertex, fragment, geoShader;
//    int success;
//    char infoLog[512];
//    vertex = glCreateShader(GL_VERTEX_SHADER);
//    fragment = glCreateShader(GL_FRAGMENT_SHADER);
//    CREATE_SHADER_GL(vertex, vShaderCode);
//    CREATE_SHADER_GL(fragment, fShaderCode);
//  
//
//    ID = glCreateProgram();
//    glAttachShader(ID, vertex);
//    glAttachShader(ID, fragment);
//    if (geometryPath) {
//        const char* gShaderCode = ShaderCodeFromFile(geometryPath);
//        geoShader = glCreateShader(GL_GEOMETRY_SHADER);
//        CREATE_SHADER_GL(geoShader, gShaderCode);
//        glAttachShader(ID, geoShader);
//    }
//    glLinkProgram(ID);
//    glGetProgramiv(ID, GL_LINK_STATUS, &success);
//    if (!success)
//    {
//        glGetProgramInfoLog(ID, 512, NULL, infoLog);
//        LOGERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
//    }
//
//    glDeleteShader(vertex);
//    glDeleteShader(fragment);
//    if (geometryPath) {
//        glDeleteShader(geoShader);
//    }
//}
//
//void GPUProgramGL::Use()
//{
//    glUseProgram(ID);
//}
//
//void GPUProgramGL::SetBool(const std::string& name, bool value) const
//{
//    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
//}
//void GPUProgramGL::SetInt(const std::string& name, int value) const
//{
//    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
//}
//void GPUProgramGL::SetFloat(const std::string& name, float value) const
//{
//    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
//}
//void GPUProgramGL::SetVec3(const std::string& name, glm::vec3 value) const
//{
//    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
//}
//void GPUProgramGL::SetVec3(const std::string& name, float x, float y, float z) const
//{
//    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
//}
//void GPUProgramGL::SetVec4(const std::string& name, glm::vec4 value) const
//{
//    glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
//}
//
//void GPUProgramGL::SetMat4(const std::string& name, const glm::mat4& mat) const
//{
//    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//}