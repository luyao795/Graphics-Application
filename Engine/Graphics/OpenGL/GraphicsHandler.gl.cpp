// Include Files
//==============

#include "Includes.h"

#include "../sContext.h"
#include "../GraphicsHandler.h"
#include "../Color.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

void eae6320::Graphics::ClearView(Color i_clearColor)
{
	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		// Clear back buffer to input color
		{
			glClearColor(i_clearColor.R(), i_clearColor.G(), i_clearColor.B(), i_clearColor.A());
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		{
			constexpr GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
			glClear(clearColor);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
	}
}

void eae6320::Graphics::ClearDepth(float i_depth)
{
	{
		glDepthMask(GL_TRUE);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		glClearDepth(i_depth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	{
		constexpr GLbitfield clearDepth = GL_DEPTH_BUFFER_BIT;
		glClear(clearDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}

void eae6320::Graphics::SwapRender()
{
	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (or "swapped" with the "front buffer")
	{
		const auto deviceContext = sContext::g_context.deviceContext;
		EAE6320_ASSERT(deviceContext != NULL);

		const auto glResult = SwapBuffers(deviceContext);
		EAE6320_ASSERT(glResult != FALSE);
	}
}

eae6320::cResult eae6320::Graphics::InitializeRenderingView(const sInitializationParameters& i_initializationParameters)
{
	// This function does nothing under OpenGL configuration.

	return Results::Success;
}

void eae6320::Graphics::CleanUpGraphics()
{
	// This function does nothing under OpenGL configuration.
}
