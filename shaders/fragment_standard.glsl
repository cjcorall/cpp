#version 330 core
struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirectionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight{
	vec3 position;
	
	float kc;
	float kl;
	float kq;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight{
	vec3 position;
	vec3 direction;
	float phi;
	float gamma;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

#define NR_POINT_LIGHTS 4
#define NR_DIR_LIGHTS 4
uniform DirectionalLight dLights[NR_DIR_LIGHTS];
uniform PointLight pLights[NR_POINT_LIGHTS];
uniform SpotLight sLight;

out vec4 FragColor;

uniform vec3 cameraPos;

in vec3 normal_in;
in vec3 fragPos;
in vec2 texCoords;

out vec3 output_diffuse;
out vec3 output_specular;

uniform int has_diffuse;
uniform int has_specular;

uniform samplerCube skybox;

uniform vec3 output_color;

vec3 calcSpecular(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 lightSpec){
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(normal, halfDir), 0.0),material.shininess);
	vec3 specular = lightSpec * (output_specular * spec);
	return specular;
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * output_diffuse;

	vec3 ambient = light.ambient * output_diffuse;

	vec3 specular = calcSpecular(lightDir, normal, viewDir, light.specular);

	vec3 result = (ambient + diffuse + specular);

	return result;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	
	vec3 lightDir = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	float d = length(light.position - fragPos);
	float attenuation = 1.0 / (light.kc + (light.kl * d) + (light.kq * d * d));

	vec3 ambient = light.ambient * output_diffuse;
	vec3 diffuse = light.diffuse * diff * output_diffuse;
	vec3 specular = calcSpecular(lightDir, normal, viewDir, light.specular);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;
	return result;
}

void main()
{
	if(has_diffuse > 0){
		output_diffuse = vec3(texture(material.diffuse,texCoords));
	}else{
		output_diffuse = output_color;
	}

	if(has_specular > 0){
		output_specular = vec3(texture(material.specular,texCoords));
	}else{
		output_specular = output_color;
	}

	vec3 norm = normalize(normal_in);
	vec3 cameraDir = normalize(cameraPos - fragPos);

	vec3 I = normalize(fragPos - cameraPos);
	vec3 R = reflect(I, norm);

	vec3 result;

	//result = texture(skybox, R).rgb;
	//output_diffuse = result;
	//output_specular = result;
	
	for(int i=0;i<NR_DIR_LIGHTS; i++){
		result += calcDirLight(dLights[i], norm, cameraDir);
	}

	for(int i=0;i<NR_POINT_LIGHTS; i++){
		result += calcPointLight(pLights[i], norm, fragPos, cameraDir);
	}
	
	FragColor = vec4(result, 1.0);
	
}