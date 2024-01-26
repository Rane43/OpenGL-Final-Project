#version 330

in vec3 LightIntensity;
in vec2 TexCoord;
uniform sampler2D texture0;

void main(){
	gl_FragColor = vec4 (LightIntensity, 1.0);
}