/*
Direct3D specific code for Mesh
*/

// Include Files
//==============

#include "../sContext.h"
#include "../VertexFormats.h"
#include "../Mesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Logging/Logging.h>

eae6320::cResult eae6320::Graphics::Mesh::InitializeMesh(std::vector<eae6320::Graphics::VertexFormats::sMesh> i_vertexData, std::vector<uint16_t> i_indexData)
{
	auto result = eae6320::Results::Success;

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;

	// Initialize vertex format
	{
		// Load the compiled binary vertex shader for the input layout
		eae6320::Platform::sDataFromFile vertexShaderDataFromFile;
		std::string errorMessage;
		if (result = eae6320::Platform::LoadBinaryFile("data/Shaders/Vertex/vertexInputLayout_mesh.binshd", vertexShaderDataFromFile, &errorMessage))
		{
			// Create the vertex layout

			// These elements must match the VertexFormats::sMesh layout struct exactly.
			// They instruct Direct3D how to match the binary data in the vertex buffer
			// to the input elements in a vertex shader
			// (by using so-called "semantic" names so that, for example,
			// "POSITION" here matches with "POSITION" in shader code).
			// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
			constexpr unsigned int vertexElementCount = 2;
			D3D11_INPUT_ELEMENT_DESC layoutDescription[vertexElementCount] = {};
			{
				// Slot 0

				// POSITION
				// 3 floats == 12 bytes
				// Offset = 0
				{
					auto& positionElement = layoutDescription[0];

					positionElement.SemanticName = "POSITION";
					positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
					positionElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					positionElement.InputSlot = 0;
					positionElement.AlignedByteOffset = offsetof(eae6320::Graphics::VertexFormats::sMesh, x);
					positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
				}
			}
			{
				// Slot 1

				// COLOR0
				// 4 uint8_ts = 4 bytes
				// Offset = 12
				{
					auto& texcoordElement = layoutDescription[1];

					texcoordElement.SemanticName = "COLOR";
					texcoordElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
					texcoordElement.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					texcoordElement.InputSlot = 0;
					texcoordElement.AlignedByteOffset = offsetof(eae6320::Graphics::VertexFormats::sMesh, r);
					texcoordElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					texcoordElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
				}
			}

			const auto d3dResult = direct3dDevice->CreateInputLayout(layoutDescription, vertexElementCount,
				vertexShaderDataFromFile.data, vertexShaderDataFromFile.size, &s_vertexInputLayout);
			if (FAILED(d3dResult))
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, "Mesh vertex input layout creation failed (HRESULT %#010x)", d3dResult);
				eae6320::Logging::OutputError("Direct3D failed to create the mesh vertex input layout (HRESULT %#010x)", d3dResult);
			}

			vertexShaderDataFromFile.Free();
		}
		else
		{
			EAE6320_ASSERTF(false, errorMessage.c_str());
			eae6320::Logging::OutputError("The mesh vertex input layout shader couldn't be loaded: %s", errorMessage.c_str());
			goto OnExit;
		}
	}
	// Vertex Buffer
	{
		const auto vertexCount = i_vertexData.size();

		eae6320::Graphics::VertexFormats::sMesh* localMeshData = new eae6320::Graphics::VertexFormats::sMesh[vertexCount];
		{
			for (size_t i = 0; i < vertexCount; i++)
			{
				localMeshData[i] = i_vertexData[i];
			}
		}

		D3D11_BUFFER_DESC VertexBufferDescription{};
		{
			const auto bufferSize = vertexCount * sizeof(eae6320::Graphics::VertexFormats::sMesh);
			EAE6320_ASSERT(bufferSize < (uint64_t(1u) << (sizeof(VertexBufferDescription.ByteWidth) * 8)));
			VertexBufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
			VertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE; // In our class the buffer will never change after it's been created
			VertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			VertexBufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			VertexBufferDescription.MiscFlags = 0;
			VertexBufferDescription.StructureByteStride = 0;	// Not used
		}
		D3D11_SUBRESOURCE_DATA InitialVertexData{};
		{
			InitialVertexData.pSysMem = localMeshData;
			// (The other data members are ignored for non-texture buffers)
		}

		const auto d3dResultForVertexBuffer = direct3dDevice->CreateBuffer(&VertexBufferDescription, &InitialVertexData, &s_vertexBuffer);
		if (FAILED(d3dResultForVertexBuffer))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "Mesh vertex buffer creation failed (HRESULT %#010x)", d3dResultForVertexBuffer);
			eae6320::Logging::OutputError("Direct3D failed to create a mesh vertex buffer (HRESULT %#010x)", d3dResultForVertexBuffer);
			goto OnExit;
		}

		delete[] localMeshData;
	}

	// Index Buffer
	{
		const auto indexArraySize = i_indexData.size();
		uint16_t* d3dIndexData = new uint16_t[indexArraySize];
		for (size_t i = 0; i < indexArraySize; i += 3)
		{
			// Direct3D Rendering Order: Clockwise (CW)
			// Since the input is clockwise (CW), thus example input
			// like ABC should be assigned here with the order ABC
			d3dIndexData[i] = i_indexData[i];
			d3dIndexData[i + 1] = i_indexData[i + 1];
			d3dIndexData[i + 2] = i_indexData[i + 2];
		}

		D3D11_BUFFER_DESC IndexBufferDescription{};
		{
			const auto bufferSize = indexArraySize * sizeof(uint16_t);
			EAE6320_ASSERT(bufferSize < (uint64_t(1u) << (sizeof(IndexBufferDescription.ByteWidth) * 8)));
			IndexBufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
			IndexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE; // In our class the buffer will never change after it's been created
			IndexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			IndexBufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			IndexBufferDescription.MiscFlags = 0;
			IndexBufferDescription.StructureByteStride = 0; // Not used
		}
		D3D11_SUBRESOURCE_DATA InitialIndexData{};
		{
			InitialIndexData.pSysMem = d3dIndexData;
			// (The other data members are ignored for non-texture buffers)
		}

		const auto d3dResultForIndexBuffer = direct3dDevice->CreateBuffer(&IndexBufferDescription, &InitialIndexData, &s_indexBuffer);
		if (FAILED(d3dResultForIndexBuffer))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "Mesh index buffer creation failed (HRESULT %#010x)", d3dResultForIndexBuffer);
			eae6320::Logging::OutputError("Direct3D failed to create a mesh index buffer (HRESULT %#010x)", d3dResultForIndexBuffer);
			goto OnExit;
		}

		delete[] d3dIndexData;
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::Mesh::CleanUpMesh()
{
	cResult result = Results::Success;
	if (s_vertexBuffer)
	{
		s_vertexBuffer->Release();
		s_vertexBuffer = nullptr;
	}
	if (s_vertexInputLayout)
	{
		s_vertexInputLayout->Release();
		s_vertexInputLayout = nullptr;
	}
	if (s_indexBuffer)
	{
		s_indexBuffer->Release();
		s_indexBuffer = nullptr;
	}
	return result;
}

