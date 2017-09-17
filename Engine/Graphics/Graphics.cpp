// Include Files
//==============

#include "Direct3D/Includes.h"
#include "OpenGL/Includes.h"

#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cSamplerState.h"
#include "sContext.h"

#include "Effect.h"
#include "Sprite.h"
#include "GraphicsHandler.h"
#include "Graphics.h"

#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserOutput/UserOutput.h>

namespace
{
	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perFrame(eae6320::Graphics::ConstantBufferTypes::PerFrame);
	// In our class we will only have a single sampler state
	eae6320::Graphics::cSamplerState s_samplerState;

	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sPerFrame constantData_perFrame;
		eae6320::Graphics::Color cachedColorForRenderingInNextFrame;
		std::vector<std::pair<eae6320::Effect*, eae6320::Sprite*>> cachedEffectSpritePairForRenderingInNextFrame;
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be getting populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated, 
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;

	//// Shading Data
	////-------------

	//// This effect contains color changing property.
	//eae6320::Effect* s_effect = nullptr;
	//// This effect contains white static property.
	//eae6320::Effect* s_effect_static = nullptr;

	//// Geometry Data
	////--------------

	//// These two sprites form the color changing plus sign.
	//eae6320::Sprite* s_sprite = nullptr;
	//eae6320::Sprite* s_sprite2 = nullptr;
	//// These four sprites form the static white rectangles.
	//eae6320::Sprite* s_sprite_static = nullptr;
	//eae6320::Sprite* s_sprite_static2 = nullptr;
	//eae6320::Sprite* s_sprite_static3 = nullptr;
	//eae6320::Sprite* s_sprite_static4 = nullptr;
}

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	constantData_perFrame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_perFrame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

void eae6320::Graphics::SubmitColorToBeRendered(const eae6320::Graphics::Color colorForNextFrame)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	s_dataBeingSubmittedByApplicationThread->cachedColorForRenderingInNextFrame = colorForNextFrame;
}

void eae6320::Graphics::SubmitEffectSpritePairToBeRendered(eae6320::Effect* effect, eae6320::Sprite* sprite)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	std::pair<eae6320::Effect*, eae6320::Sprite*> pair = std::make_pair(effect, sprite);
	s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame.push_back(pair);
	effect->IncrementReferenceCount();
	sprite->IncrementReferenceCount();
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		const auto result = Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread);
		if (result)
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			const auto result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal();
			if (!result)
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);
	// Update color for next frame
	{
		const Color cachedColor = s_dataBeingRenderedByRenderThread->cachedColorForRenderingInNextFrame;
		ClearView(cachedColor);
	}

	// Update the per-frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_perFrame = s_dataBeingRenderedByRenderThread->constantData_perFrame;
		s_constantBuffer_perFrame.Update(&constantData_perFrame);
	}

	// Bind shading data and draw geometry
	{
		for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.size(); i++)
		{
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].first->BindShadingData();
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].second->DrawGeometry();
		}
	}

	SwapRender();

	// Once everything has been drawn the data that was submitted for this frame
	// should be cleaned up and cleared.
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		{
			for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.size(); i++)
			{
				s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].first->DecrementReferenceCount();
				s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].second->DecrementReferenceCount();
			}
		}
		s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.clear();
	}
}

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;
	const uint8_t defaultRenderState = 0;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	// Initialize the asset managers
	{
		if (!(result = cShader::s_manager.Initialize()))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_perFrame.Initialize())
		{
			// There is only a single per-frame constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perFrame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				ShaderTypes::Vertex | ShaderTypes::Fragment);
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		if (result = s_samplerState.Initialize())
		{
			// There is only a single sampler state that is re-used
			// and so it can be bound at initialization time and never unbound
			s_samplerState.Bind();
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	// Initialize the views
	{
		result = InitializeRenderingView(i_initializationParameters);
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	CleanUpGraphics();

	{
		const auto localResult = s_constantBuffer_perFrame.CleanUp();
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
		const auto localResult = s_samplerState.CleanUp();
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
		const auto localResult = cShader::s_manager.CleanUp();
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
		const auto localResult = sContext::g_context.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.clear();
	s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame.clear();

	return result;
}
