#include "vk.h"
#include "vk_descriptorSet.h"


namespace vk
{
	DescriptorSet::DescriptorSet()
		: m_descriptorSet(VK_NULL_HANDLE)
	{
	}

	DescriptorSet::~DescriptorSet()
	{
		destroy();
	}

	VkResult DescriptorSet::create(
		VkDescriptorPool descriptorPool,
		uint32_t descriptorSetCount,
		const VkDescriptorSetLayout* pSetLayouts)
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
		descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;
		return vkAllocateDescriptorSets(getDevice(), &descriptorSetAllocateInfo, &m_descriptorSet);
	}

	void DescriptorSet::destroy()
	{
	}

	void DescriptorSet::update(VkBuffer buffer, VkDeviceSize size)
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
