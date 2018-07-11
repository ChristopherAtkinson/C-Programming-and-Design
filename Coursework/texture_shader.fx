//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D diffused_texture : register(t0);
SamplerState state : register( s0 );

struct Light
{
	float3 position;
	float range; 
	float3 direction; 
	float cone; 
	float3 attenuation; 
	float4 ambient; 
	float4 diffuse;
};

cbuffer SCENE : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer CAMERA : register( b1 )
{
	float4 camera_position;
};

#define MAX_LIGHTS 10
cbuffer LIGHT : register( b2 )
{
	Light lights[MAX_LIGHTS];
	int quantity;
};

cbuffer MATERIAL : register( b3 )
{
	float4 Ka;
	float4 Kd;
	float4 Ks;
	float power;
};

cbuffer ANIMATION : register(b4)
{
	bool aniamtion;
	float time;
};

struct VS_INPUT
{
	float4 position           : POSITION;         //position
	float3 normal             : NORMAL;           //normal
	float2 text_coord         : TEXCOORD0;        //texture coordinate
};

struct PS_INPUT
{
	float3 world              : POSITION0;
	float4 position           : SV_POSITION0;
	float3 normal             : NORMAL;
	float2 text_coord         : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	if (aniamtion == true) 
	{
		if (input.position.x > 0.032f || input.position.x < -0.032f)
		{
			if (input.position.y < 0.2f && input.position.y > 0.1f)
			{
				input.position.y = lerp(input.position.y, (input.position.y - input.position.y / 2)
					+ sin(time  * 0.00000001f), max(input.position.x, input.position.x * -1));
			}
		}
	}

	output.world = mul(input.position, World);

	// Convert float3 to float4
	input.position.w = 1.0f;

	// Set position into clip space
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	// Set normal 
	output.normal = normalize(mul(input.normal, (float3x3)World));

	// Set tecture coord 
	output.text_coord = input.text_coord;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
	float4 tex = diffused_texture.Sample(state, input.text_coord);
	if (tex.a == 0.0f) { tex = float4(1, 1, 1, 1); }

	float3 Ambient = float4(0, 0, 0, 0);
	for (int i = 0; i < quantity; i++)
	{ 
		Ambient += saturate(tex * lights[i].ambient);
	}
	Ambient /= quantity;

	float3 Diffuse = float4(0, 0, 0, 0); 
	float3 Specular = float4(0, 0, 0, 0);

	[unroll]
	for (int i = 0; i < quantity; ++i)
	{
		float3 L = lights[i].position - input.world;
		float D = length(L); L = normalize(L);

		if (D > lights[i].range) { continue; }

		float3 N = normalize(input.normal);
		float3 V = normalize(camera_position - input.world); 

		float Di = max(dot(L, N), 0.0f);
		if (Di > 0.0f)
		{
			float3 attenuation = tex * lights[i].diffuse * Di;
			attenuation /= (lights[i].attenuation.x + (lights[i].attenuation.y * D)
				+ (lights[i].attenuation.z * (D*D)));
			Diffuse += attenuation * saturate(pow(max(dot(-L, lights[i].direction), 0.0f), lights[i].cone)) / D;

			//float3 R = normalize(reflect(L, V));
			float3 R = normalize(2 * dot(L, N) * N +L);
			Specular += saturate(pow(max(dot(normalize(R + V), N), 0.0f), power));
		}
	}

	return float4(saturate(Ka * Ambient)
		+ Kd.xyz * Diffuse  * 0.6f
		+ Ks.xyz * Specular * 0.5f
		, Kd.a * tex.a);
}