// Include Files
//==============

#include "Mesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <new> // This library is needed for std::nothrow
#include <iostream>

namespace eae6320
{
	namespace Graphics
	{
		Mesh::Mesh()
		{

		}

		Mesh::~Mesh()
		{

		}

		cResult Mesh::Load(std::vector<eae6320::Graphics::VertexFormats::sMesh> vertexData, std::vector<uint16_t> indexData, Mesh *& o_mesh)
		{
			// Input array data should always be clockwise (CW)
			// (We could make it either always clockwise or counterclockwise)

			cResult result = Results::Success;
			Mesh* mesh = nullptr;

			mesh = new (std::nothrow) Mesh();

			// The size of the index array should always be a multiple of 3
			const auto indexArraySize = indexData.size();
			constexpr unsigned int vertexPerTriangle = 3;
			EAE6320_ASSERTF(indexArraySize % vertexPerTriangle == 0, "Invalid array size for indices, it has to be a multiple of 3");
			mesh->s_indexCount = indexArraySize;

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

			if (!(result = mesh->InitializeMesh(vertexData, indexData)))
			{
				EAE6320_ASSERTF(false, "Initialization of new mesh failed");
				goto OnExit;
			}

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

		eae6320::cResult Mesh::CleanUp()
		{
			cResult result = Results::Success;
			if (result = CleanUpMesh())
				this->DecrementReferenceCount();
			else
			{
				EAE6320_ASSERTF(false, "Failed to clean up mesh");
				Logging::OutputError("Failed to clean up mesh");
			}
			return result;
		}
	}
}

// Helper Function Definitions
//============================

eae6320::cResult eae6320::Graphics::Mesh::LoadTableValues(lua_State & io_luaState)
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::Mesh::LoadTableValues_vertices(io_luaState)))
	{
		return result;
	}

	if (!(result = eae6320::Graphics::Mesh::LoadTableValues_indices(io_luaState)))
	{
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::Mesh::LoadTableValues_vertices(lua_State & io_luaState)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "vertexData" table will be at -1:
	constexpr auto* const key = "vertexData";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	// It can be hard to remember where the stack is at
	// and how many values to pop.
	// One strategy I would suggest is to always call a new function
	// When you are at a new level:
	// Right now we know that we have an original table at -2,
	// and a new one at -1,
	// and so we _know_ that we always have to pop at least _one_
	// value before leaving this function
	// (to make the original table be back to index -1).
	// If we don't do any further stack manipulation in this function
	// then it becomes easy to remember how many values to pop
	// because it will always be one.
	// This is the strategy I'll take in this example
	// (look at the "OnExit" label):
	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = eae6320::Graphics::Mesh::LoadTableValues_vertices_values(io_luaState)))
		{
			goto OnExit;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		goto OnExit;
	}

OnExit:

	// Pop the vertexData table
	lua_pop(&io_luaState, 1);

	return result;
}

