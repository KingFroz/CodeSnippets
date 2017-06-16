#pragma once

namespace GXII_ListonH
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4X4 world;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normals;
	};

	struct DirectionalLight
	{
		DirectionalLight() { ZeroMemory(this, sizeof(DirectionalLight)); }
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT3 dir;
		float spec;
	};

	struct PointLight
	{
		PointLight() { ZeroMemory(this, sizeof(PointLight)); }
		DirectX::XMFLOAT3 dir;
		float spec;
		DirectX::XMFLOAT3 pos;
		float range;
		DirectX::XMFLOAT3 att;
		float padding2;
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
	};

	struct SpotLight
	{
		SpotLight() { ZeroMemory(this, sizeof(SpotLight)); }
		DirectX::XMFLOAT3 pos;
		float range;
		DirectX::XMFLOAT3 dir;
		float cone;
		DirectX::XMFLOAT3 att;
		float spec;
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
	};

	struct perFrame
	{
		DirectionalLight dirlight;
		PointLight light;
		SpotLight _spotlight;
		DirectX::XMFLOAT3 padding;
		bool GrayScale;
		DirectX::XMFLOAT4 camPosition;
	};

	
}