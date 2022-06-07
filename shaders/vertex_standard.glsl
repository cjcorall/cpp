#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_proj;

out vec3 normal_in;
out vec3 fragPos;
out vec2 texCoords;

void main()
{
   normal_in = mat3(transpose(inverse(mat_model))) * aNormal;
   fragPos = vec3(mat_model * vec4(aPos, 1.0));
   gl_Position = mat_proj * mat_view * vec4(fragPos, 1.0);

   texCoords = aTexCoords;
}