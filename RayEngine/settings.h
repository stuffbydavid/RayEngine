//// Program default configuration ////

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define RENDER_MODE RM_OPTIX
#define ENABLE_REFLECTIONS 1
#define ENABLE_REFRACTIONS 1
#define ENABLE_AO 0

#define MAX_REFLECTIONS 1
#define MAX_REFRACTIONS 4
#define AO_SAMPLES_SQRT 5
#define AO_SAMPLES_SQRT_MAX 10
#define AO_SAMPLES_MAX AO_SAMPLES_SQRT_MAX * AO_SAMPLES_SQRT_MAX
#define AO_NOISE_SCALE 100
#define AO_NOISE_WIDTH 50
#define AO_NOISE_HEIGHT 50
#define AO_POWER 1.f

#define EMBREE_NUM_THREADS 16
#define EMBREE_RENDER_TILES 1     // 1 = Split into tiles, 0 = Use a single loop
#define EMBREE_TILE_WIDTH 16
#define EMBREE_TILE_HEIGHT 16
#define EMBREE_PACKET_PRIMARY 1   // 1 = Use packets for primary rays, 0 = Shoot single rays
#define EMBREE_PACKET_SECONDARY 0 // 1 = Use packets for secondary rays (eg. shadows, reflections), 0 = use single rays

#define OPTIX_PROGRESSIVE 0
#define OPTIX_NUM_THREADS 1
#define OPTIX_STACK_SIZE 4096

#define HYBRID_THREADED 1
#define HYBRID_BALANCE_MODE BM_MANUAL
#define HYBRID_DISPLAY_PARTITION 1
#define HYBRID_PARTITION 0.4
#define HYBRID_EMBREE 1
#define HYBRID_OPTIX 1

//// OpenGL compile settings ////

#define OPENGL_SHADER OpenGL.shdrPhong

//// Embree compile settings ////

#define EMBREE_HIGHLIGHT_COLOR Color(0.6f, 0.6f, 1.f)
#define EMBREE_ONE_LIGHT 1 // Limit to one light to get rid of some loops
#define EMBREE_PACKET_SIZE 8
#define EMBREE_PACKET_TYPE RTCRay8
#define EMBREE_RAY_VALID -1
#define EMBREE_RAY_INVALID 0
#define EMBREE_SFLAGS_SCENE RTC_SCENE_STATIC | RTC_SCENE_COHERENT | RTC_SCENE_HIGH_QUALITY
#define EMBREE_SFLAGS_OBJECT RTC_SCENE_STATIC | RTC_SCENE_COHERENT | RTC_SCENE_HIGH_QUALITY
#define EMBREE_AFLAGS_SCENE RTC_INTERSECT8 | RTC_INTERSECT1
#define EMBREE_AFLAGS_OBJECT RTC_INTERSECT8 | RTC_INTERSECT1

//// OptiX compile settings ////

#define OPTIX_HIGHLIGHT_COLOR Color(1.f, 0.6f, 0.6f)
#define OPTIX_ENABLE 1
#define OPTIX_USE_OPENGL_TEXTURE 0 // This is broken with the normal attribute in OpenGL shaders?
#define OPTIX_USE_GEOMETRY_VBO 0   // Can't combine VBOs
#define OPTIX_USE_OUTPUT_VBO 0     // OptiX must run on Master thread