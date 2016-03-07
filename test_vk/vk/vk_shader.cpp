#include "vk.h"
#include "vk_Shader.h"


namespace vk
{
	Shader::Shader()
		: m_shaderModule(VK_NULL_HANDLE)
	{
	}

	Shader::~Shader()
	{
	}

	VkResult Shader::create(const void* raw, size_t size)
	{
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = size;
		shaderModuleCreateInfo.pCode = (const uint32_t*)raw;

		return vkCreateShaderModule(getDevice(), &shaderModuleCreateInfo, nullptr, &m_shaderModule);
	}

	void Shader::destroy()
	{
		vkDestroyShaderModule(getDevice(), m_shaderModule, nullptr);
	}
}
