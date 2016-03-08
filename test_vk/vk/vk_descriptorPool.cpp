#include "vk.h"
#include "vk_descriptorPool.h"


namespace vk
{
	DescriptorPool::DescriptorPool()
		: m_descriptorPool(VK_NULL_HANDLE)
	{
	}

	DescriptorPool::~DescriptorPool()
	{
		destroy();
	}

	VkResult DescriptorPool::create()
	{
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
		memset(&descriptorPoolCreateInfo, 0, sizeof(VkDescriptorPoolCreateInfo));
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = 1;
		descriptorPoolCreateInfo.poolSizeCount = 1;
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;
		return vkCreateDescriptorPool(getDevice(), &descriptorPoolCreateInfo, nullptr, &m_descriptorPool);
	}

	void DescriptorPool::destroy()
	{
		if (m_descriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(getDevice(), m_descriptorPool, nullptr);
			m_descriptorPool = VK_NULL_HANDLE;
		}
	}
}
