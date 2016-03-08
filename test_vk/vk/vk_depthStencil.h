#pragma once

namespace vk
{
	class DepthStencil
	{
	public:

		DepthStencil();
		virtual ~DepthStencil();

		VkResult create(VkFormat format, const VkExtent2D& extent);
		void destroy();

		VkImage m_image;
		VkImageView m_imageView;
	};
}
