Texture2D       DiffuseTexture : register( t0 );           
SamplerState    TextureSampler : register( s0 );

cbuffer GSTransformMatrices
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;

	matrix WorldViewProjMatrix;	
};

cbuffer instance
{
	float4 instancePosition1;
	float4 instancePosition2;
	float4 instancePosition3;
	float4 instancePosition4;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float2 tex		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 tex		: TEXCOORD0;
};

struct GS_INPUTOUTPUT
{
	float4 position : SV_Position;
	float2 tex		: TEXCOORD0;
};


float4x4 TransMatrix(float4 trans)
{
	return float4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		trans.x, trans.y, trans.z, 1
		);
}


// Geometry Shader
[instance(4)]
[maxvertexcount(3)]
void GSMain(triangle GS_INPUTOUTPUT input[3],
	inout TriangleStream<GS_INPUTOUTPUT> TriangleOutputStream,
	uint InstanceID : SV_GSInstanceID)
{
	GS_INPUTOUTPUT GSOutput;

	float4 instancePositions[4];
	instancePositions[0] = instancePosition1;
	instancePositions[1] = instancePosition2;
	instancePositions[2] = instancePosition3;
	instancePositions[3] = instancePosition4;

	float4x4 transMatrix = TransMatrix(instancePositions[InstanceID]);


	for (int i = 0; i < 3; i++)
	{
		float4 position = input[i].position;
			//position = mul(position, WorldViewProjMatrix);
		position = mul(position, WorldMatrix);
		position = mul(position, transMatrix);
		position = mul(position, ViewMatrix);
		GSOutput.position = mul(position, ProjMatrix );
		GSOutput.tex = input[i].tex;
		TriangleOutputStream.Append(GSOutput);
	}

	TriangleOutputStream.RestartStrip();
}


VS_OUTPUT VSMain( in VS_INPUT input )
{
	VS_OUTPUT VSOutput;
	
	VSOutput.position = float4(input.position, 1);
	VSOutput.tex = input.tex;

	return VSOutput;
}


float4 PSMain(in VS_OUTPUT input) : SV_Target
{
	float4 pixelcolour = DiffuseTexture.Sample(TextureSampler, input.tex);

	return pixelcolour;;
}

