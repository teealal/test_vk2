#include "vk.h"
#include "vk_swapchain.h"


namespace vk
{
	Swapchain::Swapchain()
		: m_swapchain(VK_NULL_HANDLE)
	{
	}

	Swapchain::~Swapchain()
	{
		destroy();
	}

	VkResult Swapchain::create(VkSurfaceKHR surface, VkFormat format, const VkExtent2D& extent)
	{
		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.minImageCount = VK_NUM_BUFFERS;
		swapchainCreateInfo.imageFormat = format;
		swapchainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		swapchainCreateInfo.imageExtent = extent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		return vkCreateSwapchainKHR(getDevice(), &swapchainCreateInfo, nullptr, &m_swapchain);
	}

	void Swapchain::destroy()
	{
		vkDestroySwapchainKHR(getDevice(), m_swapchain, nullptr);
	}

	VkResult Swapchain::getSwapchainImages(uint32_t* pSwapchainImagesCount, VkImage* pSwapchainImages)
	{
		return vkGetSwapchainImagesKHR(getDevice(), m_swapchain, pSwapchainImagesCount, pSwapchainImages);
	}
}
