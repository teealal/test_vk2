#include "vk.h"
#include "vk_Image.h"


namespace vk
{
	Image::Image()
		: m_image(VK_NULL_HANDLE)
	{
	}

	Image::~Image()
	{
	}

	VkResult Image::create()
	{
		VkImageCreateInfo imageCreateInfo = {};

#if 0
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = imageType;
		imageCreateInfo.format = format;
		imageCreateInfo.extent = extent;
		imageCreateInfo.mipLevels = mipLevels;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = samples;
		imageCreateInfo.tiling = tiling;
		imageCreateInfo.usage = usage;
		imageCreateInfo.sharingMode = sharingMode;
		imageCreateInfo.queueFamilyIndexCount = queueFamilyIndexCount;
		imageCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		imageCreateInfo.initialLayout = initialLayout;
#endif

		return vkCreateImage(getDevice(), &imageCreateInfo, nullptr, &m_image);
	}

	void Image::destroy()
	{
		vkDestroyImage(getDevice(), m_image, nullptr);
	}
}
