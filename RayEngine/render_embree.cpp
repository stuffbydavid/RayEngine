#include "rayengine.h"

void RayEngine::renderEmbree() {

	float ratio = (float)window.width / window.height;
	float tfov = embree::tan(embree::deg2rad(scenes[0]->camera.fov / 2.f));

	Vec3 rxaxis = scenes[0]->camera.xaxis * ratio * tfov;
	Vec3 ryaxis = scenes[0]->camera.yaxis * tfov;
	Vec3 rzaxis = scenes[0]->camera.zaxis;

	// TODO: Add ray 1..8 and use queue system depending on branko's results
    //#pragma omp parallel for schedule(dynamic)
	for (int y = 0; y < window.height; y++) {
		for (int x = 0; x < window.width * (rayTracingTarget == RTT_HYBRID ? hybridPartition : 1.0); x++) {

			float dx = ((float)x / window.width) * 2 - 1;
			float dy = ((float)y / window.height) * 2 - 1;
			Vec3 dir = dx * rxaxis + dy * ryaxis + rzaxis;

			RTCRay ray;
			ray.org[0] = scenes[0]->camera.position.x();
			ray.org[1] = scenes[0]->camera.position.y();
			ray.org[2] = scenes[0]->camera.position.z();
			ray.dir[0] = dir.x();
			ray.dir[1] = dir.y();
			ray.dir[2] = dir.z();
			ray.tnear = 0.1f;
			ray.tfar = FLT_MAX;
			ray.instID = ray.geomID = ray.primID = RTC_INVALID_GEOMETRY_ID;
			ray.mask = -1;
			ray.time = 0;

			rtcIntersect(scenes[0]->EmbreeData.scene, ray);

			if (ray.geomID != RTC_INVALID_GEOMETRY_ID) {
				Vec3 n = Vec3::normalize(Vec3(ray.Ng)) * 0.5 + Vec3(0.5);
				EmbreeData.buffer[y * window.width + x] = Color(n.x(), n.y(), n.z(), 1); // getRayColor(&ray, Scene);
			} else
				EmbreeData.buffer[y * window.width + x] = Color(0, 0, 0, 1);

		}
	}

	glBindTexture(GL_TEXTURE_2D, EmbreeData.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.width, window.height, 0, GL_RGBA, GL_FLOAT, EmbreeData.buffer);

	// TODO: buffer for this?
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f);
	glVertex2f(0.f, 0.f);
	glTexCoord2f(1.f, 0.f);
	glVertex2f(1.f, 0.f);
	glTexCoord2f(1.f, 1.f);
	glVertex2f(1.f, 1.f);
	glTexCoord2f(0.f, 1.f);
	glVertex2f(0.f, 1.f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

}

/*#include "renderer.h"

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

			//rtcIntersect(Scene->embreeScene, ray);

			if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
				pixels[y * width + x] = Color(ray.u, ray.v, 1, 1); // getRayColor(&ray, Scene);
			else
				pixels[y * width + x] = Color(0, 0, 0, 1);

		}
	}

}*/


