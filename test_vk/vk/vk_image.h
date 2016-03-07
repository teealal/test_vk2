#pragma once

namespace vk
{
	class Image
	{
	public:

		Image();
		virtual ~Image();

		VkResult create();
		void destroy();

		VkImage m_image;
	};
}
