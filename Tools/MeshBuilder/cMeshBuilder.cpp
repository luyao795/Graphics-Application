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