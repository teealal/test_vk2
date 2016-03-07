#pragma once

namespace vk
{
	class Instance final
	{
	public:

		Instance();
		~Instance();

		VkResult create();
		void destroy();

		VkInstance	m_instance;
	};

	VkInstance& getInstance();
}
