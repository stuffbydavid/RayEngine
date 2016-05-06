// Program settings

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 400

#define RENDER_MODE RM_OPENGL
#define RAY_TRACING_TARGET RTT_CPU
#define MAX_REFLECTIONS 1

#define OPENGL_SHADER shdrPhong

#define OPTIX_ENABLE 1
#define OPTIX_PRINT_TIME 0
#define OPTIX_USE_OPENGL_TEXTURE 1 // This is broken with the normal attribute in OpenGL shaders?
#define OPTIX_USE_GEOMETRY_VBO 1 // Can't combine VBOs
#define OPTIX_USE_OUTPUT_VBO 1 // OptiX must run on Master thread

#define EMBREE_PRINT_TIME 0
#define EMBREE_RENDER_OLD 0 // Old method (single loop, single ray function)
#define EMBREE_RENDER_TILES 1 // Tiles and packet function
#define EMBREE_RENDER_LISTS 0 // Slower and heavily uses stacks which leads to weird errors with recursion
#define EMBREE_SFLAGS_SCENE RTC_SCENE_DYNAMIC | RTC_SCENE_COHERENT | RTC_SCENE_HIGH_QUALITY
#define EMBREE_SFLAGS_OBJECT RTC_SCENE_STATIC | RTC_SCENE_COHERENT | RTC_SCENE_HIGH_QUALITY
#define EMBREE_AFLAGS_SCENE RTC_INTERSECT8 | RTC_INTERSECT1
#define EMBREE_AFLAGS_OBJECT RTC_INTERSECT8 | RTC_INTERSECT1

#define RAY_VALID -1
#define RAY_INVALID 0

#define HYBRID_THREADED 1
#define HYBRID_PARTITION 0.4