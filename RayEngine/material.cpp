#include "material.h"

Material::Material() :
    diffuse({ 1.f, 1.f, 1.f }),
    shininess(0.05f)
{
	OptixData.material = nullptr;
}