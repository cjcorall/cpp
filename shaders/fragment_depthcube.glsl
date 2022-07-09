#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

out vec4 FragColor;

void main(){
	float lightDist = length(FragPos.xyz - lightPos);

	lightDist = lightDist / far_plane;

	gl_FragDepth = lightDist;
	//FragColor = vec4(gl_FragDepth,1.0);

}