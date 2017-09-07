/*
These are code snippets that represent sprites that are separated from Graphics source file
*/

#ifndef EAE6320_SPRITE_H
#define EAE6320_SPRITE_H

// Include Files
//==============

#include "sContext.h"
#include "VertexFormats.h"

#if defined ( EAE6320_PLATFORM_D3D )
	#include <Engine/Graphics/Direct3D/Includes.h>
#elif defined ( EAE6320_PLATFORM_GL )
	#include <Engine/Graphics/OpenGL/Includes.h>
#endif

namespace eae6320
{
	class Sprite
	{
	public:

		Sprite();
		~Sprite();

#if defined ( EAE6320_PLATFORM_D3D )
		// Geometry Data
		//--------------

		// A vertex buffer holds the data for each vertex
		ID3D11Buffer* s_vertexBuffer = nullptr;
		// D3D has an "input layout" object that associates the layout of the vertex format struct
		// with the input from a vertex shader
		ID3D11InputLayout* s_vertexInputLayout = nullptr;
		ID3D11DeviceContext* direct3dImmediateContext;// = Graphics::sContext::g_context.direct3dImmediateContext;
#elif defined ( EAE6320_PLATFORM_GL )
		// Geometry Data
		//--------------

		// A vertex buffer holds the data for each vertex
		GLuint s_vertexBufferId = 0;
		// A vertex array encapsulates the vertex data as well as the vertex input layout
		GLuint s_vertexArrayId = 0;
#endif

		cResult InitializeGeometry();
		void CleanUpGeometry(eae6320::cResult result);
		void DrawGeometry();
	};
}

#endif // !EAE6320_SPRITE_H
