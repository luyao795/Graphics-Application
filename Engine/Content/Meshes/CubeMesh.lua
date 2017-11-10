--[[
	This file contains vertex and index data for drawing a cube
]]

return
{
	-- This is the set of unique vertex data
	vertexData = 
	{
		-- Vertex 0
		{
			-- position info
			x = -1,
			y = -1,
			z = 1,
			-- color info
			r = 1,
			g = 0,
			b = 0,
			a = 1,
		},		
		-- Vertex 1
		{
			-- position info
			x = -1,
			y = 1,
			z = 1,
			-- color info
			r = 0.5,
			g = 0.5,
			b = 0.5,
			a = 1,
		},		
		-- Vertex 2
		{
			-- position info
			x = 1,
			y = 1,
			z = 1,
			-- color info
			r = 0,
			g = 0,
			b = 1,
			a = 1,
		},		
		-- Vertex 3
		{
			-- position info
			x = 1,
			y = -1,
			z = 1,
			-- color info
			r = 1,
			g = 1,
			b = 0,
			a = 1,
		},		
		-- Vertex 4
		{
			-- position info
			x = -1,
			y = -1,
			z = -1,
			-- color info
			r = 0,
			g = 0,
			b = 0,
			a = 1,
		},		
		-- Vertex 5
		{
			-- position info
			x = -1,
			y = 1,
			z = -1,
			-- color info
			r = 0,
			g = 1,
			b = 1,
			a = 1,
		},		
		-- Vertex 6
		{
			-- position info
			x = 1,
			y = 1,
			z = -1,
			-- color info
			r = 1,
			g = 1,
			b = 1,
			a = 1,
		},		
		-- Vertex 7
		{
			-- position info
			x = 1,
			y = -1,
			z = -1,
			-- color info
			r = 0,
			g = 1,
			b = 0,
			a = 1,
		},
	},
	-- This is the set of index data being used to grab vertices
	indexData = 
	{
	-- Front side
		-- Triangle 1
		{
			0, 1, 2,
		},	
		-- Triangle 2
		{
			0, 2, 3,
		},	
	-- Right side
		-- Triangle 3
		{
			3, 2, 6,
		},
		-- Triangle 4
		{
			3, 6, 7,
		},
	-- Left side
		-- Triangle 5
		{
			5, 1, 0,
		},
		-- Triangle 6
		{
			5, 0, 4,
		},
	-- Bottom side
		-- Triangle 7
		{
			0, 3, 4,
		},
		-- Triangle 8
		{
			3, 7, 4,
		},
	-- Top side
		-- Triangle 9
		{
			2, 5, 6,
		},
		-- Triangle 10
		{
			5, 2, 1,
		},
	-- Back side
		-- Triangle 11
		{
			6, 5, 4,
		},
		-- Triangle 12
		{
			6, 4, 7,
		},
	},
}