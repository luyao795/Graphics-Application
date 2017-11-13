/*
These are code snippets that represent meshes that are separated from Graphics source file
*/

#ifndef EAE6320_MESH_H
#define EAE6320_MESH_H

// Include Files
//==============

#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Results/Results.h>
#include <Engine/Graphics/VertexFormats.h>
#include <External/Lua/Includes.h>
#include <Engine/Assets/cHandle.h>
#include <Engine/Assets/cManager.h>

#include <vector>

#if defined ( EAE6320_PLATFORM_D3D )
#include <Engine/Graphics/Direct3D/Includes.h>
#elif defined ( EAE6320_PLATFORM_GL )
#include <Engine/Graphics/OpenGL/Includes.h>
#endif

#define MAX_MESH_PATH_LENGTH 100

namespace eae6320
{
	namespace Graphics
	{
		class Mesh
		{
		public:

			// Initialization / Clean Up
			//--------------------------

			static cResult Load(const char * i_meshFileName, Mesh *& o_mesh);
			cResult CleanUp();

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Mesh)

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

			// Render
			//-------

			void DrawMesh();

			// Access
			//-------

			using Handle = Assets::cHandle<Mesh>;
			static Assets::cManager<Mesh> s_manager;

		private:

			Mesh();
			~Mesh();

			// Initialization / Clean Up
			//--------------------------

			// vertexData is the array for all vertices, indexData is the array for index information for rendering the mesh
			cResult InitializeMesh(std::vector<eae6320::Graphics::VertexFormats::sMesh> vertexData, std::vector<uint16_t> indexData);
			cResult CleanUpMesh();

			// Lua data operation
			//-------------------

			cResult LoadTableValues(lua_State& io_luaState);

			cResult LoadTableValues_vertices(lua_State& io_luaState);
			cResult LoadTableValues_vertices_values(lua_State& io_luaState);

			cResult LoadTableValues_indices(lua_State& io_luaState);
			cResult LoadTableValues_indices_values(lua_State& io_luaState);

			cResult LoadAsset(const char* const i_path);

#if defined ( EAE6320_PLATFORM_D3D )
			// Geometry Data
			//--------------

			// A vertex buffer holds the data for each vertex
			ID3D11Buffer * s_vertexBuffer = nullptr;
			// An index buffer holds the data for indices
			ID3D11Buffer * s_indexBuffer = nullptr;
			// D3D has an "input layout" object that associates the layout of the vertex format struct
			// with the input from a vertex shader
			ID3D11InputLayout * s_vertexInputLayout = nullptr;
#elif defined ( EAE6320_PLATFORM_GL )
			// Geometry Data
			//--------------

			// A vertex buffer holds the data for each vertex
			GLuint s_vertexBufferId = 0;
			// An index buffer holds the data for indices
			GLuint s_indexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint s_vertexArrayId = 0;
#endif

			// Member data
			//============

			size_t s_indexCount;

			std::vector<eae6320::Graphics::VertexFormats::sMesh> s_vertexData;

			std::vector<uint16_t> s_indexData;

			// Reference counting
			//===================

			EAE6320_ASSETS_DECLAREREFERENCECOUNT()

		};
	}
}

#endif // !EAE6320_MESH_H
