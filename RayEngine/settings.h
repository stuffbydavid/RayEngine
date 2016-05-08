// Program settings

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define RENDER_MODE RM_RAY_TRACING
#define RAY_TRACING_TARGET RTT_CPU
#define MAX_REFLECTIONS 1

#define OPENGL_SHADER shdrPhong

#define OPTIX_ENABLE 1
#define OPTIX_USE_OPENGL_TEXTURE 1 // This is broken with the normal attribute in OpenGL shaders?
#define OPTIX_USE_GEOMETRY_VBO 1 // Can't combine VBOs
#define OPTIX_USE_OUTPUT_VBO 1 // OptiX must run on Master thread

#define EMBREE_RENDER_TILES 0 // 1 = Split into tiles, 0 = Use a single loop
#define EMBREE_PACKET_PRIMARY 1 // 1 = Use packets for primary rays, 0 = Shoot single rays
#define EMBREE_PACKET_SECONDARY 0 // 1 = Use packets for secondary rays (eg. shadows, reflections), 0 = use single rays
#define EMBREE_PACKET_SIZE 8
#define EMBREE_PACKET_TYPE RTCRay8
#define EMBREE_RAY_VALID -1
#define EMBREE_RAY_INVALID 0
#define EMBREE_SFLAGS_SCENE RTC_SCENE_STATIC | RTC_SCENE_INCOHERENT | RTC_SCENE_HIGH_QUALITY
#define EMBREE_SFLAGS_OBJECT RTC_SCENE_STATIC | RTC_SCENE_INCOHERENT | RTC_SCENE_HIGH_QUALITY
#define EMBREE_AFLAGS_SCENE RTC_INTERSECT8 | RTC_INTERSECT1
#define EMBREE_AFLAGS_OBJECT RTC_INTERSECT8 | RTC_INTERSECT1

#define HYBRID_THREADED 1
#define HYBRID_PARTITION 0.4