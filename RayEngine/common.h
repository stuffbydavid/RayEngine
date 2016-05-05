#pragma once
#pragma warning(disable: 4005) // Macro redefinitions from Embree+OptiX

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "embree.lib")
#pragma comment(lib, "optix.1.lib")
#pragma comment(lib, "CORE_RL_Magick++_.lib")
//#pragma comment(lib, "freetype.lib") TODO

#include <functional>
#include <string>
#include <vector>
#include <map>

#include "vec3.h"
#include "vec2.h"
#include "mat4x4.h"
#include "color.h"

#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <optix.h>
#include <optix_world.h>
#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_namespace.h>
//#include "ft2build.h"
//#include FT_FREETYPE_H
#include <Magick++.h>  // Put C:\Libs\imagemagick-6.9.3-Q16 in path
#include <omp.h>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "settings.h"

using namespace std;
using namespace placeholders;

typedef unsigned int uint;
typedef unsigned char uchar;