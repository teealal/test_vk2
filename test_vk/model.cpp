#include "stdafx.h"
#include "model.h"

void Model::load(const std::string& filepath)
{
	std::ifstream fin(filepath.c_str(), std::ios::in | std::ios::binary);

	if (fin.is_open())
	{
		fin.read(reinterpret_cast<char*>(&m_header), sizeof(m_header));

		m_vertices = new vec3[m_header.numVerts];
		m_faces = new Face[m_header.numFaces];

		fin.read(reinterpret_cast<char*>(m_vertices), sizeof(vec3) * m_header.numVerts);
		fin.read(reinterpret_cast<char*>(m_faces), sizeof(Face) * m_header.numFaces);

		fin.close();
	}
}
