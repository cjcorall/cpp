#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoords;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} vs_out;

uniform mat4 mat_proj;
uniform mat4 mat_view;
uniform mat4 mat_model;

uniform bool reverse_normals;

void main(){
	vs_out.fragPos = vec3(mat_model * vec4(aPos, 1.0));
	if(reverse_normals){
		vs_out.normal = transpose(inverse(mat3(mat_model))) * (-1.0 * aNormal);
	}else{
		vs_out.normal = transpose(inverse(mat3(mat_model))) * aNormal;
	}
	vs_out.texCoords = aTexCoords;
	gl_Position = mat_proj * mat_view * mat_model * vec4(aPos, 1.0);
}