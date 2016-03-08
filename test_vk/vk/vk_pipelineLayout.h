#pragma once

namespace vk
{
	class PipelineLayout
	{
	public:

		PipelineLayout();
		virtual ~PipelineLayout();

		VkResult create(uint32_t setLayoutCount, const VkDescriptorSetLayout* pSetLayout);
		void destroy();

		VkPipelineLayout	m_pipelineLayout;
	};
}
