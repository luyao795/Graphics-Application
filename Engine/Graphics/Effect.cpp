// Include Files
//==============

#include "cRenderState.h"
#include "cShader.h"

#include "Effect.h"

namespace eae6320
{
	namespace Graphics
	{
		Effect::Effect()
		{

		}

		Effect::~Effect()
		{

		}

		cShader::Handle eae6320::Graphics::Effect::GetVertexShader()
		{
			return s_vertexShader;
		}

		cShader::Handle Effect::GetFragmentShader()
		{
			return s_fragmentShader;
		}

		cRenderState Effect::GetRenderState()
		{
			return s_renderState;
		}

		void Effect::SetVertexShader(cShader::Handle i_vertexShader)
		{
			s_vertexShader = i_vertexShader;
		}

		void Effect::SetFragmentShader(cShader::Handle i_fragmentShader)
		{
			s_fragmentShader = i_fragmentShader;
		}

		void Effect::SetRenderState(cRenderState i_renderState)
		{
			s_renderState = i_renderState;
		}

#if defined ( EAE6320_PLATFORM_GL )
		GLuint Effect::GetProgramID()
		{
			return s_programId;
		}

		void Effect::SetProgramID(GLuint i_programId)
		{
			s_programId = i_programId;
		}
#endif
	}
}

