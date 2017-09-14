/*
	The main() function is where the program starts execution
*/

// Include Files
//==============

#include <cstdlib>
#include <Engine/Platform/Platform.h>
#include <Engine/Results/Results.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <string>

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	auto result = eae6320::Results::Success;

	// Get the necessary directories
	std::string engineSourceContentDir, gameInstallDir, gameLicenseDir, licenseDir, outputDir;
	{
		std::string errorMessage;
		if ( !( result = eae6320::Platform::GetEnvironmentVariable( "EngineSourceContentDir", engineSourceContentDir, &errorMessage ) ) )
		{
			eae6320::Assets::OutputErrorMessageWithFileInfo( __FILE__, __LINE__, errorMessage.c_str() );
			goto OnExit;
		}
		if ( !( result = eae6320::Platform::GetEnvironmentVariable( "GameInstallDir", gameInstallDir, &errorMessage ) ) )
		{
			eae6320::Assets::OutputErrorMessageWithFileInfo( __FILE__, __LINE__, errorMessage.c_str() );
			goto OnExit;
		}
		if ( !( result = eae6320::Platform::GetEnvironmentVariable( "GameLicenseDir", gameLicenseDir, &errorMessage ) ) )
		{
			eae6320::Assets::OutputErrorMessageWithFileInfo( __FILE__, __LINE__, errorMessage.c_str() );
			goto OnExit;
		}
		if ( !( result = eae6320::Platform::GetEnvironmentVariable( "LicenseDir", licenseDir, &errorMessage ) ) )
		{
			eae6320::Assets::OutputErrorMessageWithFileInfo( __FILE__, __LINE__, errorMessage.c_str() );
			goto OnExit;
		}
		if ( !( result = eae6320::Platform::GetEnvironmentVariable( "OutputDir", outputDir, &errorMessage ) ) )
		{
			eae6320::Assets::OutputErrorMessageWithFileInfo( __FILE__, __LINE__, errorMessage.c_str() );
			goto OnExit;
		}
	}
	// Build the shaders and copy them to the installation location
	{
		const auto path_shaderBuilder = outputDir + "ShaderBuilder.exe";
		std::string errorMessage;
		int exitCode;
		{
			// Sprite vertex shader
			const auto shader_vertex_sprite = engineSourceContentDir + "Shaders/Vertex/Sprite.eae6320shader";
			const auto shader_vertex_sprite_built = gameInstallDir + "data/Shaders/Vertex/Sprite.shd";

			// Completion for Sprite vertex shader
			if (result = eae6320::Platform::CreateDirectoryIfItDoesntExist(shader_vertex_sprite_built, &errorMessage))
			{
				if (result = eae6320::Platform::ExecuteCommand((std::string("\"") + path_shaderBuilder
					+ "\" \"" + shader_vertex_sprite + "\" \"" + shader_vertex_sprite_built + "\" vertex").c_str(),
					&exitCode, &errorMessage))
				{
					if (exitCode != EXIT_SUCCESS)
					{
						return exitCode;
					}
				}
				else
				{
					eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__, errorMessage.c_str());
					goto OnExit;
				}
		}
			else
			{
				eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__, errorMessage.c_str());
				goto OnExit;
			}
		}
		{
			// Sprite fragment shader
			const auto shader_fragment_sprite = engineSourceContentDir + "Shaders/Fragment/Sprite.eae6320shader";
			const auto shader_fragment_sprite_built = gameInstallDir + "data/Shaders/Fragment/Sprite.shd";

			// Completion for Sprite fragment shader
			if (result = eae6320::Platform::CreateDirectoryIfItDoesntExist(shader_fragment_sprite_built, &errorMessage))
			{
				if (result = eae6320::Platform::ExecuteCommand((std::string("\"") + path_shaderBuilder
					+ "\" \"" + shader_fragment_sprite + "\" \"" + shader_fragment_sprite_built + "\" fragment").c_str(),
					&exitCode, &errorMessage))
				{
					if (exitCode != EXIT_SUCCESS)
					{
						return exitCode;
					}
				}
				else
				{
					eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__, errorMessage.c_str());
					goto OnExit;
				}
			}
			else
			{
				eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__, errorMessage.c_str());
				goto OnExit;
			}

			// Static fragment shader
			const auto shader_fragment_sprite_static = engineSourceContentDir + "Shaders/Fragment/Static.eae6320shader";
			const auto shader_fragment_sprite_static_built = gameInstallDir + "data/Shaders/Fragment/Static.shd";

			// Completion for Sprite fragment shader
			if (result = eae6320::Platform::CreateDirectoryIfItDoesntExist(shader_fragment_sprite_static_built, &errorMessage))
			{
				if (result = eae6320::Platform::ExecuteCommand((std::string("\"") + path_shaderBuilder
					+ "\" \"" + shader_fragment_sprite_static + "\" \"" + shader_fragment_sprite_static_built + "\" fragment").c_str(),
					&exitCode, &errorMessage))
				{
					if (exitCode != EXIT_SUCCESS)
					{
						return exitCode;
					}
				}
				else
				{
					eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__, errorMessage.c_str());
					goto OnExit;
				}
			}
			else
			{
				eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__, errorMessage.c_str());
				goto OnExit;
			}
		}
