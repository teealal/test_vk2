#pragma once

namespace vk
{
	class Framebuffer
	{
	public:

		Framebuffer();
		virtual ~Framebuffer();

		VkResult create(
			uint32_t width,
			uint32_t height,
			VkRenderPass renderPass,
			uint32_t attachmentCount,
			const VkImageView* pAttachments);

		void destroy();

		VkFramebuffer	m_framebuffer;
	};
}
