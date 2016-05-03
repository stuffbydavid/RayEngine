#include "object.h"
#include "tiny_obj_loader.h"

Object::Object() {}

Object::~Object() {
	//delete geometry;
}

void Object::translate(Vec3 vector) {
	matrix = Mat4x4::translate(vector) * matrix;
}

void Object::rotate(Vec3 vector, float angle) {
	matrix = Mat4x4::rotate(vector, angle) * matrix;
}

void Object::scale(Vec3 vector) {
	matrix = Mat4x4::scale(vector) * matrix;
}

Object* Object::load(string file) {

	static Image defaultTexture({ 1.f });
	vector<tinyobj::shape_t> fileShapes;
	vector<tinyobj::material_t> fileMaterials;
	string err;
	string path = file.substr(0, file.find_last_of("/\\") + 1);

	// Load file
	cout << "Loading " << file << "..." << endl;
	tinyobj::LoadObj(fileShapes, fileMaterials, err, &file[0], &path[0]);

	if (!err.empty()) {
		cerr << err << endl;
		return nullptr;
	}
	if (fileShapes.empty()) {
		cerr << "No shapes found" << endl;
		return nullptr;
	}

	// Convert to our material type
	vector<Material*> materials(fileMaterials.size());
	if (fileMaterials.size() > 0) {
		for (int i = 0; i < materials.size(); i++) {

			Material* mat = new Material();

			// Colors
			mat->ambient = Color(fileMaterials[i].ambient);
			mat->specular = Color(fileMaterials[i].specular);
			mat->diffuse = Color(fileMaterials[i].diffuse);

			// Specular
			if (mat->specular != Color(0.f))
				mat->shininess = fileMaterials[i].shininess;
			else
				mat->shininess = 0.f;

			// Diffuse image
			if (fileMaterials[i].diffuse_texname != "") {

				string imageFilename, alphaFilename;
				imageFilename = path + fileMaterials[i].diffuse_texname;

				// Alpha image
				if (fileMaterials[i].alpha_texname != "")
					alphaFilename = path + fileMaterials[i].alpha_texname;
				else
					alphaFilename = "";

				// Detect filter from extension (.png=nearest, other=linear)
				GLuint filter = (imageFilename.substr(imageFilename.rfind('.') + 1) == "png") ? GL_NEAREST : GL_LINEAR;
				mat->image = new Image(filter, imageFilename, alphaFilename);

			} else
				mat->image = &defaultTexture;

			materials[i] = mat;

		}
	} else {

		Material* mat = new Material();
		mat->image = &defaultTexture;
		materials.push_back(mat);

	}

	// Create a root
	Object* obj = new Object();

	cout << "Shapes found: " << fileShapes.size() << endl;
	for (uint i = 0; i < fileShapes.size(); i++) {

		TriangleMesh* triangleMesh = new TriangleMesh();
		obj->geometries.push_back(triangleMesh);
		
		cout << "  " << fileShapes[i].name << endl;

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

		// Texture coords
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
		triangleMesh->indexData = vector<TrianglePrimitive>(triangles);
		for (int t = 0; t < triangles; t++) {
			triangleMesh->indexData[t] = {
				(uint)fileShapes[i].mesh.indices[3 * t + 0],
				(uint)fileShapes[i].mesh.indices[3 * t + 1],
				(uint)fileShapes[i].mesh.indices[3 * t + 2]
			};
		}

		// Fetch geometry material from first triangle
		int material_id = fileShapes[i].mesh.material_ids[0];
		if (material_id < 0)
			material_id = 0;
		triangleMesh->material = materials[material_id];

		cout << "    Vertices: " << vertices << endl;
		cout << "    Triangles: " << triangles << endl;

		// Normals
		triangleMesh->normalData = vector<Vec3>(vertices);
		for (uint n = 0; n < vertices; n++)
			triangleMesh->normalData[n] = Vec3(0.f, 0.f, 0.f);

		//TODO: Check if obj file has normals
		// Calculate normals by going through each triangle
		for (uint i = 0; i < triangles; i++) {
			TrianglePrimitive& currentTriangle = triangleMesh->indexData[i];

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

		// Set up VBOs (Vertex buffer objects) and IBO (Index buffer object)
		// OptiX doesn't accept a single buffer for position+normal+texture, so we need three VBOs.
		glGenBuffers(1, &triangleMesh->vboPos);
		glGenBuffers(1, &triangleMesh->vboNormal);
		glGenBuffers(1, &triangleMesh->vboTexCoord);
		glGenBuffers(1, &triangleMesh->ibo);

		glBindBuffer(GL_ARRAY_BUFFER, triangleMesh->vboPos);
		glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vec3), &triangleMesh->posData[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, triangleMesh->vboNormal);
		glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vec3), &triangleMesh->normalData[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, triangleMesh->vboTexCoord);
		glBufferData(GL_ARRAY_BUFFER, vertices * sizeof(Vec2), &triangleMesh->texCoordData[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleMesh->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles * sizeof(TrianglePrimitive), &triangleMesh->indexData[0], GL_STATIC_DRAW);

	}

	cout << endl;

	return obj;

}