/*
	Direct3D specific code for Effect
*/

// Include Files
//==============

#include "../Effect.h"

eae6320::cResult eae6320::Effect::InitializeShadingData()
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::s_manager.Load("data/Shaders/Vertex/example.shd",
		s_vertexShader, eae6320::Graphics::ShaderTypes::Vertex)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = eae6320::Graphics::cShader::s_manager.Load("data/Shaders/Fragment/example.shd",
		s_fragmentShader, eae6320::Graphics::ShaderTypes::Fragment)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	{
		constexpr uint8_t defaultRenderState = 0;
		if (!(result = s_renderState.Initialize(defaultRenderState)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

OnExit:

	return result;
}