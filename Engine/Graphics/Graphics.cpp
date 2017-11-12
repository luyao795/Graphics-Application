// Include Files
//==============

#include "Direct3D/Includes.h"
#include "OpenGL/Includes.h"

#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cSamplerState.h"
#include "sContext.h"

#include "Colors.h"
#include "Effect.h"
#include "Sprite.h"
#include "cTexture.h"
#include "Mesh.h"
#include "GraphicsHandler.h"
#include "Graphics.h"

#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserOutput/UserOutput.h>

namespace
{
	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perFrame(eae6320::Graphics::ConstantBufferTypes::PerFrame);
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perDrawCall(eae6320::Graphics::ConstantBufferTypes::PerDrawCall);
	// In our class we will only have a single sampler state
	eae6320::Graphics::cSamplerState s_samplerState;

	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sPerFrame constantData_perFrame;
		eae6320::Graphics::ConstantBufferFormats::sPerDrawCall constantData_perDrawCall;
		eae6320::Graphics::Color cachedColorForRenderingInNextFrame;
		std::vector<eae6320::Graphics::DataSetForRenderingSprite> cachedEffectSpritePairForRenderingInNextFrame;
		std::vector<eae6320::Graphics::DataSetForRenderingMesh> cachedEffectMeshPairForRenderingInNextFrame;
		eae6320::Graphics::Camera cameraForView;
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

void eae6320::Graphics::SubmitCameraForView(eae6320::Graphics::Camera i_camera, const float i_secondCountToExtrapolate)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	i_camera.rigidBody.IncrementPredictionOntoRotation(i_secondCountToExtrapolate);
	i_camera.rigidBody.IncrementPredictionOntoMovement(i_secondCountToExtrapolate);
	constantData_perFrame.g_transform_cameraToProjected = eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(i_camera.fieldOfView, i_camera.aspectRatio, i_camera.nearPlaneDistance, i_camera.farPlaneDistance);
	constantData_perFrame.g_transform_worldToCamera = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(i_camera.rigidBody.orientation, i_camera.rigidBody.position);
	s_dataBeingSubmittedByApplicationThread->cameraForView = i_camera;
	i_camera.rigidBody.DecrementPredictionOntoMovement(i_secondCountToExtrapolate);
	i_camera.rigidBody.DecrementPredictionOntoRotation(i_secondCountToExtrapolate);
}

void eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(DataSetForRenderingSprite renderData)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame.push_back(renderData);
	renderData.effect->IncrementReferenceCount();
	renderData.sprite->IncrementReferenceCount();
	renderData.texture->IncrementReferenceCount();
}

void eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRendered(DataSetForRenderingMesh renderData)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	s_dataBeingSubmittedByApplicationThread->cachedEffectMeshPairForRenderingInNextFrame.push_back(renderData);
	renderData.effect->IncrementReferenceCount();
	renderData.mesh->IncrementReferenceCount();
}

void eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRenderedUsingPredictionIfNeeded(eae6320::Graphics::DataSetForRenderingMesh & i_meshToBeRendered, const float i_elapsedSecondCount_sinceLastSimulationUpdate, const bool i_doesTheMovementOfTheMeshNeedsToBePredicted)
{
	if (i_doesTheMovementOfTheMeshNeedsToBePredicted)
		i_meshToBeRendered.rigidBody.IncrementPredictionOntoMovement(i_elapsedSecondCount_sinceLastSimulationUpdate);

	eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRendered(i_meshToBeRendered);

	if (i_doesTheMovementOfTheMeshNeedsToBePredicted)
		i_meshToBeRendered.rigidBody.DecrementPredictionOntoMovement(i_elapsedSecondCount_sinceLastSimulationUpdate);
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

	// Update depth buffer for next frame
	{
		constexpr float defaultDepth = 1.0f;
		ClearDepth(defaultDepth);
	}

	// Update the per-frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_perFrame = s_dataBeingRenderedByRenderThread->constantData_perFrame;
		s_constantBuffer_perFrame.Update(&constantData_perFrame);
	}

	// Copy the data from the system memory that the application owns to GPU memory and the buffer will be updated later
	auto& constantData_perDrawCall = s_dataBeingRenderedByRenderThread->constantData_perDrawCall;

	// Bind shading data and draw mesh
	{
		for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame.size(); i++)
		{
			// Update the per-draw call constant buffer
			constantData_perDrawCall.g_transform_localToWorld = eae6320::Math::cMatrix_transformation(s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].rigidBody.orientation, s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].rigidBody.position);
			s_constantBuffer_perDrawCall.Update(&constantData_perDrawCall);

			s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].effect->BindShadingData();
			s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].mesh->DrawMesh();
		}
	}

	// Bind shading data, bind texture and draw geometry
	{
		for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.size(); i++)
		{
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].effect->BindShadingData();
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].texture->Bind(0);
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].sprite->DrawGeometry();
		}
	}

	// Once everything has been drawn the data that was submitted for this frame
	// should be cleaned up and cleared.
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		{
			for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.size(); i++)
			{
				s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].effect->DecrementReferenceCount();
				s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].sprite->DecrementReferenceCount();
				s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].texture->DecrementReferenceCount();
			}
		}
		s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.clear();
	}

	// Once everything has been drawn the data that was submitted for this frame
	// should be cleaned up and cleared.
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		{
			for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame.size(); i++)
			{
				s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].effect->DecrementReferenceCount();
				s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].mesh->DecrementReferenceCount();
			}
		}
		s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame.clear();
	}

	SwapRender();
}

float eae6320::Graphics::ConvertDegreeToRadian(float i_degree)
{
	constexpr float PI = 3.14159265358f;
	return (i_degree * PI) / 180.0f;
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
		if (result = s_constantBuffer_perDrawCall.Initialize())
		{
			// There is only a single per-draw call constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perDrawCall.Bind(
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
		if (!(result = InitializeRenderingView(i_initializationParameters)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	if (s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.size() > 0)
	{
		for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.size(); i++)
		{
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].effect->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].sprite->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].texture->DecrementReferenceCount();

			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].effect = nullptr;
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].sprite = nullptr;
			s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame[i].texture = nullptr;
		}
	}
	s_dataBeingRenderedByRenderThread->cachedEffectSpritePairForRenderingInNextFrame.clear();

	if (s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame.size() > 0)
	{
		for (size_t i = 0; i < s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame.size(); i++)
		{
			s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].effect->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].mesh->DecrementReferenceCount();

			s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].effect = nullptr;
			s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame[i].mesh = nullptr;
		}
	}
	s_dataBeingRenderedByRenderThread->cachedEffectMeshPairForRenderingInNextFrame.clear();

	if (s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame.size() > 0)
	{
		for (size_t i = 0; i < s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame.size(); i++)
		{
			s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame[i].effect->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame[i].sprite->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame[i].texture->DecrementReferenceCount();

			s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame[i].effect = nullptr;
			s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame[i].sprite = nullptr;
			s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame[i].texture = nullptr;
		}
	}
	s_dataBeingSubmittedByApplicationThread->cachedEffectSpritePairForRenderingInNextFrame.clear();

	if (s_dataBeingSubmittedByApplicationThread->cachedEffectMeshPairForRenderingInNextFrame.size() > 0)
	{
		for (size_t i = 0; i < s_dataBeingSubmittedByApplicationThread->cachedEffectMeshPairForRenderingInNextFrame.size(); i++)
		{
			s_dataBeingSubmittedByApplicationThread->cachedEffectMeshPairForRenderingInNextFrame[i].effect->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->cachedEffectMeshPairForRenderingInNextFrame[i].mesh->DecrementReferenceCount();

			s_dataBeingSubmittedByApplicationThread->cachedEffectMeshPairForRenderingInNextFrame[i].effect = nullptr;
			s_dataBeingSubmittedByApplicationThread->cachedEffectMeshPairForRenderingInNextFrame[i].mesh = nullptr;
		}
	}
	s_dataBeingSubmittedByApplicationThread->cachedEffectMeshPairForRenderingInNextFrame.clear();

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
		const auto localResult = s_constantBuffer_perDrawCall.CleanUp();
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

	return result;
}