#if defined( EAE6320_PLATFORM_D3D )
		{
			// Sprite vertex shader
			const auto shader_vertex_layout_sprite = engineSourceContentDir + "Shaders/Vertex/vertexInputLayout_sprite.eae6320shader";
			const auto shader_vertex_layout_sprite_built = gameInstallDir + "data/Shaders/Vertex/vertexInputLayout_sprite.shd";

			// Completion of Sprite vertex shader
			if (result = eae6320::Platform::CreateDirectoryIfItDoesntExist(shader_vertex_layout_sprite_built, &errorMessage))
			{
				if (result = eae6320::Platform::ExecuteCommand((std::string("\"") + path_shaderBuilder
					+ "\" \"" + shader_vertex_layout_sprite + "\" \"" + shader_vertex_layout_sprite_built + "\" vertex").c_str(),
					&exitCode, &errorMessage))
				{
					if (exitCode != EXIT_SUCCESS)
					{
						return exitCode;
					}
				}
				else
				{
					eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__, errorMessage.c_str());
					goto OnExit;
				}
			}
			else
			{
				eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__, errorMessage.c_str());
				goto OnExit;
			}
		}
#endif
	}
	// Copy the licenses to the installation location
	{
		std::string errorMessage;
		if ( result = eae6320::Platform::CreateDirectoryIfItDoesntExist( gameLicenseDir, &errorMessage ) )
		{
			std::vector<std::string> licensePaths;
			constexpr auto searchRecursively = true;
			if ( result = eae6320::Platform::GetFilesInDirectory( licenseDir, licensePaths, searchRecursively, &errorMessage ) )
			{
				for ( const auto& sourcePath : licensePaths )
				{
					const auto pos_lastSlash = sourcePath.find_last_of( "/\\" );
					const auto fileName = ( pos_lastSlash != sourcePath.npos )
						? sourcePath.substr( pos_lastSlash + 1 ) : sourcePath;
					const auto targetPath = gameLicenseDir + fileName;
					constexpr auto dontFailIfTargetAlreadyExists = false;
					constexpr auto modifyTargetTime = true;
					if ( !( result = eae6320::Platform::CopyFile( sourcePath.c_str(), targetPath.c_str(),
						dontFailIfTargetAlreadyExists, modifyTargetTime, &errorMessage ) ) )
					{
						eae6320::Assets::OutputErrorMessageWithFileInfo( __FILE__, __LINE__, errorMessage.c_str() );
						goto OnExit;
					}
				}
			}
			else
			{
				eae6320::Assets::OutputErrorMessageWithFileInfo( __FILE__, __LINE__, errorMessage.c_str() );
				goto OnExit;
			}
		}
		else
		{
			eae6320::Assets::OutputErrorMessageWithFileInfo( __FILE__, __LINE__, errorMessage.c_str() );
			goto OnExit;
		}
	}

OnExit:

	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
