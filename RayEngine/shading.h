#pragma once

#include "common.h"
#include "light.h"
#include "material.h"

namespace Shading {

	// TODO
	// Flexible shaders
	Color blinnPhong(const Vec3& position, const Vec3& camera, const Light& Light, const Vec3& normal, const Material& material, const Color& textureDiffuse);

}
