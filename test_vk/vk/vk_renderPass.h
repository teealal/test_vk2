#pragma once

namespace vk
{
	class RenderPass
	{
	public:

		RenderPass();
		virtual ~RenderPass();

		VkResult create(VkFormat format);
		void destroy();

		VkRenderPass	m_renderPass;
	};
}
