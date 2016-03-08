#include "vk.h"
#include "vk_UniformBuffer.h"


namespace vk
{
	VkDeviceSize getDeviceSize(size_t currentSize, size_t stride)
	{
		return VkDeviceSize((currentSize / stride) * stride + ((currentSize % stride) > 0 ? stride : 0));
	}

	UniformBuffer::UniformBuffer()
		: m_buffer(VK_NULL_HANDLE)
		, m_deviceMemory(VK_NULL_HANDLE)
		, m_memoryRequirements()
	{
	}

	UniformBuffer::~UniformBuffer()
	{
		destroy();
	}

	VkResult UniformBuffer::create(uint32_t size, uint32_t count)
	{
		VkResult result = VK_SUCCESS;

		m_size = size;

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = getDeviceSize(size * count, 16);
		bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		result = vkCreateBuffer(getDevice(), &bufferCreateInfo, nullptr, &m_buffer);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		vkGetBufferMemoryRequirements(getDevice(), m_buffer, &m_memoryRequirements);

		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = m_memoryRequirements.size;
		memoryAllocInfo.memoryTypeIndex = 0;

		result = vkAllocateMemory(getDevice(), &memoryAllocInfo, nullptr, &m_deviceMemory);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		return vkBindBufferMemory(getDevice(), m_buffer, m_deviceMemory, 0);
	}

	VkResult UniformBuffer::upload(size_t offset, const void* data, size_t size)
	{
		void* mappedData = nullptr;

		VkResult result = vkMapMemory(getDevice(), m_deviceMemory, offset * m_size, m_memoryRequirements.size, 0, &mappedData);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		memcpy(mappedData, data, size);

		vkUnmapMemory(getDevice(), m_deviceMemory);

		return result;
	}

	void UniformBuffer::destroy()
	{
		vkDestroyBuffer(getDevice(), m_buffer, nullptr);
	}
}
