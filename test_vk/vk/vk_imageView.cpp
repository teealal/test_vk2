#include "vk.h"
#include "vk_imageView.h"


namespace vk
{
	ImageView::ImageView()
		: m_imageView(VK_NULL_HANDLE)
	{
	}

	ImageView::~ImageView()
	{
		destroy();
	}

	VkResult ImageView::create(VkImage image, VkFormat format)
	{
		VkImageViewCreateInfo imageViewCreateInfo;
		memset(&imageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		return vkCreateImageView(getDevice(), &imageViewCreateInfo, nullptr, &m_imageView);
	}

	void ImageView::destroy()
	{
		vkDestroyImageView(getDevice(), m_imageView, nullptr);
	}
}
