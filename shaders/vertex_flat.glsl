#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_proj;

void main()
{
   gl_Position = mat_proj * mat_view * mat_model * vec4(aPos, 1.0);
}