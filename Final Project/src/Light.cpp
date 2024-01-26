#include <light.h>


Light::Light(float intensity, vec3 position, int lightShaderID, const char* filepath, int type) : Model(filepath, lightShaderID)
{
	this->intensity = intensity;
	if (intensity < 0) {
		this->intensity = 0.0f;
		std::cout << "Negative light intensity has been clamped to zero.\n";
	}
	this->updateModelPosition(position);

	this->type = type;
}


int Light::getLightType()
{
	return type;
}

// Getter Functions
float Light::getIntensity() {
	return intensity;
}

vec3 Light::getColor()
{
	return color;
}


// Setter functions
void Light::updateIntensity(float LI) {
	intensity = LI;
}

void Light::updateColor(vec3 color)
{
	this->color = color;
}
