#pragma once

namespace vk
{
	inline VkDeviceSize getDeviceSize(size_t currentSize, size_t stride)
	{
		return VkDeviceSize((currentSize / stride) * stride + ((currentSize % stride) > 0 ? stride : 0));
	}
}
