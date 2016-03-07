#include "vk.h"
#include "vk_Semaphore.h"


namespace vk
{
	Semaphore::Semaphore()
		: m_semaphore(VK_NULL_HANDLE)
	{
	}

	Semaphore::~Semaphore()
	{
		destroy();
	}

	VkResult Semaphore::create()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.flags = 0;
		return vkCreateSemaphore(getDevice(), &semaphoreCreateInfo, nullptr, &m_semaphore);
	}

	void Semaphore::destroy()
	{
	}
}
