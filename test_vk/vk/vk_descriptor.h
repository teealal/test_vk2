#pragma once

namespace vk
{
	class Descriptor
	{
	public:

		Descriptor();
		virtual ~Descriptor();

		void create();
		void destroy();

		VkDescriptorSetLayout m_descriptorSetLayout;
		VkDescriptorPool m_descriptorPool;
		VkDescriptorSet m_descriptorSet;
	};
}
