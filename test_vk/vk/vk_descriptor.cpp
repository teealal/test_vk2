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

	void Descriptor::create()
	{
	}

	void Descriptor::destroy()
	{
	}

	void Descriptor::update(VkBuffer buffer, VkDeviceSize size)
	{
		VkDescriptorBufferInfo descriptorBufferInfos[2];
		memset(descriptorBufferInfos, 0, sizeof(descriptorBufferInfos));

		descriptorBufferInfos[0].buffer = buffer;
		descriptorBufferInfos[0].offset = 0;
		descriptorBufferInfos[0].range = size;

		VkWriteDescriptorSet writeDescriptorSets[2];
		memset(writeDescriptorSets, 0, sizeof(writeDescriptorSets));
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = VK_NULL_HANDLE;	// Defined later.
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].pImageInfo = nullptr;
		writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfos[0];
		writeDescriptorSets[0].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(getDevice(), 2, writeDescriptorSets, 0, nullptr);
	}
}
