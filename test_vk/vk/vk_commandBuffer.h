#pragma once

namespace vk
{
	class CommandBuffer
	{
	public:

		CommandBuffer();
		virtual ~CommandBuffer();

		VkResult create(VkCommandPool commandPool);
		void destroy();

		VkResult begin();
		VkResult end();

		VkCommandBuffer m_commandBuffer;

	private:
		VkCommandPool	m_commandPool;
	};
}
