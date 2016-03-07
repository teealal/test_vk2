#include "vk.h"
#include "vk_device.h"


namespace vk
{
	Device* _instance = nullptr;

	Device::Device()
		: m_device(VK_NULL_HANDLE)
		, m_physicalDevice(VK_NULL_HANDLE)
	{
		_instance = this;
	}

	Device::~Device()
	{
		destroy();
	}

	VkResult Device::create()
	{
		VkResult result = VK_SUCCESS;

		uint32_t physCount = 0;
		result = vkEnumeratePhysicalDevices(vk::getInstance(), &physCount, nullptr);
		if (result != VK_SUCCESS || physCount == 0)
		{
			return result;
		}

		physCount = 1;
		result = vkEnumeratePhysicalDevices(vk::getInstance(), &physCount, &m_physicalDevice);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		float queuePriorities[1] = { 0.0f };

		VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.flags = 0;
		deviceQueueCreateInfo.queueFamilyIndex = 0;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

		const char* enabledExtensionNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;
		deviceCreateInfo.enabledExtensionCount = 1;
		deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames;
		deviceCreateInfo.pEnabledFeatures = nullptr;

		return vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device);
	}

	void Device::destroy()
	{
		vkDestroyDevice(m_device, nullptr);
	}

	VkDevice& getDevice()
	{
		return _instance->m_device;
	}

	VkPhysicalDevice& getPhysicalDevice()
	{
		return _instance->m_physicalDevice;
	}
}
