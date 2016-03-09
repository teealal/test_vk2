#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <array>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "vk_utility.h"
#include "vk_instance.h"
#include "vk_device.h"
#include "vk_framebuffer.h"
#include "vk_commandBuffer.h"
#include "vk_commandPool.h"
#include "vk_queue.h"
#include "vk_semaphore.h"
#include "vk_renderPass.h"
#include "vk_swapchain.h"
#include "vk_imageView.h"
#include "vk_image.h"
#include "vk_depthStencil.h"
#include "vk_pipelineCache.h"
#include "vk_pipelineLayout.h"
#include "vk_pipeline.h"
#include "vk_descriptorPool.h"
#include "vk_descriptorSetLayout.h"
#include "vk_descriptorSet.h"
#include "vk_vertexBuffer.h"
#include "vk_uniformBuffer.h"
#include "vk_shader.h"
#include "vk_model.h"

#define VK_NUM_BUFFERS		2

namespace vk
{
	VkResult initialize(HINSTANCE hInstance, HWND hWnd);
	void finalize();
	void render();
}
