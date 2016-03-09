#include "vk.h"
#include "vk_vertexBuffer.h"


namespace vk
{
	VertexBuffer::VertexBuffer()
		: m_vertexBuffer(VK_NULL_HANDLE)
		, m_deviceMemoryVertexBuffer(VK_NULL_HANDLE)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	VkResult VertexBuffer::create()
	{
		return VK_SUCCESS;
	}

	void VertexBuffer::destroy()
	{
		vkDestroyBuffer(getDevice(), m_vertexBuffer, nullptr);
	}
}
