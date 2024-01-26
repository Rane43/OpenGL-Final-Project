#version 330

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texture;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

uniform float lightIntensity;
uniform vec3 color;

out vec3 LightIntensity;

void main(){
	gl_Position = proj * view * model * vec4(vertex_position,1.0);

	LightIntensity = lightIntensity*color;
}
