#version 430 core

layout(binding = 0, std140) uniform UBO
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
} ubo;

layout(location = 0) in vec3 i_vertex;
layout(location = 1) in vec3 i_normal;

out vec3 v_normal;

void main(void)
{
//	v_normal = u_bufferTransform.normalMatrix * i_normal;
	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(i_vertex, 1.0);
}
