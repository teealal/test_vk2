#include "vk.h"
#include "vk_descriptor.h"


namespace vk
{
	Descriptor::Descriptor()
		: m_descriptor(VK_NULL_HANDLE)
	{
	}

	Descriptor::~Descriptor()
	{
		destroy();
	}

	void Descriptor::create()
	{
		VkDescriptorBufferInfo descriptorBufferInfos[2];
		memset(descriptorBufferInfos, 0, sizeof(descriptorBufferInfos));

		descriptorBufferInfos[0].buffer = vertexViewProjectionUniformBuffer->getBuffer()->getBuffer();
		descriptorBufferInfos[0].offset = 0;
		descriptorBufferInfos[0].range = vertexViewProjectionUniformBuffer->getBuffer()->getSize();

		descriptorBufferInfos[1].buffer = fragmentUniformBuffer->getBuffer()->getBuffer();
		descriptorBufferInfos[1].offset = 0;
		descriptorBufferInfos[1].range = fragmentUniformBuffer->getBuffer()->getSize();


		VkWriteDescriptorSet writeDescriptorSets[2];
		memset(writeDescriptorSets, 0, sizeof(writeDescriptorSets));
		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = VK_NULL_HANDLE;	// Defined later.
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_VERTEX_VIEWPROJECTION].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_VERTEX_VIEWPROJECTION;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_VERTEX_VIEWPROJECTION].dstArrayElement = 0;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_VERTEX_VIEWPROJECTION].descriptorCount = 1;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_VERTEX_VIEWPROJECTION].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_VERTEX_VIEWPROJECTION].pImageInfo = nullptr;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_VERTEX_VIEWPROJECTION].pBufferInfo = &descriptorBufferInfos[0];
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_VERTEX_VIEWPROJECTION].pTexelBufferView = nullptr;

		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT].dstSet = VK_NULL_HANDLE;	// Defined later.
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT].dstArrayElement = 0;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT].descriptorCount = 1;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT].pImageInfo = nullptr;
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT].pBufferInfo = &descriptorBufferInfos[1];
		writeDescriptorSets[VKTS_BINDING_UNIFORM_BUFFER_FRAGMENT_LIGHT].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(getDevice(), 1, &writeDescriptorSets, 0, nullptr);
	}

	void Descriptor::destroy()
	{
	}
}
