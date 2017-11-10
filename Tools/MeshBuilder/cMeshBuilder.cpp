// Include Files
//==============

#include "cMeshBuilder.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Asserts/Asserts.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>&)
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Platform::CopyFileA(m_path_source, m_path_target, false, true)))
	{
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to copy file to destination");
	}

	return result;
}