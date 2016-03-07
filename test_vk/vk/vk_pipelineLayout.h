#pragma once

namespace vk
{
	class PipelineLayout
	{
	public:

		PipelineLayout();
		virtual ~PipelineLayout();

		VkResult create();
		void destroy();

		VkPipelineLayout	m_pipelineLayout;
	};
}
