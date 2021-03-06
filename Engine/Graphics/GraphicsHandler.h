/*
	This file handles all platform specific code for Graphics files.
*/

#ifndef EAE6320_GRAPHICSHANDLER_H
#define EAE6320_GRAPHICSHANDLER_H

// Include Files
//==============

#include "Configuration.h"

#include <cstdint>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		struct sInitializationParameters;

		// Direct3D specific helper function
		eae6320::cResult InitializeViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);

		// Platform specific code in RenderFrame function for clearing view
		void ClearView(Color i_clearColor);

		// Platform specific code in RenderFrame function for clearing depth buffer
		void ClearDepth(float i_depth);

		// Platform specific code in RenderFrame function for swapping render buffer
		void SwapRender();

		// Direct3D specific code in Initialize function
		eae6320::cResult InitializeRenderingView(const sInitializationParameters& i_initializationParameters);
		
		// Direct3D specific code in CleanUp function
		void CleanUpGraphics();
	}
}

#endif
