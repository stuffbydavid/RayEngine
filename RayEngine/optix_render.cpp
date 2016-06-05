#include "rayengine.h"

void RayEngine::optixRender() {

	if (!OPTIX_ENABLE || Optix.width == 0 || (renderMode == RM_HYBRID && !Hybrid.enableOptix))
		return;

	try {

		Optix.context["org"]->setFloat(rayOrg.x(), rayOrg.y(), rayOrg.z());
		Optix.context["xaxis"]->setFloat(rayXaxis.x(), rayXaxis.y(), rayXaxis.z());
		Optix.context["yaxis"]->setFloat(rayYaxis.x(), rayYaxis.y(), rayYaxis.z());
		Optix.context["zaxis"]->setFloat(rayZaxis.x(), rayZaxis.y(), rayZaxis.z());
		Optix.context["offset"]->setFloat(Optix.offset);
		Optix.context["enableReflections"]->setInt(enableReflections);
		Optix.context["maxReflections"]->setInt(maxReflections);
		Optix.context["enableRefractions"]->setInt(enableRefractions);
		Optix.context["maxRefractions"]->setInt(maxRefractions);
		Optix.context["enableAo"]->setInt(enableAo);
		Optix.context["aoSamples"]->setInt(aoSamples);
		Optix.context["aoSamplesSqrt"]->setInt(aoSamplesSqrt);
		Optix.context["aoNoiseScale"]->setFloat(aoNoiseScale);
		Optix.context["aoPower"]->setFloat(aoPower);
		Optix.context["aoRadius"]->setFloat(curScene->aoRadius);

		if (Optix.enableProgressive) {

			// Non-blocking, progressive launch

			// The hybrid balancing won't work with this since there's no way
			// to accurately measure the time taken for progressive rendering.

			Optix.context["renderBuffer"]->set(Optix.streamRenderBuffer);
			Optix.context->launchProgressive(0, window.width, window.height, 1);

		} else {

			// Blocking launch

			Optix.renderTimer.start();

			Optix.context["renderBuffer"]->set(Optix.renderBuffer);
			Optix.context->launch(0, window.width, window.height);
			
			Optix.renderTimer.stop();

		}
		
	} catch (optix::Exception e) {

		cout << "OptiX error: " << e.getErrorString() << endl;
		system("pause");
	}

}

void RayEngine::optixRenderUpdateTexture() {

	if (!OPTIX_ENABLE || (renderMode == RM_HYBRID && !Hybrid.enableOptix))
		return;

	Optix.textureTimer.start();

	if (Optix.enableProgressive) {

		int ready = false;
		uint subFrames, maxSubFrames;

		Optix.streamBuffer->getProgressiveUpdateReady(&ready, &subFrames, &maxSubFrames);

		if (ready) {

			// A frame is done, load into texture

			Optix.renderTimer.stop();

			glBindTexture(GL_TEXTURE_2D, Optix.texture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window.width, window.height, GL_RGBA, GL_UNSIGNED_BYTE, Optix.streamBuffer->map());
			Optix.streamBuffer->unmap();
			glBindTexture(GL_TEXTURE_2D, 0);

			Optix.renderTimer.start();

		}

	} else {

		glBindTexture(GL_TEXTURE_2D, Optix.texture);
#if OPTIX_USE_OUTPUT_VBO
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, Optix.vbo);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window.width, window.height, GL_RGBA, GL_FLOAT, 0);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#else
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window.width, window.height, GL_RGBA, GL_FLOAT, Optix.renderBuffer->map());
		Optix.renderBuffer->unmap();
#endif
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	OpenGL.shdrTexture->render2DBox(window.ortho, 0, 0, window.width, window.height, Optix.texture, (renderMode == RM_HYBRID && Hybrid.displayPartition) ? OPTIX_HIGHLIGHT_COLOR : Color(1.f));

	Optix.textureTimer.stop();

}