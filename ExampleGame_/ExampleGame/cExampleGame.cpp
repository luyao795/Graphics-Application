// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Results/Results.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserOutput/UserOutput.h>

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cExampleGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	return Results::Success;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	return Results::Success;
}

void eae6320::cExampleGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	eae6320::Graphics::SubmitElapsedTime(i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
}
