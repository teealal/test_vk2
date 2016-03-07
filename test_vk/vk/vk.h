#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "vk_instance.h"
#include "vk_device.h"
#include "vk_framebuffer.h"
#include "vk_commandBuffer.h"
#include "vk_commandPool.h"
#include "vk_queue.h"
#include "vk_semaphore.h"
#include "vk_renderPass.h"
#include "vk_swapchain.h"
#include "vk_image.h"
#include "vk_imageView.h"

#define VK_NUM_BUFFERS		2

namespace vk
{
	VkResult initialize(HINSTANCE hInstance, HWND hWnd);
	void finalize();
	void render();
}
