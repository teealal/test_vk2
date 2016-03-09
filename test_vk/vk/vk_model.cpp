#include "vk.h"
#include "vk_model.h"

struct ResVec3
{
	float		x;
	float		y;
	float		z;
};

struct ResHeader
{
	uint32_t	version;
	uint32_t	flag;
	uint32_t	numVerts;
	uint32_t	numFaces;
	ResVec3		aabb_min;
	ResVec3		aabb_max;
};

struct ResFace
{
	uint32_t	id;
	uint32_t	index[3];
	uint32_t	attr;
	ResVec3		normal;
};

struct Vertex
{
	float x, y, z;
	float nx, ny, nz;
};

namespace vk
{
	Model::Model()
		: m_vertices()
		, m_indices()
		, m_indexCount(0)
	{
	}

	Model::~Model()
	{
		destroy();
	}

	void Model::create(const std::string& filepath)
	{
		std::ifstream fin(filepath.c_str(), std::ios::in | std::ios::binary);

		if (fin.is_open())
		{
			ResHeader header = {};
			fin.read(reinterpret_cast<char*>(&header), sizeof(header));

			m_bindingDescriptions[0].binding = 0;
			m_bindingDescriptions[0].stride = sizeof(Vertex);
			m_bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			m_attributeDescriptions[0].binding = 0;
			m_attributeDescriptions[0].location = 0;
			m_attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			m_attributeDescriptions[0].offset = 0;

			m_attributeDescriptions[1].binding = 0;
			m_attributeDescriptions[1].location = 1;
			m_attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			m_attributeDescriptions[1].offset = sizeof(float) * 3;

			m_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			m_vertexInputInfo.pNext = NULL;
			m_vertexInputInfo.flags = 0;
			m_vertexInputInfo.vertexBindingDescriptionCount = 1;
			m_vertexInputInfo.pVertexBindingDescriptions = &m_bindingDescriptions[0];
			m_vertexInputInfo.vertexAttributeDescriptionCount = 2;
			m_vertexInputInfo.pVertexAttributeDescriptions = &m_attributeDescriptions[0];

			ResVec3* vertices = new ResVec3[header.numVerts];
			ResFace* faces = new ResFace[header.numFaces];

			fin.read(reinterpret_cast<char*>(vertices), sizeof(ResVec3) * header.numVerts);
			fin.read(reinterpret_cast<char*>(faces), sizeof(ResFace) * header.numFaces);

			{
				VkBufferCreateInfo bufInfo = {};
				bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufInfo.pNext = NULL;
				bufInfo.size = getDeviceSize(sizeof(Vertex) * header.numVerts, 16);
				bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				bufInfo.flags = 0;

				auto err = vkCreateBuffer(getDevice(), &bufInfo, nullptr, &m_vertices.buffer);

				VkMemoryRequirements memReqs = {};
				vkGetBufferMemoryRequirements(getDevice(), m_vertices.buffer, &memReqs);

				VkMemoryAllocateInfo memAlloc = {};
				memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memAlloc.allocationSize = memReqs.size;

				err = vkAllocateMemory(getDevice(), &memAlloc, nullptr, &m_vertices.memory);


				Vertex* mapped = nullptr;
				err = vkMapMemory(getDevice(), m_vertices.memory, 0, memAlloc.allocationSize, 0, reinterpret_cast<void**>(&mapped));

				for (uint32_t i = 0; i < header.numVerts; i++)
				{
					mapped[i].x = vertices[i].x;
					mapped[i].y = vertices[i].y;
					mapped[i].z = vertices[i].z;

					mapped[i].nx = 0.0f;
					mapped[i].ny = 1.0f;
					mapped[i].nz = 0.0f;
				}

				vkUnmapMemory(getDevice(), m_vertices.memory);
				vkBindBufferMemory(getDevice(), m_vertices.buffer, m_vertices.memory, 0);
			}

			{
				VkBufferCreateInfo bufInfo = {};
				bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufInfo.pNext = NULL;
				bufInfo.size = getDeviceSize(sizeof(uint32_t) * header.numFaces * 3, 16);
				bufInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				bufInfo.flags = 0;

				auto err = vkCreateBuffer(getDevice(), &bufInfo, nullptr, &m_indices.buffer);

				VkMemoryRequirements memReqs = {};
				vkGetBufferMemoryRequirements(getDevice(), m_indices.buffer, &memReqs);

				VkMemoryAllocateInfo memAlloc = {};
				memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memAlloc.allocationSize = memReqs.size;

				err = vkAllocateMemory(getDevice(), &memAlloc, nullptr, &m_indices.memory);

				uint32_t* mapped = nullptr;
				err = vkMapMemory(getDevice(), m_indices.memory, 0, memAlloc.allocationSize, 0, reinterpret_cast<void**>(&mapped));

				uint32_t index = 0;
				for (uint32_t i = 0; i < header.numFaces; i++)
				{
					mapped[index + 0] = faces[i].index[0];
					mapped[index + 1] = faces[i].index[1];
					mapped[index + 2] = faces[i].index[2];

					index += 3;
				}

				vkUnmapMemory(getDevice(), m_indices.memory);
				vkBindBufferMemory(getDevice(), m_indices.buffer, m_indices.memory, 0);

				m_indexCount = header.numFaces * 3;
			}

			fin.close();
		}
	}

	void Model::destroy()
	{
		vkDestroyBuffer(getDevice(), m_vertices.buffer, nullptr);
		vkFreeMemory(getDevice(), m_vertices.memory, nullptr);

		vkDestroyBuffer(getDevice(), m_indices.buffer, nullptr);
		vkFreeMemory(getDevice(), m_indices.memory, nullptr);
	}

	void Model::draw(VkCommandBuffer commandBuffer)
	{
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_vertices.buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_indices.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 1);
	}
}
