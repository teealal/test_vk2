#include <fstream>
#include <memory>
#include <glm/glm.hpp>
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

	VertexBuffer vertexBuffer;

	PipelineCache pipelineCache;
	PipelineLayout pipelineLayout;
	Pipeline pipeline;

	Descriptor descriptor;

	Shader vertexShader;
	Shader fragmentShader;

	UniformBuffer view_proj;

	glm::mat4 frustumMat4(const float left, const float right, const float bottom, const float top, const float nearVal, const float farVal)
	{
		if ((right - left) == 0.0f || (top - bottom) == 0.0f || (farVal - nearVal) == 0.0f)
		{
			return glm::mat4();
		}

		glm::mat4 result;

		result[0][0] = 2.0f * nearVal / (right - left);
		result[0][1] = 0.0f;
		result[0][2] = 0.0f;
		result[0][3] = 0.0f;
		result[1][0] = 0.0f;
		// Window clip origin is upper left.
		result[1][1] = -2.0f * nearVal / (top - bottom);
		result[1][2] = 0.0f;
		result[1][3] = 0.0f;
		result[2][0] = (right + left) / (right - left);
		result[2][1] = (top + bottom) / (top - bottom);
		result[2][2] = -(farVal + nearVal) / (farVal - nearVal);
		result[2][3] = -1.0f;
		result[3][0] = 0.0f;
		result[3][1] = 0.0f;
		result[3][2] = -(2.0f * farVal * nearVal) / (farVal - nearVal);
		result[3][3] = 0.0f;

		return result;
	}

	glm::mat4 perspectiveMat4(const float fovy, const float aspect, const float zNear, const float zFar)
	{
		if (fovy <= 0.0f || fovy >= 180.0f)
		{
			return glm::mat4();
		}

		float xmin, xmax, ymin, ymax;

		ymax = zNear * tanf(glm::radians(fovy * 0.5f));
		ymin = -ymax;
		xmin = ymin * aspect;
		xmax = ymax * aspect;

		return frustumMat4(xmin, xmax, ymin, ymax, zNear, zFar);
	}

	glm::mat4 lookAtMat4(const float eyeX, const float eyeY, const float eyeZ, const float centerX, const float centerY, const float centerZ, const float upX, const float upY, const float upZ)
	{
		glm::vec3 forward;
		glm::vec3 side;
		glm::vec3 up;

		forward[0] = centerX - eyeX;
		forward[1] = centerY - eyeY;
		forward[2] = centerZ - eyeZ;

		forward = glm::normalize(forward);

		up[0] = upX;
		up[1] = upY;
		up[2] = upZ;

		side = glm::cross(forward, up);
		side = glm::normalize(side);

		up = glm::cross(side, forward);

		glm::mat4 result;

		result[0][0] = side[0];
		result[0][1] = up[0];
		result[0][2] = -forward[0];
		result[0][3] = 0.0f;
		result[1][0] = side[1];
		result[1][1] = up[1];
		result[1][2] = -forward[1];
		result[1][3] = 0.0f;
		result[2][0] = side[2];
		result[2][1] = up[2];
		result[2][2] = -forward[2];
		result[2][3] = 0.0f;
		result[3][0] = 0.0f;
		result[3][1] = 0.0f;
		result[3][2] = 0.0f;
		result[3][3] = 1.0f;

		glm::mat4 translate(1.0f);
		translate[3][0] = -eyeX;
		translate[3][1] = -eyeY;
		translate[3][2] = -eyeZ;

		return result * translate;
	}

	glm::mat4 lookAtMat4(const glm::vec4& eye, const glm::vec4& center, const glm::vec3& up)
	{
		return lookAtMat4(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
	}

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
		clearColorValue.float32[0] = 1.0f;
		clearColorValue.float32[1] = 1.0f;
		clearColorValue.float32[2] = 1.0f;
		clearColorValue.float32[3] = 1.0f;
		VkClearValue clearValues[1] = { clearColorValue };
		VkRenderPassBeginInfo renderPassBeginInfo;
		memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass.m_renderPass;
		renderPassBeginInfo.framebuffer = framebuffer;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent = extent;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = clearValues;
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.m_pipeline);

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.m_pipelineLayout, 0, 1, &descriptor.m_descriptorSet, 0, nullptr);

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

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.m_vertexBuffer, offsets);
			vkCmdDraw(commandBuffer, 3, 1, 0, 0);
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

		result = vertexBuffer.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = descriptor.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		{
			size_t size;
			void* raw = load_binary("vert.spv", &size);
			result = vertexShader.create(raw, size);
			if (result != VK_SUCCESS)
			{
				return result;
			}
		}

		{
			size_t size;
			void* raw = load_binary("frag.spv", &size);
			result = fragmentShader.create(raw, size);
			if (result != VK_SUCCESS)
			{
				return result;
			}
		}

		result = pipelineCache.create();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = pipelineLayout.create();
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

		view_proj.create(sizeof(float) * 16, 2);
		descriptor.update(view_proj.m_buffer, view_proj.m_size);

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
			glm::mat4 projectionMatrix(1.0f);
			glm::mat4 viewMatrix(1.0f);

			projectionMatrix = perspectiveMat4(45.0f, (float)1024 / (float)768, 1.0f, 100.0f);
			viewMatrix = lookAtMat4(0.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

			view_proj.upload(0, &projectionMatrix, sizeof(projectionMatrix));
			view_proj.upload(1, &viewMatrix, sizeof(viewMatrix));

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
