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
	// Constant data for comparison
	static const eae6320::Math::sVector Zero = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
	static const eae6320::Math::sVector X = eae6320::Math::sVector(1.0f, 0.0f, 0.0f);
	static const eae6320::Math::sVector Y = eae6320::Math::sVector(0.0f, 1.0f, 0.0f);
	static const eae6320::Math::sVector Z = eae6320::Math::sVector(0.0f, 0.0f, 1.0f);

	// External constant data for movable mesh size
	constexpr float movableMeshSideLength = 1.0f;
	constexpr float planeMeshLongSideLength = 2.0f;
	constexpr float planeMeshShortSideLength = 0.125f;

	// External constant for default render state
	constexpr uint8_t defaultRenderState = 0;

	// External counter used for Rendering based on time
	float previousTimeElapsedCounter = 0.0f;
	float currentTimeElapsedCounter = 0.0f;
	bool flagForSwappingTexturesBasedOnTime = false;

	// External multiplier constants used for control with acceleration on mesh
	constexpr float accelerationMultiplier = 0.25f;
	constexpr float normalAccelerationIncrement = 1.0f;
	constexpr float frictionAccelerationIncrement = 5.0f;
	constexpr float deaccelerationMultiplier = -3.0f;
	constexpr float epsilonForVelocityOffset = 0.01f;
	constexpr float epsilonForAccelerationOffset = 0.0001f;

	// External multiplier constants used for control on camera
	constexpr float speedMultiplierForCamera = 2.0f;
	constexpr float rotationAmountForCamera = 0.5f;

	// External constants for defining camera distance
	constexpr float cameraDistanceX = 0.00f;
	constexpr float cameraDistanceY = 0.50f;
	constexpr float cameraDistanceZ = 10.0f;

	// External constants for defining the camera properties
	constexpr float aspectRatio = 1.0f;
	const float cameraFieldOfView = eae6320::Graphics::ConvertDegreeToRadian(45.0f);
	constexpr float nearPlaneDistance = 0.1f;
	constexpr float farPlaneDistance = 100.0f;

	// Shading Data
	//-------------
	// This effect contains color changing property.
	eae6320::Graphics::Effect* s_effect = nullptr;
	// This effect contains white static property.
	eae6320::Graphics::Effect* s_effect_static = nullptr;
	// This effect contains mesh render data for solid shapes.
	eae6320::Graphics::Effect* s_effect_mesh_solid = nullptr;
	// This effect contains mesh render data for exposed shapes.
	eae6320::Graphics::Effect* s_effect_mesh_exposed = nullptr;
	// This effect contains mesh render data for translucent effect
	eae6320::Graphics::Effect* s_effect_mesh_translucent = nullptr;

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
	//-------------
	eae6320::Graphics::cTexture::Handle pikachuTexture;
	eae6320::Graphics::cTexture::Handle pokeballTexture;
	eae6320::Graphics::cTexture::Handle electroballTexture;
	eae6320::Graphics::cTexture::Handle flowerShibeTexture;
	eae6320::Graphics::cTexture::Handle evilShibeTexture;
	eae6320::Graphics::cTexture::Handle AKMTexture;

	// Mesh Data
	//----------
	// Weapon Mesh
	eae6320::Graphics::Mesh::Handle AKMMesh;
	eae6320::Math::sVector cubeInitLocation = Zero;
	eae6320::Math::sVector cubeInitVelocity = Zero;
	eae6320::Math::sVector cubeInitAcceleration = Zero;
	eae6320::Physics::sRigidBodyState cubeRigidBody = eae6320::Physics::sRigidBodyState();
	// Plane Mesh
	eae6320::Graphics::Mesh::Handle planeMesh;
	eae6320::Math::sVector planeLocation = eae6320::Math::sVector(0.0f, movableMeshSideLength * (-1.0f), 0.0f);
	eae6320::Math::sVector planeVelocity = Zero;
	eae6320::Math::sVector planeAcceleration = Zero;
	eae6320::Physics::sRigidBodyState planeRigidBody = eae6320::Physics::sRigidBodyState();
	// Sphere Mesh with 2 different instances
	eae6320::Graphics::Mesh::Handle sphereMesh;
	// First instance
	eae6320::Math::sVector sphere1Location = eae6320::Math::sVector(0.0f, 5.0f, 0.0f);
	eae6320::Math::sVector sphere1Velocity = Zero;
	eae6320::Math::sVector sphere1Acceleration = Zero;
	eae6320::Physics::sRigidBodyState sphere1RigidBody = eae6320::Physics::sRigidBodyState();
	// Second instance
	eae6320::Math::sVector sphere2Location = eae6320::Math::sVector(-1.0f, 5.0f, -10.0f);
	eae6320::Math::sVector sphere2Velocity = Zero;
	eae6320::Math::sVector sphere2Acceleration = Zero;
	eae6320::Physics::sRigidBodyState sphere2RigidBody = eae6320::Physics::sRigidBodyState();

	// Combined Rendering Data with Sprite & Texture
	eae6320::Graphics::DataSetForRenderingSprite s_render = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render2 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static2 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static3 = eae6320::Graphics::DataSetForRenderingSprite();
	eae6320::Graphics::DataSetForRenderingSprite s_render_static4 = eae6320::Graphics::DataSetForRenderingSprite();

	// Combined Rendering Data with Mesh
	eae6320::Graphics::DataSetForRenderingMesh s_render_movableAKM = eae6320::Graphics::DataSetForRenderingMesh();
	eae6320::Graphics::DataSetForRenderingMesh s_render_staticPlane = eae6320::Graphics::DataSetForRenderingMesh();
	eae6320::Graphics::DataSetForRenderingMesh s_render_staticSphere1 = eae6320::Graphics::DataSetForRenderingMesh();
	eae6320::Graphics::DataSetForRenderingMesh s_render_staticSphere2 = eae6320::Graphics::DataSetForRenderingMesh();

	// Camera Data
	//------------
	eae6320::Graphics::Camera viewCamera;
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
	float accelerationBaseFactorDepth = 0.0f;

	float accelerationVertical = 0.0f;
	float accelerationHorizontal = 0.0f;
	float accelerationDepth = 0.0f;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
		if(s_render_movableAKM.rigidBody.velocity.x > 0.0f)
			accelerationBaseFactorHorizontal += frictionAccelerationIncrement * (-1.0f);
		else
			accelerationBaseFactorHorizontal += normalAccelerationIncrement * (-1.0f);

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
		if (s_render_movableAKM.rigidBody.velocity.x < 0.0f)
			accelerationBaseFactorHorizontal += frictionAccelerationIncrement;
		else
			accelerationBaseFactorHorizontal += normalAccelerationIncrement;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
		if (s_render_movableAKM.rigidBody.velocity.y < 0.0f)
			accelerationBaseFactorVertical += frictionAccelerationIncrement;
		else
			accelerationBaseFactorVertical += normalAccelerationIncrement;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
		if (s_render_movableAKM.rigidBody.velocity.y > 0.0f)
			accelerationBaseFactorVertical += frictionAccelerationIncrement * (-1.0f);
		else
			accelerationBaseFactorVertical += normalAccelerationIncrement * (-1.0f);

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::PageUp))
		if (s_render_movableAKM.rigidBody.velocity.z > 0.0f)
			accelerationBaseFactorDepth += frictionAccelerationIncrement * (-1.0f);
		else
			accelerationBaseFactorDepth += normalAccelerationIncrement * (-1.0f);

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::PageDown))
		if (s_render_movableAKM.rigidBody.velocity.z < 0.0f)
			accelerationBaseFactorDepth += frictionAccelerationIncrement;
		else
			accelerationBaseFactorDepth += normalAccelerationIncrement;
	
	accelerationHorizontal = accelerationBaseFactorHorizontal * accelerationMultiplier;
	accelerationVertical = accelerationBaseFactorVertical * accelerationMultiplier;
	accelerationDepth = accelerationBaseFactorDepth * accelerationMultiplier;

	s_render_movableAKM.rigidBody.acceleration = eae6320::Math::sVector(accelerationHorizontal, accelerationVertical, accelerationDepth);

	// Update for camera
	float speedVerticalCamera = 0.0f;
	float speedHorizontalCamera = 0.0f;
	float speedDepthCamera = 0.0f;

	static float rotationAngle = 0.0f;

	if (UserInput::IsKeyPressed('A'))
		speedHorizontalCamera += speedMultiplierForCamera * (-1.0f);

	if (UserInput::IsKeyPressed('D'))
		speedHorizontalCamera += speedMultiplierForCamera;

	if (UserInput::IsKeyPressed('W'))
		speedVerticalCamera += speedMultiplierForCamera;

	if (UserInput::IsKeyPressed('S'))
		speedVerticalCamera += speedMultiplierForCamera * (-1.0f);

	if (UserInput::IsKeyPressed('Z'))
		speedDepthCamera += speedMultiplierForCamera * (-1.0f);

	if (UserInput::IsKeyPressed('X'))
		speedDepthCamera += speedMultiplierForCamera;

	if (UserInput::IsKeyPressed('Q'))
		rotationAngle += rotationAmountForCamera * (-1.0f);

	if (UserInput::IsKeyPressed('E'))
		rotationAngle += rotationAmountForCamera;

	viewCamera.rigidBody.velocity = eae6320::Math::sVector(speedHorizontalCamera, speedVerticalCamera, speedDepthCamera);
	viewCamera.rigidBody.orientation = eae6320::Math::cQuaternion(eae6320::Graphics::ConvertDegreeToRadian(rotationAngle), Y);
}

