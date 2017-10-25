// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/cTexture.h>
#include <Engine/Graphics/Effect.h>
#include <Engine/Graphics/Sprite.h>
#include <Engine/Graphics/Mesh.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Math/Functions.h>


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
	eae6320::Graphics::Mesh* s_movableMesh = nullptr;
	eae6320::Math::sVector movableInitLocation = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
	eae6320::Math::sVector movableInitVelocity = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
	eae6320::Math::sVector movableInitAcceleration = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
	eae6320::Graphics::Mesh* s_staticMesh = nullptr;
	eae6320::Math::sVector staticLocation = eae6320::Math::sVector(0.0f, 0.8f, 0.0f);
	eae6320::Math::sVector staticVelocity = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
	eae6320::Math::sVector staticAcceleration = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);

	// Combined Rendering Data with Sprite & Texture
	eae6320::Graphics::DataSetForRenderingSprite s_render = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render2 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static2 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static3 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static4 = eae6320::Graphics::DataSetForRenderingSprite();

	// Combined Rendering Data with Mesh
	eae6320::Graphics::DataSetForRenderingMesh s_render_movableMesh = eae6320::Graphics::DataSetForRenderingMesh();
	eae6320::Graphics::DataSetForRenderingMesh s_render_staticMesh = eae6320::Graphics::DataSetForRenderingMesh();

	// External counter used for Rendering based on time
	float previousTimeElapsedCounter = 0.0f;
	float currentTimeElapsedCounter = 0.0f;
	bool flagForSwappingTexturesBasedOnTime = false;

	// External multiplier used for control with acceleration
	const float accelerationMultiplier = 0.05f;
	const float normalAccelerationIncrement = 1.0f;
	const float frictionAccelerationIncrement = 5.0f;
	const float deaccelerationMultiplier = -3.0f;
	const float epsilonForVelocityOffset = 0.01f;
	const float epsilonForAccelerationOffset = 0.0001f;

	// Constant data for comparison
	const static eae6320::Math::sVector Zero = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
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

void eae6320::cExampleGame::UpdateSimulationBasedOnInput()
{
	float accelerationBaseFactorVertical = 0.0f;
	float accelerationBaseFactorHorizontal = 0.0f;

	float accelerationVertical = 0.0f;
	float accelerationHorizontal = 0.0f;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
		if(s_render_movableMesh.velocity.x > 0.0f)
			accelerationBaseFactorHorizontal += -1.0f * frictionAccelerationIncrement;
		else
			accelerationBaseFactorHorizontal += -1.0f * normalAccelerationIncrement;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
		if (s_render_movableMesh.velocity.x < 0.0f)
			accelerationBaseFactorHorizontal += frictionAccelerationIncrement;
		else
			accelerationBaseFactorHorizontal += normalAccelerationIncrement;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
		if (s_render_movableMesh.velocity.y < 0.0f)
			accelerationBaseFactorVertical += frictionAccelerationIncrement;
		else
			accelerationBaseFactorVertical += normalAccelerationIncrement;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
		if (s_render_movableMesh.velocity.y > 0.0f)
			accelerationBaseFactorVertical += -1.0f * frictionAccelerationIncrement;
		else
			accelerationBaseFactorVertical += -1.0f * normalAccelerationIncrement;
	
	accelerationHorizontal = accelerationBaseFactorHorizontal * accelerationMultiplier;
	accelerationVertical = accelerationBaseFactorVertical * accelerationMultiplier;
	s_render_movableMesh.acceleration = eae6320::Math::sVector(accelerationHorizontal, accelerationVertical, 0.0f);
}

