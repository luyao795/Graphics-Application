/*
	This file declares the external interface for the graphics system
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Include Files
//==============

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>
#include <Engine/Math/sVector.h>
#include <Engine/Physics/sRigidBodyState.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class Color;
		class Effect;
		class Sprite;
		class cTexture;
		class Mesh;

		// Struct for render data that contain sprites
		struct DataSetForRenderingSprite
		{
			DataSetForRenderingSprite()
			{

			}

			DataSetForRenderingSprite(eae6320::Graphics::Effect* i_effect, eae6320::Graphics::Sprite* i_sprite, eae6320::Graphics::cTexture* i_texture)
			{
				effect = i_effect;
				sprite = i_sprite;
				texture = i_texture;
			}

			eae6320::Graphics::Effect* effect;
			eae6320::Graphics::Sprite* sprite;
			eae6320::Graphics::cTexture* texture;
		};

		// Struct for render data that contain meshes
		struct DataSetForRenderingMesh
		{
			DataSetForRenderingMesh()
			{

			}

			DataSetForRenderingMesh(eae6320::Graphics::Effect* i_effect, eae6320::Graphics::Mesh* i_mesh, eae6320::Graphics::cTexture* i_texture, eae6320::Physics::sRigidBodyState i_rigidBody)
			{
				effect = i_effect;
				mesh = i_mesh;
				texture = i_texture;
				rigidBody = i_rigidBody;
			}

			eae6320::Graphics::Effect* effect;
			eae6320::Graphics::Mesh* mesh;
			eae6320::Graphics::cTexture* texture;
			eae6320::Physics::sRigidBodyState rigidBody;
		};

		// Struct for camera for observation
		struct Camera
		{
			eae6320::Physics::sRigidBodyState rigidBody;
			float aspectRatio;
			float fieldOfView;
			float nearPlaneDistance;
			float farPlaneDistance;
		};

		// Submission
		//-----------

		// These functions should be called from the application (on the application loop thread)

		// As the class progresses you will add your own functions for submitting data,
		// but the following is an example (that gets called automatically)
		// of how the application submits the total elapsed times
		// for the frame currently being submitted
		void SubmitElapsedTime( const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime );

		void SubmitColorToBeRendered(const Color colorForNextFrame);

		void SubmitCameraForView(Camera i_camera, const float i_secondCountToExtrapolate);

		void SubmitEffectSpritePairToBeRenderedWithTexture(DataSetForRenderingSprite renderData);

		void SubmitEffectMeshPairWithPositionToBeRendered(DataSetForRenderingMesh renderData);

		void SubmitEffectMeshPairWithPositionToBeRenderedUsingPredictionIfNeeded(DataSetForRenderingMesh & i_meshToBeRendered, const float i_elapsedSecondCount_sinceLastSimulationUpdate, const bool i_doesTheMovementOfTheMeshNeedsToBePredicted);

		// When the application is ready to submit data for a new frame
		// it should call this before submitting anything
		// (or, said another way, it is not safe to submit data for a new frame
		// until this function returns successfully)
		cResult WaitUntilDataForANewFrameCanBeSubmitted( const unsigned int i_timeToWait_inMilliseconds );
		// When the application has finished submitting data for a frame
		// it must call this function
		cResult SignalThatAllDataForAFrameHasBeenSubmitted();

		// Render
		//-------

		// This is called (automatically) from the main/render thread.
		// It will render a submitted frame as soon as it is ready
		// (i.e. as soon as SignalThatAllDataForAFrameHasBeenSubmitted() has been called)
		void RenderFrame();

		// Initialization / Clean Up
		//--------------------------

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow = NULL;
	#if defined( EAE6320_PLATFORM_D3D )
			uint16_t resolutionWidth, resolutionHeight;
	#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication = NULL;
	#endif
#endif
		};

		cResult Initialize( const sInitializationParameters& i_initializationParameters );

		cResult CleanUp();

		cResult WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds);

		cResult SignalThatAllDataForAFrameHasBeenSubmitted();

		// Helper functions
		//-----------------
		float ConvertDegreeToRadian(const float i_degree);
	}
}

#endif	// EAE6320_GRAPHICS_H
