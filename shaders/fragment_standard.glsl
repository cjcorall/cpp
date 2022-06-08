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

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
	vec4 fragPosLightSpace;
} vs_out;

uniform Material material;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

uniform sampler2D shadowMap;

#define NR_POINT_LIGHTS 4
#define NR_DIR_LIGHTS 1
uniform DirectionalLight dLights[NR_DIR_LIGHTS];
uniform PointLight pLights[NR_POINT_LIGHTS];
uniform SpotLight sLight;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform vec3 lightPos;

in vec3 normal_in;
in vec3 fragPos;
in vec2 texCoords;

out vec3 output_diffuse;
out vec3 output_specular;

uniform int has_diffuse;
uniform int has_specular;

uniform samplerCube skybox;

uniform vec3 output_color;

float shadowCalc(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir){
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	
	float fragDepth = projCoords.z;
	float shadow = 0.0;
	if(fragDepth > 1.0){
		shadow = 0.0;
	}else{
		float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
		vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
		for(int x = -1; x <=1; ++x){
			for(int y = -1; y <=1; ++y){
				float mapDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;				
				shadow += fragDepth - bias > mapDepth ? 1.0 : 0.0;
				
			}
		}
		shadow /= 15.0;
	}
	return shadow;
}

vec3 calcSpecular(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 lightSpec){
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(normal, halfDir), 0.0),material.shininess);
	vec3 specular = lightSpec * (output_specular * spec);
	return specular;
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	vec3 lightColor = vec3(1.0);
	vec3 ambient = 0.15 * lightColor;

	vec3 lightDir = normalize(lightPos - vs_out.fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 specular = calcSpecular(lightDir, normal, viewDir, vec3(1.0));
	float shadow = shadowCalc(vs_out.fragPosLightSpace, normal, lightDir);

	vec3 result  =  (((1.0 - shadow) * (diffuse + specular)) + ambient) * output_diffuse;

	//vec3 result = ambient + diffuse + specular;

	return result;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	
	vec3 lightDir = normalize(light.position - vs_out.fragPos);

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
		output_diffuse = vec3(texture(material.diffuse,vs_out.texCoords)).rgb;
	}else{
		output_diffuse = output_color;
	}

	if(has_specular > 0){
		output_specular = vec3(texture(material.specular,vs_out.texCoords)).rgb;
	}else{
		output_specular = output_color;
	}

	vec3 normal = normalize(vs_out.normal);
	vec3 cameraDir = normalize(cameraPos - vs_out.fragPos);

	//vec3 I = normalize(vs_out.fragPos - cameraPos);
	//vec3 R = reflect(I, norm);

	vec3 result = vec3(0.0);

	//result = texture(skybox, R).rgb;
	//output_diffuse = result;
	//output_specular = result;
	
	for(int i=0;i<1; i++){
		result += calcDirLight(dLights[i], normal, cameraDir);
	}

	for(int i=0;i<NR_POINT_LIGHTS; i++){
		result += calcPointLight(pLights[i], normal, vs_out.fragPos, cameraDir);
	}
	/*
	//output_diffuse = texture(material.diffuse,vs_out.texCoords).rgb;
	//output_diffuse = vec3(0.8, 0.5, 0.3);
	//vec3 normal = normalize(vs_out.normal);
	vec3 lightColor = vec3(1.0);

	vec3 ambient = 0.3 * lightColor;

	vec3 lightDir = normalize(lightPos - vs_out.fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 viewDir = normalize(cameraPos - vs_out.fragPos);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;

	float shadow = shadowCalc(vs_out.fragPosLightSpace, normal, lightDir);
	vec3 lighting =  (((1.0 - shadow) * (diffuse + specular)) + ambient) * output_diffuse;

	for(int i=0;i<2; i++){
		lighting += calcPointLight(pLights[i], normal, fragPos, cameraDir);
	}
	*/
	FragColor = vec4(result, 1.0);
	
}