#include "renderer.h"

EmbreeRenderer::EmbreeRenderer(EmbreeHandler* handler) {
	this->handler = handler;
}

void EmbreeRenderer::getPixels(Color* pixels, int width, int height, Scene* Scene) {

	float ratio = (float)width / height;
	float tfov = embree::tan(embree::deg2rad(Scene->camera.fov / 2.f));

	// TODO: Add ray 1..8 and use queue system depending on branko's results
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			float dx = ((float)x / width) * 2 - 1;
			float dy = ((float)y / height) * 2 - 1;
			Vec3 dir = dx * Scene->camera.xaxis * ratio * tfov + dy * Scene->camera.yaxis * tfov + Scene->camera.zaxis;

			RTCRay ray;
			ray.org[0] = Scene->camera.position.x();
			ray.org[1] = Scene->camera.position.y();
			ray.org[2] = Scene->camera.position.z();
			ray.dir[0] = dir.x();
			ray.dir[1] = dir.y();
			ray.dir[2] = dir.z();
			ray.tnear = 0.1f;
			ray.tfar = FLT_MAX;
			ray.instID = ray.geomID = ray.primID = RTC_INVALID_GEOMETRY_ID;
			ray.mask = -1;
			ray.time = 0;

			rtcIntersect(Scene->embreeScene, ray);

			if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
				pixels[y * width + x] = Color(ray.u, ray.v, 1, 1); // getRayColor(&ray, Scene);
			else
				pixels[y * width + x] = Color(0, 0, 0, 1);

		}
	}

}
