/*
	These are code snippets that represent effects that are separated from Graphics source file
*/

#ifndef EAE6320_EFFECT_H
#define EAE6320_EFFECT_H

// Include Files
//==============

#include "cShader.h"
#include "cRenderState.h"

#include <Engine/Assets/ReferenceCountedAssets.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif

#define MAX_EFFECT_PATH_LENGTH 100

namespace eae6320
{
	namespace Graphics
	{
		class Effect
		{
		public:

			// Initialization / Clean Up
			//--------------------------

			static cResult Load(const char * i_vertexShaderFileName, const char * i_fragmentShaderFileName, const uint8_t i_RenderState, Effect *& o_effect);
			cResult CleanUp();

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Effect)

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
			
			// Render
			//-------

			void BindShadingData();

		private:

			Effect();
			~Effect();

			// Initialization / Clean Up
			//--------------------------

			// vertexShaderFileName is the file name of the vertex shader, fragmentShaderFileName is
			// the file name of the fragment shader. Both file names should include extension.
			// Do NOT include file path in these parameters.
			// i_RenderState is the render state the user wants to use, the default value is 0.
			cResult InitializeShadingData(const char * vertexShaderFileName, const char * fragmentShaderFileName, const uint8_t i_RenderState);
			cResult CleanUpShadingData();

			// Shading Data
			//-------------

			Graphics::cShader::Handle s_vertexShader;
			Graphics::cShader::Handle s_fragmentShader;

#if defined ( EAE6320_PLATFORM_GL )
			GLuint s_programId = 0;
#endif

			Graphics::cRenderState s_renderState;

			// Reference counting
			//===================

			EAE6320_ASSETS_DECLAREREFERENCECOUNT()

		};
	}
}

#endif // !EAE6320_EFFECT_H

