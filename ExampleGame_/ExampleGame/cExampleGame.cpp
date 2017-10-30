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
	eae6320::Physics::sRigidBodyState movableRigidBody = eae6320::Physics::sRigidBodyState();

	eae6320::Graphics::Mesh* s_staticMesh = nullptr;
	eae6320::Math::sVector staticLocation = eae6320::Math::sVector(0.0f, 0.8f, 0.0f);
	eae6320::Math::sVector staticVelocity = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
	eae6320::Math::sVector staticAcceleration = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
	eae6320::Physics::sRigidBodyState staticRigidBody = eae6320::Physics::sRigidBodyState();

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

	// Camera Data
	eae6320::Graphics::Camera viewCamera;

	// External counter used for Rendering based on time
	float previousTimeElapsedCounter = 0.0f;
	float currentTimeElapsedCounter = 0.0f;
	bool flagForSwappingTexturesBasedOnTime = false;

	// External multiplier used for control with acceleration
	constexpr float accelerationMultiplier = 0.1f;
	constexpr float normalAccelerationIncrement = 1.0f;
	constexpr float frictionAccelerationIncrement = 5.0f;
	constexpr float deaccelerationMultiplier = -3.0f;
	constexpr float epsilonForVelocityOffset = 0.01f;
	constexpr float epsilonForAccelerationOffset = 0.0001f;

	// External constant for defining camera distance
	constexpr float cameraDistance = 10.0f;

	// Constant data for comparison
	static const eae6320::Math::sVector Zero = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
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
	// Update for mesh
	float accelerationBaseFactorVertical = 0.0f;
	float accelerationBaseFactorHorizontal = 0.0f;

	float accelerationVertical = 0.0f;
	float accelerationHorizontal = 0.0f;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
		if(s_render_movableMesh.rigidBody.velocity.x > 0.0f)
			accelerationBaseFactorHorizontal += -1.0f * frictionAccelerationIncrement;
		else
			accelerationBaseFactorHorizontal += -1.0f * normalAccelerationIncrement;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
		if (s_render_movableMesh.rigidBody.velocity.x < 0.0f)
			accelerationBaseFactorHorizontal += frictionAccelerationIncrement;
		else
			accelerationBaseFactorHorizontal += normalAccelerationIncrement;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
		if (s_render_movableMesh.rigidBody.velocity.y < 0.0f)
			accelerationBaseFactorVertical += frictionAccelerationIncrement;
		else
			accelerationBaseFactorVertical += normalAccelerationIncrement;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
		if (s_render_movableMesh.rigidBody.velocity.y > 0.0f)
			accelerationBaseFactorVertical += -1.0f * frictionAccelerationIncrement;
		else
			accelerationBaseFactorVertical += -1.0f * normalAccelerationIncrement;
	
	accelerationHorizontal = accelerationBaseFactorHorizontal * accelerationMultiplier;
	accelerationVertical = accelerationBaseFactorVertical * accelerationMultiplier;
	s_render_movableMesh.rigidBody.acceleration = eae6320::Math::sVector(accelerationHorizontal, accelerationVertical, 0.0f);

	// Update for camera
	const float speedMultiplierForCamera = 0.25f;
	float speedVerticalCamera = 0.0f;
	float speedHorizontalCamera = 0.0f;

	if (UserInput::IsKeyPressed('A'))
		speedHorizontalCamera += -1.0f * speedMultiplierForCamera;

	if (UserInput::IsKeyPressed('D'))
		speedHorizontalCamera += speedMultiplierForCamera;

	if (UserInput::IsKeyPressed('W'))
		speedVerticalCamera += speedMultiplierForCamera;

	if (UserInput::IsKeyPressed('S'))
		speedVerticalCamera += -1.0f * speedMultiplierForCamera;

	viewCamera.rigidBody.velocity = eae6320::Math::sVector(speedHorizontalCamera, speedVerticalCamera, 0.0f);
}

