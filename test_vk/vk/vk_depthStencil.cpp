#include "vk.h"
#include "vk_depthStencil.h"


namespace vk
{
	DepthStencil::DepthStencil()
		: m_image(VK_NULL_HANDLE)
		, m_imageView(VK_NULL_HANDLE)
	{
	}

	DepthStencil::~DepthStencil()
	{
		destroy();
	}

	VkResult DepthStencil::create(VkFormat format, const VkExtent2D& extent)
	{
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.extent = { extent.width, extent.height, 1 };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		auto result = vkCreateImage(getDevice(), &imageCreateInfo, nullptr, &m_image);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkMemoryRequirements memoryRequirements = {};
		vkGetImageMemoryRequirements(getDevice(), m_image, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memoryRequirements.size;
		memoryAllocInfo.memoryTypeIndex = 1;

		VkDeviceMemory deviceMemory;
		vkAllocateMemory(getDevice(), &memoryAllocInfo, nullptr, &deviceMemory);

		result = vkBindImageMemory(getDevice(), m_image, deviceMemory, 0);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = m_image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components = componentMapping;
		imageViewCreateInfo.subresourceRange = subresourceRange;

		return vkCreateImageView(getDevice(), &imageViewCreateInfo, nullptr, &m_imageView);
	}

	void DepthStencil::destroy()
	{
		if (m_imageView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(getDevice(), m_imageView, nullptr);
			m_imageView = VK_NULL_HANDLE;
		}

		if (m_image != VK_NULL_HANDLE)
		{
			vkDestroyImage(getDevice(), m_image, nullptr);
			m_image = VK_NULL_HANDLE;
		}
	}
}
