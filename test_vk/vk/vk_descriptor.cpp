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

		VkDescriptorPoolSize descriptorPoolSize[2];
		memset(&descriptorPoolSize, 0, sizeof(descriptorPoolSize));
		descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSize[0].descriptorCount = 1;
		descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSize[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
		memset(&descriptorPoolCreateInfo, 0, sizeof(VkDescriptorPoolCreateInfo));
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = 1;
		descriptorPoolCreateInfo.poolSizeCount = 1;
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSize;
		VkDescriptorPool descriptorPool;
		result = vkCreateDescriptorPool(getDevice(), &descriptorPoolCreateInfo, nullptr, &descriptorPool);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[2];
		memset(&descriptorSetLayoutBinding, 0, sizeof(descriptorSetLayoutBinding));
		descriptorSetLayoutBinding[0].binding = 0;
		descriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetLayoutBinding[0].descriptorCount = 1;
		descriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorSetLayoutBinding[0].pImmutableSamplers = nullptr;
		descriptorSetLayoutBinding[1].binding = 1;
		descriptorSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetLayoutBinding[1].descriptorCount = 1;
		descriptorSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorSetLayoutBinding[1].pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = 2;
		descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBinding;
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
