#pragma once

namespace vk
{
	class PipelineCache
	{
	public:

		PipelineCache();
		virtual ~PipelineCache();

		VkResult create();
		void destroy();

		VkPipelineCache	m_pipelineCache;
	};
}
