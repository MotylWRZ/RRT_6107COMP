
SamplerState    TextureSampler : register(s0);

Texture2D       DiffuseTexture : register(t0);
Texture2D       BumpTexture : register(t1);

#include "RRTLightsInclude.hlsl"







cbuffer WorldTransforms
{
    matrix WorldMatrix;
    matrix WorldViewProjMatrix;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float4 worldPos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};


VS_OUTPUT VSMain(in VS_INPUT input)
{
    VS_OUTPUT output;

    output.position = mul(float4(input.position, 1.0f), WorldViewProjMatrix);

    output.worldPos = mul(float4(input.position, 1.0f), WorldMatrix);
    output.normal = mul(normalize(input.normal), WorldMatrix);

    output.tangent = mul(normalize(input.tangent), WorldMatrix);
    output.binormal = mul(normalize(input.binormal), WorldMatrix);

    output.color = input.color;
    output.tex = input.tex;
    return output;
}

float4 PSMain(in VS_OUTPUT input) : SV_Target
{
    float4 lightIntensity4f = float4(0.0f, 0.0f, 0.0f, 0.0f); // computeLights(input.normal, input.tangent, input.binormal, input.worldPos);

    float4 pixelcolour = DiffuseTexture.Sample(TextureSampler, input.tex) * lightIntensity4f;

   // pixelcolour.r = testFunction();


    return pixelcolour;
}
