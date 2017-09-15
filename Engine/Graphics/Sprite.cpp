// Include Files
//==============

#include "Sprite.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

namespace eae6320
{
	Sprite::Sprite()
	{

	}

	Sprite::~Sprite()
	{
		
	}

	cResult Sprite::Load(float tr_X, float tr_Y, float sideH, float sideV, Sprite *& o_sprite)
	{
		cResult result = Results::Success;
		Sprite* sprite = nullptr;
			
		sprite = new (std::nothrow) Sprite();

		// Allocate a new Sprite
		{
			if (!sprite)
			{
				result = Results::OutOfMemory;
				EAE6320_ASSERTF(false, "Couldn't allocate memory for the sprite.");
				Logging::OutputError("Failed to allocate memory for the sprite.");
				goto OnExit;
			}
		}

		if (!(result = sprite->InitializeGeometry(tr_X, tr_Y, sideH, sideV)))
		{
			EAE6320_ASSERTF(false, "Initialization of new sprite failed.");
			goto OnExit;
		}

	OnExit:

		if (result)
		{
			EAE6320_ASSERT(sprite);
			o_sprite = sprite;
		}
		else
		{
			if (sprite)
			{
				sprite->DecrementReferenceCount();
				sprite = nullptr;
			}
			o_sprite = nullptr;
		}
		return result;
	}

	eae6320::cResult Sprite::CleanUp()
	{
		cResult result = Results::Success;
		if(result = CleanUpGeometry())
			this->DecrementReferenceCount();
		else
		{
			EAE6320_ASSERTF(false, "Failed to clean up geometry.");
			Logging::OutputError("Failed to clean up geometry.");
		}
		return result;
	}
}