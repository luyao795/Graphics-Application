/*
	This file defines the layout of the constant data
	that the CPU sends to the GPU

	These must exactly match the constant buffer definitions in shader programs.
*/

#ifndef EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H
#define EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H

// Include Files
//==============

#include "Configuration.h"

#include <Engine/Math/cMatrix_transformation.h> 

// Format Definitions
//===================

namespace eae6320
{
	namespace Graphics
	{
		namespace ConstantBufferFormats
		{
			struct sPerFrame
			{
				eae6320::Math::cMatrix_transformation g_transform_worldToCamera;
				eae6320::Math::cMatrix_transformation g_transform_cameraToProjected;

				float g_elapsedSecondCount_systemTime = 0.0f;
				float g_elapsedSecondCount_simulationTime = 0.0f;
				float padding[2];	// For float4 alignment
			};

			struct sPerMaterial
			{
				struct
				{
					float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
				} g_color;
			};

			struct sPerDrawCall
			{
				eae6320::Math::cMatrix_transformation g_transform_localToWorld;
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H