void eae6320::cExampleGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	float deaccelerationX = s_render_movableMesh.acceleration.x;
	float deaccelerationY = s_render_movableMesh.acceleration.y;

	// If the velocity is not zero
	if (s_render_movableMesh.velocity != Zero)
	{
		// And the acceleration x component is zero
		if (eae6320::Math::AreAboutEqual(s_render_movableMesh.acceleration.x, 0.0f, epsilonForAccelerationOffset))
		{
			// If the velocity x component amount is tiny enough to be ignored, ignore the amount and make the mesh static
			s_render_movableMesh.velocity.x = eae6320::Math::AreAboutEqual(s_render_movableMesh.velocity.x, 0.0f, epsilonForVelocityOffset) ? 0.0f : s_render_movableMesh.velocity.x;
			// Otherwise, decrease the velocity by applying a deacceleration on x component
			deaccelerationX = eae6320::Math::AreAboutEqual(s_render_movableMesh.velocity.x, 0.0f, epsilonForAccelerationOffset) ? 0.0f : s_render_movableMesh.velocity.x / abs(s_render_movableMesh.velocity.x) * accelerationMultiplier * deaccelerationMultiplier;
		}
		// And the acceleration y component is zero
		if (eae6320::Math::AreAboutEqual(s_render_movableMesh.acceleration.y, 0.0f, epsilonForAccelerationOffset))
		{
			// If the velocity y component amount is tiny enough to be ignored, ignore the amount and make the mesh static
			s_render_movableMesh.velocity.y = eae6320::Math::AreAboutEqual(s_render_movableMesh.velocity.y, 0.0f, epsilonForVelocityOffset) ? 0.0f : s_render_movableMesh.velocity.y;
			// Otherwise, decrease the velocity by applying a deacceleration on y component
			deaccelerationY = eae6320::Math::AreAboutEqual(s_render_movableMesh.velocity.y, 0.0f, epsilonForAccelerationOffset) ? 0.0f : s_render_movableMesh.velocity.y / abs(s_render_movableMesh.velocity.y) * accelerationMultiplier * deaccelerationMultiplier;
		}
	}
	// Calculate the actual acceleration
	s_render_movableMesh.acceleration = eae6320::Math::sVector(deaccelerationX, deaccelerationY, 0.0f);
	// v = a * t
	s_render_movableMesh.velocity += s_render_movableMesh.acceleration * i_elapsedSecondCount_sinceLastUpdate;
	// s = v * t = a * (t ^ 2)
	s_render_movableMesh.position += s_render_movableMesh.velocity * i_elapsedSecondCount_sinceLastUpdate;
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
	std::vector<eae6320::Graphics::VertexFormats::sMesh> movableVertexData(6);
	std::vector<uint16_t> movableIndexData(12);
	{
		movableVertexData[0].x = 0.25f;
		movableVertexData[0].y = 0.25f;
		movableVertexData[0].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawCyan.R());
		movableVertexData[0].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawCyan.G());
		movableVertexData[0].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawCyan.B());
		movableVertexData[0].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawCyan.A());

		movableVertexData[1].x = 0.25f;
		movableVertexData[1].y = -0.25f;
		movableVertexData[1].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.R());
		movableVertexData[1].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.G());
		movableVertexData[1].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.B());
		movableVertexData[1].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.A());

		movableVertexData[2].x = 0.0f;
		movableVertexData[2].y = -0.5f;
		movableVertexData[2].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.R());
		movableVertexData[2].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.G());
		movableVertexData[2].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.B());
		movableVertexData[2].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.A());

		movableVertexData[3].x = -0.25f;
		movableVertexData[3].y = -0.25f;
		movableVertexData[3].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.R());
		movableVertexData[3].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.G());
		movableVertexData[3].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.B());
		movableVertexData[3].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.A());

		movableVertexData[4].x = -0.25f;
		movableVertexData[4].y = 0.25f;
		movableVertexData[4].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.R());
		movableVertexData[4].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.G());
		movableVertexData[4].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.B());
		movableVertexData[4].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.A());

		movableVertexData[5].x = 0.0f;
		movableVertexData[5].y = 0.5f;
		movableVertexData[5].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.R());
		movableVertexData[5].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.G());
		movableVertexData[5].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.B());
		movableVertexData[5].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.A());
	}
	{
		movableIndexData[0] = 0;
		movableIndexData[1] = 4;
		movableIndexData[2] = 5;

		movableIndexData[3] = 0;
		movableIndexData[4] = 1;
		movableIndexData[5] = 3;

		movableIndexData[6] = 0;
		movableIndexData[7] = 3;
		movableIndexData[8] = 4;

		movableIndexData[9] = 1;
		movableIndexData[10] = 2;
		movableIndexData[11] = 3;
	}

	std::vector<eae6320::Graphics::VertexFormats::sMesh> staticVertexData(4);
	std::vector<uint16_t> staticIndexData(6);
	{
		staticVertexData[0].x = 0.1f;
		staticVertexData[0].y = 0.0f;
		staticVertexData[0].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.R());
		staticVertexData[0].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.G());
		staticVertexData[0].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.B());
		staticVertexData[0].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.A());

		staticVertexData[1].x = 0.0f;
		staticVertexData[1].y = -0.1f;
		staticVertexData[1].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.R());
		staticVertexData[1].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.G());
		staticVertexData[1].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.B());
		staticVertexData[1].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.A());

		staticVertexData[2].x = -0.1f;
		staticVertexData[2].y = 0.0f;
		staticVertexData[2].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.R());
		staticVertexData[2].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.G());
		staticVertexData[2].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.B());
		staticVertexData[2].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.A());

		staticVertexData[3].x = 0.0f;
		staticVertexData[3].y = 0.1f;
		staticVertexData[3].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.R());
		staticVertexData[3].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.G());
		staticVertexData[3].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.B());
		staticVertexData[3].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.A());
	}
	{
		staticIndexData[0] = 0;
		staticIndexData[1] = 1;
		staticIndexData[2] = 2;

		staticIndexData[3] = 0;
		staticIndexData[4] = 2;
		staticIndexData[5] = 3;
	}

	if (!(result = eae6320::Graphics::Mesh::Load(movableVertexData, movableIndexData, s_movableMesh)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::Mesh::Load(staticVertexData, staticIndexData, s_staticMesh)))
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
	s_render_movableMesh = eae6320::Graphics::DataSetForRenderingMesh(s_effect_mesh, s_movableMesh, movableInitLocation, movableInitVelocity, movableInitAcceleration);
	s_render_staticMesh = eae6320::Graphics::DataSetForRenderingMesh(s_effect_mesh, s_staticMesh, staticLocation, staticVelocity, staticAcceleration);
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

	if (s_movableMesh)
	{
		result = s_movableMesh->CleanUp();
		if (result)
			s_movableMesh = nullptr;
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (s_staticMesh)
	{
		result = s_staticMesh->CleanUp();
		if (result)
			s_staticMesh = nullptr;
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
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static2);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static3);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static4);

	// Submit Effect Mesh pair data
	eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRendered(s_render_staticMesh);
	eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRendered(s_render_movableMesh);
}
