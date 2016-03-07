#pragma once

namespace vk
{
	class Device final
	{
	public:

		Device();
		~Device();

		VkResult create();
		void destroy();

		VkDevice m_device;
		VkPhysicalDevice m_physicalDevice;
	};

	VkDevice& getDevice();
	VkPhysicalDevice& getPhysicalDevice();
}
