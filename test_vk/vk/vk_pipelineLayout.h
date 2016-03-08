#pragma once

namespace vk
{
	class PipelineLayout
	{
	public:

		PipelineLayout();
		virtual ~PipelineLayout();

		VkResult create(const VkDescriptorSetLayout* pSetLayout, uint32_t setLayoutCount);
		void destroy();

		VkPipelineLayout	m_pipelineLayout;
	};
}
