// TODO Remove


#include "shading.h"

Color computeDiffuseIntensity(const Vec3& light, const Vec3& normal, const Light& Light, const Material& material, const Color& textureColor);
Color computeSpecularIntensity(const Vec3& halfway, const Vec3& normal, const Light& Light, const Material& material);

Color Shading::blinnPhong(const Vec3& position, const Vec3& camera, const Light& Light, const Vec3& normal, const Material& material, const Color& textureColor) {

	Vec3 light, viewpoint, halfway, normal_;

	// L - Light Vector
	light = Light.position - position;
	light = Vec3::normalize(light);

	// V - Viewpoint Vector
	viewpoint = camera - position;
	viewpoint = Vec3::normalize(viewpoint);

	// H - Halfway Vector
	halfway = viewpoint + light;
	halfway = Vec3::normalize(halfway);

	// N - Normal Vector
	normal_ = Vec3::normalize(normal);

	Color diffuse_intensity = computeDiffuseIntensity(light, normal_, Light, material, textureColor);
	Color specular_intensity = computeSpecularIntensity(halfway, normal_, Light, material);

	return material.ambient + diffuse_intensity;// +specular_intensity;

}

__forceinline
Color computeDiffuseIntensity(const Vec3& light, const Vec3& normal, const Light& Light, const Material& material, const Color& textureColor) {
	return material.diffuse * textureColor * Light.color * embree::max(Vec3::dot(light, normal), 0.f);
}

__forceinline
Color computeSpecularIntensity(const Vec3& halfway, const Vec3& normal, const Light& Light, const Material& material) {
	return material.specular * Light.color * embree::pow(Vec3::dot(halfway, normal), material.shininess);
}