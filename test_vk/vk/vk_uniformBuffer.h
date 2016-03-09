#pragma once

namespace vk
{
	class UniformBuffer
	{
	public:

		UniformBuffer();
		virtual ~UniformBuffer();

		VkResult create(uint32_t size);
		void destroy();

		VkResult upload(size_t offset, const void* data, size_t size);

		VkBuffer m_buffer;
		VkDeviceSize m_size;
		VkDeviceMemory m_deviceMemory;
		VkMemoryRequirements m_memoryRequirements;
	};
}
