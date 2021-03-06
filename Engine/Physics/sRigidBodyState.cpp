// Include Files
//==============

#include "sRigidBodyState.h"

// Interface
//==========

void eae6320::Physics::sRigidBodyState::Update( const float i_secondCountToIntegrate )
{
	// Update position
	{
		position += velocity * i_secondCountToIntegrate;
	}
	// Update velocity
	{
		velocity += acceleration * i_secondCountToIntegrate;
	}
	// Update orientation
	{
		const auto rotation = Math::cQuaternion( angularSpeed * i_secondCountToIntegrate, angularVelocity_axis_local );
		orientation = orientation * rotation;
		orientation.Normalize();
	}
}

eae6320::Math::sVector eae6320::Physics::sRigidBodyState::IncrementPredictionOntoMovement( const float i_secondCountToExtrapolate )
{
	velocity += (acceleration * i_secondCountToExtrapolate);
	position += (velocity * i_secondCountToExtrapolate);
	return position;
}

eae6320::Math::sVector eae6320::Physics::sRigidBodyState::DecrementPredictionOntoMovement( const float i_secondCountToExtrapolate )
{
	position -= (velocity * i_secondCountToExtrapolate);
	velocity -= (acceleration * i_secondCountToExtrapolate);
	return position;
}

eae6320::Math::cQuaternion eae6320::Physics::sRigidBodyState::IncrementPredictionOntoRotation( const float i_secondCountToExtrapolate )
{
	const auto rotation = Math::cQuaternion( angularSpeed * i_secondCountToExtrapolate, angularVelocity_axis_local );
	orientation = orientation * rotation;
	orientation = orientation.GetNormalized();
	return orientation;
}

eae6320::Math::cQuaternion eae6320::Physics::sRigidBodyState::DecrementPredictionOntoRotation( const float i_secondCountToExtrapolate )
{
	const auto rotation = Math::cQuaternion( angularSpeed * i_secondCountToExtrapolate, angularVelocity_axis_local );
	orientation = orientation * rotation.GetInverse();
	orientation = orientation.GetNormalized();
	return orientation;
}
