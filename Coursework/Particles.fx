Texture2D tex : register(t0);
SamplerState state : register( s0 );

cbuffer cbChangesEveryFrame : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
};

struct VS_INPUT
{
	float4 position           : POSITION;
	float2 text_coord         : TEXCOORD0;
	float4 color              : COLOR;
};

struct PS_INPUT
{
	float4 position           : SV_POSITION;
	float2 text_coord         : TEXCOORD0;
	float4 color              : COLOR;
};

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	// Convert float3 to float4
	input.position.w = 1.0f;

	// Set position into clip space
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	output.text_coord = input.text_coord;
	output.color = input.color;

	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
	return tex.Sample(state, input.text_coord);
}