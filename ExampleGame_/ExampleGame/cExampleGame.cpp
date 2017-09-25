// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Effect.h>
#include <Engine/Graphics/Sprite.h>
#include <Engine/Graphics/Graphics.h>

// Inherited Implementation
//=========================

// Run
//----

namespace
{
	// Shading Data
	//-------------
	// This effect contains color changing property.
	eae6320::Graphics::Effect* s_effect = nullptr;
	// This effect contains white static property.
	eae6320::Graphics::Effect* s_effect_static = nullptr;

	// Geometry Data
	//--------------

	// These two sprites form the color changing plus sign.
	eae6320::Graphics::Sprite* s_sprite = nullptr;
	eae6320::Graphics::Sprite* s_sprite2 = nullptr;
	// These four sprites form the static white rectangles.
	eae6320::Graphics::Sprite* s_sprite_static = nullptr;
	eae6320::Graphics::Sprite* s_sprite_static2 = nullptr;
	eae6320::Graphics::Sprite* s_sprite_static3 = nullptr;
	eae6320::Graphics::Sprite* s_sprite_static4 = nullptr;
}

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
	cResult result = Results::Success;
	const uint8_t defaultRenderState = 0;

	// Initialize the shading data
	{
		if (!(result = eae6320::Graphics::Effect::Load("Sprite.shd", "Sprite.shd", defaultRenderState, s_effect)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::Effect::Load("Sprite.shd", "Static.shd", defaultRenderState, s_effect_static)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	// Initialize the geometry
	{
		if (!(result = eae6320::Graphics::Sprite::Load(0.75f, 0.25f, 1.5f, 0.5f, s_sprite)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::Sprite::Load(0.25f, 0.75f, 0.5f, 1.5f, s_sprite2)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::Sprite::Load(1.0f, 1.0f, 0.5f, 0.5f, s_sprite_static)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::Sprite::Load(-0.5f, 1.0f, 0.5f, 0.5f, s_sprite_static2)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::Sprite::Load(1.0f, -0.5f, 0.5f, 0.5f, s_sprite_static3)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!(result = eae6320::Graphics::Sprite::Load(-0.5f, -0.5f, 0.5f, 0.5f, s_sprite_static4)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	cResult result = Results::Success;
	{
		result = s_sprite->CleanUp();
		if (result)
			s_sprite = nullptr;

		result = s_sprite2->CleanUp();
		if (result)
			s_sprite2 = nullptr;

		result = s_effect->CleanUp();
		if (result)
			s_effect = nullptr;

		result = s_sprite_static->CleanUp();
		if (result)
			s_sprite_static = nullptr;

		result = s_sprite_static2->CleanUp();
		if (result)
			s_sprite_static2 = nullptr;

		result = s_sprite_static3->CleanUp();
		if (result)
			s_sprite_static3 = nullptr;

		result = s_sprite_static4->CleanUp();
		if (result)
			s_sprite_static4 = nullptr;

		result = s_effect_static->CleanUp();
		if (result)
			s_effect_static = nullptr;
	}
	return result;
}

void eae6320::cExampleGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	// Submit color data
	eae6320::Graphics::SubmitColorToBeRendered(COLOR_DEFAULT.Magenta());

	// Submit Effect Sprite pair data
	eae6320::Graphics::SubmitEffectSpritePairToBeRendered(s_effect, s_sprite);
	eae6320::Graphics::SubmitEffectSpritePairToBeRendered(s_effect, s_sprite2);
	eae6320::Graphics::SubmitEffectSpritePairToBeRendered(s_effect_static, s_sprite_static);
	eae6320::Graphics::SubmitEffectSpritePairToBeRendered(s_effect_static, s_sprite_static2);
	eae6320::Graphics::SubmitEffectSpritePairToBeRendered(s_effect_static, s_sprite_static3);
	eae6320::Graphics::SubmitEffectSpritePairToBeRendered(s_effect_static, s_sprite_static4);
}
