#pragma once

namespace vk
{
	class Shader
	{
	public:

		Shader();
		virtual ~Shader();

		VkResult create(const void* raw, size_t size);
		void destroy();

		VkShaderModule m_shaderModule;
	};
}
