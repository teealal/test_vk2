#include <glm/glm.hpp>
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

			std::unique_ptr<ResVec3> vertices;
			std::unique_ptr<ResFace> faces;
			
			vertices.reset(new ResVec3[header.numVerts]);
			faces.reset(new ResFace[header.numFaces]);

			fin.read(reinterpret_cast<char*>(vertices.get()), sizeof(ResVec3) * header.numVerts);
			fin.read(reinterpret_cast<char*>(faces.get()), sizeof(ResFace) * header.numFaces);

			{
				VkBufferCreateInfo bufInfo = {};
				bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufInfo.pNext = NULL;
				bufInfo.size = getDeviceSize(sizeof(Vertex) * header.numVerts, 256);
				bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				bufInfo.flags = 0;

				auto err = vkCreateBuffer(getDevice(), &bufInfo, nullptr, &m_vertices.buffer);

				VkMemoryRequirements memReqs = {};
				vkGetBufferMemoryRequirements(getDevice(), m_vertices.buffer, &memReqs);

				VkMemoryAllocateInfo memAlloc = {};
				memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memAlloc.allocationSize = memReqs.size;
				getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAlloc.memoryTypeIndex);

				err = vkAllocateMemory(getDevice(), &memAlloc, nullptr, &m_vertices.memory);


				Vertex* mapped = nullptr;
				err = vkMapMemory(getDevice(), m_vertices.memory, 0, bufInfo.size, 0, reinterpret_cast<void**>(&mapped));

				for (uint32_t i = 0; i < header.numVerts; i++)
				{
					mapped[i].x = vertices.get()[i].x;
					mapped[i].y = vertices.get()[i].y;
					mapped[i].z = vertices.get()[i].z;

					mapped[i].nx = 0.0f;
					mapped[i].ny = 0.0f;
					mapped[i].nz = 0.0f;
				}

				for (uint32_t i = 0; i < header.numFaces; i++)
				{
					Vertex* p0 = &mapped[faces.get()[i].index[0]];
					Vertex* p1 = &mapped[faces.get()[i].index[1]];
					Vertex* p2 = &mapped[faces.get()[i].index[2]];

					p0->nx += faces.get()[i].normal.x;
					p0->ny += faces.get()[i].normal.y;
					p0->nz += faces.get()[i].normal.z;

					p1->nx += faces.get()[i].normal.x;
					p1->ny += faces.get()[i].normal.y;
					p1->nz += faces.get()[i].normal.z;

					p2->nx += faces.get()[i].normal.x;
					p2->ny += faces.get()[i].normal.y;
					p2->nz += faces.get()[i].normal.z;
				}

				for (uint32_t i = 0; i < header.numVerts; i++)
				{
					glm::vec3 n(mapped[i].nx, mapped[i].ny, mapped[i].nz);
					n = glm::normalize(n);
					mapped[i].nx = n.x;
					mapped[i].ny = n.y;
					mapped[i].nz = n.z;
				}

				vkUnmapMemory(getDevice(), m_vertices.memory);
				vkBindBufferMemory(getDevice(), m_vertices.buffer, m_vertices.memory, 0);
			}

			{
				VkBufferCreateInfo bufInfo = {};
				bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufInfo.pNext = NULL;
				bufInfo.size = getDeviceSize(sizeof(uint32_t) * header.numFaces * 3, 256);
				bufInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				bufInfo.flags = 0;

				auto err = vkCreateBuffer(getDevice(), &bufInfo, nullptr, &m_indices.buffer);

				VkMemoryRequirements memReqs = {};
				vkGetBufferMemoryRequirements(getDevice(), m_indices.buffer, &memReqs);

				VkMemoryAllocateInfo memAlloc = {};
				memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memAlloc.allocationSize = memReqs.size;
				getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAlloc.memoryTypeIndex);

				err = vkAllocateMemory(getDevice(), &memAlloc, nullptr, &m_indices.memory);

				uint32_t* mapped = nullptr;
				err = vkMapMemory(getDevice(), m_indices.memory, 0, bufInfo.size, 0, reinterpret_cast<void**>(&mapped));

				uint32_t index = 0;
				for (uint32_t i = 0; i < header.numFaces; i++)
				{
					mapped[index + 0] = faces.get()[i].index[0];
					mapped[index + 1] = faces.get()[i].index[1];
					mapped[index + 2] = faces.get()[i].index[2];

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
