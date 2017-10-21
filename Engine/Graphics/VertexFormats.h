/*
	A vertex format determines the layout of the geometric data
	that the CPU will send to the GPU
*/

#ifndef EAE6320_GRAPHICS_VERTEXFORMATS_H
#define EAE6320_GRAPHICS_VERTEXFORMATS_H

// Include Files
//==============

#include "Configuration.h"

// Vertex Formats
//===============

namespace eae6320
{
	namespace Graphics
	{
		namespace VertexFormats
		{
			struct sGeometry
			{
				// POSITION
				// 2 floats == 8 bytes
				// Offset = 0
				float x, y;
			};

			struct sSprite
			{
				// POSITION
				// 2 floats == 8 bytes
				// Offset = 0
				//====================
				// TEXCOORD0
				// 2 floats == 8 bytes
				// Offset = 8
				float x, y, u, v;
			};

			struct sMesh
			{
				// POSITION
				// 2 floats == 8 bytes
				// Offset = 0
				//======================
				// COLOR0
				// 4 uint8_ts == 4 bytes
				// Offset = 8
				float x, y;
				uint8_t r, g, b, a;
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_VERTEXFORMATS_H
