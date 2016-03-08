#include "vk.h"
#include "vk_pipelineLayout.h"


namespace vk
{
	PipelineLayout::PipelineLayout()
		: m_pipelineLayout(VK_NULL_HANDLE)
	{
	}

	PipelineLayout::~PipelineLayout()
	{
		destroy();
	}

	VkResult PipelineLayout::create(uint32_t setLayoutCount, const VkDescriptorSetLayout* pSetLayout)
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
		pipelineLayoutCreateInfo.pSetLayouts = pSetLayout;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		return vkCreatePipelineLayout(getDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
	}

	void PipelineLayout::destroy()
	{
		vkDestroyPipelineLayout(getDevice(), m_pipelineLayout, nullptr);
	}
}
