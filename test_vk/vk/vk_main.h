#pragma once

namespace vk
{
	VkResult initialize(HINSTANCE nativeDisplay, HWND nativeWindow);
	void finalize();
	void doRender(float width, float height);
}
