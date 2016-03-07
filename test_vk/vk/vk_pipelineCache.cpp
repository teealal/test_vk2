#include "vk.h"
#include "vk_pipelineCache.h"


namespace vk
{
	PipelineCache::PipelineCache()
		: m_pipelineCache(VK_NULL_HANDLE)
	{
	}

	PipelineCache::~PipelineCache()
	{
		destroy();
	}

	VkResult PipelineCache::create()
	{
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pipelineCacheCreateInfo.flags = 0;
		pipelineCacheCreateInfo.initialDataSize = 0;
		pipelineCacheCreateInfo.pInitialData = nullptr;

		return vkCreatePipelineCache(getDevice(), &pipelineCacheCreateInfo, nullptr, &m_pipelineCache);

	}

	void PipelineCache::destroy()
	{
		vkDestroyPipelineCache(getDevice(), m_pipelineCache, nullptr);
	}
}