eae6320::cResult eae6320::Graphics::Mesh::LoadTableValues_vertices_values(lua_State & io_luaState)
{
	// Right now the asset table is at -2
	// and the vertexData table is at -1.
	// NOTE, however, that it doesn't matter to me in this function
	// that the asset table is at -2.
	// Because I've carefully called a new function for every "stack level"
	// The only thing I care about is that the vertexData table that I care about
	// is at the top of the stack.
	// As long as I make sure that when I leave this function it is _still_
	// at -1 then it doesn't matter to me at all what is on the stack below it.

	auto result = eae6320::Results::Success;

	std::cout << "Iterating through vertex data:" << std::endl;
	const auto vertexCount = luaL_len(&io_luaState, -1);
	for (int i = 1; i <= vertexCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::Graphics::VertexFormats::sMesh currentVertex;
		auto key = "";
		auto value = 0.0;
		if (lua_istable(&io_luaState, -1))
		{
			// Position: x
			//============
			key = "x";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			// First, then, we will make sure that a value (_any_ value) existed for the key:
			if (lua_isnil(&io_luaState, -1))
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "No value for \"" << key << "\" was found in the asset table" << std::endl;
				// When using Lua in C/C++ it is important
				// to always return the stack to its original state.
				// Leaving objects on the stack is kind of like a Lua memory leak.
				// Remember that our value is currently at -1 and the table is at -2;
				// In order to restore it to the way it was when we entered this function
				// we need to pop the value off the stack:
				lua_pop(&io_luaState, 1);
				// Now the only thing on the stack is the asset table at -1,
				// and the calling function will deal with this
				// (regardless of whether this function succeeds or fails).
				return result;
			}

			// If we reach this code then we know that a value exists,
			// but we don't know if it's the right type or not.
			// One way we could find out in the current example is this:
			//	if ( lua_isstring( &io_luaState, -1 ) )
			// This would work (and is perfectly valid),
			// but it actually returns true if the value is a number
			// (because numbers are always convertible to strings).
			// If we really want to be strict, we can do the following instead:
			if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "The value for \"" << key << "\" must be a number "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
				return result;
			}

			value = lua_tonumber(&io_luaState, -1);
			currentVertex.x = static_cast<float>(value);
			lua_pop(&io_luaState, 1);

			// Position: y
			//============
			key = "y";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			// First, then, we will make sure that a value (_any_ value) existed for the key:
			if (lua_isnil(&io_luaState, -1))
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "No value for \"" << key << "\" was found in the asset table" << std::endl;
				// When using Lua in C/C++ it is important
				// to always return the stack to its original state.
				// Leaving objects on the stack is kind of like a Lua memory leak.
				// Remember that our value is currently at -1 and the table is at -2;
				// In order to restore it to the way it was when we entered this function
				// we need to pop the value off the stack:
				lua_pop(&io_luaState, 1);
				// Now the only thing on the stack is the asset table at -1,
				// and the calling function will deal with this
				// (regardless of whether this function succeeds or fails).
				return result;
			}

			// If we reach this code then we know that a value exists,
			// but we don't know if it's the right type or not.
			// One way we could find out in the current example is this:
			//	if ( lua_isstring( &io_luaState, -1 ) )
			// This would work (and is perfectly valid),
			// but it actually returns true if the value is a number
			// (because numbers are always convertible to strings).
			// If we really want to be strict, we can do the following instead:
			if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "The value for \"" << key << "\" must be a number "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
				return result;
			}

			value = lua_tonumber(&io_luaState, -1);
			currentVertex.y = static_cast<float>(value);
			lua_pop(&io_luaState, 1);

			// Position: z
			//============
			key = "z";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			// First, then, we will make sure that a value (_any_ value) existed for the key:
			if (lua_isnil(&io_luaState, -1))
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "No value for \"" << key << "\" was found in the asset table" << std::endl;
				// When using Lua in C/C++ it is important
				// to always return the stack to its original state.
				// Leaving objects on the stack is kind of like a Lua memory leak.
				// Remember that our value is currently at -1 and the table is at -2;
				// In order to restore it to the way it was when we entered this function
				// we need to pop the value off the stack:
				lua_pop(&io_luaState, 1);
				// Now the only thing on the stack is the asset table at -1,
				// and the calling function will deal with this
				// (regardless of whether this function succeeds or fails).
				return result;
			}

			// If we reach this code then we know that a value exists,
			// but we don't know if it's the right type or not.
			// One way we could find out in the current example is this:
			//	if ( lua_isstring( &io_luaState, -1 ) )
			// This would work (and is perfectly valid),
			// but it actually returns true if the value is a number
			// (because numbers are always convertible to strings).
			// If we really want to be strict, we can do the following instead:
			if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "The value for \"" << key << "\" must be a number "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
				return result;
			}

			value = lua_tonumber(&io_luaState, -1);
			currentVertex.z = static_cast<float>(value);
			lua_pop(&io_luaState, 1);

			// Color: r
			//=========
			key = "r";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			// First, then, we will make sure that a value (_any_ value) existed for the key:
			if (lua_isnil(&io_luaState, -1))
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "No value for \"" << key << "\" was found in the asset table" << std::endl;
				// When using Lua in C/C++ it is important
				// to always return the stack to its original state.
				// Leaving objects on the stack is kind of like a Lua memory leak.
				// Remember that our value is currently at -1 and the table is at -2;
				// In order to restore it to the way it was when we entered this function
				// we need to pop the value off the stack:
				lua_pop(&io_luaState, 1);
				// Now the only thing on the stack is the asset table at -1,
				// and the calling function will deal with this
				// (regardless of whether this function succeeds or fails).
				return result;
			}

			// If we reach this code then we know that a value exists,
			// but we don't know if it's the right type or not.
			// One way we could find out in the current example is this:
			//	if ( lua_isstring( &io_luaState, -1 ) )
			// This would work (and is perfectly valid),
			// but it actually returns true if the value is a number
			// (because numbers are always convertible to strings).
			// If we really want to be strict, we can do the following instead:
			if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "The value for \"" << key << "\" must be a number "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
				return result;
			}

			value = lua_tonumber(&io_luaState, -1);
			currentVertex.r = static_cast<uint8_t>(value * 255);
			lua_pop(&io_luaState, 1);

			// Color: g
			//=========
			key = "g";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			// First, then, we will make sure that a value (_any_ value) existed for the key:
			if (lua_isnil(&io_luaState, -1))
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "No value for \"" << key << "\" was found in the asset table" << std::endl;
				// When using Lua in C/C++ it is important
				// to always return the stack to its original state.
				// Leaving objects on the stack is kind of like a Lua memory leak.
				// Remember that our value is currently at -1 and the table is at -2;
				// In order to restore it to the way it was when we entered this function
				// we need to pop the value off the stack:
				lua_pop(&io_luaState, 1);
				// Now the only thing on the stack is the asset table at -1,
				// and the calling function will deal with this
				// (regardless of whether this function succeeds or fails).
				return result;
			}

			// If we reach this code then we know that a value exists,
			// but we don't know if it's the right type or not.
			// One way we could find out in the current example is this:
			//	if ( lua_isstring( &io_luaState, -1 ) )
			// This would work (and is perfectly valid),
			// but it actually returns true if the value is a number
			// (because numbers are always convertible to strings).
			// If we really want to be strict, we can do the following instead:
			if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "The value for \"" << key << "\" must be a number "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
				return result;
			}

			value = lua_tonumber(&io_luaState, -1);
			currentVertex.g = static_cast<uint8_t>(value * 255);
			lua_pop(&io_luaState, 1);

			// Color: b
			//=========
			key = "b";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			// First, then, we will make sure that a value (_any_ value) existed for the key:
			if (lua_isnil(&io_luaState, -1))
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "No value for \"" << key << "\" was found in the asset table" << std::endl;
				// When using Lua in C/C++ it is important
				// to always return the stack to its original state.
				// Leaving objects on the stack is kind of like a Lua memory leak.
				// Remember that our value is currently at -1 and the table is at -2;
				// In order to restore it to the way it was when we entered this function
				// we need to pop the value off the stack:
				lua_pop(&io_luaState, 1);
				// Now the only thing on the stack is the asset table at -1,
				// and the calling function will deal with this
				// (regardless of whether this function succeeds or fails).
				return result;
			}

			// If we reach this code then we know that a value exists,
			// but we don't know if it's the right type or not.
			// One way we could find out in the current example is this:
			//	if ( lua_isstring( &io_luaState, -1 ) )
			// This would work (and is perfectly valid),
			// but it actually returns true if the value is a number
			// (because numbers are always convertible to strings).
			// If we really want to be strict, we can do the following instead:
			if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "The value for \"" << key << "\" must be a number "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
				return result;
			}

			value = lua_tonumber(&io_luaState, -1);
			currentVertex.b = static_cast<uint8_t>(value * 255);
			lua_pop(&io_luaState, 1);

			// Color: a
			//=========
			key = "a";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			// First, then, we will make sure that a value (_any_ value) existed for the key:
			if (lua_isnil(&io_luaState, -1))
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "No value for \"" << key << "\" was found in the asset table" << std::endl;
				// When using Lua in C/C++ it is important
				// to always return the stack to its original state.
				// Leaving objects on the stack is kind of like a Lua memory leak.
				// Remember that our value is currently at -1 and the table is at -2;
				// In order to restore it to the way it was when we entered this function
				// we need to pop the value off the stack:
				lua_pop(&io_luaState, 1);
				// Now the only thing on the stack is the asset table at -1,
				// and the calling function will deal with this
				// (regardless of whether this function succeeds or fails).
				return result;
			}

			// If we reach this code then we know that a value exists,
			// but we don't know if it's the right type or not.
			// One way we could find out in the current example is this:
			//	if ( lua_isstring( &io_luaState, -1 ) )
			// This would work (and is perfectly valid),
			// but it actually returns true if the value is a number
			// (because numbers are always convertible to strings).
			// If we really want to be strict, we can do the following instead:
			if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "The value for \"" << key << "\" must be a number "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
				return result;
			}

			value = lua_tonumber(&io_luaState, -1);
			currentVertex.a = static_cast<uint8_t>(value * 255);
			lua_pop(&io_luaState, 1);
		}
		s_vertexData.push_back(currentVertex);
	}
	return result;
}

