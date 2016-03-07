#include "stdafx.h"
#include "vk_main.h"

namespace vk
{
	VkCommandBuffer cmd;

	VkImage swapchainImage[VKTS_NUMBER_BUFFERS];
	VkImageView swapchainImageView[VKTS_NUMBER_BUFFERS];
	VkFramebuffer framebuffer[VKTS_NUMBER_BUFFERS];
	VkCommandBuffer cmdBuffer[VKTS_NUMBER_BUFFERS];

//	VkFramebuffer framebuffer;
	VkDevice device;
	VkPipelineLayout pipelineLayout;
	VkPipelineCache pipelineCache;
	VkPipeline pipeline;
	VkRenderPass renderPass;
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;

	VkBuffer vertexBuffer;
	VkDeviceMemory deviceMemoryVertexBuffer;

	VkSemaphore imageAcquiredSemaphore;
	VkSemaphore renderingCompleteSemaphore;

	VkSwapchainKHR swapchain;

	VkResult buildPipelineCache()
	{
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pipelineCacheCreateInfo.flags = 0;
		pipelineCacheCreateInfo.initialDataSize = 0;
		pipelineCacheCreateInfo.pInitialData = nullptr;

		return vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache);
	}

	VkResult buildShader()
	{
		VkResult result = VK_SUCCESS;

		{
			std::ifstream f("vert.spv", std::ios::in | std::ios::binary);
			size_t file_size = f.seekg(0, std::ios::end).tellg();
			f.seekg(0, std::ios::beg);

			char* read_binary = new char[file_size];
			f.read(read_binary, file_size);
			f.close();

			VkShaderModuleCreateInfo shaderModuleCreateInfo;
			memset(&shaderModuleCreateInfo, 0, sizeof(VkShaderModuleCreateInfo));
			shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleCreateInfo.flags = 0;
			shaderModuleCreateInfo.codeSize = file_size;
			shaderModuleCreateInfo.pCode = (const uint32_t*)read_binary;
			result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &vertexShaderModule);

			if (result != VK_SUCCESS)
			{
				return result;
			}
		}

		{
			std::ifstream f("frag.spv", std::ios::in | std::ios::binary);
			size_t file_size = f.seekg(0, std::ios::end).tellg();
			f.seekg(0, std::ios::beg);

			char* read_binary = new char[file_size];
			f.read(read_binary, file_size);
			f.close();

			VkShaderModuleCreateInfo shaderModuleCreateInfo;
			memset(&shaderModuleCreateInfo, 0, sizeof(VkShaderModuleCreateInfo));
			shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleCreateInfo.flags = 0;
			shaderModuleCreateInfo.codeSize = file_size;
			shaderModuleCreateInfo.pCode = (const uint32_t*)read_binary;
			result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &fragmentShaderModule);

			if (result != VK_SUCCESS)
			{
				return result;
			}
		}

		return result;
	}

	VkResult buildPipeline(const VkExtent2D& extent)
	{
		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[2];
		memset(&pipelineShaderStageCreateInfo, 0, sizeof(pipelineShaderStageCreateInfo));
		pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo[0].flags = 0;
		pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		pipelineShaderStageCreateInfo[0].module = vertexShaderModule;
		pipelineShaderStageCreateInfo[0].pName = "main";
		pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

		pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo[1].flags = 0;
		pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pipelineShaderStageCreateInfo[1].module = fragmentShaderModule;
		pipelineShaderStageCreateInfo[1].pName = "main";
		pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

		VkVertexInputBindingDescription vertexInputBindingDescription;
		memset(&vertexInputBindingDescription, 0, sizeof(VkVertexInputBindingDescription));
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = 4 * sizeof(float);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription vertexInputAttributeDescription;
		memset(&vertexInputAttributeDescription, 0, sizeof(VkVertexInputAttributeDescription));
		vertexInputAttributeDescription.location = 0;
		vertexInputAttributeDescription.binding = 0;
		vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexInputAttributeDescription.offset = 0;

		VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo;
		memset(&pipelineVertexInputCreateInfo, 0, sizeof(VkPipelineVertexInputStateCreateInfo));
		pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineVertexInputCreateInfo.flags = 0;
		pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
		pipelineVertexInputCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
		pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = 1;
		pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = &vertexInputAttributeDescription;

		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
		memset(&pipelineInputAssemblyStateCreateInfo, 0, sizeof(VkPipelineInputAssemblyStateCreateInfo));
		pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineInputAssemblyStateCreateInfo.flags = 0;
		pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport;
		memset(&viewport, 0, sizeof(VkViewport));
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)1024;
		viewport.height = (float)768;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor;
		memset(&scissor, 0, sizeof(VkRect2D));
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent = extent;

		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
		memset(&pipelineViewportStateCreateInfo, 0, sizeof(VkPipelineViewportStateCreateInfo));
		pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCreateInfo.flags = 0;
		pipelineViewportStateCreateInfo.viewportCount = 1;
		pipelineViewportStateCreateInfo.pViewports = &viewport;
		pipelineViewportStateCreateInfo.scissorCount = 1;
		pipelineViewportStateCreateInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
		memset(&pipelineRasterizationStateCreateInfo, 0, sizeof(VkPipelineRasterizationStateCreateInfo));
		pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineRasterizationStateCreateInfo.flags = 0;
		pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		pipelineRasterizationStateCreateInfo.lineWidth = 0.0f;

		VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo;
		memset(&pipelineMultisampleStateCreateInfo, 0, sizeof(VkPipelineMultisampleStateCreateInfo));
		pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMultisampleStateCreateInfo.flags = 0;
		pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		pipelineMultisampleStateCreateInfo.minSampleShading = 0.0f;
		pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
		pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState;
		memset(&pipelineColorBlendAttachmentState, 0, sizeof(VkPipelineColorBlendAttachmentState));
		pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
		pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		pipelineColorBlendAttachmentState.colorWriteMask = 0xf;

		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo;
		memset(&pipelineColorBlendStateCreateInfo, 0, sizeof(VkPipelineColorBlendStateCreateInfo));
		pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfo.flags = 0;
		pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		pipelineColorBlendStateCreateInfo.attachmentCount = 1;
		pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
		pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		VkDynamicState dynamicState[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo;
		memset(&pipelineDynamicStateCreateInfo, 0, sizeof(VkPipelineDynamicStateCreateInfo));
		pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineDynamicStateCreateInfo.flags = 0;
		pipelineDynamicStateCreateInfo.dynamicStateCount = 2;
		pipelineDynamicStateCreateInfo.pDynamicStates = dynamicState;

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
		memset(&graphicsPipelineCreateInfo, 0, sizeof(VkGraphicsPipelineCreateInfo));
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.flags = 0;
		graphicsPipelineCreateInfo.stageCount = 2;
		graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfo;
		graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
		graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
		graphicsPipelineCreateInfo.layout = pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = renderPass;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = 0;

		VkResult result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
		return result;
	}

	VkResult buildVertexBuffer()
	{
		static const float vertices[3 * 4] =
		{
			-0.5f, 0.5f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.0f, 1.0f,
			0.0f, -0.5f, 0.0f, 1.0f
		};

		VkResult result;

		VkBufferCreateInfo bufferCreateInfo;
		memset(&bufferCreateInfo, 0, sizeof(VkBufferCreateInfo));
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = sizeof(vertices);
		bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;
		result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &vertexBuffer);

		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, vertexBuffer, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocInfo;
		memset(&memoryAllocInfo, 0, sizeof(VkMemoryAllocateInfo));
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memoryRequirements.size;
		memoryAllocInfo.memoryTypeIndex = 0;	// Gathered in next function.

		void* mappedData;
		result = vkAllocateMemory(device, &memoryAllocInfo, nullptr, &deviceMemoryVertexBuffer);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = vkMapMemory(device, deviceMemoryVertexBuffer, 0, memoryRequirements.size, 0, &mappedData);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		memcpy(mappedData, vertices, sizeof(vertices));
		vkUnmapMemory(device, deviceMemoryVertexBuffer);

		result = vkBindBufferMemory(device, vertexBuffer, deviceMemoryVertexBuffer, 0);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		return result;
	}

	VkResult buildCmdBuffer(int32_t usedBuffer, uint32_t width, uint32_t height)
	{
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo;
		memset(&commandBufferInheritanceInfo, 0, sizeof(VkCommandBufferInheritanceInfo));
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		commandBufferInheritanceInfo.queryFlags = 0;
		commandBufferInheritanceInfo.pipelineStatistics = 0;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		memset(&commandBufferBeginInfo, 0, sizeof(VkCommandBufferBeginInfo));
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;
		vkBeginCommandBuffer(cmd, &commandBufferBeginInfo);

		VkExtent2D extent;
		extent.width = width;
		extent.height = height;

		VkClearColorValue clearColorValue;
		memset(&clearColorValue, 0, sizeof(VkClearColorValue));
		clearColorValue.float32[0] = 0.0f;
		clearColorValue.float32[1] = 0.0f;
		clearColorValue.float32[2] = 1.0f;
		clearColorValue.float32[3] = 1.0f;
		VkClearValue clearValues[1] = { clearColorValue };

		VkRenderPassBeginInfo renderPassBeginInfo;
		memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = framebuffer[usedBuffer];
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent = extent;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = clearValues;
		vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	VkResult buildFramebuffer(int32_t usedBuffer, uint32_t width, uint32_t height)
	{
		VkFramebufferCreateInfo framebufferCreateInfo;
		memset(&framebufferCreateInfo, 0, sizeof(VkFramebufferCreateInfo));
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = &swapchainImageView[usedBuffer];
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1;

		return vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer[usedBuffer]);
	}

	VkResult buildSwapchainImageView(int32_t usedBuffer, VkFormat format)
	{
		VkImageViewCreateInfo imageViewCreateInfo;
		memset(&imageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = swapchainImage[usedBuffer];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		return vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapchainImageView[usedBuffer]);
	}

	void buildSwapchainImageLayout(int32_t usedBuffer, VkCommandBuffer cmdBuffer)
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
		imageMemoryBarrier.image = swapchainImage[usedBuffer];
		imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}

	VkResult initialize(HINSTANCE nativeDisplay, HWND nativeWindow)
	{
		const char* extensionNames[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

		VkApplicationInfo applicationInfo;
		memset(&applicationInfo, 0, sizeof(VkApplicationInfo));
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "test_vk";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "test_vk";
		applicationInfo.engineVersion = VK_MAKE_VERSION(0, 9, 0);
		applicationInfo.apiVersion = VK_API_VERSION;

		VkInstanceCreateInfo instanceCreateInfo;
		memset(&instanceCreateInfo, 0, sizeof(VkInstanceCreateInfo));
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = nullptr;
		instanceCreateInfo.enabledExtensionCount = 2;
		instanceCreateInfo.ppEnabledExtensionNames = extensionNames;

		VkResult result;

		VkInstance instance;
		result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		// The enumeration pattern SHOULD be to call with last parameter NULL to
		// get the count, then call again to get the handles. For brevity, omitted
		VkPhysicalDevice phys[4];
		uint32_t physCount = 4;
		result = vkEnumeratePhysicalDevices(instance, &physCount, &phys[0]);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		float queuePriorities[1] = { 0.0f };

		VkDeviceQueueCreateInfo deviceQueueCreateInfo;
		memset(&deviceQueueCreateInfo, 0, sizeof(VkDeviceQueueCreateInfo));
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.flags = 0;
		deviceQueueCreateInfo.queueFamilyIndex = 0;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

		const char* enabledExtensionNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		VkDeviceCreateInfo deviceCreateInfo;
		memset(&deviceCreateInfo, 0, sizeof(VkDeviceCreateInfo));
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;
		deviceCreateInfo.enabledExtensionCount = 1;
		deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames;
		deviceCreateInfo.pEnabledFeatures = nullptr;

		result = vkCreateDevice(phys[0], &deviceCreateInfo, nullptr, &device);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		// fetch vkCreateWin32SurfaceKHR extension function pointer via vkGetInstanceProcAddr
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.flags = 0;
		surfaceCreateInfo.hinstance = nativeDisplay;
		surfaceCreateInfo.hwnd = nativeWindow;

		VkSurfaceKHR surface;
		result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkExtent2D extent = { 1024, 768 };

		VkSwapchainCreateInfoKHR swapchainCreateInfo;
		memset(&swapchainCreateInfo, 0, sizeof(VkSwapchainCreateInfoKHR));
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.minImageCount = 2;
		swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
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
		swapchainCreateInfo.oldSwapchain = nullptr;

		VkSwapchainKHR swapChain;
		result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapChain);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		// Again this should be properly enumerated
		VkImage images[4]; uint32_t swapCount;
		result = vkGetSwapchainImagesKHR(device, swapChain, &swapCount, images);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkSemaphoreCreateInfo semaphoreCreateInfo;
		memset(&semaphoreCreateInfo, 0, sizeof(VkSemaphoreCreateInfo));
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.flags = 0;

		VkSemaphore semaphore;
		result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		// Synchronisation is needed here!
		uint32_t currentSwapImage;
		result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, semaphore, NULL, &currentSwapImage);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		VkImageViewCreateInfo imageViewCreateInfo;
		memset(&imageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = images[0];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		imageViewCreateInfo.components = componentMapping;
		imageViewCreateInfo.subresourceRange = imageSubresourceRange;

		// pass appropriate creation info to create view of image
		VkImageView backbufferView;
		result = vkCreateImageView(device, &imageViewCreateInfo, NULL, &backbufferView);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkQueue queue;
		vkGetDeviceQueue(device, 0, 0, &queue);

		VkAttachmentDescription attachmentDescription;
		memset(&attachmentDescription, 0, sizeof(VkAttachmentDescription));
		attachmentDescription.flags = 0;
		attachmentDescription.format = VK_FORMAT_B8G8R8A8_UNORM;
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentReference;
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription;
		memset(&subpassDescription, 0, sizeof(VkSubpassDescription));
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentReference;
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkRenderPassCreateInfo renderPassCreateInfo;
		memset(&renderPassCreateInfo, 0, sizeof(VkRenderPassCreateInfo));
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &attachmentDescription;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 0;
		renderPassCreateInfo.pDependencies = nullptr;
		result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkFramebufferCreateInfo framebufferCreateInfo;
		memset(&framebufferCreateInfo, 0, sizeof(VkFramebufferCreateInfo));
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = &backbufferView;
		framebufferCreateInfo.width = 1024;
		framebufferCreateInfo.height = 768;
		framebufferCreateInfo.layers = 1;
		result = vkCreateFramebuffer(device, &framebufferCreateInfo, NULL, &framebuffer);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		memset(&pipelineLayoutCreateInfo, 0, sizeof(VkPipelineLayoutCreateInfo));
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = buildVertexBuffer();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = buildShader();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = buildPipelineCache();
		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = buildPipeline(extent);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkDescriptorPoolSize descriptorPoolSize[12];

		memset(&descriptorPoolSize, 0, sizeof(descriptorPoolSize));

		for (int32_t i = 0; i < 3; i++)
		{
			descriptorPoolSize[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorPoolSize[i].descriptorCount = 1;
		}

		for (int32_t i = 3; i < 11; i++)
		{
			descriptorPoolSize[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorPoolSize[i].descriptorCount = 1;
		}

		for (int32_t i = 11; i < 12; i++)
		{
			descriptorPoolSize[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorPoolSize[i].descriptorCount = 1;
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
		memset(&descriptorPoolCreateInfo, 0, sizeof(VkDescriptorPoolCreateInfo));
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		descriptorPoolCreateInfo.maxSets = 1;
		descriptorPoolCreateInfo.poolSizeCount = 12;
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSize;

		VkDescriptorPool descriptorPool;
		result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		return VK_SUCCESS;
	}

	void finalize()
	{

	}

	void doRender(float width, float height)
	{
		// bind the pipeline
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		// set the viewport
		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)width;
		viewport.height = (float)height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmd, 0, 1, &viewport);

		// scissor
		VkRect2D scissor;
		memset(&scissor, 0, sizeof(VkRect2D));
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent = VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		// draw primitive
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, offsets);
		vkCmdDraw(cmd, 3, 1, 0, 0);

		vkCmdEndRenderPass(cmd);
		vkEndCommandBuffer(cmd);

		VkResult result;
		uint32_t currentBuffer;
		result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &currentBuffer);

		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		VkQueue queue;
		VkSubmitInfo submitInfo;
		memset(&submitInfo, 0, sizeof(VkSubmitInfo));
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &imageAcquiredSemaphore;
		submitInfo.pWaitDstStageMask = &waitDstStageMask;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer[currentBuffer];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderingCompleteSemaphore;

		vkQueueSubmit(queue, 1, &submitInfo, NULL);

#if 0
		// now we can present
		VkPresentInfoKHR presentInfo;
		memset(&presentInfo, 0, sizeof(VkPresentInfoKHR));
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = waitSemaphoreCount;
		presentInfo.pWaitSemaphores = waitSemaphores;
		presentInfo.swapchainCount = swapchainCount;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = imageIndices;
		presentInfo.pResults = results;

		vkQueuePresentKHR(queue, &presentInfo);
#endif
	}
}