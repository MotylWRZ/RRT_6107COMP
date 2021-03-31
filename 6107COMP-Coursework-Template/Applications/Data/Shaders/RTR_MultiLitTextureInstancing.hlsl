Texture2D       DiffuseTexture1 : register( t0 );
Texture2D		DiffuseTexture2 : register (t1);
Texture2D		DiffuseTexture3 : register(t2);
Texture2D		DiffuseTexture4 : register(t3);
SamplerState    TextureSampler : register( s0 );

#define LIGHT_TYPE_NONE			 0
#define LIGHT_TYPE_POINT         1
#define LIGHT_TYPE_SPOT          2
#define LIGHT_TYPE_DIRECTIONAL   3


cbuffer SurfaceReflectanceInfo
{
    float4 SurfaceEmissiveColour;
    float4 SurfaceConstants; // x: ambient, y: diffuse, z: specular, w: shininess
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


#define INSTANCES_NUM_MAX 32

cbuffer GSTransformMatrices
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;

	matrix WorldViewProjMatrix;
};

struct StaticMeshInstanceInfo
{
    float3 InstancePosition;
    int InstanceTexture;
    int InstanceId;
    float3 padding;
};

cbuffer cInstances
{
    StaticMeshInstanceInfo instanceInfos[INSTANCES_NUM_MAX];
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


struct GS_INPUTOUTPUT
{
	float2 texType  : TEXCOORD1;
    float4 position : SV_Position;
    float4 worldPos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
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

    if (instanceInfos[InstanceID].InstanceId == -1)
    {
        return;
    }



    float4 instancePosition;
    instancePosition.x = instanceInfos[InstanceID].InstancePosition.x;
    instancePosition.y = instanceInfos[InstanceID].InstancePosition.y;
    instancePosition.z = instanceInfos[InstanceID].InstancePosition.z;
    instancePosition.w = 1.0f;

    float4x4 transMatrix = TransMatrix(instancePosition);


    for (int i = 0; i < 3; i++)
    {


        GSOutput.position = input[i].position;
        GSOutput.tex = input[i].tex;
        GSOutput.texType.x = 0;//     instanceInfos[InstanceID].InstanceTexture;
        GSOutput.texType.y = 0;
        GSOutput.binormal = input[i].binormal;
        GSOutput.color = input[i].color;
        GSOutput.normal = input[i].normal;
        GSOutput.tangent = input[i].tangent;
        GSOutput.worldPos = input[i].worldPos;// mul(float4(input[i].position.xyz, 1.0f), WorldMatrix);

        //float4 position = input[i].position;
        //position = mul(position, WorldMatrix);
        //position = mul(position, transMatrix);
        //position = mul(position, ViewMatrix);
        //GSOutput.position = mul(position, ProjMatrix);
        //GSOutput.tex = input[i].tex;
        //GSOutput.texType.x = instanceInfos[InstanceID].InstanceTexture;
        //GSOutput.texType.y = 0;
        //GSOutput.binormal = input[i].binormal;
        //GSOutput.color = input[i].color;
        //GSOutput.normal = input[i].normal;
        //GSOutput.tangent = input[i].tangent;
        //GSOutput.worldPos = mul(float4(input[i].position.xyz, 1.0f), WorldMatrix);

       // TriangleOutputStream.Append(GSOutput);
    }



    //TriangleOutputStream.RestartStrip();
}


VS_OUTPUT VSMain( in VS_INPUT input )
{
	VS_OUTPUT VSOutput;

    VSOutput.position = mul(float4(input.position, 1.0f), WorldViewProjMatrix);

    VSOutput.worldPos = mul(float4(input.position, 1.0f), WorldMatrix);
    VSOutput.normal = mul(normalize(input.normal), WorldMatrix);

    VSOutput.tangent = mul(normalize(input.tangent), WorldMatrix);
    VSOutput.binormal = mul(normalize(input.binormal), WorldMatrix);

    VSOutput.color = input.color;
    VSOutput.tex = input.tex;

 //   VSOutput.worldPos = mul(float4(input.position, 1.0f), WorldMatrix);
 //   VSOutput.normal = mul(normalize(input.normal), WorldMatrix);

 //   VSOutput.tangent = mul(normalize(input.tangent), WorldMatrix);
 //   VSOutput.binormal = mul(normalize(input.binormal), WorldMatrix);

 //  // VSOutput.position = mul(float4(input.position, 1.0f), WorldViewProjMatrix);
	//VSOutput.position = float4(input.position, 1);
	//VSOutput.tex = input.tex;

	return VSOutput;
}


float4 PSMain(in GS_INPUTOUTPUT input) : SV_Target
{


    //float3 bump = 4 * (BumpTexture.Sample(TextureSampler, input.tex) - 0.5f);
    //float3 normalVector = input.normal + bump.x * input.tangent + bump.y * input.binormal;
    //normalVector = normalize(normalVector);

    float3 normalVector = normalize(input.normal);

    float3 worldPosition = input.worldPos.xyz;


    float3 pointlightIntensity = float3(0.0f, 0.0f, 0.0f);
    float3 directionallightIntensity = float3(0.0f, 0.0f, 0.0f);
    float3 spotlightIntensity = float3(0.0f, 0.0f, 0.0f);
    float3 lightIntensity = float3(0.0f, 0.0f, 0.0f);

	// Compute Lights
    for (int i = 0; i < 10; i++)
    {

        pointlightIntensity += calculatePointLight(Lights[i], normalVector, worldPosition);
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



	float4 pixelcolour;
	//int texType = round(input.texType.x);
    pixelcolour = DiffuseTexture1.Sample(TextureSampler, input.tex);// * lightIntensity4f;
	//if (texType == 0)
	//{
 //       pixelcolour = DiffuseTexture1.Sample(TextureSampler, input.tex) * lightIntensity4f;
 //   }
	//if (texType == 1)
	//{
 //       pixelcolour = DiffuseTexture2.Sample(TextureSampler, input.tex) * lightIntensity4f;
 //   }
	//if (texType == 2)
	//{
 //       pixelcolour = DiffuseTexture3.Sample(TextureSampler, input.tex) * lightIntensity4f;
 //   }
	//if (texType == 3)
	//{
 //       pixelcolour = DiffuseTexture4.Sample(TextureSampler, input.tex) * lightIntensity4f;
 //   }

	return pixelcolour;;
}

