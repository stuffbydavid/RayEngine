#include "object.h"

Object::Object(string name) :
  name(name),
  geometry(nullptr)
{
	
}

Object::~Object() {
	//delete geometry;
}

void Object::initEmbreeScene(RTCDevice device) {

	//TODO: Check other instance modes
	RTCSceneFlags sFlags = RTC_SCENE_DYNAMIC | RTC_SCENE_COMPACT;
	RTCAlgorithmFlags aFlags = RTC_INTERSECT1 | RTC_INTERSECT4 | RTC_INTERSECT8;

	embreeScene = rtcDeviceNewScene(device, sFlags, aFlags);

	switch (geometry->getGeometryType()) {
		case Geometry::TriangleMesh:

			TriangleMesh* pMesh = (TriangleMesh*)geometry;
			pMesh->id = rtcNewTriangleMesh(embreeScene, RTC_GEOMETRY_STATIC, pMesh->primitives.size(), pMesh->posData.size());

			struct Vertex   { float x, y, z, a; };
			struct Triangle { int v0, v1, v2; };

			Vertex* vertices = (Vertex*)rtcMapBuffer(embreeScene, pMesh->id, RTC_VERTEX_BUFFER);
			// fill vertices here
			for (int i = 0; i < pMesh->posData.size(); i++) {
				vertices[i].x = pMesh->posData[i].x();
				vertices[i].y = pMesh->posData[i].y();
				vertices[i].z = pMesh->posData[i].z();
				vertices[i].a = 0;
			}
			rtcUnmapBuffer(embreeScene, pMesh->id, RTC_VERTEX_BUFFER);

			Triangle* triangles = (Triangle*)rtcMapBuffer(embreeScene, pMesh->id, RTC_INDEX_BUFFER);
			// fill triangle indices here
			for (int i = 0; i < pMesh->primitives.size(); i++) {
				triangles[i].v0 = pMesh->primitives[i].indices[0];
				triangles[i].v1 = pMesh->primitives[i].indices[1];
				triangles[i].v2 = pMesh->primitives[i].indices[2];
			}
			rtcUnmapBuffer(embreeScene, pMesh->id, RTC_INDEX_BUFFER);

			break;
	}

	rtcCommit(embreeScene);

}