/*#include "renderer.h"


Renderer::Renderer() {
#ifdef EMBREE_RENDERER
	device = rtcNewDevice(NULL);
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif
}

Renderer::~Renderer() {
#ifdef EMBREE_RENDERER
	rtcDeleteDevice(device);
#endif
}

void Renderer::getPixels(Color* pixels, int width, int height, Scene* Scene) {

	float ratio = (float)width / height;

#ifdef EMBREE_RENDERER
#ifdef EMBREE_MATH	// Protected due to the use of Embree's tan and deg2rad
	float tfov = embree::tan(embree::deg2rad(Scene->camera.fov / 2.f));
#endif

#ifdef METHOD_1
#ifdef RAY_1
#pragma omp parallel for schedule(dynamic)
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			float dx = ((float)x / width) * 2 - 1;
			float dy = ((float)y / height) * 2 - 1;
			Vec3 dir = dx * Scene->camera.xaxis * ratio * tfov + dy * Scene->camera.yaxis * tfov + Scene->camera.zaxis;

			RTCRay ray;
			defineRay(&ray, Scene->camera.position, dir);

			rtcIntersect(Scene->scene, ray);

			pixels[y * width + x] = getRayColor(&ray, Scene);

		}
	}
#endif
#ifdef RAY_8
	__aligned(32) int valid[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < (height * width) / 8 + ((height * width) % 8 == 0 ? 0 : 1); i++) {

		RTCRay8 ray;
		for (int index = 0; index < 8; index++) {
			int x = (i * 8 + index) % width;
			int y = (i * 8 + index) / width;
			if (y == height)
				break;
			float dx = ((float)x / width) * 2 - 1;
			float dy = ((float)y / height) * 2 - 1;
			Vec3 dir = dx * Scene->camera.xaxis * ratio * tfov + dy * Scene->camera.yaxis * tfov + Scene->camera.zaxis;

			defineRay(&ray, index, Scene->camera.position, dir);
		}

		rtcIntersect8(valid, Scene->scene, ray);

		for (int index = 0; index < 8; index++) {
			int x = (i * 8 + index) % width;
			int y = (i * 8 + index) / width;
			if (y == height)
				break;
			pixels[x + y * width] = getRayColor(&ray, index, Scene);
		}

	}
#endif
#endif	// METHOD_1

#ifdef METHOD_2
	// Rays definition
#ifdef RAY_1
	RTCRay* rays = new RTCRay[height * width];
#endif
#ifdef RAY_8
	RTCRay8* rays = new RTCRay8[(height * width) / 8 + ((height * width) % 8 == 0 ? 0 : 1)];
	__aligned(32) int valid[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
#endif
#pragma omp parallel for schedule(dynamic)
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			float dx = ((float)x / width) * 2 - 1;
			float dy = ((float)y / height) * 2 - 1;
			Vec3 dir = dx * Scene->camera.xaxis * ratio * tfov + dy * Scene->camera.yaxis * tfov + Scene->camera.zaxis;
#ifdef RAY_1
			defineRay(&rays[x + y * width], Scene->camera.position, dir);
#endif
#ifdef RAY_8
			defineRay(&rays[(x + y * width) / 8], (x + y * width) % 8, Scene->camera.position, dir);
#endif
		}
	}

	// Intersections
#ifdef RAY_1
#pragma omp parallel for schedule(dynamic)
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			rtcIntersect(Scene->scene, rays[x + y * width]);

		}
	}
#endif
#ifdef RAY_8
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < (height * width) / 8 + ((height * width) % 8 == 0 ? 0 : 1); i++) {

		rtcIntersect8(valid, Scene->scene, rays[i]);

	}
#endif

	// Color
#pragma omp parallel for schedule(dynamic)
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
#ifdef RAY_1
			pixels[y * width + x] = getRayColor(&rays[x + y * width], Scene);
#endif
#ifdef RAY_8
			pixels[y * width + x] = getRayColor(&rays[(x + y * width) / 8], (x + y * width) % 8, Scene);
#endif
		}
	}

	delete rays;

#endif	// METHOD_2

#endif	// EMBREE_RENDERER

}

#ifdef EMBREE_RENDERER
void Renderer::defineRay(RTCRay* pRay, const Vec3& origin, const Vec3& direction) {

	pRay->org[0] = origin.x();
	pRay->org[1] = origin.y();
	pRay->org[2] = origin.z();
	pRay->dir[0] = direction.x();
	pRay->dir[1] = direction.y();
	pRay->dir[2] = direction.z();
	pRay->tnear = 0.1f;
	pRay->tfar = FLT_MAX;
	pRay->instID = pRay->geomID = pRay->primID = RTC_INVALID_GEOMETRY_ID;
	pRay->mask = -1;
	pRay->time = 0;

}
void Renderer::defineRay(RTCRay8* pRay, const int index, const Vec3& origin, const Vec3& direction) {

	pRay->orgx[index] = origin.x();
	pRay->orgy[index] = origin.y();
	pRay->orgz[index] = origin.z();
	pRay->dirx[index] = direction.x();
	pRay->diry[index] = direction.y();
	pRay->dirz[index] = direction.z();
	pRay->tnear[index] = 0.1f;
	pRay->tfar[index] = FLT_MAX;
	pRay->instID[index] = pRay->geomID[index] = pRay->primID[index] = RTC_INVALID_GEOMETRY_ID;
	pRay->mask[index] = -1;
	pRay->time[index] = 0;

}
Color Renderer::getRayColor(RTCRay* pRay, Scene* Scene, int iteration) {

	Color color = Scene->ambient;

	if (pRay->geomID != RTC_INVALID_GEOMETRY_ID) {

		Object* hitObj = Scene->objectsMap[pRay->instID];
		TriangleMesh* mesh = (TriangleMesh*)hitObj->geometry;
		Material* hitMat = mesh->indexData[pRay->primID].material;
		Vec3 hitNorm = mesh->getNormal(pRay->primID, pRay->u, pRay->v);
		Color texColor = { 1, 1, 1, 1 };

		if (hitMat->textureFile != "") {
			Vec2 hitTex = mesh->getTexCoord(pRay->primID, pRay->u, pRay->v);

			int px = fmod(hitTex.x(), 1.f)*(float)hitMat->imageWidth;
			int py = fmod(1.0 - hitTex.y(), 1.f)*(float)hitMat->imageHeight;
			int i = px * hitMat->imageHeight + py;

			texColor = {
				(float)hitMat->imageData[i].red / USHRT_MAX,
				(float)hitMat->imageData[i].green / USHRT_MAX,
				(float)hitMat->imageData[i].blue / USHRT_MAX,
				1.f
			};
		}

		// Check light
		Vec3 intersection = Vec3(pRay->org) + Vec3(pRay->dir) * pRay->tfar;

		for (int i = 0; i < Scene->lights.size(); i++) {
#ifdef SHADOWS
			RTCRay shadowRay;
			Vec3 toLight = Scene->lights[i]->position - intersection;

			defineRay(&shadowRay, intersection, normalize(toLight));
			shadowRay.tnear = 0.01f;
			shadowRay.tfar = length(toLight) - shadowRay.tnear;

			rtcOccluded(Scene->scene, shadowRay);

			if (shadowRay.geomID)
#endif	// SHADOWS
				color = color + Shading::blinnPhong(intersection, Scene->camera.position, *Scene->lights[i], hitNorm, *hitMat, texColor);
		}

		// Reflections
#ifdef REFLECTIONS
#define MAX_ITERATIONS 3
		if (hitMat->shininess > 0.0 && iteration < MAX_ITERATIONS) {
			Vec3 reflectDir = reflect(-Vec3(pRay->dir), hitNorm);
			RTCRay reflectRay;
			defineRay(&reflectRay, intersection, reflectDir);
			rtcIntersect(Scene->scene, reflectRay);
			color = color + getRayColor(&reflectRay, Scene, iteration + 1) * hitMat->shininess;
		}
#endif	// REFLECTIONS
	} else {
		color = Scene->background;
	}

	return color;

}

float fmod2(float x, float y) {
	while (x < 0) x += y;
	return fmod(x, y);
}
Color Renderer::getRayColor(RTCRay8* pRay, const int index, Scene* Scene, int iteration) {

	Color color = Scene->ambient;

	if (pRay->geomID[index] != RTC_INVALID_GEOMETRY_ID) {

		Object* hitObj = Scene->objectsMap[pRay->instID[index]];
		TriangleMesh* mesh = (TriangleMesh*)hitObj->geometry;
		Material* hitMat = mesh->indexData[pRay->primID[index]].material;
		Vec3 hitNorm = mesh->getNormal(pRay->primID[index], pRay->u[index], pRay->v[index]);
		Color texColor = { 1, 1, 1, 1 };

		if (hitMat->textureFile != "") {
			Vec2 hitTex = mesh->getTexCoord(pRay->primID[index], pRay->u[index], pRay->v[index]);
			
			int px = fmod2(hitTex.x(), 1.f)*(float)hitMat->imageWidth;
			int py = fmod2(1.0 - hitTex.y(), 1.f)*(float)hitMat->imageHeight;
			int i = px + py * hitMat->imageWidth;

			texColor = {
				(float)hitMat->imageData[i].red / USHRT_MAX,
				(float)hitMat->imageData[i].green / USHRT_MAX,
				(float)hitMat->imageData[i].blue / USHRT_MAX,
				1.f
			};
		}

		// Check light
		Vec3 intersection = Vec3(pRay->orgx[index], pRay->orgy[index], pRay->orgz[index]) + Vec3(pRay->dirx[index], pRay->diry[index], pRay->dirz[index]) * pRay->tfar[index];

		for (int i = 0; i < Scene->lights.size(); i++) {
#ifdef SHADOWS
			RTCRay shadowRay;
			Vec3 toLight = Scene->lights[i]->position - intersection;

			defineRay(&shadowRay, intersection, normalize(toLight));
			shadowRay.tnear = 0.01f;
			shadowRay.tfar = length(toLight) - shadowRay.tnear;

			rtcOccluded(Scene->scene, shadowRay);

			if (shadowRay.geomID)
#endif	// SHADOWS
				color = color + Shading::blinnPhong(intersection, Scene->camera.position, *Scene->lights[i], hitNorm, *hitMat, texColor);
		}

		// Reflections
#ifdef REFLECTIONS
#define MAX_ITERATIONS 3
		if (hitMat->shininess > 0.0 && iteration < MAX_ITERATIONS) {
			Vec3 reflectDir = reflect(-Vec3(pRay->dirx[index], pRay->diry[index], pRay->dirz[index]), hitNorm);
			RTCRay reflectRay;
			defineRay(&reflectRay, intersection, reflectDir);
			rtcIntersect(Scene->scene, reflectRay);
			color = color + getRayColor(&reflectRay, Scene, iteration + 1) * 0.1;// *hitMat->shininess; // TODO
		}
#endif	// REFLECTIONS
		color = texColor;
	} else {
		color = Scene->background;
	}

	return color;

}
#endif
*/