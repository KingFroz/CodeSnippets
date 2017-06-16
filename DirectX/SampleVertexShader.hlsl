// A constant buffer that stores the three basic column-major matrices for composing geometry.

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 reflect;
};

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	matrix world;
	Material mat;
};

cbuffer CamBuffer
{
	float3 camPosition;
	float pad;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float2 uv : UV;
	float3 norm : NORMALS;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 norm : NORMALS;
	float4 wPos : W_POSITION;
	float3 viewDir : TEXCOORD1;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input/*, uint instanceID : SV_InstanceID*/)
{
	PixelShaderInput output;
	float4 worldPosition;
	//if (isInstance)
	//{
	//	input.pos.y += instanceID;
	//}
	float4 pos = float4(input.pos, 1.0f);
	float2 uv = float2(input.uv);
	float3 norm = float3(input.norm);
	float4 wPos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	wPos = pos;
	pos = mul(pos, view);
	pos = mul(pos, projection);
	
	output.pos = pos;
	output.uv = uv;
	output.norm = norm;
	output.wPos = wPos;

	worldPosition = mul(input.pos, model);
	output.viewDir = camPosition.xyz - worldPosition.xyz;
	output.viewDir = normalize(output.viewDir);

	return output;
}
