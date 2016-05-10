#include "material.h"

Material::Material() :
    ambient({ 0.f }),
    specular({ 1.f }),
    diffuse({ 1.f }),
    shininess(10.f)
{
	Optix.material = nullptr;
}