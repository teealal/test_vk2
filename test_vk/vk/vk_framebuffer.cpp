#include "vk.h"
#include "vk_framebuffer.h"


namespace vk
{
	Framebuffer::Framebuffer()
		: m_framebuffer(VK_NULL_HANDLE)
	{
	}

	Framebuffer::~Framebuffer()
	{
		destroy();
	}

	VkResult Framebuffer::create(
		uint32_t width,
		uint32_t height,
		VkRenderPass renderPass,
		uint32_t attachmentCount,
		const VkImageView* pAttachments)
	{
		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = attachmentCount;
		framebufferCreateInfo.pAttachments = pAttachments;
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1;

		return vkCreateFramebuffer(vk::getDevice(), &framebufferCreateInfo, nullptr, &m_framebuffer);
	}

	void Framebuffer::destroy()
	{
		vkDestroyFramebuffer(vk::getDevice(), m_framebuffer, nullptr);
	}
}
