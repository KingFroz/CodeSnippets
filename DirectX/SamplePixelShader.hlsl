// Per-pixel color data passed through the pixel shader.
texture2D tex : register(t0);
SamplerState samplestate: register(s0);

extern bool _switch = true;

struct DirectionalLight
{
	float4 ambient;
	float4 diffuse;
	float3 dir;
	float spec;
};

struct PointLight
{
	float3 dir;
	float spec;
	float3 pos;
	float range;
	float3 att;
	float padding2;
	float4 ambient;
	float4 diffuse;
};

struct SpotLight
{
	float3 pos;
	float range;
	float3 dir;
	float cone;
	float3 att;
	float spec;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame
{
	DirectionalLight dirlight;
	PointLight light;
	SpotLight _spotlight;
	float3 padding;
	bool GrayScale;
	float4 camPosition;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 norm : NORMALS;
	float4 wPos : W_POSITION;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	////////////LIGHTING/////////////
	input.norm = normalize(input.norm);
	float4 diffuse = tex.Sample(samplestate, input.uv);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);



	//////////////PointLight/////////////		
	float3 ambient = diffuse * light.ambient;

	float3 _plightvec = light.pos - input.wPos;
	float _pldistance = length(_plightvec);

	if (_pldistance > light.range)
		return float4(ambient, diffuse.a);

	float lightValue = dot(normalize(_plightvec), input.norm);
	if (lightValue > 0.0f)
	{
		finalColor += lightValue * light.diffuse;
	}

	//IGNORE
	if (GrayScale == true)
	{
		float ret = (finalColor.x + finalColor.y + finalColor.z) / 3;

		finalColor.x = ret;
		finalColor.y = ret;
		finalColor.z = ret;
	}

	//////SPECULAR/////////
	float3 toEye = normalize(camPosition - input.wPos);
	float3 reflection = reflect(-normalize(_plightvec), input.norm);
	float specularFactor = clamp(dot(toEye, reflection), 0, 1);

	specular = float4(1,1,1,1) * pow(dot(reflection, toEye), 64.0f);

	finalColor = (lightValue*diffuse + specular*float4(1,1,1,1));
	
	//////////////Directional Light/////////////	
	finalColor += saturate((clamp(dot(dirlight.dir, input.norm), 0, 1) * float4(1,0,0,1)/* + specular*/)) * diffuse;

	////////////SPOT LIGHT/////////////		
	//To Light
	float3 lightvec = _spotlight.pos - input.wPos;
	float distance = length(lightvec);

	float3 toPixel = -lightvec;

	float spotfactor = dot(normalize(toPixel), normalize(_spotlight.dir));

	float angle = cos((_spotlight.cone / 180.0f) * 3.14);

	if (angle < spotfactor)
	{
		float3 ambient = diffuse * _spotlight.ambient;

		if (distance > _spotlight.range)
			return float4(ambient, diffuse.a);

		float lightValue = dot(normalize(lightvec), input.norm);
		if (lightValue > 0.0f)
		{
			finalColor += lightValue * float4(0, 1, 0, 1);//_spotlight.diffuse;
		}
	}

	if (diffuse.a < 0.01f)
	{
		discard;
	}


	return float4(finalColor, diffuse.a);
}

