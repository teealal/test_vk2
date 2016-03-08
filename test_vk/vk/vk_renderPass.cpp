#include "vk.h"
#include "vk_renderPass.h"


namespace vk
{
	RenderPass::RenderPass()
		: m_renderPass(VK_NULL_HANDLE)
	{
	}

	RenderPass::~RenderPass()
	{
		destroy();
	}

	VkResult RenderPass::create(VkFormat format)
	{
		VkAttachmentDescription attachmentDescription[2] = {};
		attachmentDescription[0].flags = 0;
		attachmentDescription[0].format = format;
		attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		attachmentDescription[1].flags = 0;
		attachmentDescription[1].format = VK_FORMAT_D16_UNORM;
		attachmentDescription[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachmentDescription[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentReference = {};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference deptStencilAttachmentReference = {};
		deptStencilAttachmentReference.attachment = 1;
		deptStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentReference;
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = &deptStencilAttachmentReference;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = 2;
		renderPassCreateInfo.pAttachments = attachmentDescription;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 0;
		renderPassCreateInfo.pDependencies = nullptr;

		return vkCreateRenderPass(getDevice(), &renderPassCreateInfo, nullptr, &m_renderPass);
	}

	void RenderPass::destroy()
	{
		vkDestroyRenderPass(vk::getDevice(), m_renderPass, nullptr);
	}
}
