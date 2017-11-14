--[[
	This file contains the logic for building assets
]]

-- Static Data Initialization
--===========================

-- Environment Variables
local EngineSourceContentDir, GameInstallDir, GameLicenseDir, GameSourceContentDir, LicenseDir, OutputDir 
do
	-- EngineSourceContentDir
	do
		local errorMessage
		EngineSourceContentDir, errorMessage = GetEnvironmentVariable( "EngineSourceContentDir" )
		if not EngineSourceContentDir then
			error( errorMessage )
		end
	end
	-- GameInstallDir
	do
		local errorMessage
		GameInstallDir, errorMessage = GetEnvironmentVariable( "GameInstallDir" )
		if not GameInstallDir then
			error( errorMessage )
		end
	end
	-- GameLicenseDir
	do
		local errorMessage
		GameLicenseDir, errorMessage = GetEnvironmentVariable( "GameLicenseDir" )
		if not GameLicenseDir then
			error( errorMessage )
		end
	end
	-- GameSourceContentDir
	do
		local errorMessage
		GameSourceContentDir, errorMessage = GetEnvironmentVariable( "GameSourceContentDir" )
		if not GameSourceContentDir then
			error( errorMessage )
		end
	end
	-- LicenseDir
	do
		local errorMessage
		LicenseDir, errorMessage = GetEnvironmentVariable( "LicenseDir" )
		if not LicenseDir then
			error( errorMessage )
		end
	end
	-- OutputDir
	do
		local errorMessage
		OutputDir, errorMessage = GetEnvironmentVariable( "OutputDir" )
		if not OutputDir then
			error( errorMessage )
		end
	end
end

-- External Interface
--===================

