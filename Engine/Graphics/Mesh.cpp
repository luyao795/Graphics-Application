// Include Files
//==============

#include "Mesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

namespace eae6320
{
	namespace Graphics
	{
		Mesh::Mesh()
		{

		}

		Mesh::~Mesh()
		{

		}

		cResult Mesh::Load(float tr_X, float tr_Y, float sideH, float sideV, Mesh *& o_mesh)
		{
			cResult result = Results::Success;
			Mesh* mesh = nullptr;

			mesh = new (std::nothrow) Mesh();

			// Allocate a new Mesh
			{
				if (!mesh)
				{
					result = Results::OutOfMemory;
					EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh");
					Logging::OutputError("Failed to allocate memory for the mesh");
					goto OnExit;
				}
			}

			if (!(result = mesh->InitializeMesh(tr_X, tr_Y, sideH, sideV)))
			{
				EAE6320_ASSERTF(false, "Initialization of new mesh failed");
				goto OnExit;
			}

		OnExit:

			if (result)
			{
				EAE6320_ASSERT(mesh);
				o_mesh = mesh;
			}
			else
			{
				if (mesh)
				{
					mesh->DecrementReferenceCount();
					mesh = nullptr;
				}
				o_mesh = nullptr;
			}
			return result;
		}

		eae6320::cResult Mesh::CleanUp()
		{
			cResult result = Results::Success;
			if (result = CleanUpMesh())
				this->DecrementReferenceCount();
			else
			{
				EAE6320_ASSERTF(false, "Failed to clean up mesh");
				Logging::OutputError("Failed to clean up mesh");
			}
			return result;
		}
	}
}