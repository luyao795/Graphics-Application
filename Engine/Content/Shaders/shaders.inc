/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Types
//======

#if defined( EAE6320_PLATFORM_D3D )

	#define vec2 float2
	#define vec3 float3
	#define vec4 float4
	#define mat2 float2x2
	#define mat3 float3x3
	#define mat4 float4x4

#elif defined( EAE6320_PLATFORM_GL )

	#define float2 vec2
	#define float3 vec3
	#define float4 vec4
	#define float2x2 mat2
	#define float3x3 mat3
	#define float4x4 mat4

#endif

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )
	#define DeclareTextureBuffer( i_name, i_id ) Texture2D i_name : register( t##i_id )
	#define DeclareSamplerState( i_name, i_id ) SamplerState i_name : register( s##i_id )
	#define CreateSampledColor( i_texture, i_texcoord, i_samplerState ) i_texture.Sample( i_samplerState, i_texcoord )

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name
	#define DeclareTextureBuffer( i_name, i_id ) layout( binding = i_id ) uniform sampler2D i_name
	#define DeclareSamplerState( i_name, i_id )
	#define CreateSampledColor( i_texture, i_texcoord, i_samplerState ) texture2D( i_texture, i_texcoord )
	#define o_position gl_Position

#endif
