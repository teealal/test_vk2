#pragma once

namespace vk
{
	class CommandPool
	{
	public:

		CommandPool();
		virtual ~CommandPool();

		VkResult create();
		void destroy();

		VkCommandPool m_commandPool;
	};
}
