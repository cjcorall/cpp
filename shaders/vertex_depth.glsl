#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 mat_light;
uniform mat4 mat_model;

void main(){
	gl_Position = mat_light * mat_model * vec4(aPos, 1.0);
}