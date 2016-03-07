#pragma once

namespace vk
{
	class ImageView
	{
	public:

		ImageView();
		virtual ~ImageView();

		VkResult create(VkImage image, VkFormat format);
		void destroy();

		VkImageView	m_imageView;
	};
}
