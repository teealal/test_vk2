#version 430 core

layout(binding = 0, std140) uniform UBO
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
} ubo;

layout(location = 0) in vec3 i_vertex;
layout(location = 1) in vec3 i_normal;

out vec3 v_normal;
out vec3 v_view;

void main(void)
{
	mat4 inv_view = inverse(ubo.viewMatrix);

	vec3 view = vec3(inv_view[3][0], inv_view[3][1], inv_view[3][2]);

	v_normal = i_normal;
	v_view = normalize(view - i_vertex);

	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(i_vertex, 1.0);
}
