
// Constant buffers we want to use
cbuffer Transforms
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
};

struct VS_INPUT
{
	float4 Position : POSITION;
	float4 Color : COLOR;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 Color : COLOR;
};

VS_OUTPUT VSMain(in VS_INPUT v)
{
	VS_OUTPUT Output;

	float4 WorldSpace = mul(v.Position, WorldMatrix);
	float4 ViewSpace = mul(WorldSpace, ViewMatrix);
	float4 ScreenSpace = mul(ViewSpace, ProjMatrix);

	Output.Position = ScreenSpace;
	Output.Color = v.Color;

	return Output;
}

//Pixel Shader
float4 PSMain(in VS_OUTPUT input) : SV_Target
{
	float4 Color = input.Color;
	return(Color);
}