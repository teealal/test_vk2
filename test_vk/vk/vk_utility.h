#pragma once

namespace vk
{
	inline VkDeviceSize getDeviceSize(size_t currentSize, size_t stride)
	{
		return VkDeviceSize((currentSize / stride) * stride + ((currentSize % stride) > 0 ? stride : 0));
	}

	inline VkBool32 getMemoryType(uint32_t typeBits, VkFlags properties, uint32_t * typeIndex)
	{
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties = {};
		vkGetPhysicalDeviceMemoryProperties(getPhysicalDevice(), &deviceMemoryProperties);

		for (uint32_t i = 0; i < 32; i++)
		{
			if ((typeBits & 1) == 1)
			{
				if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					*typeIndex = i;
					return true;
				}
			}
			typeBits >>= 1;
		}
		return false;
	}
}
