#include "vk.h"
#include "vk_descriptorSetLayout.h"


namespace vk
{
	DescriptorSetLayout::DescriptorSetLayout()
		: m_descriptorSetLayout(VK_NULL_HANDLE)
	{
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		destroy();
	}

	VkResult DescriptorSetLayout::create()
	{
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = 0;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = 1;
		descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;
		return vkCreateDescriptorSetLayout(getDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout);
	}

	void DescriptorSetLayout::destroy()
	{
		if (m_descriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(getDevice(), m_descriptorSetLayout, nullptr);
			m_descriptorSetLayout = VK_NULL_HANDLE;
		}
	}
}
