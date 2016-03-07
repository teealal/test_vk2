#include <memory>
#include "vk.h"


namespace vk
{
	Instance instance;
	Device device;
	CommandPool commandPool;
	Queue queue;
	uint32_t queueFamilyIndex;

	Semaphore imageAcquiredSemaphore;
	Semaphore renderingCompleteSemaphore;

	RenderPass renderPass;
	Swapchain swapchain;

	Framebuffer framebuffer[VK_NUM_BUFFERS];
	ImageView imageView[VK_NUM_BUFFERS];
	CommandBuffer commandBuffer[VK_NUM_BUFFERS];

	void buildSwapchainImageLayout(uint32_t index, CommandBuffer& commandBuffer, VkImage swapchainImage)
	{
		VkImageMemoryBarrier imageMemoryBarrier;

		memset(&imageMemoryBarrier, 0, sizeof(VkImageMemoryBarrier));

		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = 0;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		imageMemoryBarrier.srcQueueFamilyIndex = 0;
		imageMemoryBarrier.dstQueueFamilyIndex = 0;
		imageMemoryBarrier.image = swapchainImage;
		imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		vkCmdPipelineBarrier(commandBuffer.m_commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}

	VkResult initialize(HINSTANCE nativeDisplay, HWND nativeWindow)
	{
		VkResult result = VK_SUCCESS;

		result = instance.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = device.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		// surface
		VkSurfaceKHR surface;
		VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfoKHR = {};
		win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		win32SurfaceCreateInfoKHR.flags = 0;
		win32SurfaceCreateInfoKHR.hinstance = nativeDisplay;
		win32SurfaceCreateInfoKHR.hwnd = nativeWindow;
		result = vkCreateWin32SurfaceKHR(getInstance(), &win32SurfaceCreateInfoKHR, nullptr, &surface);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		// queue
		queue.create();

		// command pool
		result = commandPool.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = imageAcquiredSemaphore.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = renderingCompleteSemaphore.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkExtent2D extent = {1024, 768};
		const VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;

		result = swapchain.create(surface, format, extent);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = renderPass.create(format);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		uint32_t swapchainImagesCount = 2;
		VkImage swapchainImage[VK_NUM_BUFFERS] = { VK_NULL_HANDLE, VK_NULL_HANDLE };
		result = swapchain.getSwapchainImages(&swapchainImagesCount, swapchainImage);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		{
			CommandBuffer updateCommandBuffer;
			updateCommandBuffer.create(commandPool.m_commandPool);

			updateCommandBuffer.begin();
			{
				for (uint32_t i = 0; i < VK_NUM_BUFFERS; i++)
				{
					buildSwapchainImageLayout(i, updateCommandBuffer, swapchainImage[i]);
				}
			}
			updateCommandBuffer.end();

			// ì‚Á‚½updateCommandBuffer‚ð‚·‚®ŽÀs‚·‚é
			VkSubmitInfo submitInfo;
			memset(&submitInfo, 0, sizeof(VkSubmitInfo));
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &updateCommandBuffer.m_commandBuffer;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;
			result = vkQueueSubmit(queue.m_queue, 1, &submitInfo, VK_NULL_HANDLE);
			if (result != VK_SUCCESS)
			{
				return result;
			}

			result = vkQueueWaitIdle(queue.m_queue);
			if (result != VK_SUCCESS)
			{
				return result;
			}
		}

		for (uint32_t i = 0; i < VK_NUM_BUFFERS; i++)
		{
			result = imageView[i].create(swapchainImage[i], format);
			if (result != VK_SUCCESS)
			{
				return result;
			}

			result = framebuffer[i].create(extent.width, extent.height, renderPass.m_renderPass, &imageView[i].m_imageView);
			if (result != VK_SUCCESS)
			{
				return result;
			}

			// command buffer
			result = commandBuffer[i].create(commandPool.m_commandPool);
			if (result != VK_SUCCESS)
			{
				return result;
			}

			// display list
			commandBuffer[i].begin();
			{
				VkImageMemoryBarrier imageMemoryBarrier;
				memset(&imageMemoryBarrier, 0, sizeof(VkImageMemoryBarrier));
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.image = swapchainImage[i];
				imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
				vkCmdPipelineBarrier(commandBuffer[i].m_commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

				VkClearColorValue clearColorValue;
				memset(&clearColorValue, 0, sizeof(VkClearColorValue));
				clearColorValue.float32[0] = 1.0f;
				clearColorValue.float32[1] = 1.0f;
				clearColorValue.float32[2] = 1.0f;
				clearColorValue.float32[3] = 1.0f;
				VkClearValue clearValues[1] = { clearColorValue };
				VkRenderPassBeginInfo renderPassBeginInfo;
				memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
				renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassBeginInfo.renderPass = renderPass.m_renderPass;
				renderPassBeginInfo.framebuffer = framebuffer[i].m_framebuffer;
				renderPassBeginInfo.renderArea.offset.x = 0;
				renderPassBeginInfo.renderArea.offset.y = 0;
				renderPassBeginInfo.renderArea.extent = extent;
				renderPassBeginInfo.clearValueCount = 1;
				renderPassBeginInfo.pClearValues = clearValues;
				vkCmdBeginRenderPass(commandBuffer[i].m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

				// The window is cleaned with above defined blue color due to the VK_ATTACHMENT_LOAD_OP_CLEAR (see render pass creation).

				vkCmdEndRenderPass(commandBuffer[i].m_commandBuffer);

				//
				memset(&imageMemoryBarrier, 0, sizeof(VkImageMemoryBarrier));
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.image = swapchainImage[i];
				imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
				vkCmdPipelineBarrier(commandBuffer[i].m_commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
			}
			commandBuffer[i].end();
		}

		return result;
	}

	void finalize()
	{

	}

	void render()
	{
		VkResult result;
		uint32_t currentBuffer;
		result = vkAcquireNextImageKHR(getDevice(), swapchain.m_swapchain, UINT64_MAX, imageAcquiredSemaphore.m_semaphore, VK_NULL_HANDLE, &currentBuffer);

		if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
		{
			VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &imageAcquiredSemaphore.m_semaphore;
			submitInfo.pWaitDstStageMask = &waitDstStageMask;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer[currentBuffer].m_commandBuffer;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &renderingCompleteSemaphore.m_semaphore;
			result = vkQueueSubmit(queue.m_queue, 1, &submitInfo, VK_NULL_HANDLE);

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &renderingCompleteSemaphore.m_semaphore;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &swapchain.m_swapchain;
			presentInfo.pImageIndices = &currentBuffer;
			presentInfo.pResults = nullptr;

			result = vkQueuePresentKHR(queue.m_queue, &presentInfo);
			result = vkQueueWaitIdle(queue.m_queue);
		}

	}
}
