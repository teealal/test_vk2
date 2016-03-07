#include "vk.h"
#include "vk_queue.h"


namespace vk
{
	Queue::Queue()
		: m_queue(VK_NULL_HANDLE)
	{
	}

	Queue::~Queue()
	{
		destroy();
	}

	void Queue::create()
	{
		vkGetDeviceQueue(getDevice(), 0, 0, &m_queue);
	}

	void Queue::destroy()
	{
	}
}
