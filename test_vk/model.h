#ifndef MODEL_H
#define MODEL_H

class Model
{
public:

	struct vec3
	{
		float		x;
		float		y;
		float		z;
	};

	struct Header
	{
		uint32_t	version;
		uint32_t	flag;
		uint32_t	numVerts;
		uint32_t	numFaces;
		vec3		aabb_min;
		vec3		aabb_max;
	};

	struct Face
	{
		uint32_t	id;				///< 面ID
		uint32_t	index[3];		///< 頂点インデックス
		uint32_t	attr;			///< 属性
		vec3		normal;			///< 法線
	};

	void load(const std::string& filepath);

private:

	Header	m_header;
	vec3*	m_vertices;
	Face*	m_faces;
};

#endif