void eae6320::cExampleGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	float deaccelerationX = s_render_movableAKM.rigidBody.acceleration.x;
	float deaccelerationY = s_render_movableAKM.rigidBody.acceleration.y;
	float deaccelerationZ = s_render_movableAKM.rigidBody.acceleration.z;

	// If the velocity is not zero
	if (s_render_movableAKM.rigidBody.velocity != Zero)
	{
		// And the acceleration x component is zero
		if (eae6320::Math::AreAboutEqual(s_render_movableAKM.rigidBody.acceleration.x, 0.0f, epsilonForAccelerationOffset))
		{
			// If the velocity x component amount is tiny enough to be ignored, ignore the amount and make the mesh static
			s_render_movableAKM.rigidBody.velocity.x = eae6320::Math::AreAboutEqual(s_render_movableAKM.rigidBody.velocity.x, 0.0f, epsilonForVelocityOffset) ? 0.0f : s_render_movableAKM.rigidBody.velocity.x;
			// Otherwise, decrease the velocity by applying a deacceleration on x component
			deaccelerationX = eae6320::Math::AreAboutEqual(s_render_movableAKM.rigidBody.velocity.x, 0.0f, epsilonForAccelerationOffset) ? 0.0f : s_render_movableAKM.rigidBody.velocity.x / abs(s_render_movableAKM.rigidBody.velocity.x) * accelerationMultiplier * deaccelerationMultiplier;
		}
		// And the acceleration y component is zero
		if (eae6320::Math::AreAboutEqual(s_render_movableAKM.rigidBody.acceleration.y, 0.0f, epsilonForAccelerationOffset))
		{
			// If the velocity y component amount is tiny enough to be ignored, ignore the amount and make the mesh static
			s_render_movableAKM.rigidBody.velocity.y = eae6320::Math::AreAboutEqual(s_render_movableAKM.rigidBody.velocity.y, 0.0f, epsilonForVelocityOffset) ? 0.0f : s_render_movableAKM.rigidBody.velocity.y;
			// Otherwise, decrease the velocity by applying a deacceleration on y component
			deaccelerationY = eae6320::Math::AreAboutEqual(s_render_movableAKM.rigidBody.velocity.y, 0.0f, epsilonForAccelerationOffset) ? 0.0f : s_render_movableAKM.rigidBody.velocity.y / abs(s_render_movableAKM.rigidBody.velocity.y) * accelerationMultiplier * deaccelerationMultiplier;
		}
		// And the acceleration z component is zero
		if (eae6320::Math::AreAboutEqual(s_render_movableAKM.rigidBody.acceleration.z, 0.0f, epsilonForAccelerationOffset))
		{
			// If the velocity z component amount is tiny enough to be ignored, ignore the amount and make the mesh static
			s_render_movableAKM.rigidBody.velocity.z = eae6320::Math::AreAboutEqual(s_render_movableAKM.rigidBody.velocity.z, 0.0f, epsilonForVelocityOffset) ? 0.0f : s_render_movableAKM.rigidBody.velocity.z;
			// Otherwise, decrease the velocity by applying a deacceleration on z component
			deaccelerationZ = eae6320::Math::AreAboutEqual(s_render_movableAKM.rigidBody.velocity.z, 0.0f, epsilonForAccelerationOffset) ? 0.0f : s_render_movableAKM.rigidBody.velocity.z / abs(s_render_movableAKM.rigidBody.velocity.z) * accelerationMultiplier * deaccelerationMultiplier;
		}
	}
	// Calculate the actual acceleration
	s_render_movableAKM.rigidBody.acceleration = eae6320::Math::sVector(deaccelerationX, deaccelerationY, deaccelerationZ);
	// Update transform information about the mesh
	s_render_movableAKM.rigidBody.Update(i_elapsedSecondCount_sinceLastUpdate);
	// Update transform information about the camera
	viewCamera.rigidBody.Update(i_elapsedSecondCount_sinceLastUpdate);
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	cResult result = Results::Success;
	const uint8_t defaultRenderState = 0;

	InitializeCamera();

	// Initialize the shading data
	if (!(result = InitializeEffect()))
	{
		EAE6320_ASSERTF(false, "Effect initialization failed");
		goto OnExit;
	}

	// Initialize the geometry data
	if (!(result = InitializeSprite()))
	{
		EAE6320_ASSERTF(false, "Sprite initialization failed");
		goto OnExit;
	}

	// Initialize the texture data
	if (!(result = InitializeTexture()))
	{
		EAE6320_ASSERTF(false, "Texture initialization failed");
		goto OnExit;
	}

	// Initialize the mesh data
	if (!(result = InitializeMesh()))
	{
		EAE6320_ASSERTF(false, "Mesh initialization failed");
		goto OnExit;
	}

	// Initialize the rendering data
	InitializeRenderData();

