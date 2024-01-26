#include <material.h>

Material::Material()
{
}

Material::Material(vec3 ambientReflectance, vec3 diffuseReflectance, vec3 specularReflectance, int phongExponenent)
	: ambientCI(ambientReflectance), diffuseCI(diffuseReflectance), specularCI(specularReflectance), phong(phongExponenent)
{

}
