#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 mat_proj;
uniform mat4 mat_view;

void main(){
	texCoords = aPos;
	gl_Position = mat_proj * mat_view * vec4(aPos, 1.0);
}