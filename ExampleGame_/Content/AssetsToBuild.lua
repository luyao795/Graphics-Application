--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	meshes =
	{
		"Meshes/AKM.mayamsh",
		"Meshes/Cube.mayamsh",
		"Meshes/Plane.mayamsh",
		"Meshes/Sphere.mayamsh",
	},
	shaders =
	{
		{ path = "Shaders/Vertex/vertexInputLayout_mesh.eae6320shader", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/vertexInputLayout_sprite.eae6320shader", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/Mesh.eae6320shader", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/Sprite.eae6320shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/Sprite.eae6320shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/Static.eae6320shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/MeshColor.eae6320shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/MeshTexture.eae6320shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/MeshColorTexture.eae6320shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/MeshColorTexture_Translucent.eae6320shader", arguments = { "fragment" } },
	},
	textures =
	{
		"Textures/Pokeball.png",
		"Textures/Pikachu.png",
		"Textures/Electroball.png",
		"Textures/FlowerShibe.jpg",
		"Textures/EvilShibe.jpg",
		"Textures/AKM.tga",
	},
}
