#pragma once

#include "image.h"

#include <optixu/optixpp_namespace.h>

struct Material {

	Material();

	Color ambient, specular, diffuse;
	float shineExponent, reflectIntensity, refractIndex;
	Image* image;

	struct Optix {
		optix::Material material;
		optix::TextureSampler sampler;
	} Optix;

};
