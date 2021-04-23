Texture2D       DiffuseTexture1 : register( t0 );
Texture2D		DiffuseTexture2 : register (t1);
Texture2D		DiffuseTexture3 : register(t2);
Texture2D		DiffuseTexture4 : register(t3);
SamplerState    TextureSampler : register( s0 );

#define INSTANCES_NUM_MAX 32

cbuffer GSTransformMatrices
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
};

struct StaticMeshInstanceInfo
{
    float3 InstancePosition;
    float InstanceScale;
    float4x4 InstanceRotation;
    int InstanceTexture;
    int InstanceId;
    float2 padding;
};

cbuffer cInstances
{
    StaticMeshInstanceInfo instanceInfos[INSTANCES_NUM_MAX];
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
	float2 texType  : TEXCOORD1;
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
[instance(INSTANCES_NUM_MAX)]
[maxvertexcount(3)]
void GSMain(triangle GS_INPUTOUTPUT input[3],
	inout TriangleStream<GS_INPUTOUTPUT> TriangleOutputStream,
	uint InstanceID : SV_GSInstanceID)
{
	GS_INPUTOUTPUT GSOutput;

    if(instanceInfos[InstanceID].InstanceId == -1)
    {
        return;
    }


    float4 instancePosition;
	instancePosition.x = instanceInfos[InstanceID].InstancePosition.x;
    instancePosition.y = instanceInfos[InstanceID].InstancePosition.y;
    instancePosition.z = instanceInfos[InstanceID].InstancePosition.z;
    instancePosition.w = 1.0f;

    float instanceScale = instanceInfos[InstanceID].InstanceScale;

	// Scale matrix
    float4x4 scale4x4 = float4x4(
	instanceScale, 0, 0, 0,
	0, instanceScale, 0, 0,
	0, 0, instanceScale, 0,
	0, 0, 0, 1);

    float4x4 transMatrix = TransMatrix(instancePosition);

    for (int i = 0; i < 3; i++)
    {
        float4 position = input[i].position;

		// Set instance rotation
        position = mul(position, instanceInfos[InstanceID].InstanceRotation);

		// Scale this instance by multiplication of it's position vector by the scaling matrix
        position = mul(position, scale4x4);

        position = mul(position, WorldMatrix);
        position = mul(position, transMatrix);
        position = mul(position, ViewMatrix);
        GSOutput.position = mul(position, ProjMatrix);
        GSOutput.tex = input[i].tex;
        GSOutput.texType.x = instanceInfos[InstanceID].InstanceTexture;
        GSOutput.texType.y = 0;

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


float4 PSMain(in GS_INPUTOUTPUT input) : SV_Target
{
	float4 pixelcolour;
	int texType = round(input.texType.x);

	if (texType == 0)
	{
		pixelcolour = DiffuseTexture1.Sample(TextureSampler, input.tex);
	}
	if (texType == 1)
	{
		pixelcolour = DiffuseTexture2.Sample(TextureSampler, input.tex);
	}
	if (texType == 2)
	{
		pixelcolour = DiffuseTexture3.Sample(TextureSampler, input.tex);
	}
	if (texType == 3)
	{
		pixelcolour = DiffuseTexture4.Sample(TextureSampler, input.tex);
	}

	return pixelcolour;;
}

