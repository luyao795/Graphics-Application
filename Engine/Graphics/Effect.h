/*
	These are code snippets that represent effects that are separated from Graphics source file
*/

#ifndef EAE6320_EFFECT_H
#define EAE6320_EFFECT_H

// Include Files
//==============

#include "cRenderState.h"
#include "cShader.h"
#include "sContext.h"

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
#elif defined ( EAE6320_PLATFORM_D3D )
		ID3D11DeviceContext* direct3dImmediateContext; // = Graphics::sContext::g_context.direct3dImmediateContext;
#endif

		cResult InitializeShadingData();
		void CleanUpShadingData(eae6320::cResult result);
		void BindShadingData();
	};
}

#endif // !EAE6320_EFFECT_H

