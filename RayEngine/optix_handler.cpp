#include "rayengine.h"

void printException(optix::Exception e) {

	cout << "OptiX error: " << e.getErrorString() << endl;
	system("pause");

}

void RayEngine::initOptix() {

	try {

		/// Generate buffer
		glGenBuffers(1, &OptixData.vbo);

		// Generate texture
		glGenTextures(1, &OptixData.texture);
		glBindTexture(GL_TEXTURE_2D, OptixData.texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Make context
		OptixData.context = optix::Context::create();
		OptixData.context->setRayTypeCount(1);
		OptixData.context->setEntryPointCount(1);

		// Make output buffer
		OptixData.buffer = OptixData.context->createBufferFromGLBO(RT_BUFFER_OUTPUT, OptixData.vbo);
		OptixData.buffer->setFormat(RT_FORMAT_FLOAT4);
		OptixData.buffer->setSize(window.width, window.height);
		OptixData.context["outputBuffer"]->set(OptixData.buffer);

		// Make ray generation program
		OptixData.context->setRayGenerationProgram(0, OptixData.context->createProgramFromPTXFile("ptx/camera_program.cu.ptx", "camera"));

		// Make miss program
		OptixData.context->setMissProgram(0, OptixData.context->createProgramFromPTXFile("ptx/miss_program.cu.ptx", "miss"));

		// Init scenes
		for (uint i = 0; i < scenes.size(); i++)
			scenes[i]->initOptix(OptixData.context);

		OptixData.context["bgColor"]->setFloat(0.3f, 0.3f, 0.3f);
		OptixData.context["sceneObj"]->set(scenes[0]->OptixData.group);

		// Compile
		OptixData.context->validate();
		OptixData.context->compile();

	} catch (optix::Exception e) {
		printException(e);
	}

}

void Scene::initOptix(optix::Context context) {

	OptixData.group = context->createGroup();
	OptixData.group->setAcceleration(context->createAcceleration("Trbvh", "Bvh")); // TODO: change during runtime?

	for (uint i = 0; i < objects.size(); i++) {
		objects[i]->initOptix(context);
		OptixData.group->addChild(objects[i]->OptixData.transform);
	}

}


void Object::initOptix(optix::Context context) {

	try {
		
		// Make geometry group
		OptixData.geometryGroup = context->createGeometryGroup();
		OptixData.geometryGroup->setAcceleration(context->createAcceleration("Trbvh", "Bvh")); // TODO: change during runtime?


		// Add geometries
		for (uint i = 0; i < geometries.size(); i++) {
			geometries[i]->initOptix(context);
			OptixData.geometryGroup->addChild(((TriangleMesh*)geometries[i])->OptixData.geometryInstance);
		}

		// Make transform
		Mat4x4 def;
		OptixData.transform = context->createTransform();
		OptixData.transform->setChild(OptixData.geometryGroup);
		OptixData.transform->setMatrix(true, def.e, NULL);

	} catch (optix::Exception e) {
		printException(e);
	}

}

void TriangleMesh::initOptix(optix::Context context) {

	try {

		static optix::Program intersectProgram = context->createProgramFromPTXFile("ptx/triangle_mesh_program.cu.ptx", "intersect");
		static optix::Program boundsProgram = context->createProgramFromPTXFile("ptx/triangle_mesh_program.cu.ptx", "bounds");

		// Bind vertex VBO
		OptixData.posBuffer = context->createBufferFromGLBO(RT_BUFFER_INPUT, vboPos);
		OptixData.posBuffer->setFormat(RT_FORMAT_FLOAT3);
		OptixData.posBuffer->setSize(posData.size());

		// Bind index IBO
		OptixData.indexBuffer = context->createBufferFromGLBO(RT_BUFFER_INPUT, ibo);
		OptixData.indexBuffer->setFormat(RT_FORMAT_UNSIGNED_INT);
		OptixData.indexBuffer->setSize(indexData.size());

		// Make geometry
		OptixData.geometry = context->createGeometry();
		OptixData.geometry->setIntersectionProgram(intersectProgram);
		OptixData.geometry->setBoundingBoxProgram(boundsProgram);
		OptixData.geometry->setPrimitiveCount(indexData.size());
		OptixData.geometry["posData"]->setBuffer(OptixData.posBuffer);
		OptixData.geometry["indexData"]->setBuffer(OptixData.indexBuffer);

		// Make instance
		optix::Material material = context->createMaterial();
		material->setClosestHitProgram(0, context->createProgramFromPTXFile("ptx/material_program.cu.ptx", "closestHit"));

		OptixData.geometryInstance = context->createGeometryInstance();
		OptixData.geometryInstance->setGeometry(OptixData.geometry);
		OptixData.geometryInstance->addMaterial(material);

	} catch (optix::Exception e) {
		printException(e);
	}

}

void RayEngine::resizeOptix() {

	// Resize buffer object
	OptixData.buffer->setSize(window.width, window.height);

	// Resize VBO
	OptixData.buffer->unregisterGLBuffer();
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, OptixData.vbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(float) * 4 * window.width * window.height, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	OptixData.buffer->registerGLBuffer();

}