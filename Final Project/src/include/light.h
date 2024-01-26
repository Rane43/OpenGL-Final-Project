#ifndef light_h
#define light_h

#include <maths_funcs.h>
#include <model.h>
#include <shader.h>

#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2


class Light : public Model{
private:
	float intensity = 1.0;
	vec3 color = vec3(1.0, 1.0, 1.0);

	int type = NULL;

public:
	Light(float intensity, vec3 position, int lightShaderID, const char* filepath, int type);

	// Getter functions
	int getLightType();
	float getIntensity();
	vec3 getColor();

	// Setter functions
	void updateIntensity(float LI);
	void updateColor(vec3 color);

};

#endif