function BuildAssets()
	local wereThereErrors = false

	-- Build the shaders and copy them to the installation location
	do
		local path_shaderBuilder = OutputDir .. "ShaderBuilder.exe"
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Vertex/Sprite.eae6320shader"
			local shader_built = GameInstallDir .. "data/Shaders/Vertex/Sprite.binshd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" vertex"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Vertex/Mesh.eae6320shader"
			local shader_built = GameInstallDir .. "data/Shaders/Vertex/Mesh.binshd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" vertex"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/Sprite.eae6320shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/Sprite.binshd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/Static.eae6320shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/Static.binshd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/MeshColorTexture.eae6320shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/MeshColorTexture.binshd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/MeshColor.eae6320shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/MeshColor.binshd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end
		do
			local shader_authored = EngineSourceContentDir .. "Shaders/Fragment/MeshTexture.eae6320shader"
			local shader_built = GameInstallDir .. "data/Shaders/Fragment/MeshTexture.binshd"
			CreateDirectoryIfItDoesntExist( shader_built )
			local command = "\"" .. path_shaderBuilder .. "\""
				.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" fragment"
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. shader_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
			end
		end

		if EAE6320_PLATFORM_D3D then
			do
				local shader_authored = EngineSourceContentDir .. "Shaders/Vertex/vertexInputLayout_sprite.eae6320shader"
				local shader_built = GameInstallDir .. "data/Shaders/Vertex/vertexInputLayout_sprite.binshd"
				CreateDirectoryIfItDoesntExist( shader_built )
				local command = "\"" .. path_shaderBuilder .. "\""
					.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" vertex"
				local result, exitCode = ExecuteCommand( command )
				if result then
					if exitCode == 0 then
						-- Display a message for each asset
						print( "Built " .. shader_authored )
					else
						wereThereErrors = true
						-- The builder should already output a descriptive error message if there was an error
						-- (remember that you write the builder code,
						-- and so if the build process failed it means that _your_ code has returned an error code)
						-- but it can be helpful to still return an additional vague error message here
						-- in case there is a bug in the specific builder that doesn't output an error message
						OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
					end
				else
					wereThereErrors = true
					-- If the command wasn't executed then the second return value is an error message
					OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
				end
			end
			do
				local shader_authored = EngineSourceContentDir .. "Shaders/Vertex/vertexInputLayout_mesh.eae6320shader"
				local shader_built = GameInstallDir .. "data/Shaders/Vertex/vertexInputLayout_mesh.binshd"
				CreateDirectoryIfItDoesntExist( shader_built )
				local command = "\"" .. path_shaderBuilder .. "\""
					.. " \"" .. shader_authored .. "\" \"" .. shader_built .. "\" vertex"
				local result, exitCode = ExecuteCommand( command )
				if result then
					if exitCode == 0 then
						-- Display a message for each asset
						print( "Built " .. shader_authored )
					else
						wereThereErrors = true
						-- The builder should already output a descriptive error message if there was an error
						-- (remember that you write the builder code,
						-- and so if the build process failed it means that _your_ code has returned an error code)
						-- but it can be helpful to still return an additional vague error message here
						-- in case there is a bug in the specific builder that doesn't output an error message
						OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), shader_authored )
					end
				else
					wereThereErrors = true
					-- If the command wasn't executed then the second return value is an error message
					OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), shader_authored )
				end
			end
		end
	end

	-- Build the textures and copy them to the installation location
	do
		local path_textureBuilder = OutputDir .. "TextureBuilder.exe"
		do
			local texture_authored = EngineSourceContentDir .. "Textures/Pikachu.png"
			local texture_built = GameInstallDir .. "data/Textures/Pikachu.bintxr"
			CreateDirectoryIfItDoesntExist( texture_built )
			local command = "\"" .. path_textureBuilder .. "\""
				.. " \"" .. texture_authored .. "\" \"" .. texture_built .. "\""
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. texture_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), texture_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), texture_authored )
			end
		end
		do
			local texture_authored = EngineSourceContentDir .. "Textures/Pokeball.png"
			local texture_built = GameInstallDir .. "data/Textures/Pokeball.bintxr"
			CreateDirectoryIfItDoesntExist( texture_built )
			local command = "\"" .. path_textureBuilder .. "\""
				.. " \"" .. texture_authored .. "\" \"" .. texture_built .. "\""
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. texture_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), texture_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), texture_authored )
			end
		end
		do
			local texture_authored = EngineSourceContentDir .. "Textures/Electroball.png"
			local texture_built = GameInstallDir .. "data/Textures/Electroball.bintxr"
			CreateDirectoryIfItDoesntExist( texture_built )
			local command = "\"" .. path_textureBuilder .. "\""
				.. " \"" .. texture_authored .. "\" \"" .. texture_built .. "\""
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. texture_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), texture_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), texture_authored )
			end
		end
		do
			local texture_authored = EngineSourceContentDir .. "Textures/FlowerShibe.jpg"
			local texture_built = GameInstallDir .. "data/Textures/FlowerShibe.bintxr"
			CreateDirectoryIfItDoesntExist( texture_built )
			local command = "\"" .. path_textureBuilder .. "\""
				.. " \"" .. texture_authored .. "\" \"" .. texture_built .. "\""
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. texture_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), texture_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), texture_authored )
			end
		end
		do
			local texture_authored = EngineSourceContentDir .. "Textures/EvilShibe.jpg"
			local texture_built = GameInstallDir .. "data/Textures/EvilShibe.bintxr"
			CreateDirectoryIfItDoesntExist( texture_built )
			local command = "\"" .. path_textureBuilder .. "\""
				.. " \"" .. texture_authored .. "\" \"" .. texture_built .. "\""
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. texture_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), texture_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), texture_authored )
			end
		end
	end

	-- Build the meshes and copy them to the installation location
	do
		local path_meshBuilder = OutputDir .. "MeshBuilder.exe"
		do
			local mesh_authored = EngineSourceContentDir .. "Meshes/Cube.mayamsh"
			local mesh_built = GameInstallDir .. "data/Meshes/Cube.binmsh"
			CreateDirectoryIfItDoesntExist( mesh_built )
			local command = "\"" .. path_meshBuilder .. "\""
				.. " \"" .. mesh_authored .. "\" \"" .. mesh_built .. "\""
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. mesh_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), mesh_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), mesh_authored )
			end
		end
		do
			local mesh_authored = EngineSourceContentDir .. "Meshes/Plane.mayamsh"
			local mesh_built = GameInstallDir .. "data/Meshes/Plane.binmsh"
			CreateDirectoryIfItDoesntExist( mesh_built )
			local command = "\"" .. path_meshBuilder .. "\""
				.. " \"" .. mesh_authored .. "\" \"" .. mesh_built .. "\""
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. mesh_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), mesh_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), mesh_authored )
			end
		end
		do
			local mesh_authored = EngineSourceContentDir .. "Meshes/Sphere.mayamsh"
			local mesh_built = GameInstallDir .. "data/Meshes/Sphere.binmsh"
			CreateDirectoryIfItDoesntExist( mesh_built )
			local command = "\"" .. path_meshBuilder .. "\""
				.. " \"" .. mesh_authored .. "\" \"" .. mesh_built .. "\""
			local result, exitCode = ExecuteCommand( command )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. mesh_authored )
				else
					wereThereErrors = true
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. command .. " failed with exit code " .. tostring( exitCode ), mesh_authored )
				end
			else
				wereThereErrors = true
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. command .. " couldn't be executed: " .. tostring( exitCode ), mesh_authored )
			end
		end
	end

	-- Copy the licenses to the installation location
	do
		CreateDirectoryIfItDoesntExist( GameLicenseDir )
		local sourceLicenses = GetFilesInDirectory( LicenseDir )
		for i, sourceLicense in ipairs( sourceLicenses ) do
			local sourceFileName = sourceLicense:sub( #LicenseDir + 1 )
			local targetPath = GameLicenseDir .. sourceFileName
			local result, errorMessage = CopyFile( sourceLicense, targetPath )
			if result then
				-- Display a message
				print( "Installed " .. sourceFileName )
			else
				wereThereErrors = true
				OutputErrorMessage( "The license \"" .. sourceLicense .. "\" couldn't be copied to \"" .. targetPath .. "\": " .. errorMessage )
			end
		end
	end

	-- Copy the Settings to the installation location
	do		
		local sourceFileName = "Settings.ini"
		local sourcePath = OutputDir .. sourceFileName
		local targetPath = GameInstallDir .. sourceFileName
		local result, errorMessage = CopyFile( sourcePath, targetPath )
		if result then
			-- Display a message
			print( "Installed " .. sourceFileName )
		else
			wereThereErrors = true
			OutputErrorMessage( "The setting \"" .. sourceFileName .. "\" couldn't be copied to \"" .. targetPath .. "\": " .. errorMessage )
		end
	end

	return not wereThereErrors
end
