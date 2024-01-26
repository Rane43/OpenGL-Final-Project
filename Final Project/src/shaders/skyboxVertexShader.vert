#version 330

layout (location = 0) in vec3 apos;

out vec3 texCoords;

uniform mat4 proj;
uniform mat4 view;

void main() {
	texCoords = apos;
	
	vec4 pos = proj*view*vec4(apos, 1.0);
	gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
}