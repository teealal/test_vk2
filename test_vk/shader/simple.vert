#version 430 core

layout(binding = 0, std140) uniform _u_bufferViewProjection
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
} u_bufferViewProjection;

layout(binding = 1, std140) uniform _u_bufferTransform
{
	mat4 modelMatrix;
	mat3 normalMatrix;
} u_bufferTransform;

layout(location = 0) in vec4 i_vertex;
//layout(location = 1) in vec3 i_normal;

out vec3 v_normal;

void main(void)
{
//	v_normal = u_bufferTransform.normalMatrix * i_normal;
	gl_Position = /*u_bufferViewProjection.projectionMatrix*/ u_bufferViewProjection.projectionMatrix * i_vertex;
}
