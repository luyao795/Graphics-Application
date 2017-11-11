// Include Files
//==============

#include "Mesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <new> // This library is needed for std::nothrow
#include <External/Lua/Includes.h>
#include <iostream>

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

// Helper Function Definitions
//============================

eae6320::cResult eae6320::Graphics::Mesh::LoadTableValues(lua_State & io_luaState)
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::Mesh::LoadTableValues_vertices(io_luaState)))
	{
		return result;
	}

	if (!(result = eae6320::Graphics::Mesh::LoadTableValues_indices(io_luaState)))
	{
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::Mesh::LoadAsset(const char* const i_path)
{
	auto result = eae6320::Results::Success;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			std::cerr << "Failed to create a new Lua state" << std::endl;
			goto OnExit;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	result = eae6320::Graphics::Mesh::LoadTableValues(*luaState);

	// Pop the table
	lua_pop(luaState, 1);

OnExit:

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = nullptr;
	}

	return result;
}