#pragma once

#include "common.h"
#include "image.h"

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
