#pragma once

namespace vk
{
	class Descriptor
	{
	public:

		Descriptor();
		virtual ~Descriptor();

		VkResult create();
		void destroy();
		void update(VkBuffer buffer, VkDeviceSize size);

		VkDescriptorSetLayout m_descriptorSetLayout;
		VkDescriptorPool m_descriptorPool;
		VkDescriptorSet m_descriptorSet;
	};
}
