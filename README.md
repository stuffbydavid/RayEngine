# RayEngine
RayEngine is a C++ rendering engine for CPU/GPU accelerated ray tracing utilizing [Embree](https://embree.github.io/) and [OptiX](https://developer.nvidia.com/optix). It was created for a Bachelor's Thesis in Computer Science and the report can be found here:

## Running the program
RayEngine is for Windows only (64-bit) and requires a NVIDIA GPU to be able to launch. The executable can be found in the "RayEngine" folder as "RayEngine.exe". The following must also be installed:
* **ImageMagick 6.9.4**: http://www.imagemagick.org/download/binaries/ImageMagick-6.9.4-9-Q16-x64-dll.exe

## Controls and settings
The camera is oriented by left clicking and moving the mouse around.
While the left button is pressed, the following key commands become active:
* **W/S/A/D**
Move forward/back/left/right.
* **Q/E**
Roll the camera.
* **Space/Shift**
Speed up/Slow down movement.
* **T/G**
Increase/Decrease FOV (Field of View).
The following key commands are also available:
* **F1**
Show/Hide GUI.
* **F2**
Start/Stop benchmarking.
* **F3**
Save a HD screenshot into the renders/ folder.

The up/down arrow keys are used to navigate through the settings menu, while
right/left will change the selected value. Here are short descriptions of the settings:
* **Scene**
Changes the current scene from the ones pre-loaded into memory.
* **Render mode**
Switches the render mode between OpenGL, Embree, OptiX and Hybrid.
* **Resolution**
Switches the window resolution between 800x600, 1280x720 and 1980x1080.
* **Camera path**
Sets the camera to follow a predetermined path, used for benchmarking.
* **Reflections**
Enables/Disables reflections in the scene.
* **Max reflections**
Sets the maximum number of recursive calls for reflections.
* **Refractions**
Enables/Disables refractions/transparent surfaces in the scene.
* **Max refractions**
Sets the maximum number of recursive calls for refractions/transparent surfaces.
* **Ambient Occlusion**
Enables/Disables ambient occlusion in the scene.
* **AO samples**
The amount of rays to send in a hemisphere around the intersection point. A larger value will give softer (less noisy) shades, but require more processing.
* **AO radius**
The radius of the sampling hemisphere.
* **AO power**
The power/strength of the ambient occlusion effect.
* **AO noise scale**
Determines the scale of the noise texture used when randomly sampling.
* **Embree threads**
Tells OpenMP how many threads to use when Embree is rendering, this value is used with omp_set_num_threads.
* **Embree tiles**
Enables/Disables tiles when Embree is rendering. If disabled, a single loop will be used.
* **Width/Height**
Determines the dimensions of the tiles. A value larger or equal to the packet size (8) is recommended to assure coherency.
* **Embree primary packets**
Packets (RTCRay8) will be used for all the primary rays.
* **Embree secondary packets**
Packets will be used for all the secondary rays (shadows, reflections, refractions, ambient occlusion). This setting has shown to give a slowdown.
* **OptiX progressive render**
If enabled, progressive rendering will be used by OptiX, i.e. the program will use a non-blocking launch call.
* **OptiX stack size**
Sets the size of the GPU stack. A small value will lead to distorted reflections and refractions.
* **Hybrid threaded**
The hybrid mode will use a threaded approach, where the master approach runs OptiX while the other runs Embree. After both threads have finished, the result will be fetched and rendered on the screen.
* **Hybrid balance mode**
Changes the Hybrid balancing mode, either manual or based on the render time.
* **Partition**
When the balance mode is manual, this setting will determine the amount of the screen to give to Embree and OptiX, respectively.
* **Hybrid Display Partition**
When enabled, the Embree and OptiX images will be highlighted blue and red.
* **Hybrid Enable Embree**
Enables/Disables Embree. Used for debugging purposes.
* **Hybrid Enable OptiX**
Enables/Disables OptiX. Used for debugging purposes.

## Building the program
Currently only the Glass Scene is loaded, but other scenes can be enabled or built by editing the main.cpp file and recompiling. This requires C++ library knowledge. To build from source (Visual Studio), the following are needed:
* **ImageMagick 6.9.4**: Same link as above, just make sure "Install development headers and libraries for C and C++" is checked when installing.
* **OptiX**: https://developer.nvidia.com/optix
* **Embree**: https://embree.github.io/
* **CUDA Toolkit 7.0**: https://developer.nvidia.com/cuda-toolkit-70
* **GLFW**: https://github.com/glfw/glfw/releases/download/3.1.2/glfw-3.1.2.bin.WIN64.zip
* **GLEW**: https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0-win32.zip/download
* **FreeType**: https://www.freetype.org/ (you need to build .lib files from source for Win64, email me if you have issues)
After installing these requirements, make sure the Include and Library directories are correct.
