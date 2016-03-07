#include "vk.h"
#include "vk_commandBuffer.h"


namespace vk
{
	CommandBuffer::CommandBuffer()
		: m_commandBuffer(VK_NULL_HANDLE)
		, m_commandPool(VK_NULL_HANDLE)
	{
	}

	CommandBuffer::~CommandBuffer()
	{
		destroy();
	}

	VkResult CommandBuffer::create(VkCommandPool commandPool)
	{
		m_commandPool = commandPool;

		VkCommandBufferAllocateInfo cmdBufferCreateInfo = {};
		cmdBufferCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufferCreateInfo.commandPool = commandPool;
		cmdBufferCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufferCreateInfo.commandBufferCount = 1;
		return vkAllocateCommandBuffers(getDevice(), &cmdBufferCreateInfo, &m_commandBuffer);
	}

	void CommandBuffer::destroy()
	{
		if (m_commandBuffer != VK_NULL_HANDLE && m_commandPool != VK_NULL_HANDLE)
		{
			vkFreeCommandBuffers(getDevice(), m_commandPool, 1, &m_commandBuffer);
			m_commandBuffer = VK_NULL_HANDLE;
			m_commandPool = VK_NULL_HANDLE;
		}
	}

	VkResult CommandBuffer::begin()
	{
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		commandBufferInheritanceInfo.queryFlags = 0;
		commandBufferInheritanceInfo.pipelineStatistics = 0;

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;
		return vkBeginCommandBuffer(m_commandBuffer, &commandBufferBeginInfo);
	}

	VkResult CommandBuffer::end()
	{
		return vkEndCommandBuffer(m_commandBuffer);
	}
}
