// Include Files
//==============

#include "cMeshBuilder.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Results/Results.h>
#include <External/Lua/Includes.h>
#include <iostream>

// Helper Function Declarations
//=============================

namespace
{
	eae6320::cResult LoadTableValues(lua_State& io_luaState);

	eae6320::cResult LoadTableValues_vertices(lua_State& io_luaState);
	eae6320::cResult LoadTableValues_vertices_values(lua_State& io_luaState);

	eae6320::cResult LoadTableValues_indices(lua_State& io_luaState);
	eae6320::cResult LoadTableValues_indices_values(lua_State& io_luaState);

	eae6320::cResult LoadAsset(const char* const i_path);
}

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>&)
{
	auto result = eae6320::Results::Success;

	std::string * errorMessage;

	if (!(result = eae6320::Platform::CopyFileA(m_path_source, m_path_target, false, true, errorMessage)))
	{
		OutputErrorMessageWithFileInfo(m_path_source, errorMessage->c_str());
	}

	return result;
}

// Helper Function Definitions
//============================

eae6320::cResult LoadTableValues(lua_State & io_luaState)
{
	auto result = eae6320::Results::Success;

	if (!(result = LoadTableValues_vertices(io_luaState)))
	{
		return result;
	}

	if (!(result = LoadTableValues_indices(io_luaState)))
	{
		return result;
	}

	return result;
}

eae6320::cResult LoadAsset(const char* const i_path)
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
	result = LoadTableValues(*luaState);

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