
SamplerState    TextureSampler : register(s0);

Texture2D       DiffuseTexture : register(t0);
Texture2D       BumpTexture : register(t1);
Texture2D       LightTexture : register(t2);

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

struct PointLightInfo
{
	float4 PointLightColour;
	float3 PointLightPosition;
	float2 PointLightRange;
};

struct DirectionalLightInfo
{
	float4 DirectionalLightColour;
	float3 DirectionalLightDirection;
};

struct SpotLightInfo
{
	float4 SpotLightColour;
	float3 SpotLightPosition;
	float3 SpotLightDirection;
	float2 SpotLightRange;
	float2 SpotLightFocus;
	float2 Padding;
	// Total 4 * 16 bytes
};

cbuffer Spot
{
	SpotLightInfo SpotInfo;
};

cbuffer PointLights : register(b1)
{
	PointLightInfo PointLights[10];
};

cbuffer DirectionalLight : register(b2)
{
	DirectionalLightInfo DirectionalLight;
};

cbuffer SpotLights : register(b3)
{
	SpotLightInfo SpotLights[10];
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

float3 calculateDirectionalLight(DirectionalLightInfo directionalLight, float3 surfaceNormal, float3 position)
{
	float3 viewVector_d = normalize(position - ViewPosition.xyz);
		float3 directionallightreflectVector = reflect(directionalLight.DirectionalLightDirection, surfaceNormal);

	// Ambience
	float3 DirectionalLightAmbience = directionalLight.DirectionalLightColour.xyz * SurfaceConstants.x;

	// Diffuse
	float3 DirectionalDiffuse = saturate(dot(surfaceNormal, -directionalLight.DirectionalLightDirection)) * directionalLight.DirectionalLightColour.xyz * SurfaceConstants.y;

	// Specular
	float3 DirectionalSpecular = pow(saturate(dot(directionallightreflectVector, -viewVector_d)), SurfaceConstants.w) * directionalLight.DirectionalLightColour.xyz * SurfaceConstants.z;

	return (DirectionalLightAmbience + DirectionalDiffuse + DirectionalSpecular);

}

float3 calculatePointLight(PointLightInfo pointLight, float3 surfaceNormal, float3 position)
{

	float pointDistance = distance(pointLight.PointLightPosition, position);
	float pointDistAtt = saturate(1 - pointDistance / pointLight.PointLightRange.x);
	float3 pointlightDirection = normalize(position - pointLight.PointLightPosition);
		float3 viewVector_p = normalize(position - ViewPosition.xyz);
		float3 pointlightreflectVector = reflect(pointlightDirection, surfaceNormal);

		// Ambience
		float3 PointLightAmbience = pointLight.PointLightColour.xyz * SurfaceConstants.x;

		// Diffuse
		float3 PointDiffuse = saturate(dot(surfaceNormal, -pointlightDirection)) * pointDistAtt * pointLight.PointLightColour.xyz * SurfaceConstants.y;

		// Specular
		float3 PointSpecular = pow(saturate(dot(pointlightreflectVector, -viewVector_p)), SurfaceConstants.w) * pointDistAtt * pointLight.PointLightColour.xyz * SurfaceConstants.z;

		return (PointLightAmbience + PointDiffuse + PointSpecular);
}

float3 calculateSpotLight(SpotLightInfo spotLight, float3 surfaceNormal, float3 position)
{

	float spotDistance = distance(spotLight.SpotLightPosition, position);
	float spotDistAtt = saturate(1 - spotDistance / spotLight.SpotLightRange.x);
	float3 spotlight2PixelVector = normalize(position - spotLight.SpotLightPosition);
		float spotAngularAtt = saturate(pow(dot(spotlight2PixelVector, spotLight.SpotLightDirection), spotLight.SpotLightFocus.x));

	// Ambience
	float3 SpotLightAmbience = spotLight.SpotLightColour.xyz * SurfaceConstants.x;

	// Diffuse
	float3 SpotDiffuse = saturate(dot(surfaceNormal, -spotLight.SpotLightDirection)) * spotDistAtt * spotAngularAtt * spotLight.SpotLightColour.xyz * SurfaceConstants.y;

	// Specular
	float3 viewVector_s = normalize(position - ViewPosition.xyz);
	float3 spotlightreflectVector = reflect(spotLight.SpotLightDirection, surfaceNormal);
	float3 SpotSpecular = pow(saturate(dot(spotlightreflectVector, -viewVector_s)), SurfaceConstants.w) * spotDistAtt * spotAngularAtt * spotLight.SpotLightColour.xyz * SurfaceConstants.z;

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

	float3 bump = 4 * (BumpTexture.Sample(TextureSampler, input.tex) - 0.5f);
	float3 normalVector = input.normal + bump.x * input.tangent + bump.y * input.binormal;
	normalVector = normalize(normalVector);

	float3 worldPosition = input.worldPos.xyz;

	float3 pointlightIntensity = float3(0.0f, 0.0f, 0.0f);
	float3 directionallightIntensity = float3(0.0f, 0.0f, 0.0f);
	float3 spotlightIntensity = float3(0.0f, 0.0f, 0.0f);

	// Compute Point Lights
	for (int i = 0; i < 10; i++)
	{
		pointlightIntensity += calculatePointLight(PointLights[i], normalVector, worldPosition);
	}

	// Compute Spot Lights
	for (int i = 0; i < 10; i++)
	{
		spotlightIntensity += calculateSpotLight(SpotLights[i], normalVector, worldPosition);
	}

	// Calculate Directional Light
	directionallightIntensity = calculateDirectionalLight(DirectionalLight, normalVector, worldPosition);


		
	float3 lightIntensity3f = directionallightIntensity + pointlightIntensity + spotlightIntensity + SurfaceEmissiveColour.xyz;
	float4 lightIntensity4f = float4(lightIntensity3f, 1);

	float4 pixelcolour = DiffuseTexture.Sample(TextureSampler, input.tex) * lightIntensity4f;

	return pixelcolour;
}
