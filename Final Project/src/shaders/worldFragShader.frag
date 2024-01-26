#version 330

#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

struct Light {
	vec3 position;
	float intensity;
	vec3 color;
};

struct Material {
	vec3 ambientCI;
	vec3 diffuseCI;
	vec3 specularCI;
	int phong;
};



uniform Light light;

#define MAX_NUM_LIGHTS 10
uniform int numLights;
uniform int lightTypes[MAX_NUM_LIGHTS];
uniform vec3 lightPositions[MAX_NUM_LIGHTS];
uniform vec3 lightColors[MAX_NUM_LIGHTS];
uniform float lightIntensities[MAX_NUM_LIGHTS];


uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;


uniform Material material;

in vec2 TexCoord;
uniform sampler2D texture0;

uniform vec3 camPos;

in vec3 FragPos;
in vec3 Normal;


void main(){
	
    vec3 result;
    for (int i = 0; i < numLights; i++) {
        float irradiance;
        vec3 lightVector;
        if (lightTypes[i] == POINT_LIGHT) {
            irradiance = lightIntensities[i]/distance(lightPositions[i], FragPos);
            lightVector = normalize(lightPositions[i] - FragPos);
        } 
        else if (lightTypes[i] == DIRECTIONAL_LIGHT) {
            irradiance = lightIntensities[i];
            lightVector = normalize(lightPositions[i]);
        }

        // ambient
        vec3 ambient = material.ambientCI*irradiance;
  	
        // diffuse 
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightVector), 0.0);
        vec3 diffuse = diff * material.diffuseCI*irradiance;
    
        // specular
        vec3 camDir = normalize(camPos - FragPos);
        vec3 reflectDir = reflect(-lightVector, norm);  
        float spec = pow(max(dot(camDir, reflectDir), 0.0), material.phong);
        vec3 specular = irradiance * material.specularCI * lightColors[i] * spec;  
        
        result += ambient + diffuse + specular;
    }
    

	gl_FragColor = texture(texture0, TexCoord)* vec4 (result, 1);
}