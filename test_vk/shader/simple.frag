#version 430 core

in vec3 v_normal;

layout(location = 0) out vec4 o_color;

void main(void)
{
	vec3 normal = normalize(v_normal);
	float n_dot_l = max(dot(vec3(0, 1, 0), normal) * 0.5 + 0.5, 0.0);

	o_color = vec4(n_dot_l.xxx, 1.0);
}
