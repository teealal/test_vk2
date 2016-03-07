#pragma once

namespace vk
{
	class Queue
	{
	public:

		Queue();
		virtual ~Queue();

		void create();
		void destroy();

		VkQueue m_queue;
	};
}
