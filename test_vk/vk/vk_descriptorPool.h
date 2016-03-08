#pragma once

namespace vk
{
	class DescriptorPool
	{
	public:

		DescriptorPool();
		virtual ~DescriptorPool();

		VkResult create();
		void destroy();

		VkDescriptorPool m_descriptorPool;
	};
}
