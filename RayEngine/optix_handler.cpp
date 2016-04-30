#include "rayengine.h"

void printException(optix::Exception e) {

	cout << "OptiX error: " << e.getErrorString() << endl;
	system("pause");

}

void RayEngine::initOptix() {

	cout << "Starting OptiX..." << endl;

	try {
		
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

#if OPTIX_USE_OPENGL
		/// Generate buffer
		glGenBuffers(1, &OptixData.vbo);

		OptixData.buffer = OptixData.context->createBufferFromGLBO(RT_BUFFER_OUTPUT, OptixData.vbo[0]);
		OptixData.buffer->setFormat(RT_FORMAT_FLOAT4);
		OptixData.buffer ->setSize(window.width, window.height);
#else
		OptixData.buffer = OptixData.context->createBuffer(RT_BUFFER_OUTPUT, RT_FORMAT_FLOAT4, window.width, window.height);
#endif

		// Make ray generation program
		OptixData.context->setRayGenerationProgram(0, OptixData.context->createProgramFromPTXFile("ptx/camera_program.cu.ptx", "camera"));

		// Make miss program
		OptixData.context->setMissProgram(0, OptixData.context->createProgramFromPTXFile("ptx/miss_program.cu.ptx", "miss"));

		// Init scenes
		for (uint i = 0; i < scenes.size(); i++)
			scenes[i]->initOptix(OptixData.context);

		OptixData.context["bgColor"]->setFloat(0.9f, 0.3f, 0.3f);
		OptixData.context["sceneObj"]->set(scenes[0]->OptixData.group);
		OptixData.context["outputBuffer"]->set(OptixData.buffer);

		// Compile
		OptixData.context->validate();
		OptixData.context->compile();

	} catch (optix::Exception e) {
		printException(e);
	}

}

void Scene::initOptix(optix::Context context) {

	OptixData.group = context->createGroup();
	OptixData.group->setAcceleration(context->createAcceleration("Sbvh", "Bvh")); // TODO: change during runtime?

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
		OptixData.transform = context->createTransform();
		OptixData.transform->setChild(OptixData.geometryGroup);
		OptixData.transform->setMatrix(true, Mat4x4(matrix).e, NULL);

	} catch (optix::Exception e) {
		printException(e);
	}

}

void TriangleMesh::initOptix(optix::Context context) {

	try {

		static optix::Program intersectProgram = context->createProgramFromPTXFile("ptx/triangle_mesh_program.cu.ptx", "intersect");
		static optix::Program boundsProgram = context->createProgramFromPTXFile("ptx/triangle_mesh_program.cu.ptx", "bounds");

#if OPTIX_USE_OPENGL
		// Bind vertex VBO
		OptixData.posBuffer = context->createBufferFromGLBO(RT_BUFFER_INPUT, vboPos);
		OptixData.posBuffer->setFormat(RT_FORMAT_FLOAT3);
		OptixData.posBuffer->setSize(posData.size());

		// Bind vertex VBO
		OptixData.normalBuffer = context->createBufferFromGLBO(RT_BUFFER_INPUT, vboNormal);
		OptixData.normalBuffer->setFormat(RT_FORMAT_FLOAT3);
		OptixData.normalBuffer->setSize(normalData.size());

		// Bind index IBO
		OptixData.indexBuffer = context->createBufferFromGLBO(RT_BUFFER_INPUT, ibo);
		OptixData.indexBuffer->setFormat(RT_FORMAT_UNSIGNED_INT);
		OptixData.indexBuffer->setSize(indexData.size());
#else
		// Copy position buffer
		OptixData.posBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, posData.size());
		memcpy(OptixData.posBuffer->map(), &posData[0], posData.size() * sizeof(Vec3));
		OptixData.posBuffer->unmap();

		// Copy normal buffer
		OptixData.normalBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, normalData.size());
		memcpy(OptixData.normalBuffer->map(), &normalData[0], normalData.size() * sizeof(Vec3));
		OptixData.normalBuffer->unmap();

		// Copy index buffer
		OptixData.indexBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT3, indexData.size());
		memcpy(OptixData.indexBuffer->map(), &indexData[0], indexData.size() * sizeof(TrianglePrimitive));
		OptixData.indexBuffer->unmap();
#endif

		// Make geometry
		OptixData.geometry = context->createGeometry();
		OptixData.geometry->setIntersectionProgram(intersectProgram);
		OptixData.geometry->setBoundingBoxProgram(boundsProgram);
		OptixData.geometry->setPrimitiveCount(indexData.size());
		OptixData.geometry["posData"]->setBuffer(OptixData.posBuffer);
		OptixData.geometry["normalData"]->setBuffer(OptixData.normalBuffer);
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

	// Set dimensions
	if (rayTracingTarget == RTT_HYBRID) {
		OptixData.offset = EmbreeData.width;
		OptixData.width = window.width - OptixData.offset;
	} else {
		OptixData.offset = 0;
		OptixData.width = window.width;
	}

	// Resize buffer object
	OptixData.buffer->setSize(OptixData.width, window.height);

	// Resize VBO
#if OPTIX_USE_OPENGL
	OptixData.buffer->unregisterGLBuffer();
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, OptixData.vbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(float) * 4 * OptixData.width * window.height, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	OptixData.buffer->registerGLBuffer();
#endif

}