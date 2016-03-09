#pragma once

namespace vk
{
	class Pipeline
	{
	public:

		Pipeline();
		virtual ~Pipeline();

		VkResult create(
			VkRenderPass renderPass,
			VkShaderModule vertexShaderModule,
			VkShaderModule fragmentShaderModule,
			VkPipelineCache pipelineCache,
			VkPipelineLayout pipelineLayout,
			VkPipelineVertexInputStateCreateInfo* pVertexInputInfo,
			const VkExtent2D& extent);
		void destroy();

		VkPipeline	m_pipeline;
	};
}
