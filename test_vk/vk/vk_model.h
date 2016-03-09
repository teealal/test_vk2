#pragma once

namespace vk
{
	class Model
	{
	public:

		Model();
		virtual ~Model();

		void create(const std::string& filepath);
		void destroy();
		void draw(VkCommandBuffer commandBuffer);

		VkPipelineVertexInputStateCreateInfo* getVertexInputInfoPtr()
		{
			return &m_vertexInputInfo;
		}

	private:

		struct Buffer
		{
			VkBuffer buffer;
			VkDeviceMemory memory;
		};

		Buffer m_vertices;
		Buffer m_indices;
		uint32_t m_indexCount;

		VkPipelineVertexInputStateCreateInfo m_vertexInputInfo;
		std::array<VkVertexInputBindingDescription, 4> m_bindingDescriptions;
		std::array<VkVertexInputAttributeDescription, 4> m_attributeDescriptions;
	};
}
