#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
	DepthStencil depthStencil;

	Framebuffer framebuffer[VK_NUM_BUFFERS];
	ImageView imageView[VK_NUM_BUFFERS];
	CommandBuffer commandBuffer[VK_NUM_BUFFERS];

	PipelineCache pipelineCache;
	PipelineLayout pipelineLayout;
	Pipeline pipeline;

	DescriptorPool descriptorPool;
	DescriptorSetLayout descriptorSetLayout;
	DescriptorSet descriptorSet;

	Shader vertexShader;
	Shader fragmentShader;

	UniformBuffer view_proj;

	Model model;

	char* load_binary(const char* path, size_t* p_size = nullptr)
	{
		std::ifstream f(path, std::ios::in | std::ios::binary);
		size_t file_size = f.seekg(0, std::ios::end).tellg();
		f.seekg(0, std::ios::beg);

		char* binary = new char[file_size];
		f.read(binary, file_size);
		f.close(); 

		if (p_size)
		{
			*p_size = file_size;
		}

		return binary;
	}

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

	void buildCommand(VkCommandBuffer& commandBuffer, VkImage& swapchainImage, VkFramebuffer& framebuffer, const VkExtent2D& extent)
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
		imageMemoryBarrier.image = swapchainImage;
		imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		VkClearColorValue clearColorValue;
		memset(&clearColorValue, 0, sizeof(VkClearColorValue));
		clearColorValue.float32[0] = 0.9f;
		clearColorValue.float32[1] = 1.0f;
		clearColorValue.float32[2] = 1.0f;
		clearColorValue.float32[3] = 1.0f;
		VkClearValue clearValues[2];
		clearValues[0].color = clearColorValue;
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo;
		memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass.m_renderPass;
		renderPassBeginInfo.framebuffer = framebuffer;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent = extent;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		{
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.m_pipelineLayout, 0, 1, &descriptorSet.m_descriptorSet, 0, NULL);

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.m_pipeline);

			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)extent.width;
			viewport.height = (float)extent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissor = {};
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			scissor.extent = extent;
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			model.draw(commandBuffer);
			//VkDeviceSize offsets[1] = { 0 };
			//vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.m_vertexBuffer, offsets);
			//vkCmdDraw(commandBuffer, 3, 1, 0, 0);
			// vkCmdDrawIndexed(commandBuffer, )
		}

		vkCmdEndRenderPass(commandBuffer);

		//
		memset(&imageMemoryBarrier, 0, sizeof(VkImageMemoryBarrier));
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = swapchainImage;
		imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
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

		//result = vertexBuffer.create();
		//if (result != VK_SUCCESS)
		//{
		//	return result;
		//}

		result = descriptorPool.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = descriptorSetLayout.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = descriptorSet.create(descriptorPool.m_descriptorPool, 1, &descriptorSetLayout.m_descriptorSetLayout);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		{
			size_t size;
			void* raw = load_binary("simple.vspv", &size);
			result = vertexShader.create(raw, size);
			if (result != VK_SUCCESS)
			{
				return result;
			}
		}

		{
			size_t size;
			void* raw = load_binary("simple.fspv", &size);
			result = fragmentShader.create(raw, size);
			if (result != VK_SUCCESS)
			{
				return result;
			}
		}

		{
			model.create("teapot.mf0");
		}

		result = pipelineCache.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = pipelineLayout.create(1, &descriptorSetLayout.m_descriptorSetLayout);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkExtent2D extent = { 1024, 768 };
		const VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;

		result = pipeline.create(
			renderPass.m_renderPass,
			vertexShader.m_shaderModule,
			fragmentShader.m_shaderModule,
			pipelineCache.m_pipelineCache,
			pipelineLayout.m_pipelineLayout,
			model.getVertexInputInfoPtr(),
			extent);
		if (result != VK_SUCCESS)
		{
			return result;
		}

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

		result = depthStencil.create(VK_FORMAT_D16_UNORM, extent);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		uint32_t swapchainImagesCount = VK_NUM_BUFFERS;
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

		view_proj.create(sizeof(float) * 16 * 2);

		// store information in the uniform's descriptor
		descriptorSet.update(view_proj.m_buffer, view_proj.m_memoryRequirements.size);

		for (uint32_t i = 0; i < VK_NUM_BUFFERS; i++)
		{
			result = imageView[i].create(swapchainImage[i], format);
			if (result != VK_SUCCESS)
			{
				return result;
			}

			VkImageView imageViews[2] = {
				imageView[i].m_imageView,
				depthStencil.m_imageView
			};

			result = framebuffer[i].create(extent.width, extent.height, renderPass.m_renderPass, 2, imageViews);
			if (result != VK_SUCCESS)
			{
				return result;
			}

			result = commandBuffer[i].create(commandPool.m_commandPool);
			if (result != VK_SUCCESS)
			{
				return result;
			}

			commandBuffer[i].begin();
			buildCommand(commandBuffer[i].m_commandBuffer, swapchainImage[i], framebuffer[i].m_framebuffer, extent);
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
			struct
			{
				glm::mat4 solid[2];
			} uniform;

			glm::mat4 projectionMatrix(1.0f);
			glm::mat4 viewMatrix(1.0f);

			projectionMatrix = glm::perspective(glm::radians(45.0f), (float)1024 / (float)768, 0.1f, 1000.0f);

			static float zoom = 200.0f;
			static float rot_x;
			static float rot_y;
			static POINT op = {};
			static POINT zp = {};
			POINT np;

			if (GetKeyState(VK_LBUTTON) < 0)
			{
				GetCursorPos(&np);
				if (op.x == 0 && op.y == 0)
				{
					op = np;
				}

				rot_x -= (float)(op.x - np.x) / 90.0f;
				rot_y -= (float)(op.y - np.y) / 90.0f;

				op = np;
			}

			if (GetKeyState(VK_RBUTTON) < 0)
			{
				GetCursorPos(&np);
				if (zp.x == 0 && zp.y == 0)
				{
					zp = np;
				}

				zoom -= (float)(zp.y - np.y) / 10.0f;

				zp = np;
			}

			glm::vec3 eye(0.0f, 0.0f, zoom);
			glm::vec3 center(0.0f, 0.0f, 0.0f);
			glm::vec3 up(0.0f, 1.0f, 0.0);
			
			viewMatrix = glm::lookAt(eye, center, up);
			viewMatrix = glm::rotate(viewMatrix, rot_x, glm::vec3(0.0f, 1.0f, 0.0f));
			viewMatrix = glm::rotate(viewMatrix, rot_y, glm::vec3(1.0f, 0.0f, 0.0f));

			uniform.solid[0] = projectionMatrix;
			uniform.solid[1] = viewMatrix;

			view_proj.upload(0, &uniform, sizeof(uniform));

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