eae6320::cResult eae6320::Graphics::Mesh::LoadTableValues_indices(lua_State & io_luaState)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "parameters" table will be at -1:
	constexpr auto* const key = "indexData";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = eae6320::Graphics::Mesh::LoadTableValues_indices_values(io_luaState)))
		{
			goto OnExit;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		goto OnExit;
	}

OnExit:

	// Pop the indexData table
	lua_pop(&io_luaState, 1);

	return result;
}

eae6320::cResult eae6320::Graphics::Mesh::LoadTableValues_indices_values(lua_State & io_luaState)
{
	auto result = eae6320::Results::Success;

	std::cout << "Iterating through index data:" << std::endl;
	const auto triangleCount = luaL_len(&io_luaState, -1);
	for (int i = 1; i <= triangleCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		if (lua_istable(&io_luaState, -1))
		{
			const auto vertexCountPerTriangle = luaL_len(&io_luaState, -1);
			for (int j = 1; j <= vertexCountPerTriangle; ++j)
			{
				auto value = 0.0;

				lua_pushinteger(&io_luaState, j);
				lua_gettable(&io_luaState, -2);

				// First, then, we will make sure that a value (_any_ value) existed for the key == j:
				if (lua_isnil(&io_luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "No value for \"" << j << "\" was found in the asset table" << std::endl;
					// When using Lua in C/C++ it is important
					// to always return the stack to its original state.
					// Leaving objects on the stack is kind of like a Lua memory leak.
					// Remember that our value is currently at -1 and the table is at -2;
					// In order to restore it to the way it was when we entered this function
					// we need to pop the value off the stack:
					lua_pop(&io_luaState, 1);
					// Now the only thing on the stack is the asset table at -1,
					// and the calling function will deal with this
					// (regardless of whether this function succeeds or fails).
					return result;
				}

				// If we reach this code then we know that a value exists,
				// but we don't know if it's the right type or not.
				// One way we could find out in the current example is this:
				//	if ( lua_isstring( &io_luaState, -1 ) )
				// This would work (and is perfectly valid),
				// but it actually returns true if the value is a number
				// (because numbers are always convertible to strings).
				// If we really want to be strict, we can do the following instead:
				if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "The value for \"" << j << "\" must be a number "
						"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
					// Pop the value
					lua_pop(&io_luaState, 1);
					// (The asset table is now at -1)
					return result;
				}

				value = lua_tonumber(&io_luaState, -1);
				uint16_t finalValue = static_cast<uint16_t>(value);
				s_indexData.push_back(finalValue);
				lua_pop(&io_luaState, 1);
			}
		}
	}
	return result;
}

eae6320::cResult eae6320::Graphics::Mesh::LoadAsset(const char* const i_path)
{
	auto result = eae6320::Results::Success;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			std::cerr << "Failed to create a new Lua state" << std::endl;
			goto OnExit;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	result = eae6320::Graphics::Mesh::LoadTableValues(*luaState);

	// Pop the table
	lua_pop(luaState, 1);

OnExit:

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = nullptr;
	}

	return result;
}