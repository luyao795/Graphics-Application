// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/cTexture.h>
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

	// Texture Data
	eae6320::Graphics::cTexture::Handle pikachuTexture;
	eae6320::Graphics::cTexture::Handle pokeballTexture;
	eae6320::Graphics::cTexture::Handle electroballTexture;

	// Combined Rendering Data
	eae6320::Graphics::DataSetForRenderingSprite s_render = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render2 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static2 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static3 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static4 = eae6320::Graphics::DataSetForRenderingSprite();

	// External counter used for Rendering based on Time
	float previousTimeElapsedCounter = 0.0f;
	float currentTimeElapsedCounter = 0.0f;
	bool flagForSwappingTexturesBasedOnTime = false;
}

void eae6320::cExampleGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Escape))
	{
		// Exit the application
		const auto result = Exit(EXIT_SUCCESS);
		EAE6320_ASSERT(result);
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	{
		s_render_static.texture = eae6320::Graphics::cTexture::s_manager.Get(electroballTexture);
		s_render_static2.texture = eae6320::Graphics::cTexture::s_manager.Get(pokeballTexture);
	}
	else
	{
		s_render_static.texture = eae6320::Graphics::cTexture::s_manager.Get(pokeballTexture);
		s_render_static2.texture = eae6320::Graphics::cTexture::s_manager.Get(electroballTexture);
	}
}

void eae6320::cExampleGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	currentTimeElapsedCounter += i_elapsedSecondCount_sinceLastUpdate;
	if (currentTimeElapsedCounter - previousTimeElapsedCounter > 1.0f)
	{
		flagForSwappingTexturesBasedOnTime = !flagForSwappingTexturesBasedOnTime;
		previousTimeElapsedCounter = currentTimeElapsedCounter;
	}

	if (flagForSwappingTexturesBasedOnTime)
	{
		s_render_static3.texture = eae6320::Graphics::cTexture::s_manager.Get(electroballTexture);
		s_render_static4.texture = eae6320::Graphics::cTexture::s_manager.Get(pokeballTexture);
	}
	else
	{
		s_render_static3.texture = eae6320::Graphics::cTexture::s_manager.Get(pokeballTexture);
		s_render_static4.texture = eae6320::Graphics::cTexture::s_manager.Get(electroballTexture);
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	cResult result = Results::Success;
	const uint8_t defaultRenderState = 0;

	// Initialize the shading data
	if (!(result = InitializeEffect()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	// Initialize the geometry data
	if (!(result = InitializeSprite()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	// Initialize the texture data
	if (!(result = InitializeTexture()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	// Initialize the rendering data
	InitializeRenderData();

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::InitializeEffect()
{
	cResult result = Results::Success;
	const uint8_t defaultRenderState = 0;

	if (!(result = eae6320::Graphics::Effect::Load("Sprite.binshd", "Sprite.binshd", defaultRenderState, s_effect)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::Effect::Load("Sprite.binshd", "Static.binshd", defaultRenderState, s_effect_static)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::InitializeSprite()
{
	cResult result = Results::Success;

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

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::InitializeTexture()
{
	cResult result = Results::Success;

	char texturePath_pikachu[100] = "data/Textures/";
	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(strcat(texturePath_pikachu, "Pikachu.bintxr"), pikachuTexture)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	char texturePath_pokeball[100] = "data/Textures/";
	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(strcat(texturePath_pokeball, "Pokeball.bintxr"), pokeballTexture)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	char texturePath_electroball[100] = "data/Textures/";
	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(strcat(texturePath_electroball, "Electroball.bintxr"), electroballTexture)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

OnExit:
	return result;
}

void eae6320::cExampleGame::InitializeRenderData()
{
	s_render = eae6320::Graphics::DataSetForRenderingSprite(s_effect, s_sprite, eae6320::Graphics::cTexture::s_manager.Get(pikachuTexture));
	s_render2 = eae6320::Graphics::DataSetForRenderingSprite(s_effect, s_sprite2, eae6320::Graphics::cTexture::s_manager.Get(pikachuTexture));
	s_render_static = eae6320::Graphics::DataSetForRenderingSprite(s_effect_static, s_sprite_static, eae6320::Graphics::cTexture::s_manager.Get(pokeballTexture));
	s_render_static2 = eae6320::Graphics::DataSetForRenderingSprite(s_effect_static, s_sprite_static2, eae6320::Graphics::cTexture::s_manager.Get(electroballTexture));
	s_render_static3 = eae6320::Graphics::DataSetForRenderingSprite(s_effect_static, s_sprite_static3, eae6320::Graphics::cTexture::s_manager.Get(pokeballTexture));
	s_render_static4 = eae6320::Graphics::DataSetForRenderingSprite(s_effect_static, s_sprite_static4, eae6320::Graphics::cTexture::s_manager.Get(electroballTexture));
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	cResult result = Results::Success;

	// Clean up the shading data
	if (!(result = CleanUpEffect()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	// Clean up the geometry data
	if (!(result = CleanUpSprite()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	// Clean up the texture data
	if (!(result = CleanUpTexture()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUpEffect()
{
	cResult result = Results::Success;

	if (s_effect)
	{
		result = s_effect->CleanUp();
		if (result)
			s_effect = nullptr;
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (s_effect_static)
	{
		result = s_effect_static->CleanUp();
		if (result)
			s_effect_static = nullptr;
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUpSprite()
{
	cResult result = Results::Success;

	if (s_sprite)
	{
		result = s_sprite->CleanUp();
		if (result)
			s_sprite = nullptr;
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (s_sprite2)
	{
		result = s_sprite2->CleanUp();
		if (result)
			s_sprite2 = nullptr;
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (s_sprite_static)
	{
		result = s_sprite_static->CleanUp();
		if (result)
			s_sprite_static = nullptr;
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (s_sprite_static2)
	{
		result = s_sprite_static2->CleanUp();
		if (result)
			s_sprite_static2 = nullptr;
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (s_sprite_static3)
	{
		result = s_sprite_static3->CleanUp();
		if (result)
			s_sprite_static3 = nullptr;
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (s_sprite_static4)
	{
		result = s_sprite_static4->CleanUp();
		if (result)
			s_sprite_static4 = nullptr;
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUpTexture()
{
	cResult result = Results::Success;

	if (pikachuTexture.IsValid())
	{
		if (!(result = eae6320::Graphics::cTexture::s_manager.Release(pikachuTexture)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (pokeballTexture.IsValid())
	{
		if (!(result = eae6320::Graphics::cTexture::s_manager.Release(pokeballTexture)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (electroballTexture.IsValid())
	{
		if (!(result = eae6320::Graphics::cTexture::s_manager.Release(electroballTexture)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

OnExit:
	return result;
}

void eae6320::cExampleGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	// Submit color data
	eae6320::Graphics::SubmitColorToBeRendered(eae6320::Graphics::Colors::Magenta);

	// Submit Effect Sprite pair data
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render2);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static2);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static3);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static4);
}
