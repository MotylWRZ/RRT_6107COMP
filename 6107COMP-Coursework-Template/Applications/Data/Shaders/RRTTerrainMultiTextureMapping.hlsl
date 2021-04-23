Texture2D DiffuseTexture1 : register(t0);
Texture2D DiffuseTexture2 : register(t1);
SamplerState TextureSampler : register(s0);

// Constant buffers we want to use
cbuffer Transforms
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjMatrix;
};

cbuffer TerrainParams : register(b0)
{
    float4 TextureBoundary;
};



struct VS_INPUT
{
    float4 Position : POSITION;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float4 worldpos : POSITION;
    float2 tex : TEXCOORD0;
};

VS_OUTPUT VSMain(in VS_INPUT v)
{
    VS_OUTPUT Output;

    float4 WorldSpace = mul(v.Position, WorldMatrix);
    float4 ViewSpace = mul(WorldSpace, ViewMatrix);
    float4 ScreenSpace = mul(ViewSpace, ProjMatrix);

    Output.Position = ScreenSpace;
    Output.worldpos = v.Position;
    Output.tex = v.tex;

    return Output;
}

//Pixel Shader
float4 PSMain(in VS_OUTPUT input) : SV_Target
{
    float terrainheight = input.worldpos.y;
    float texweight = 1 / (1 + exp(terrainheight - TextureBoundary.x));

    float4 sampledTextureColour1 = (1 - texweight) * DiffuseTexture1.Sample(TextureSampler, input.tex);
    float4 sampledTextureColour2 = texweight * DiffuseTexture2.Sample(TextureSampler, input.tex);

    float4 sampledTextureColour = sampledTextureColour1 + sampledTextureColour2;

    return (sampledTextureColour);
}