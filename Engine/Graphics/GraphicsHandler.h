/*
	This file handles all platform specific code for Graphics files.
*/

#ifndef EAE6320_GRAPHICSHANDLER_H
#define EAE6320_GRAPHICSHANDLER_H

// Include Files
//==============

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		struct Color
		{
		public:

			// This constructor is for when you want to call predefined functions
			// below without defining the initial Color data.
			Color()
			{
				colorR = 0.0f;
				colorG = 0.0f;
				colorB = 0.0f;
				colorA = 0.0f;
			}

			// This constructor is for when you know which exact color you want
			// with corresponding RGBA values.
			Color(const float r, const float g, const float b, const float a)
			{
				colorR = r;
				colorG = g;
				colorB = b;
				colorA = a;
			}

			// Those functions below return individual components of the current color
			float R() const
			{
				return colorR;
			}

			float G() const
			{
				return colorG;
			}

			float B() const
			{
				return colorB;
			}

			float A() const
			{
				return colorA;
			}

			// Those functions below return Color structs that represent most common colors
			Color Black() const
			{
				return Color(0.0f, 0.0f, 0.0f, 1.0f);
			}

			Color White() const
			{
				return Color(1.0f, 1.0f, 1.0f, 1.0f);
			}

			Color Red() const
			{
				return Color(1.0f, 0.0f, 0.0f, 0.0f);
			}

			Color Green() const
			{
				return Color(0.0f, 1.0f, 0.0f, 1.0f);
			}

			Color Blue() const
			{
				return Color(0.0f, 0.0f, 1.0f, 1.0f);
			}

			Color Yellow() const
			{
				return Color(1.0f, 1.0f, 0.0f, 1.0f);
			}

			Color Cyan() const
			{
				return Color(0.0f, 1.0f, 1.0f, 1.0f);
			}

			Color Magenta() const
			{
				return Color(1.0f, 0.0f, 1.0f, 1.0f);
			}

		private:
			float colorR, colorG, colorB, colorA;
		};

		// Direct3D specific helper function
		eae6320::cResult InitializeViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);

		// Platform specific code in RenderFrame function for clearing view
		void ClearView(Effect &effect, Sprite &sprite, Color i_clearColor);

		// Platform specific code in RenderFrame function for swapping render buffer
		void SwapRender();

		// Direct3D specific code in Initialize function
		eae6320::cResult InitializeRenderingView(const sInitializationParameters& i_initializationParameters, eae6320::cResult result);
		
		// Direct3D specific code in CleanUp function
		void CleanUpGraphics();
	}
}

#endif
