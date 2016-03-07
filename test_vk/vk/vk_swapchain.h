#pragma once

namespace vk
{
	class Swapchain
	{
	public:

		Swapchain();
		virtual ~Swapchain();

		VkResult create(VkSurfaceKHR surface, VkFormat format, const VkExtent2D& extent);
		void destroy();

		VkResult getSwapchainImages(uint32_t* pSwapchainImagesCount, VkImage* pSwapchainImages);

		VkSwapchainKHR m_swapchain;
	};
}
