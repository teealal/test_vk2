#include "vk.h"
#include "vk_instance.h"


namespace vk
{
	Instance* _instance = nullptr;

	Instance::Instance()
		: m_instance(VK_NULL_HANDLE)
	{
		_instance = this;
	}

	Instance::~Instance()
	{
	}

	VkResult Instance::create()
	{
		const char* extensionNames[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "test_vk";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "test_vk";
		applicationInfo.engineVersion = VK_MAKE_VERSION(0, 9, 0);
		applicationInfo.apiVersion = VK_API_VERSION;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = nullptr;
		instanceCreateInfo.enabledExtensionCount = 2;
		instanceCreateInfo.ppEnabledExtensionNames = extensionNames;

		return vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);
	}

	void Instance::destroy()
	{
		vkDestroyInstance(m_instance, nullptr);
	}

	VkInstance& getInstance()
	{
		return _instance->m_instance;
	}
}
