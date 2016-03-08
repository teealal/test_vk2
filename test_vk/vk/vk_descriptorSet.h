#pragma once

namespace vk
{
	class DescriptorSet
	{
	public:

		DescriptorSet();
		virtual ~DescriptorSet();

		VkResult create(
			VkDescriptorPool descriptorPool,
			uint32_t descriptorSetCount,
			const VkDescriptorSetLayout* pSetLayouts);
		void destroy();
		void update(VkBuffer buffer, VkDeviceSize size);

		VkDescriptorSet m_descriptorSet;
	};
}
