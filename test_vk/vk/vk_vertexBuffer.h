#pragma once

namespace vk
{
	class VertexBuffer
	{
	public:

		VertexBuffer();
		virtual ~VertexBuffer();

		VkResult create();
		void destroy();

		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_deviceMemoryVertexBuffer;
	};
}
