
#define LIGHT_TYPE_NONE			 0
#define LIGHT_TYPE_POINT         1
#define LIGHT_TYPE_SPOT          2
#define LIGHT_TYPE_DIRECTIONAL   3

SamplerState    TextureSampler : register(s0);

Texture2D       DiffuseTexture : register(t0);

cbuffer WorldTransforms
{
	matrix WorldMatrix;
	matrix WorldViewProjMatrix;
};

cbuffer SurfaceReflectanceInfo
{
	float4 SurfaceEmissiveColour;
	float4 SurfaceConstants;			// x: ambient, y: diffuse, z: specular, w: shininess
};

struct Light
{
    float iLightType;
	float3 Padding;
    float4 LightColour;
    float3 LightPosition;
    float3 LightDirection;
    float2 LightRange;
    float2 LightFocus;
    float2 Padding2;
};

cbuffer cLights
{
	Light Lights[100];
}

cbuffer SceneInfo
{
	float4 ViewPosition;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
	float4 color    : COLOR;
	float2 tex		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 worldPos : POSITION;
	float3 normal   : NORMAL;
	float4 color    : COLOR;
	float2 tex		: TEXCOORD0;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};

float3 calculateDirectionalLight(Light directionalLight, float3 surfaceNormal, float3 position)
{
	float3 viewVector_d = normalize(position - ViewPosition.xyz);
		float3 directionallightreflectVector = reflect(directionalLight.LightDirection, surfaceNormal);

	// Ambience
	float3 DirectionalLightAmbience = directionalLight.LightColour.xyz * SurfaceConstants.x;

	// Diffuse
	float3 DirectionalDiffuse = saturate(dot(surfaceNormal, -directionalLight.LightDirection)) * directionalLight.LightColour.xyz * SurfaceConstants.y;

	// Specular
	float3 DirectionalSpecular = pow(saturate(dot(directionallightreflectVector, -viewVector_d)), SurfaceConstants.w) * directionalLight.LightColour.xyz * SurfaceConstants.z;

	return (DirectionalLightAmbience + DirectionalDiffuse + DirectionalSpecular);

}

float3 calculatePointLight(Light pointLight, float3 surfaceNormal, float3 position)
{

	float pointDistance = distance(pointLight.LightPosition, position);
	float pointDistAtt = saturate(1 - pointDistance / pointLight.LightRange.x);
	float3 pointlightDirection = normalize(position - pointLight.LightPosition);
		float3 viewVector_p = normalize(position - ViewPosition.xyz);
		float3 pointlightreflectVector = reflect(pointlightDirection, surfaceNormal);

		// Ambience
		float3 PointLightAmbience = pointLight.LightColour.xyz * SurfaceConstants.x;

		// Diffuse
		float3 PointDiffuse = saturate(dot(surfaceNormal, -pointlightDirection)) * pointDistAtt * pointLight.LightColour.xyz * SurfaceConstants.y;

		// Specular
		float3 PointSpecular = pow(saturate(dot(pointlightreflectVector, -viewVector_p)), SurfaceConstants.w) * pointDistAtt * pointLight.LightColour.xyz * SurfaceConstants.z;

		return (PointLightAmbience + PointDiffuse + PointSpecular);
}

float3 calculateSpotLight(Light spotLight, float3 surfaceNormal, float3 position)
{
    surfaceNormal = float3(1.0f, 1.0f, 1.0f);

	float spotDistance = distance(spotLight.LightPosition, position);
	float spotDistAtt = saturate(1 - spotDistance / spotLight.LightRange.x);
	float3 spotlight2PixelVector = normalize(position - spotLight.LightPosition);
		float spotAngularAtt = saturate(pow(dot(spotlight2PixelVector, spotLight.LightDirection), spotLight.LightFocus.x));

	// Ambience
	float3 SpotLightAmbience = spotLight.LightColour.xyz * SurfaceConstants.x;

	// Diffuse
	float3 SpotDiffuse = saturate(dot(surfaceNormal, -spotLight.LightDirection)) * spotDistAtt * spotAngularAtt * spotLight.LightColour.xyz * SurfaceConstants.y;

	// Specular
	float3 viewVector_s = normalize(position - ViewPosition.xyz);
	float3 spotlightreflectVector = reflect(spotLight.LightDirection, surfaceNormal);
	float3 SpotSpecular = pow(saturate(dot(spotlightreflectVector, -viewVector_s)), SurfaceConstants.w) * spotDistAtt * spotAngularAtt * spotLight.LightColour.xyz * SurfaceConstants.z;

	return (SpotLightAmbience + SpotDiffuse + SpotSpecular);
}

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
    float3 normalVector = float3(-1.0f, -1.0f, -1.0f);
    //input.normal * input.tangent * input.binormal;
    normalVector = normalize(normalVector);

	float3 worldPosition = input.worldPos.xyz;


	float3 pointlightIntensity = float3(0.0f, 0.0f, 0.0f);
	float3 directionallightIntensity = float3(0.0f, 0.0f, 0.0f);
	float3 spotlightIntensity = float3(0.0f, 0.0f, 0.0f);
	float3 lightIntensity = float3(0.0f, 0.0f, 0.0f);

	// Compute Lights
	for (int i = 0; i < 100; i++)
	{
        //pointlightIntensity += calculatePointLight(Lights[i], normalVector, worldPosition);
        if (Lights[i].iLightType == LIGHT_TYPE_NONE)
        {
            continue;
        }
        if (Lights[i].iLightType.x == LIGHT_TYPE_POINT)
        {
            pointlightIntensity += calculatePointLight(Lights[i], normalVector, worldPosition);
            continue;
        }
        if (Lights[i].iLightType == LIGHT_TYPE_SPOT)
        {
            spotlightIntensity += calculateSpotLight(Lights[i], normalVector, worldPosition);

            continue;
        }
        if (Lights[i].iLightType == LIGHT_TYPE_DIRECTIONAL)
        {
            directionallightIntensity += calculateDirectionalLight(Lights[i], normalVector, worldPosition);
            continue;
        }
    }

	float3 lightIntensity3f = directionallightIntensity + pointlightIntensity + spotlightIntensity + SurfaceEmissiveColour.xyz;
	float4 lightIntensity4f = float4(lightIntensity3f, 1);

	float4 pixelcolour = DiffuseTexture.Sample(TextureSampler, input.tex) * lightIntensity4f;

	return pixelcolour;
}