void eae6320::Graphics::Mesh::DrawMesh()
{
	// Draw the mesh
	{
		auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;
		EAE6320_ASSERT(direct3dImmediateContext);
		// Bind a specific vertex buffer to the device as a data source
		{
			EAE6320_ASSERT(s_vertexBuffer);
			constexpr unsigned int startingSlot = 0;
			constexpr unsigned int vertexBufferCount = 1;
			// The "stride" defines how large a single vertex is in the stream of data
			constexpr unsigned int bufferStride = sizeof(Graphics::VertexFormats::sMesh);
			// It's possible to start streaming data in the middle of a vertex buffer
			constexpr unsigned int bufferOffset = 0;
			direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &s_vertexBuffer, &bufferStride, &bufferOffset);
		}
		// Bind the index buffer to the device
		{
			EAE6320_ASSERT(s_indexBuffer);
			// The indices start at the beginning of the buffer
			const unsigned int offset = 0;
			direct3dImmediateContext->IASetIndexBuffer(s_indexBuffer, DXGI_FORMAT_R16_UINT, offset);
		}
		// Specify what kind of data the vertex buffer holds
		{
			// Set the layout (which defines how to interpret a single vertex)
			{
				EAE6320_ASSERT(s_vertexInputLayout);
				direct3dImmediateContext->IASetInputLayout(s_vertexInputLayout);
			}
			// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
			// the vertex buffer was defined as a triangle list
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		// Render triangles from the currently-bound vertex buffer
		{
			// It's possible to start rendering primitives in the middle of the stream
			const unsigned int indexOfFirstIndexToUse = 0;
			const unsigned int offsetToAddToEachIndex = 0;
			direct3dImmediateContext->DrawIndexed(static_cast<unsigned int>(s_indexCount), indexOfFirstIndexToUse, offsetToAddToEachIndex);
		}
	}
}