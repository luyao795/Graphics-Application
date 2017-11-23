/*
These are code snippets that represent sprites that are separated from Graphics source file
*/

#ifndef EAE6320_SPRITE_H
#define EAE6320_SPRITE_H

// Include Files
//==============

#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Results/Results.h>

#if defined ( EAE6320_PLATFORM_D3D )
	#include <Engine/Graphics/Direct3D/Includes.h>
#elif defined ( EAE6320_PLATFORM_GL )
	#include <Engine/Graphics/OpenGL/Includes.h>
#endif

namespace eae6320
{
	namespace Graphics
	{
		class Sprite
		{
		public:

			// Initialization / Clean Up
			//--------------------------

			static cResult Load(float tr_X, float tr_Y, float sideH, float sideV, Sprite *& o_sprite);
			cResult CleanUp();

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Sprite)

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

			// Render
			//-------

			void DrawGeometry();

		private:

			Sprite();
			~Sprite();

			// Initialization / Clean Up
			//--------------------------

			// (tr_X, tr_Y) is the coordinate of top right point, sideH is length for horizontal side,
			// sideV is length for vertical side
			cResult InitializeGeometry(float tr_X, float tr_Y, float sideH, float sideV);
			cResult CleanUpGeometry();

#if defined ( EAE6320_PLATFORM_D3D )
			// Geometry Data
			//--------------

			// A vertex buffer holds the data for each vertex
			ID3D11Buffer * s_vertexBuffer = nullptr;
			// D3D has an "input layout" object that associates the layout of the vertex format struct
			// with the input from a vertex shader
			ID3D11InputLayout * s_vertexInputLayout = nullptr;
#elif defined ( EAE6320_PLATFORM_GL )
			// Geometry Data
			//--------------

			// A vertex buffer holds the data for each vertex
			GLuint s_vertexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint s_vertexArrayId = 0;
#endif

			// Reference counting
			//===================

			EAE6320_ASSETS_DECLAREREFERENCECOUNT()
		};
	}
}

#endif // !EAE6320_SPRITE_H

