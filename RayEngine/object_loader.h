#pragma once

#include "common.h"
#include "object.h"
#include "triangle_mesh.h"
#include "material.h"

#include "tiny_obj_loader.h"

namespace ObjectLoader {

	void loadObjects(vector<Object*> *objList, vector<Material*> *matList, string inputFile);

}
