#ifndef material_h
#define material_h

#include <maths_funcs.h>

class Material {
private:

public:
	vec3 ambientCI = vec3(1.0, 1.0, 1.0);
	vec3 diffuseCI = vec3(1.0, 1.0, 1.0);	
	vec3 specularCI = vec3(1.0, 1.0, 1.0);
	int phong = 1;

	Material();

	Material(vec3 ambientReflectance, vec3 diffuseReflectance, vec3 specularReflectance, int phongExponenent);

};
#endif