OnExit:
	return result;
}

void eae6320::cExampleGame::InitializeCamera()
{
	viewCamera.rigidBody.position.x = cameraDistanceX;
	viewCamera.rigidBody.position.y = cameraDistanceY;
	viewCamera.rigidBody.position.z = cameraDistanceZ;

	viewCamera.aspectRatio = aspectRatio;
	viewCamera.fieldOfView = cameraFieldOfView;
	viewCamera.nearPlaneDistance = nearPlaneDistance;
	viewCamera.farPlaneDistance = farPlaneDistance;
}

eae6320::cResult eae6320::cExampleGame::InitializeEffect()
{
	cResult result = Results::Success;

	// Initialize render state for sprites to be the same as default render state
	uint8_t s_RenderStateForSpriteWithAlphaTransparency = defaultRenderState;

	// If alpha transparency is not enabled, enable it
	if (!eae6320::Graphics::RenderStates::IsAlphaTransparencyEnabled(s_RenderStateForSpriteWithAlphaTransparency))
		eae6320::Graphics::RenderStates::EnableAlphaTransparency(s_RenderStateForSpriteWithAlphaTransparency);

	if (!(result = eae6320::Graphics::Effect::Load("Sprite.binshd", "Sprite.binshd", s_RenderStateForSpriteWithAlphaTransparency, s_effect)))
	{
		EAE6320_ASSERTF(false, "Effect initialization failed");
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::Effect::Load("Sprite.binshd", "Static.binshd", s_RenderStateForSpriteWithAlphaTransparency, s_effect_static)))
	{
		EAE6320_ASSERTF(false, "Effect initialization failed");
		goto OnExit;
	}

	// Initialize render state for solid meshes to be the same as default render state
	uint8_t s_RenderStateForMeshWithDepthBuffering = defaultRenderState;

	// If depth buffering is not enabled, enable it
	if (!eae6320::Graphics::RenderStates::IsDepthBufferingEnabled(s_RenderStateForMeshWithDepthBuffering))
		eae6320::Graphics::RenderStates::EnableDepthBuffering(s_RenderStateForMeshWithDepthBuffering);

	if (!(result = eae6320::Graphics::Effect::Load("Mesh.binshd", "MeshTexture.binshd", s_RenderStateForMeshWithDepthBuffering, s_effect_mesh_solid)))
	{
		EAE6320_ASSERTF(false, "Effect initialization failed");
		goto OnExit;
	}

	// Initialize render state for exposed meshes to be the same as default render state
	uint8_t s_RenderStateForMeshWithDepthBufferingAndDoubleSideRendering = defaultRenderState;

	// If depth buffering is not enabled, enable it
	if (!eae6320::Graphics::RenderStates::IsDepthBufferingEnabled(s_RenderStateForMeshWithDepthBufferingAndDoubleSideRendering))
		eae6320::Graphics::RenderStates::EnableDepthBuffering(s_RenderStateForMeshWithDepthBufferingAndDoubleSideRendering);

	// Enable rendering for both sides
	eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides(s_RenderStateForMeshWithDepthBufferingAndDoubleSideRendering);

	if (!(result = eae6320::Graphics::Effect::Load("Mesh.binshd", "MeshTexture.binshd", s_RenderStateForMeshWithDepthBufferingAndDoubleSideRendering, s_effect_mesh_exposed)))
	{
		EAE6320_ASSERTF(false, "Effect initialization failed");
		goto OnExit;
	}

	// Initialize render state for translucent meshes to be the same as default render state
	uint8_t s_RenderStateForTranslucentMeshWithoutDepthBufferingOrDoubleSideRendering = defaultRenderState;
	if (!eae6320::Graphics::RenderStates::IsAlphaTransparencyEnabled(s_RenderStateForTranslucentMeshWithoutDepthBufferingOrDoubleSideRendering))
		eae6320::Graphics::RenderStates::EnableAlphaTransparency(s_RenderStateForTranslucentMeshWithoutDepthBufferingOrDoubleSideRendering);

	if (!(result = eae6320::Graphics::Effect::Load("Mesh.binshd", "MeshTexture_Translucent.binshd", s_RenderStateForTranslucentMeshWithoutDepthBufferingOrDoubleSideRendering, s_effect_mesh_translucent)))
	{
		EAE6320_ASSERTF(false, "Effect initialization failed");
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
		EAE6320_ASSERTF(false, "Sprite initialization failed");
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::Sprite::Load(0.25f, 0.75f, 0.5f, 1.5f, s_sprite2)))
	{
		EAE6320_ASSERTF(false, "Sprite initialization failed");
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::Sprite::Load(1.0f, 1.0f, 0.5f, 0.5f, s_sprite_static)))
	{
		EAE6320_ASSERTF(false, "Sprite initialization failed");
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::Sprite::Load(-0.5f, 1.0f, 0.5f, 0.5f, s_sprite_static2)))
	{
		EAE6320_ASSERTF(false, "Sprite initialization failed");
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::Sprite::Load(1.0f, -0.5f, 0.5f, 0.5f, s_sprite_static3)))
	{
		EAE6320_ASSERTF(false, "Sprite initialization failed");
		goto OnExit;
	}

	if (!(result = eae6320::Graphics::Sprite::Load(-0.5f, -0.5f, 0.5f, 0.5f, s_sprite_static4)))
	{
		EAE6320_ASSERTF(false, "Sprite initialization failed");
		goto OnExit;
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::InitializeTexture()
{
	cResult result = Results::Success;

	const char * texture_pikachu = "Pikachu.bintxr";
	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(texture_pikachu, pikachuTexture)))
	{
		EAE6320_ASSERTF(false, "Texture initialization failed");
		goto OnExit;
	}

	const char * texture_pokeball = "Pokeball.bintxr";
	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(texture_pokeball, pokeballTexture)))
	{
		EAE6320_ASSERTF(false, "Texture initialization failed");
		goto OnExit;
	}

	const char * texture_electroball = "Electroball.bintxr";
	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(texture_electroball, electroballTexture)))
	{
		EAE6320_ASSERTF(false, "Texture initialization failed");
		goto OnExit;
	}

	const char * texture_flowerShibe = "FlowerShibe.bintxr";
	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(texture_flowerShibe, flowerShibeTexture)))
	{
		EAE6320_ASSERTF(false, "Texture initialization failed");
		goto OnExit;
	}

	const char * texture_evilShibe = "EvilShibe.bintxr";
	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(texture_evilShibe, evilShibeTexture)))
	{
		EAE6320_ASSERTF(false, "Texture initialization failed");
		goto OnExit;
	}

	const char * texture_AKM = "AKM.bintxr";
	if (!(result = eae6320::Graphics::cTexture::s_manager.Load(texture_AKM, AKMTexture)))
	{
		EAE6320_ASSERTF(false, "Texture initialization failed");
		goto OnExit;
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::InitializeMesh()
{
	cResult result = Results::Success;

	const char * mesh_AKM = "AKM.binmsh";
	if (!(result = eae6320::Graphics::Mesh::s_manager.Load(mesh_AKM, AKMMesh)))
	{
		EAE6320_ASSERTF(false, "Mesh initialization failed");
		goto OnExit;
	}

	const char * mesh_plane = "Plane.binmsh";
	if (!(result = eae6320::Graphics::Mesh::s_manager.Load(mesh_plane, planeMesh)))
	{
		EAE6320_ASSERTF(false, "Mesh initialization failed");
		goto OnExit;
	}

	const char * mesh_sphere = "Sphere.binmsh";
	if (!(result = eae6320::Graphics::Mesh::s_manager.Load(mesh_sphere, sphereMesh)))
	{
		EAE6320_ASSERTF(false, "Mesh initialization failed");
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
	cubeRigidBody.position = cubeInitLocation;
	cubeRigidBody.velocity = cubeInitVelocity;
	cubeRigidBody.acceleration = cubeInitAcceleration;
	s_render_movableAKM = eae6320::Graphics::DataSetForRenderingMesh(s_effect_mesh_solid, eae6320::Graphics::Mesh::s_manager.Get(AKMMesh), eae6320::Graphics::cTexture::s_manager.Get(AKMTexture), cubeRigidBody);
	planeRigidBody.position = planeLocation;
	planeRigidBody.velocity = planeVelocity;
	planeRigidBody.acceleration = planeAcceleration;
	s_render_staticPlane = eae6320::Graphics::DataSetForRenderingMesh(s_effect_mesh_exposed, eae6320::Graphics::Mesh::s_manager.Get(planeMesh), eae6320::Graphics::cTexture::s_manager.Get(flowerShibeTexture), planeRigidBody);
	sphere1RigidBody.position = sphere1Location;
	sphere1RigidBody.velocity = sphere1Velocity;
	sphere1RigidBody.acceleration = sphere1Acceleration;
	s_render_staticSphere1 = eae6320::Graphics::DataSetForRenderingMesh(s_effect_mesh_translucent, eae6320::Graphics::Mesh::s_manager.Get(sphereMesh), eae6320::Graphics::cTexture::s_manager.Get(evilShibeTexture), sphere1RigidBody);
	sphere2RigidBody.position = sphere2Location;
	sphere2RigidBody.velocity = sphere2Velocity;
	sphere2RigidBody.acceleration = sphere2Acceleration;
	s_render_staticSphere2 = eae6320::Graphics::DataSetForRenderingMesh(s_effect_mesh_translucent, eae6320::Graphics::Mesh::s_manager.Get(sphereMesh), eae6320::Graphics::cTexture::s_manager.Get(evilShibeTexture), sphere2RigidBody);
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	cResult result = Results::Success;

	// Clean up the shading data
	if (!(result = CleanUpEffect()))
	{
		EAE6320_ASSERTF(false, "Effect cleanup failed");
		goto OnExit;
	}

	// Clean up the geometry data
	if (!(result = CleanUpSprite()))
	{
		EAE6320_ASSERTF(false, "Sprite cleanup failed");
		goto OnExit;
	}

	// Clean up the texture data
	if (!(result = CleanUpTexture()))
	{
		EAE6320_ASSERTF(false, "Texture cleanup failed");
		goto OnExit;
	}

	// Clean up the mesh data
	if (!(result = CleanUpMesh()))
	{
		EAE6320_ASSERTF(false, "Mesh cleanup failed");
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
			EAE6320_ASSERTF(false, "Effect cleanup failed");
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
			EAE6320_ASSERTF(false, "Effect cleanup failed");
			goto OnExit;
		}
	}

	if (s_effect_mesh_solid)
	{
		result = s_effect_mesh_solid->CleanUp();
		if (result)
			s_effect_mesh_solid = nullptr;
		else
		{
			EAE6320_ASSERTF(false, "Effect cleanup failed");
			goto OnExit;
		}
	}

	if (s_effect_mesh_exposed)
	{
		result = s_effect_mesh_exposed->CleanUp();
		if (result)
			s_effect_mesh_exposed = nullptr;
		else
		{
			EAE6320_ASSERTF(false, "Effect cleanup failed");
			goto OnExit;
		}
	}

	if (s_effect_mesh_translucent)
	{
		result = s_effect_mesh_translucent->CleanUp();
		if (result)
			s_effect_mesh_translucent = nullptr;
		else
		{
			EAE6320_ASSERTF(false, "Effect cleanup failed");
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
			EAE6320_ASSERTF(false, "Sprite cleanup failed");
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
			EAE6320_ASSERTF(false, "Sprite cleanup failed");
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
			EAE6320_ASSERTF(false, "Sprite cleanup failed");
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
			EAE6320_ASSERTF(false, "Sprite cleanup failed");
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
			EAE6320_ASSERTF(false, "Sprite cleanup failed");
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
			EAE6320_ASSERTF(false, "Sprite cleanup failed");
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
			EAE6320_ASSERTF(false, "Texture cleanup failed");
			goto OnExit;
		}
	}

	if (pokeballTexture.IsValid())
	{
		if (!(result = eae6320::Graphics::cTexture::s_manager.Release(pokeballTexture)))
		{
			EAE6320_ASSERTF(false, "Texture cleanup failed");
			goto OnExit;
		}
	}

	if (electroballTexture.IsValid())
	{
		if (!(result = eae6320::Graphics::cTexture::s_manager.Release(electroballTexture)))
		{
			EAE6320_ASSERTF(false, "Texture cleanup failed");
			goto OnExit;
		}
	}

	if (flowerShibeTexture.IsValid())
	{
		if (!(result = eae6320::Graphics::cTexture::s_manager.Release(flowerShibeTexture)))
		{
			EAE6320_ASSERTF(false, "Texture cleanup failed");
			goto OnExit;
		}
	}

	if (evilShibeTexture.IsValid())
	{
		if (!(result = eae6320::Graphics::cTexture::s_manager.Release(evilShibeTexture)))
		{
			EAE6320_ASSERTF(false, "Texture cleanup failed");
			goto OnExit;
		}
	}

	if (AKMTexture.IsValid())
	{
		if (!(result = eae6320::Graphics::cTexture::s_manager.Release(AKMTexture)))
		{
			EAE6320_ASSERTF(false, "Texture cleanup failed");
			goto OnExit;
		}
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUpMesh()
{
	cResult result = Results::Success;

	if (AKMMesh.IsValid())
	{
		if (!(result = eae6320::Graphics::Mesh::s_manager.Release(AKMMesh)))
		{
			EAE6320_ASSERTF(false, "Mesh cleanup failed");
			goto OnExit;
		}
	}

	if (planeMesh.IsValid())
	{
		if (!(result = eae6320::Graphics::Mesh::s_manager.Release(planeMesh)))
		{
			EAE6320_ASSERTF(false, "Mesh cleanup failed");
			goto OnExit;
		}
	}

	if (sphereMesh.IsValid())
	{
		if (!(result = eae6320::Graphics::Mesh::s_manager.Release(sphereMesh)))
		{
			EAE6320_ASSERTF(false, "Mesh cleanup failed");
			goto OnExit;
		}
	}

OnExit:
	return result;
}

void eae6320::cExampleGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	// Submit time data to constant buffer
	eae6320::Graphics::SubmitElapsedTime(i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);

	// Submit Color data
	eae6320::Graphics::SubmitColorToBeRendered(eae6320::Graphics::Colors::Magenta);
	
	// Submit Effect Mesh pair data with prediction if needed
	eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRenderedUsingPredictionIfNeeded(s_render_staticPlane, i_elapsedSecondCount_sinceLastSimulationUpdate, false, false);
	eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRenderedUsingPredictionIfNeeded(s_render_movableAKM, i_elapsedSecondCount_sinceLastSimulationUpdate, true, false);

	// Submit Effect Mesh pair data with translucent effect and prediction if needed
	eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRenderedUsingPredictionIfNeeded(s_render_staticSphere1, i_elapsedSecondCount_sinceLastSimulationUpdate, false, true);
	eae6320::Graphics::SubmitEffectMeshPairWithPositionToBeRenderedUsingPredictionIfNeeded(s_render_staticSphere2, i_elapsedSecondCount_sinceLastSimulationUpdate, false, true);

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
