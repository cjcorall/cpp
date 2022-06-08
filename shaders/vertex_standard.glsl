#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
	vec4 fragPosLightSpace;
} vs_out;

uniform mat4 mat_proj;
uniform mat4 mat_view;
uniform mat4 mat_light;
uniform mat4 mat_model;

void main(){
	vs_out.fragPos = vec3(mat_model * vec4(aPos, 1.0));
	vs_out.normal = transpose(inverse(mat3(mat_model))) * aNormal;
	vs_out.texCoords = aTexCoords;
	vs_out.fragPosLightSpace = mat_light * vec4(vs_out.fragPos, 1.0);
	gl_Position = mat_proj * mat_view * mat_model * vec4(aPos, 1.0);
}