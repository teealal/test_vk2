#pragma once

namespace vk
{
	class DescriptorSetLayout
	{
	public:

		DescriptorSetLayout();
		virtual ~DescriptorSetLayout();

		VkResult create();
		void destroy();

		VkDescriptorSetLayout m_descriptorSetLayout;
	};
}