Object* Object::load(string file, Material* defaultMaterial) {

	vector<tinyobj::shape_t> fileShapes;
	vector<tinyobj::material_t> fileMaterials;
	string path = file.substr(0, file.find_last_of("/\\") + 1);
	string err;
	Object* ret = nullptr;

	cout << "Loading " << file << "..." << endl;
	tinyobj::LoadObj(fileShapes, fileMaterials, err, &file[0], &path[0]);

	if (!err.empty()) {
		cerr << err << endl;
		return ret;
	}
	if (fileShapes.empty()) {
		cerr << "No objects found" << endl;
		return ret;
	}
	cout << "Objects found: " << fileShapes.size() << endl;

	// Convert to our material type
	vector<Material*> materials(fileMaterials.size());
	if (fileMaterials.size() > 0) {
		for (int i = 0; i < materials.size(); i++) {

			Material* mat = new Material(fileMaterials[i].name);

			mat->id = i;
			mat->ambient = Color(fileMaterials[i].ambient);
			mat->diffuse = Color(fileMaterials[i].diffuse);
			mat->specular = Color(fileMaterials[i].specular);
			mat->shininess = fileMaterials[i].shininess; // TODO: Don't use this for reflection
			mat->textureFile = fileMaterials[i].diffuse_texname;

			if (mat->textureFile != "") {
				mat->textureFile = path + mat->textureFile;
				mat->image.read(mat->textureFile);
				mat->imageData = mat->image.getPixels(0, 0, mat->image.columns(), mat->image.rows());
				mat->imageWidth = mat->image.columns();
				mat->imageHeight = mat->image.rows();
			}

			materials[i] = mat;

		}
	} else if (!defaultMaterial) {

		// Generate new basic material
		Material* mat = new Material();

		mat->id = 0;
		mat->ambient = Color(0.f, 0.f, 0.f, 1.f);
		mat->diffuse = Color(1.f, 1.f, 1.f, 1.f);
		mat->specular = Color(0.f, 0.f, 0.f, 1.f);
		mat->shininess = 0.f;
		mat->textureFile = "";

		materials.push_back(mat);

	} else
		materials.push_back(defaultMaterial);

	// Create a root
	if (fileShapes.size() > 1)
		ret = new Object();

	for (uint i = 0; i < fileShapes.size(); i++) {

		Object* obj = new Object(fileShapes[i].name);
		obj->id = i;
		if (obj->name == "")
			obj->name = "Untitled";

		TriangleMesh* triangleMesh = new TriangleMesh();
		triangleMesh->id = i;
		obj->geometry = triangleMesh;

		cout << "  " << obj->name << endl;

		// Vertices
		uint vertices = fileShapes[i].mesh.positions.size() / 3;
		triangleMesh->posData = vector<Vec3>(vertices);
		for (uint v = 0; v < vertices; v++) {
			triangleMesh->posData[v] = {
				fileShapes[i].mesh.positions[3 * v + 0],
				fileShapes[i].mesh.positions[3 * v + 1],
				fileShapes[i].mesh.positions[3 * v + 2]
			};
		}

		// Texture Coords
		triangleMesh->texCoordData = vector<Vec2>(vertices);
		if (fileShapes[i].mesh.texcoords.size() > 0) {
			for (uint v = 0; v < vertices; v++) {
				triangleMesh->texCoordData[v] = {
					fileShapes[i].mesh.texcoords[2 * v + 0],
					fileShapes[i].mesh.texcoords[2 * v + 1]
				};
			}
		} else
			for (uint v = 0; v < vertices; v++)
				triangleMesh->texCoordData[v] = { 0, 0 };

		// Triangles
		int triangles = fileShapes[i].mesh.indices.size() / 3;
		triangleMesh->primitives = vector<TrianglePrimitive>(triangles);
		for (int t = 0; t < triangles; t++) {
			triangleMesh->primitives[t].id = t;
			triangleMesh->primitives[t] = {
				(uint)fileShapes[i].mesh.indices[3 * t + 0],
				(uint)fileShapes[i].mesh.indices[3 * t + 1],
				(uint)fileShapes[i].mesh.indices[3 * t + 2]
			};

			int material_id = fileShapes[i].mesh.material_ids[t];
			if (material_id < 0)
				material_id = 0;
			for (uint m = 0; m < materials.size(); m++) {
				if (materials[m]->id == material_id) {
					triangleMesh->primitives[t].material = materials[m];
					break;
				}
			}
		}

		cout << "  Vertices: " << vertices << endl;
		cout << "  Triangles: " << triangles << endl;

		// Normals
		triangleMesh->normalData = vector<Vec3>(vertices);
		for (uint n = 0; n < vertices; n++)
			triangleMesh->normalData[n] = Vec3(0.f, 0.f, 0.f);

		//TODO: Check if obj file has normals
		// Calculate normals by going through each triangle
		for (uint i = 0; i < triangles; i++) {
			TrianglePrimitive& currentTriangle = triangleMesh->primitives[i];

			// Calculate normal of the triangle
			Vec3 normal = Vec3::cross(
				triangleMesh->posData[currentTriangle.indices[1]] - triangleMesh->posData[currentTriangle.indices[0]],
				triangleMesh->posData[currentTriangle.indices[2]] - triangleMesh->posData[currentTriangle.indices[0]]
				);

			// Add triangle normal to each vertex
			triangleMesh->normalData[currentTriangle.indices[0]] = triangleMesh->normalData[currentTriangle.indices[0]] + normal;
			triangleMesh->normalData[currentTriangle.indices[1]] = triangleMesh->normalData[currentTriangle.indices[1]] + normal;
			triangleMesh->normalData[currentTriangle.indices[2]] = triangleMesh->normalData[currentTriangle.indices[2]] + normal;
		}

		// Normalize normals
		for (uint n = 0; n < vertices; n++)
			triangleMesh->normalData[n] = Vec3::normalize(triangleMesh->normalData[n]);

		// Set up vbo (Vertex buffer object) and ibo (Index buffer object)
		glGenBuffers(1, &triangleMesh->vbo);
		glGenBuffers(1, &triangleMesh->ibo);
		uint sizePositions = vertices * sizeof(Vec3);
		uint sizeNormals = vertices * sizeof(Vec3);
		uint sizeTexCoords = vertices * sizeof(Vec2);
		uint sizeIndices = triangles * sizeof(TrianglePrimitive);

		glBindBuffer(GL_ARRAY_BUFFER, triangleMesh->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizePositions + sizeNormals + sizeTexCoords, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizePositions, &triangleMesh->posData[0]);
		glBufferSubData(GL_ARRAY_BUFFER, sizePositions, sizeNormals, &triangleMesh->normalData[0]);
		glBufferSubData(GL_ARRAY_BUFFER, sizePositions + sizeNormals, sizeTexCoords, &triangleMesh->texCoordData[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleMesh->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &triangleMesh->primitives[0], GL_STATIC_DRAW);

		if (ret)
			ret->children.push_back(obj);
		else
			ret = obj;

	}

	return ret;

}