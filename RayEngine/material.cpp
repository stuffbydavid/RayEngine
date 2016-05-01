#include "material.h"

Material::Material() :
    ambient({ 0.f, 0.f, 0.f }),
    diffuse({ 1.f, 1.f, 1.f }),
    specular({ 1.f, 1.f, 1.f }),
    shininess(0.f)
{
	OptixData.material = nullptr;
}