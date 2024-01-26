#version 330

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texture;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

uniform vec3 camPos;


void main(){
	// Vertex Texture Coords
	TexCoord = vertex_texture;

	
    FragPos = vec3(model * vec4(vertex_position, 1.0));
    Normal = normalize(mat3(transpose(inverse(model))) * vertex_normal);  
    
    gl_Position = proj * view * vec4(FragPos, 1.0);

}


  