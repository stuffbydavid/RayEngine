#include "object_loader.h"

string GetPath(const string& file) {
	size_t found = file.find_last_of("/\\");
	return(file.substr(0, found + 1));
}

void ObjectLoader::loadObjects(vector<Object*> *objList, vector<Material*> *matList, string inputFile) {

	vector<tinyobj::shape_t> fileShapes;
	vector<tinyobj::material_t> fileMaterials;
	string err, path = GetPath(inputFile);

	tinyobj::LoadObj(fileShapes, fileMaterials, err, &inputFile[0], &path[0]);

	if (!err.empty()) {
		cerr << err << endl;
		return;
	}

	// Convert to our material type
	vector<Material*> materials(fileMaterials.size());
	if (fileMaterials.size() > 0) {
		for (int i = 0; i < materials.size(); i++) {

			Material* pMat = new Material();
			Material& mat = *pMat;

			mat.id = i;
			mat.ambient = Color(fileMaterials[i].ambient);
			mat.diffuse = Color(fileMaterials[i].diffuse);
			mat.specular = Color(fileMaterials[i].specular);
			mat.shininess = fileMaterials[i].shininess; // TODO: Don't use this for reflection
			mat.textureFile = fileMaterials[i].diffuse_texname;

			if (mat.textureFile != "") {
				mat.textureFile = path + mat.textureFile;
				mat.image.read(mat.textureFile);
				mat.imageData = mat.image.getPixels(0, 0, mat.image.columns(), mat.image.rows());
				mat.imageWidth = mat.image.columns();
				mat.imageHeight = mat.image.rows();
			}

			materials[i] = pMat;
			matList->push_back(pMat);

		}
	} else {

		// Default texture
		Material* pDef = new Material();
		Material& def = *pDef;

		def.id = 0;
		def.ambient = Color(0.f, 0.f, 0.f, 1.f);
		def.diffuse = Color(1.f, 1.f, 1.f, 1.f);
		def.specular = Color(0.f, 0.f, 0.f, 1.f);
		def.shininess = 0.f;
		def.textureFile = "";

		materials.push_back(pDef);

	}

	// Convert to our object type, for each shape, push into the target list
	for (uint i = 0; i < fileShapes.size(); i++) {

		Object* pObj = new Object();
		Object& obj = *pObj;
		obj.id = i;
		obj.name = fileShapes[i].name;

		TriangleMesh* pTriangleMesh = new TriangleMesh();
		TriangleMesh& triangleMesh = *pTriangleMesh;
		triangleMesh.id = i;

		// Vertices
		uint vertices = fileShapes[i].mesh.positions.size() / 3;
		triangleMesh.posData = vector<Vec3>(vertices);
		for (uint v = 0; v < vertices; v++) {
			triangleMesh.posData[v] = {
				fileShapes[i].mesh.positions[3 * v + 0],
				fileShapes[i].mesh.positions[3 * v + 1],
				fileShapes[i].mesh.positions[3 * v + 2]
			};
		}

		// Texture Coords
		triangleMesh.texCoordData = vector<Vec2>(vertices);
		if (fileShapes[i].mesh.texcoords.size() > 0) {
			for (uint v = 0; v < vertices; v++) {
				triangleMesh.texCoordData[v] = {
					fileShapes[i].mesh.texcoords[2 * v + 0],
					fileShapes[i].mesh.texcoords[2 * v + 1]
				};
			}
		} else
			for (uint v = 0; v < vertices; v++)
				triangleMesh.texCoordData[v] = { 0, 0 };

		// Triangles
		int triangles = fileShapes[i].mesh.indices.size() / 3;
		triangleMesh.primitives = vector<TrianglePrimitive>(triangles);
		for (int t = 0; t < triangles; t++) {
			triangleMesh.primitives[t].id = t;
			triangleMesh.primitives[t] = {
				(uint)fileShapes[i].mesh.indices[3 * t + 0],
				(uint)fileShapes[i].mesh.indices[3 * t + 1],
				(uint)fileShapes[i].mesh.indices[3 * t + 2]
			};

			int material_id = fileShapes[i].mesh.material_ids[t];
			if (material_id < 0)
				material_id = 0;
			for (uint m = 0; m < materials.size(); m++) {
				if (materials[m]->id == material_id) {
					triangleMesh.primitives[t].material = materials[m];
					break;
				}
			}
		}

		// Normals
		triangleMesh.normalData = vector<Vec3>(vertices);
		for (uint n = 0; n < vertices; n++)
			triangleMesh.normalData[n] = Vec3(0.f, 0.f, 0.f);

		//TODO: Check if obj file has normals
		// Calculate normals by going through each triangle
		for (uint i = 0; i < triangles; i++) {
			TrianglePrimitive& currentTriangle = triangleMesh.primitives[i];
			
			// Calculate normal of the triangle
			Vec3 normal = Vec3::cross(
				triangleMesh.posData[currentTriangle.indices[1]] - triangleMesh.posData[currentTriangle.indices[0]],
				triangleMesh.posData[currentTriangle.indices[2]] - triangleMesh.posData[currentTriangle.indices[0]]
			);

			// Add triangle normal to each vertex
			triangleMesh.normalData[currentTriangle.indices[0]] = triangleMesh.normalData[currentTriangle.indices[0]] + normal;
			triangleMesh.normalData[currentTriangle.indices[1]] = triangleMesh.normalData[currentTriangle.indices[1]] + normal;
			triangleMesh.normalData[currentTriangle.indices[2]] = triangleMesh.normalData[currentTriangle.indices[2]] + normal;
		}

		// Normalize normals
		for (uint n = 0; n < vertices; n++)
			triangleMesh.normalData[n] = Vec3::normalize(triangleMesh.normalData[n]);

		// Set up vbo (Vertex buffer object) and ibo (Index buffer object)
		glGenBuffers(1, &triangleMesh.vbo);
		glGenBuffers(1, &triangleMesh.ibo);
		uint sizePositions = vertices * sizeof(Vec3);
		uint sizeNormals = vertices * sizeof(Vec3);
		uint sizeTexCoords = vertices * sizeof(Vec2);
		uint sizeIndices = triangles * sizeof(TrianglePrimitive);

		glBindBuffer(GL_ARRAY_BUFFER, triangleMesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizePositions + sizeNormals + sizeTexCoords, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizePositions, &triangleMesh.posData[0]);
		glBufferSubData(GL_ARRAY_BUFFER, sizePositions, sizeNormals, &triangleMesh.normalData[0]);
		glBufferSubData(GL_ARRAY_BUFFER, sizePositions + sizeNormals, sizeTexCoords, &triangleMesh.texCoordData[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleMesh.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &triangleMesh.primitives[0], GL_STATIC_DRAW);

		obj.geometry = pTriangleMesh;
		objList->push_back(pObj);

	}

}