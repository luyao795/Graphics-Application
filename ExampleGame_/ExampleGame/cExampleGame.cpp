// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/cTexture.h>
#include <Engine/Graphics/Effect.h>
#include <Engine/Graphics/Sprite.h>
#include <Engine/Graphics/Mesh.h>
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
	// This effect contains mesh renderint data.
	eae6320::Graphics::Effect* s_effect_mesh = nullptr;

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

	// Mesh Data
	eae6320::Graphics::Mesh* s_polygonMesh = nullptr;

	// Combined Rendering Data with Sprite & Texture
	eae6320::Graphics::DataSetForRenderingSprite s_render = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render2 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static2 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static3 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static4 = eae6320::Graphics::DataSetForRenderingSprite();

	// Combined Rendering Data with Mesh
	eae6320::Graphics::DataSetForRenderingMesh s_render_mesh = eae6320::Graphics::DataSetForRenderingMesh();

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

	// Initialize the mesh data
	if (!(result = InitializeMesh()))
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

	if (!(result = eae6320::Graphics::Effect::Load("Mesh.binshd", "Mesh.binshd", defaultRenderState, s_effect_mesh)))
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

eae6320::cResult eae6320::cExampleGame::InitializeMesh()
{
	cResult result = Results::Success;

	// Initialize the vertex and index data for mesh
	std::vector<eae6320::Graphics::VertexFormats::sMesh> meshData(6);
	std::vector<uint16_t> indexData(12);
	{
		meshData[0].x = 0.25f;
		meshData[0].y = 0.25f;
		meshData[0].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawCyan.R());
		meshData[0].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawCyan.G());
		meshData[0].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawCyan.B());
		meshData[0].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawCyan.A());

		meshData[1].x = 0.25f;
		meshData[1].y = -0.25f;
		meshData[1].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.R());
		meshData[1].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.G());
		meshData[1].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.B());
		meshData[1].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.A());

		meshData[2].x = 0.0f;
		meshData[2].y = -0.5f;
		meshData[2].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.R());
		meshData[2].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.G());
		meshData[2].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.B());
		meshData[2].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.A());

		meshData[3].x = -0.25f;
		meshData[3].y = -0.25f;
		meshData[3].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.R());
		meshData[3].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.G());
		meshData[3].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.B());
		meshData[3].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.A());

		meshData[4].x = -0.25f;
		meshData[4].y = 0.25f;
		meshData[4].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.R());
		meshData[4].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.G());
		meshData[4].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.B());
		meshData[4].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.A());

		meshData[5].x = 0.0f;
		meshData[5].y = 0.5f;
		meshData[5].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.R());
		meshData[5].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.G());
		meshData[5].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.B());
		meshData[5].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.A());
	}
	{
		indexData[0] = 0;
		indexData[1] = 4;
		indexData[2] = 5;

		indexData[3] = 0;
		indexData[4] = 1;
		indexData[5] = 3;

		indexData[6] = 0;
		indexData[7] = 3;
		indexData[8] = 4;

		indexData[9] = 1;
		indexData[10] = 2;
		indexData[11] = 3;
	}

	if (!(result = eae6320::Graphics::Mesh::Load(meshData, indexData, s_polygonMesh)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

OnExit:
	return result;
}

void eae6320::cExampleGame::InitializeRenderData()
{
	// Initialize render data struct with Sprite and Texture
	s_render = eae6320::Graphics::DataSetForRenderingSprite(s_effect, s_sprite, eae6320::Graphics::cTexture::s_manager.Get(pikachuTexture));
	s_render2 = eae6320::Graphics::DataSetForRenderingSprite(s_effect, s_sprite2, eae6320::Graphics::cTexture::s_manager.Get(pikachuTexture));
	s_render_static = eae6320::Graphics::DataSetForRenderingSprite(s_effect_static, s_sprite_static, eae6320::Graphics::cTexture::s_manager.Get(pokeballTexture));
	s_render_static2 = eae6320::Graphics::DataSetForRenderingSprite(s_effect_static, s_sprite_static2, eae6320::Graphics::cTexture::s_manager.Get(electroballTexture));
	s_render_static3 = eae6320::Graphics::DataSetForRenderingSprite(s_effect_static, s_sprite_static3, eae6320::Graphics::cTexture::s_manager.Get(pokeballTexture));
	s_render_static4 = eae6320::Graphics::DataSetForRenderingSprite(s_effect_static, s_sprite_static4, eae6320::Graphics::cTexture::s_manager.Get(electroballTexture));

	// Initialize render data struct with Mesh
	s_render_mesh = eae6320::Graphics::DataSetForRenderingMesh(s_effect_mesh, s_polygonMesh, eae6320::Math::sVector(0.0f, 0.0f, 0.0f));
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

	// Clean up the mesh data
	if (!(result = CleanUpMesh()))
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

	if (s_effect_mesh)
	{
		result = s_effect_mesh->CleanUp();
		if (result)
			s_effect_mesh = nullptr;
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

eae6320::cResult eae6320::cExampleGame::CleanUpMesh()
{
	cResult result = Results::Success;

	if (s_polygonMesh)
	{
		result = s_polygonMesh->CleanUp();
		if (result)
			s_polygonMesh = nullptr;
		else
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
	//eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render);
	//eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render2);
	//eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static);
	//eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static2);
	//eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static3);
	//eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static4);

	// Submit Effect Mesh pair data
	eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRendered(s_render_mesh);
}
