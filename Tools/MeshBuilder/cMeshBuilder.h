/*
	This class builds hardware-ready meshes from source data
*/

#ifndef EAE6320_CMESHBUILDER_H
#define EAE6320_CMESHBUILDER_H

// Include Files
//==============

#include <External/Lua/Includes.h>
#include <Tools/AssetBuildLibrary/cbBuilder.h>
#include <Engine/Graphics/VertexFormats.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cMeshBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

			// Lua data operation
			//-------------------

			cResult LoadTableValues(lua_State& io_luaState);

			cResult LoadTableValues_vertices(lua_State& io_luaState);
			cResult LoadTableValues_vertices_values(lua_State& io_luaState);

			cResult LoadTableValues_indices(lua_State& io_luaState);
			cResult LoadTableValues_indices_values(lua_State& io_luaState);

			cResult LoadAsset(const char* const i_path);

		private:

			// Build
			//------

			virtual cResult Build(const std::vector<std::string>& i_arguments) override;

			std::vector<eae6320::Graphics::VertexFormats::sMesh> s_vertexData;

			std::vector<uint16_t> s_indexData;
		};
	}
}

#endif	// EAE6320_CMESHBUILDER_H
