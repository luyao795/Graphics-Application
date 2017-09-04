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
	namespace Graphics
	{
		class Effect
		{
		public:
			Effect();
			~Effect();

			cShader::Handle GetVertexShader();
			cShader::Handle GetFragmentShader();

			cRenderState GetRenderState();

			void SetVertexShader(cShader::Handle i_vertexShader);
			void SetFragmentShader(cShader::Handle i_fragmentShader);

			void SetRenderState(cRenderState i_renderState);

#if defined ( EAE6320_PLATFORM_GL )
			GLuint GetProgramID();
			void SetProgramID(GLuint i_programId);
#endif


		private:
			// Shading Data
			//-------------

			cShader::Handle s_vertexShader;
			cShader::Handle s_fragmentShader;

			cRenderState s_renderState;

#if defined ( EAE6320_PLATFORM_GL )
			GLuint s_programId = 0;
#endif
		};
	}
}

#endif // !EAE6320_EFFECT_H

