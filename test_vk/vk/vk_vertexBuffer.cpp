#include "vk.h"
#include "vk_vertexBuffer.h"


namespace vk
{
	VertexBuffer::VertexBuffer()
		: m_vertexBuffer(VK_NULL_HANDLE)
		, m_deviceMemoryVertexBuffer(VK_NULL_HANDLE)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	VkResult VertexBuffer::create()
	{
		// 
		static const float vertices[3 * 4] =
		{
			-0.5f, 0.5f, 0.0f, 1.0f,
			 0.5f, 0.5f, 0.0f, 1.0f,
			 0.0f,-0.5f, 0.0f, 1.0f
		};

		VkResult result = VK_SUCCESS;

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = sizeof(vertices);
		bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		result = vkCreateBuffer(getDevice(), &bufferCreateInfo, nullptr, &m_vertexBuffer);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(getDevice(), m_vertexBuffer, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memoryRequirements.size;
		memoryAllocInfo.memoryTypeIndex = 0;

		void* mappedData;
		result = vkAllocateMemory(getDevice(), &memoryAllocInfo, nullptr, &m_deviceMemoryVertexBuffer);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = vkMapMemory(getDevice(), m_deviceMemoryVertexBuffer, 0, memoryRequirements.size, 0, &mappedData);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		memcpy(mappedData, vertices, sizeof(vertices));

		vkUnmapMemory(getDevice(), m_deviceMemoryVertexBuffer);

		result = vkBindBufferMemory(getDevice(), m_vertexBuffer, m_deviceMemoryVertexBuffer, 0);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		return result;
	}

	void VertexBuffer::destroy()
	{
		vkDestroyBuffer(getDevice(), m_vertexBuffer, nullptr);
	}
}
