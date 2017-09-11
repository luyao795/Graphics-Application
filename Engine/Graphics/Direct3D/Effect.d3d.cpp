/*
	Direct3D specific code for Effect
*/

// Include Files
//==============

#include "../Effect.h"

#include "Includes.h"

eae6320::cResult eae6320::Effect::InitializeShadingData(char vertexShaderFileName[], char fragmentShaderFileName[])
{
	auto result = eae6320::Results::Success;

	char vertexPath[100] = "data/Shaders/Vertex/";
	std::strcat(vertexPath, vertexShaderFileName);

	char fragmentPath[100] = "data/Shaders/Fragment/";
	std::strcat(fragmentPath, fragmentShaderFileName);

	if (!(result = eae6320::Graphics::cShader::s_manager.Load(vertexPath,
		s_vertexShader, eae6320::Graphics::ShaderTypes::Vertex)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = eae6320::Graphics::cShader::s_manager.Load(fragmentPath,
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

void eae6320::Effect::CleanUpShadingData(eae6320::cResult result)
{
	if (s_vertexShader)
	{
		const auto localResult = eae6320::Graphics::cShader::s_manager.Release(s_vertexShader);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	if (s_fragmentShader)
	{
		const auto localResult = eae6320::Graphics::cShader::s_manager.Release(s_fragmentShader);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = s_renderState.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
}

void eae6320::Effect::BindShadingData()
{
	// Bind the shading data
	{
		{
			auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;
			EAE6320_ASSERT(direct3dImmediateContext);
			ID3D11ClassInstance* const* noInterfaces = nullptr;
			constexpr unsigned int interfaceCount = 0;
			// Vertex shader
			{
				EAE6320_ASSERT(s_vertexShader);
				auto* const shader = eae6320::Graphics::cShader::s_manager.Get(s_vertexShader);
				EAE6320_ASSERT(shader && shader->m_shaderObject.vertex);
				direct3dImmediateContext->VSSetShader(shader->m_shaderObject.vertex, noInterfaces, interfaceCount);
			}
			// Fragment shader
			{
				EAE6320_ASSERT(s_fragmentShader);
				auto* const shader = eae6320::Graphics::cShader::s_manager.Get(s_fragmentShader);
				EAE6320_ASSERT(shader && shader->m_shaderObject.fragment);
				direct3dImmediateContext->PSSetShader(shader->m_shaderObject.fragment, noInterfaces, interfaceCount);
			}
		}
		s_renderState.Bind();
	}
}