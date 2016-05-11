#include "material.h"

Material::Material() :
    ambient({ 0.f }),
    specular({ 1.f }),
    diffuse({ 1.f }),
    shineExponent(100.f),
	reflectIntensity(0.f),
	refractIndex(1.f)
{
	Optix.material = nullptr;
}