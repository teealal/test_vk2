#version 430 core

in vec3 v_normal;
in vec3 v_view;

layout(location = 0) out vec4 o_color;

const vec3 light_dir = vec3(-1, 1, 1);
const vec3 light_color = vec3(0.9, 0.8, 0.7);
const vec3 ambient_color = vec3(0.2, 0.2, 0.2);

void main(void)
{
	vec3 l = normalize(light_dir);
	vec3 v = normalize(v_view);
	vec3 n = normalize(v_normal);
	vec3 h = normalize(l + v);
	float nl = max(dot(l, n), 0.0);
	float nh = max(dot(n, h), 0.0);
	float vn = max(dot(v, n), 0.0);

	vec4 final_color = vec4(1);

	final_color.xyz *= light_color * nl + ambient_color;
	final_color.xyz += pow(nh, 50).xxx * 0.7;
	final_color.xyz += pow(1 - vn, 2).xxx * 0.5;

	o_color = final_color;
}
