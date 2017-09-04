/*
	These are code snippets that represent effects that are separated from Graphics source file
*/

#ifndef EAE6320_EFFECT_H
#define EAE6320_EFFECT_H

// Include Files
//==============

#include "cRenderState.h"
#include "cShader.h"

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#endif

namespace eae6320
{
	class Effect
	{
	public:

		Effect();
		~Effect();

		// Shading Data
		//-------------

		Graphics::cShader::Handle s_vertexShader;
		Graphics::cShader::Handle s_fragmentShader;

		Graphics::cRenderState s_renderState;

#if defined ( EAE6320_PLATFORM_GL )
		GLuint s_programId = 0;
#endif

		cResult InitializeShadingData();
	};
}

#endif // !EAE6320_EFFECT_H

