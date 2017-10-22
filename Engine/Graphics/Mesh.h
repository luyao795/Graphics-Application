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

#if defined ( EAE6320_PLATFORM_D3D )
#include <Engine/Graphics/Direct3D/Includes.h>
#elif defined ( EAE6320_PLATFORM_GL )
#include <Engine/Graphics/OpenGL/Includes.h>
#endif

namespace eae6320
{
	namespace Graphics
	{
		class Mesh
		{
		public:

			static cResult Load(eae6320::Graphics::VertexFormats::sMesh meshData[], uint16_t indexData[], Mesh *& o_mesh);

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Mesh)

			void DrawMesh();

			cResult CleanUp();

		private:

			Mesh();
			~Mesh();

			// (tr_X, tr_Y) is the coordinate of top right point, sideH is length for horizontal side,
			// sideV is length for vertical side
			cResult InitializeMesh(eae6320::Graphics::VertexFormats::sMesh meshData[], uint16_t indexData[]);

			cResult CleanUpMesh();

#if defined ( EAE6320_PLATFORM_D3D )
			// Geometry Data
			//--------------

			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* s_vertexBuffer = nullptr;
			// An index buffer holds the data for indices
			ID3D11Buffer* s_indexBuffer = nullptr;
			// D3D has an "input layout" object that associates the layout of the vertex format struct
			// with the input from a vertex shader
			ID3D11InputLayout* s_vertexInputLayout = nullptr;
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

			unsigned int s_indexCount;

			EAE6320_ASSETS_DECLAREREFERENCECOUNT()

		};
	}
}

#endif // !EAE6320_MESH_H
