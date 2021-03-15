Texture2D       DiffuseTexture : register( t0 );           
SamplerState    TextureSampler : register( s0 );

cbuffer TransformMatrices
{
	matrix WorldMatrix;
	matrix WorldViewProjMatrix;	
};

cbuffer SurfaceReflectanceInfo
{
	float4 SurfaceEmissiveColour;


	float4 SurfaceConstants; 	//SurfaceConstants.x // ambient constant 
								//SurfaceConstants.y // diffuse constant
								//SurfaceConstants.z // specular constant
								//SurfaceConstants.w // shininess constant
};

cbuffer SceneInfo
{
	float4 ViewPosition;
};

cbuffer DirectionalLightInfo
{
	float4 DirectionalLightColour;
	float3 DirectionalLightDirection;
};

cbuffer PointLightInfo
{
	float4 PointLightColour;
	float3 PointLightPosition;
	float2 PointLightRange;
};

cbuffer SpotLightInfo
{
	float4 SpotLightColour;
	float3 SpotLightPosition;
	float3 SpotLightDirection;
	float2 SpotLightRange;
	float2 SpotLightFocus;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float3 color	: COLOR;
	float2 tex		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float3 worldPos	: POSITION;
	float3 normal   : NORMAL;
	float3 color	: COLOR;
	float2 tex		: TEXCOORD0;
};

// Lighting Functions ---------------------------------
float3 calculateDirectionalLight(float3 surfaceNormal, float3 position)
{
	float3 viewVector_d = normalize(position - ViewPosition.xyz);
		float3 directionalLightReflectVector = reflect(DirectionalLightDirection, surfaceNormal);

		// Ambience
		float3 DirectionalLightAmbience = DirectionalLightColour.xyz * SurfaceConstants.x;

		// Diffuse
		float3 DirectionalDiffuse = saturate(dot(surfaceNormal, -DirectionalLightDirection)) *
		DirectionalLightColour.xyz * SurfaceConstants.y;

		// Specular
	float3 DirectionalSpecular = pow(saturate(dot(directionalLightReflectVector, -viewVector_d)), SurfaceConstants.w) *
								 DirectionalLightColour.xyz * SurfaceConstants.z;

	return  (DirectionalLightAmbience + DirectionalDiffuse + DirectionalSpecular);
}

float3 calculatePointLight(float3 surfaceNormal, float3 position)
{
	float pointDistance = distance(PointLightPosition, position);
	float pointDistAtt = saturate(1 - pointDistance / PointLightRange.x);
	float3 pointLightDirection = normalize(position - PointLightPosition);
	float3 viewVector_p = normalize(position - ViewPosition.xyz);
	float3 pointLightReflectVector = reflect(pointLightDirection, surfaceNormal);

	// Ambience
	float3 PointLightAmbience = PointLightColour.xyz * SurfaceConstants.x;

	//  Diffuse
	float3 PointDiffuse = saturate(dot(surfaceNormal, -pointLightDirection)) *
							  pointDistAtt * PointLightColour.xyz * SurfaceConstants.y;

	// Specular
	float3 PointSpecular = pow(saturate(dot(pointLightReflectVector, -viewVector_p)), SurfaceConstants.w) *
					   pointDistAtt * PointLightColour.xyz * SurfaceConstants.z;

	return (PointLightAmbience + PointDiffuse + PointSpecular);
}

float3 calculateSpotLight(float3 surfaceNormal, float3 position)
{
	float spotDistance = distance(SpotLightPosition, position);
	float spotDistAtt = saturate(1 - spotDistance / SpotLightRange.x);
	float3 spotLight2PixelVector = normalize(position - SpotLightPosition);
	float spotAngularAtt = saturate(pow(dot(spotLight2PixelVector, SpotLightDirection), SpotLightFocus.x));

	// Ambience
	float3 SpotlightAmbience = SpotLightColour.xyz * SurfaceConstants.x;

		// Diffuse
		float3 SpotDiffuse = saturate(dot(surfaceNormal, -SpotLightDirection)) * spotDistAtt *
							 spotAngularAtt * SpotLightColour.xyz * SurfaceConstants.y;

	// Specular
	float3 viewVector_s = normalize(position - ViewPosition.xyz);
	float3 spotLightReflectVector = reflect(SpotLightDirection, surfaceNormal);
	float3 SpotSpecular = pow(saturate(dot(spotLightReflectVector, -viewVector_s)), SurfaceConstants.w) *
							  spotDistAtt * spotAngularAtt * SpotLightColour.xyz * SurfaceConstants.z;

	return (SpotlightAmbience + SpotDiffuse + SpotSpecular);
}



VS_OUTPUT VSMain( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	

	output.position = mul(float4(input.position, 1.0f), WorldViewProjMatrix);
	output.worldPos = mul(float4(input.position, 1.0f), WorldMatrix);
	output.normal = mul(normalize(input.normal), WorldMatrix);
	output.tex = input.tex;

	return output;
}


float4 PSMain(in VS_OUTPUT input) : SV_Target
{

	float3 normalVector = normalize(input.normal);
	float3 worldPosition = input.worldPos.xyz;

	float3 directionalLightIntensity = calculateDirectionalLight(normalVector, worldPosition);
	float3 pointLightIntensity = calculatePointLight(normalVector, worldPosition);
	float3 spotLightIntensity = calculateSpotLight(normalVector, worldPosition);

	float3 lightIntensity3f = directionalLightIntensity +
	pointLightIntensity +
	spotLightIntensity +
	SurfaceEmissiveColour.xyz;


	float4 lightIntensity4f = float4(lightIntensity3f, 1.0f);

		float4 sampledTextureColour = DiffuseTexture.Sample(TextureSampler, input.tex) * lightIntensity4f;
	
	return(sampledTextureColour);
}

