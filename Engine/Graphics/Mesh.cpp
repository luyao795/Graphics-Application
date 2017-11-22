// Include Files
//==============

#include "Mesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Tools/MeshBuilder/cMeshBuilder.h>
#include <new> // This library is needed for std::nothrow
#include <iostream>

// Static Data Initialization
//===========================

eae6320::Assets::cManager<eae6320::Graphics::Mesh> eae6320::Graphics::Mesh::s_manager;

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::Graphics::Mesh::Mesh()
{

}

eae6320::Graphics::Mesh::~Mesh()
{
	CleanUp();
}

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::Mesh::Load(const char * i_meshFileName, Mesh *& o_mesh)
{
	// Input array data should always be counterclockwise (CCW)
	// (We could make it either always clockwise or counterclockwise)

	cResult result = Results::Success;

	eae6320::Platform::sDataFromFile dataFromFile;
	Mesh * mesh = nullptr;

	// Automate the file path since compiled files will have to go into this folder
	char completeFilePath[MAX_MESH_PATH_LENGTH] = "data/Meshes/";
	strcat(completeFilePath, i_meshFileName);

	mesh = new (std::nothrow) Mesh();

	// Load the binary data
	std::string errorMessage;
	if (!(result = eae6320::Platform::LoadBinaryFile(completeFilePath, dataFromFile, &errorMessage)))
	{
		EAE6320_ASSERTF(false, errorMessage.c_str());
		Logging::OutputError("Failed to load mesh data from file %s: %s", completeFilePath, errorMessage.c_str());
		goto OnExit;
	}

	// Get the start of the block and the end of the block
	auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
	const auto finalOffset = currentOffset + dataFromFile.size;

	// Use current pointer of data and get number of vertices from data chunk
	uint16_t * p_vertexCount = reinterpret_cast<uint16_t *>(currentOffset);
	mesh->s_vertexCount = *p_vertexCount;

	// Increment current pointer of data and get number of indices from data chunk
	currentOffset += sizeof(mesh->s_vertexCount);
	uint16_t * p_indexCount = reinterpret_cast<uint16_t *>(currentOffset);
	mesh->s_indexCount = *p_indexCount;

	// Increment current pointer of data and get vertex data pointer from data chunk
	currentOffset += sizeof(mesh->s_indexCount);
	eae6320::Graphics::VertexFormats::sMesh * p_vertexData = reinterpret_cast<eae6320::Graphics::VertexFormats::sMesh *>(currentOffset);
	mesh->s_vertexData = p_vertexData;

	// Increment current pointer of data and get index data pointer from data chunk
	currentOffset += sizeof(eae6320::Graphics::VertexFormats::sMesh) * mesh->s_vertexCount;
	uint16_t * p_indexData = reinterpret_cast<uint16_t *>(currentOffset);
	mesh->s_indexData = p_indexData;

	// The size of the index array should always be a multiple of 3
	constexpr unsigned int vertexPerTriangle = 3;
	EAE6320_ASSERTF(mesh->s_indexCount % vertexPerTriangle == 0, "Invalid array size for indices, it has to be a multiple of 3");

	// Allocate a new Mesh
	{
		if (!mesh)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh");
			Logging::OutputError("Failed to allocate memory for the mesh");
			goto OnExit;
		}
	}

	if (!(result = mesh->InitializeMesh(mesh->s_vertexData, mesh->s_indexData)))
	{
		EAE6320_ASSERTF(false, "Initialization of new mesh failed");
		goto OnExit;
	}

	// Free data chunk from binary file after extracting data from it
	dataFromFile.Free();

OnExit:

	if (result)
	{
		EAE6320_ASSERT(mesh);
		o_mesh = mesh;
	}
	else
	{
		if (mesh)
		{
			mesh->DecrementReferenceCount();
			mesh = nullptr;
		}
		o_mesh = nullptr;
	}
	return result;
}

eae6320::cResult eae6320::Graphics::Mesh::CleanUp()
{
	cResult result = Results::Success;
	if (!(result = CleanUpMesh()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up mesh");
		Logging::OutputError("Failed to clean up mesh");
	}
	return result;
}