void eae6320::cExampleGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	float deaccelerationX = s_render_movableMesh.rigidBody.acceleration.x;
	float deaccelerationY = s_render_movableMesh.rigidBody.acceleration.y;

	// If the velocity is not zero
	if (s_render_movableMesh.rigidBody.velocity != Zero)
	{
		// And the acceleration x component is zero
		if (eae6320::Math::AreAboutEqual(s_render_movableMesh.rigidBody.acceleration.x, 0.0f, epsilonForAccelerationOffset))
		{
			// If the velocity x component amount is tiny enough to be ignored, ignore the amount and make the mesh static
			s_render_movableMesh.rigidBody.velocity.x = eae6320::Math::AreAboutEqual(s_render_movableMesh.rigidBody.velocity.x, 0.0f, epsilonForVelocityOffset) ? 0.0f : s_render_movableMesh.rigidBody.velocity.x;
			// Otherwise, decrease the velocity by applying a deacceleration on x component
			deaccelerationX = eae6320::Math::AreAboutEqual(s_render_movableMesh.rigidBody.velocity.x, 0.0f, epsilonForAccelerationOffset) ? 0.0f : s_render_movableMesh.rigidBody.velocity.x / abs(s_render_movableMesh.rigidBody.velocity.x) * accelerationMultiplier * deaccelerationMultiplier;
		}
		// And the acceleration y component is zero
		if (eae6320::Math::AreAboutEqual(s_render_movableMesh.rigidBody.acceleration.y, 0.0f, epsilonForAccelerationOffset))
		{
			// If the velocity y component amount is tiny enough to be ignored, ignore the amount and make the mesh static
			s_render_movableMesh.rigidBody.velocity.y = eae6320::Math::AreAboutEqual(s_render_movableMesh.rigidBody.velocity.y, 0.0f, epsilonForVelocityOffset) ? 0.0f : s_render_movableMesh.rigidBody.velocity.y;
			// Otherwise, decrease the velocity by applying a deacceleration on y component
			deaccelerationY = eae6320::Math::AreAboutEqual(s_render_movableMesh.rigidBody.velocity.y, 0.0f, epsilonForAccelerationOffset) ? 0.0f : s_render_movableMesh.rigidBody.velocity.y / abs(s_render_movableMesh.rigidBody.velocity.y) * accelerationMultiplier * deaccelerationMultiplier;
		}
	}
	// Calculate the actual acceleration
	s_render_movableMesh.rigidBody.acceleration = eae6320::Math::sVector(deaccelerationX, deaccelerationY, 0.0f);
	// Update transform information about the mesh
	s_render_movableMesh.rigidBody.Update(i_elapsedSecondCount_sinceLastUpdate);
	// Update transform information about the camera
	viewCamera.rigidBody.Update(i_elapsedSecondCount_sinceLastUpdate);
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	cResult result = Results::Success;
	const uint8_t defaultRenderState = 0;

	viewCamera.rigidBody.position.z = cameraDistance;

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
	std::vector<eae6320::Graphics::VertexFormats::sMesh> movableVertexData(4);
	std::vector<uint16_t> movableIndexData(6);
	{
		movableVertexData[0].x = -1.0f;
		movableVertexData[0].y = -1.0f;
		movableVertexData[0].z = 0.0f;
		movableVertexData[0].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.R());
		movableVertexData[0].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.G());
		movableVertexData[0].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.B());
		movableVertexData[0].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawRed.A());

		movableVertexData[1].x = -1.0f;
		movableVertexData[1].y = 1.0f;
		movableVertexData[1].z = 0.0f;
		movableVertexData[1].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.R());
		movableVertexData[1].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.G());
		movableVertexData[1].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.B());
		movableVertexData[1].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGreen.A());

		movableVertexData[2].x = 1.0f;
		movableVertexData[2].y = 1.0f;
		movableVertexData[2].z = 0.0f;
		movableVertexData[2].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.R());
		movableVertexData[2].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.G());
		movableVertexData[2].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.B());
		movableVertexData[2].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.A());

		movableVertexData[3].x = 1.0f;
		movableVertexData[3].y = -1.0f;
		movableVertexData[3].z = 0.0f;
		movableVertexData[3].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.R());
		movableVertexData[3].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.G());
		movableVertexData[3].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.B());
		movableVertexData[3].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.A());
	}
	{
		movableIndexData[0] = 0;
		movableIndexData[1] = 1;
		movableIndexData[2] = 2;

		movableIndexData[3] = 0;
		movableIndexData[4] = 2;
		movableIndexData[5] = 3;
	}

	std::vector<eae6320::Graphics::VertexFormats::sMesh> staticVertexData(4);
	std::vector<uint16_t> staticIndexData(6);
	{
		staticVertexData[0].x = 0.1f;
		staticVertexData[0].y = 0.0f;
		staticVertexData[0].z = 0.0f;
		staticVertexData[0].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.R());
		staticVertexData[0].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.G());
		staticVertexData[0].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.B());
		staticVertexData[0].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawYellow.A());

		staticVertexData[1].x = 0.0f;
		staticVertexData[1].y = -0.1f;
		staticVertexData[1].z = 0.0f;
		staticVertexData[1].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.R());
		staticVertexData[1].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.G());
		staticVertexData[1].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.B());
		staticVertexData[1].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawBlue.A());

		staticVertexData[2].x = -0.1f;
		staticVertexData[2].y = 0.0f;
		staticVertexData[2].z = 0.0f;
		staticVertexData[2].r = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.R());
		staticVertexData[2].g = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.G());
		staticVertexData[2].b = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.B());
		staticVertexData[2].a = static_cast<uint8_t>(eae6320::Graphics::Colors::RawGray.A());

		staticVertexData[3].x = 0.0f;
		staticVertexData[3].y = 0.1f;
		staticVertexData[3].z = 0.0f;
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
	movableRigidBody.position = movableInitLocation;
	movableRigidBody.velocity = movableInitVelocity;
	movableRigidBody.acceleration = movableInitAcceleration;
	s_render_movableMesh = eae6320::Graphics::DataSetForRenderingMesh(s_effect_mesh, s_movableMesh, movableRigidBody);
	staticRigidBody.position = staticLocation;
	staticRigidBody.velocity = staticVelocity;
	staticRigidBody.acceleration = staticAcceleration;
	s_render_staticMesh = eae6320::Graphics::DataSetForRenderingMesh(s_effect_mesh, s_staticMesh, staticRigidBody);
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
	// Submit Color data
	eae6320::Graphics::SubmitColorToBeRendered(eae6320::Graphics::Colors::Magenta);

	// Submit Effect Mesh pair data with prediction if needed
	//eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRenderedUsingPredictionIfNeeded(s_render_staticMesh, i_elapsedSecondCount_sinceLastSimulationUpdate, false);
	eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRenderedUsingPredictionIfNeeded(s_render_movableMesh, i_elapsedSecondCount_sinceLastSimulationUpdate, true);

	// Submit Effect Sprite pair data
	//eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render);
	//eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render2);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static2);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static3);
	eae6320::Graphics::SubmitEffectSpritePairToBeRenderedWithTexture(s_render_static4);

	// Submit Camera data
	eae6320::Graphics::SubmitCameraForView(viewCamera, i_elapsedSecondCount_sinceLastSimulationUpdate);
}
