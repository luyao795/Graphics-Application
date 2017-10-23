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

		cResult Mesh::Load(std::vector<eae6320::Graphics::VertexFormats::sMesh> meshData, std::vector<uint16_t> indexData, Mesh *& o_mesh)
		{
			// Input array data should always be clockwise (CW)
			// (We could make it either always clockwise or counterclockwise)

			cResult result = Results::Success;
			Mesh* mesh = nullptr;

			mesh = new (std::nothrow) Mesh();

			// The size of the index array should always be a multiple of 3
			const auto indexArraySize = indexData.size();
			constexpr unsigned int vertexPerTriangle = 3;
			EAE6320_ASSERTF(indexArraySize % vertexPerTriangle == 0, "Invalid array size for indices, it has to be a multiple of 3");
			mesh->s_indexCount = indexArraySize;

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

			if (!(result = mesh->InitializeMesh(meshData, indexData)))
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