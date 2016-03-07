#pragma once

namespace vk
{
	class Semaphore
	{
	public:

		Semaphore();
		virtual ~Semaphore();

		VkResult create();
		void destroy();

		VkSemaphore m_semaphore;
	};
}
