// Include Files
//==============

#include "Effect.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Results/Results.h>

namespace eae6320
{
	Effect::Effect()
	{
		
	}

	Effect::~Effect()
	{

	}

	cResult Effect::Load(char vertexShaderFileName[], char fragmentShaderFileName[], const uint8_t i_RenderState, Effect *& o_effect)
	{
		cResult result = Results::Success;
		Effect* effect = nullptr;

		effect = new (std::nothrow) Effect();

		// Allocate a new Effect
		{
			if (!effect)
			{
				result = Results::OutOfMemory;
				EAE6320_ASSERTF(false, "Couldn't allocate memory for the effect");
				Logging::OutputError("Failed to allocate memory for the effect");
				goto OnExit;
			}
		}

		if (!(result = effect->InitializeShadingData(vertexShaderFileName, fragmentShaderFileName, i_RenderState)))
		{
			EAE6320_ASSERTF(false, "Initialization of new effect failed");
			goto OnExit;
		}

	OnExit:

		if (result)
		{
			EAE6320_ASSERT(effect);
			o_effect = effect;
		}
		else
		{
			if (effect)
			{
				effect->DecrementReferenceCount();
				effect = nullptr;
			}
			o_effect = nullptr;
		}
		return result;
	}

	cResult Effect::CleanUp()
	{
		cResult result = Results::Success;
		if (result = CleanUpShadingData())
			this->DecrementReferenceCount();
		else
		{
			EAE6320_ASSERTF(false, "Failed to clean up shading data.");
			Logging::OutputError("Failed to clean up shading data.");
		}
		return result;
	}
}