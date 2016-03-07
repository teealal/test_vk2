#include "vk.h"
#include "vk_commandPool.h"


namespace vk
{
	CommandPool::CommandPool()
		: m_commandPool(VK_NULL_HANDLE)
	{
	}

	CommandPool::~CommandPool()
	{
		destroy();
	}

	VkResult CommandPool::create()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = 0;
		commandPoolCreateInfo.flags = 0;
		return vkCreateCommandPool(getDevice(), &commandPoolCreateInfo, nullptr, &m_commandPool);
	}

	void CommandPool::destroy()
	{
		vkDestroyCommandPool(getDevice(), m_commandPool, nullptr);
	}
}
