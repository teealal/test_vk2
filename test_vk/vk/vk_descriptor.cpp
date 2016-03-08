#include "vk.h"
#include "vk_descriptor.h"


namespace vk
{
	Descriptor::Descriptor()
		: m_descriptorSetLayout(VK_NULL_HANDLE)
		, m_descriptorPool(VK_NULL_HANDLE)
		, m_descriptorSet(VK_NULL_HANDLE)
	{
	}

	Descriptor::~Descriptor()
	{
	}

	VkResult Descriptor::create()
	{
		VkResult result = VK_SUCCESS;

		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
		memset(&descriptorPoolCreateInfo, 0, sizeof(VkDescriptorPoolCreateInfo));
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = 1;
		descriptorPoolCreateInfo.poolSizeCount = 1;
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;
		result = vkCreateDescriptorPool(getDevice(), &descriptorPoolCreateInfo, nullptr, &m_descriptorPool);
		if (result != VK_SUCCESS)
		{
			return result;
		}

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
		result = vkCreateDescriptorSetLayout(getDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.pSetLayouts = &m_descriptorSetLayout;
		return vkAllocateDescriptorSets(getDevice(), &descriptorSetAllocateInfo, &m_descriptorSet);
	}

	void Descriptor::destroy()
	{
	}

	void Descriptor::update(VkBuffer buffer, VkDeviceSize size)
	{
		VkDescriptorBufferInfo descriptorBufferInfos = {};
		descriptorBufferInfos.buffer = buffer;
		descriptorBufferInfos.offset = 0;
		descriptorBufferInfos.range = size;

		VkWriteDescriptorSet writeDescriptorSets = {};
		writeDescriptorSets.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets.dstSet = m_descriptorSet;
		writeDescriptorSets.dstBinding = 0;
		writeDescriptorSets.dstArrayElement = 0;
		writeDescriptorSets.descriptorCount = 1;
		writeDescriptorSets.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets.pImageInfo = nullptr;
		writeDescriptorSets.pBufferInfo = &descriptorBufferInfos;
		writeDescriptorSets.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(getDevice(), 1, &writeDescriptorSets, 0, nullptr);
	}
}
