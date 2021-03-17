#include "MaterialGenerator.h"

#include "SamplerStateConfigDX11.h"

//Include the Logging System
#include "Log.h"

//Include the DirectX Rendering Components
#include "PipelineManagerDX11.h"
#include "BlendStateConfigDX11.h"
#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "MaterialGeneratorDX11.h"

#include "Light_Directional.h"
#include "Light_Point.h"
#include "Light_Spot.h"

//#define LIGHT_TYPE_NONE			 0
//#define LIGHT_TYPE_POINT         1
//#define LIGHT_TYPE_SPOT          2
//#define LIGHT_TYPE_DIRECTIONAL   3
//
//struct Light
//{
//	Vector4f iLightType = Vector4f(LIGHT_TYPE_NONE, 0.0f, 0.0f, 0.0f);
//	Vector4f LightColour = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
//	Vector4f LightPosition = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
//	Vector4f LightDirection = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
//	Vector4f LightRange = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
//	Vector4f LightFocus = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
//};

struct PointLightInfo
{
	Vector4f PointLightColour;
	Vector4f PointLightPosition;
	Vector4f PointLightRange;
};

struct DirectionalLightInfo
{
	Vector4f DirectionalLightColour;
	Vector4f DirectionalLightDirection;
};

struct SpotLightInfo
{
	Vector4f SpotLightColour;
	Vector4f SpotLightPosition;
	Vector4f SpotLightDirection;
	Vector4f SpotLightRange;
	Vector4f SpotLightFocus;
};


MaterialGenerator::MaterialGenerator()
{
}

MaterialGenerator::~MaterialGenerator()
{
}

MaterialPtr MaterialGenerator::createBasicMaterial(RendererDX11& pRenderer)
{
	MaterialPtr tNewMaterial = MaterialPtr(new MaterialDX11());
	RenderEffectDX11* tEffect = new RenderEffectDX11();

	// Setup shader here
	tEffect->SetVertexShader(pRenderer.LoadShader(ShaderType::VERTEX_SHADER,
		std::wstring(L"Basic.hlsl"),
		std::wstring(L"VSMain"),
		std::wstring(L"vs_4_0"),
		true));

	tEffect->SetPixelShader(pRenderer.LoadShader(ShaderType::PIXEL_SHADER,
		std::wstring(L"Basic.hlsl"),
		std::wstring(L"PSMain"),
		std::wstring(L"ps_4_0"),
		true));

	DepthStencilStateConfigDX11 tDSConfig;
	int tDepthStencilState = pRenderer.CreateDepthStencilState(&tDSConfig);

	if (tDepthStencilState == -1)
	{
		Log::Get().Write(L"Failed to create light depth stencil state");
		assert(false);
	}

	tEffect->m_iDepthStencilState = tDepthStencilState;
	tEffect->m_uStencilRef = tDepthStencilState;

	BlendStateConfigDX11 blendConfig;
	blendConfig.AlphaToCoverageEnable = false;
	blendConfig.IndependentBlendEnable = false;

	for (int i = 0; i < 8; ++i)
	{
		blendConfig.RenderTarget[i].BlendEnable = true;
		blendConfig.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendConfig.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendConfig.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	tEffect->m_iBlendState = RendererDX11::Get()->CreateBlendState(&blendConfig);

	RasterizerStateConfigDX11 rsConfig;
	rsConfig.CullMode = D3D11_CULL_BACK;
	int iRasterizerState = pRenderer.CreateRasterizerState(&rsConfig);

	if (iRasterizerState == -1)
	{
		Log::Get().Write(L"Failed to create light ratserizer state");
		assert(false);
	}

	tEffect->m_iRasterizerState = iRasterizerState;

	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].bRender = true;
	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].pEffect = tEffect;



	return tNewMaterial;
}

MaterialPtr MaterialGenerator::createTextureMaterial(RendererDX11& pRenderer, std::wstring shaderFile, std::wstring textureFile)
{

	MaterialPtr tNewMaterial = MaterialPtr(new MaterialDX11());
	RenderEffectDX11* tEffect = new RenderEffectDX11();

	tEffect->SetVertexShader(pRenderer.LoadShader(ShaderType::VERTEX_SHADER,
		std::wstring(shaderFile),
		std::wstring(L"VSMain"),
		std::wstring(L"vs_4_0"),
		true));

	tEffect->SetPixelShader(pRenderer.LoadShader(ShaderType::PIXEL_SHADER,
		std::wstring(shaderFile),
		std::wstring(L"PSMain"),
		std::wstring(L"ps_4_0"),
		true));

	ResourcePtr tTexture = RendererDX11::Get()->LoadTexture(textureFile);
	tNewMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture", tTexture);

	//this->m_pRenderer11->m_pParamMgr->SetVectorParameter(std::wstring(L"TextureBoundary"), &m_vTextureBoundary);
//	tNewMaterial->Parameters.SetVectorParameter(std::wstring(L"TextureBoundary"), m_vTextureBoundary);

	//D3D11_SAMPLER_DESC tSamplerConfig;
	SamplerStateConfigDX11 tSamplerConfig;
	tSamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	tSamplerConfig.MaxAnisotropy = 0;

	int tTextureSampler = RendererDX11::Get()->CreateSamplerState(&tSamplerConfig);
	tNewMaterial->Parameters.SetSamplerParameter(L"TextureSampler", tTextureSampler);

	DepthStencilStateConfigDX11 tDSConfig;
	int tDepthStencilState = pRenderer.CreateDepthStencilState(&tDSConfig);

	if (tDepthStencilState == -1)
	{
		Log::Get().Write(L"Failed to create light depth stencil state");
		assert(false);
	}

	tEffect->m_iDepthStencilState = tDepthStencilState;
	tEffect->m_uStencilRef = tDepthStencilState;

	BlendStateConfigDX11 blendConfig;
	blendConfig.AlphaToCoverageEnable = false;
	blendConfig.IndependentBlendEnable = false;

	for (int i = 0; i < 8; ++i)
	{
		blendConfig.RenderTarget[i].BlendEnable = true;
		blendConfig.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendConfig.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendConfig.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	tEffect->m_iBlendState = RendererDX11::Get()->CreateBlendState(&blendConfig);

	RasterizerStateConfigDX11 rsConfig;
	rsConfig.CullMode = D3D11_CULL_BACK;
	int iRasterizerState = pRenderer.CreateRasterizerState(&rsConfig);

	if (iRasterizerState == -1)
	{
		Log::Get().Write(L"Failed to create light ratserizer state");
		assert(false);
	}

	tEffect->m_iRasterizerState = iRasterizerState;

	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].bRender = true;
	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].pEffect = tEffect;

	return tNewMaterial;
}

MaterialPtr MaterialGenerator::createMultiTextureMaterial(RendererDX11& pRenderer, std::wstring shaderFile, std::wstring textureFile1, std::wstring textureFile2)
{
	MaterialPtr tNewMaterial = MaterialPtr(new MaterialDX11());
	RenderEffectDX11* tEffect = new RenderEffectDX11();

	tEffect->SetVertexShader(pRenderer.LoadShader(ShaderType::VERTEX_SHADER,
		std::wstring(shaderFile),
		std::wstring(L"VSMain"),
		std::wstring(L"vs_4_0"),
		true));

	tEffect->SetPixelShader(pRenderer.LoadShader(ShaderType::PIXEL_SHADER,
		std::wstring(shaderFile),
		std::wstring(L"PSMain"),
		std::wstring(L"ps_4_0"),
		true));



	ResourcePtr tTexture1 = RendererDX11::Get()->LoadTexture(textureFile1);
	ResourcePtr tTexture2 = RendererDX11::Get()->LoadTexture(textureFile2);
	tNewMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture1", tTexture1);
	tNewMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture2", tTexture2);


	//this->m_pRenderer11->m_pParamMgr->SetVectorParameter(std::wstring(L"TextureBoundary"), &m_vTextureBoundary);
//	tNewMaterial->Parameters.SetVectorParameter(std::wstring(L"TextureBoundary"), m_vTextureBoundary);

	//D3D11_SAMPLER_DESC tSamplerConfig;
	SamplerStateConfigDX11 tSamplerConfig;
	tSamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	tSamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	tSamplerConfig.MaxAnisotropy = 0;

	int tTextureSampler = RendererDX11::Get()->CreateSamplerState(&tSamplerConfig);
	tNewMaterial->Parameters.SetSamplerParameter(L"TextureSampler", tTextureSampler);

	DepthStencilStateConfigDX11 tDSConfig;
	int tDepthStencilState = pRenderer.CreateDepthStencilState(&tDSConfig);

	if (tDepthStencilState == -1)
	{
		Log::Get().Write(L"Failed to create light depth stencil state");
		assert(false);
	}

	tEffect->m_iDepthStencilState = tDepthStencilState;
	tEffect->m_uStencilRef = tDepthStencilState;

	BlendStateConfigDX11 blendConfig;
	blendConfig.AlphaToCoverageEnable = false;
	blendConfig.IndependentBlendEnable = false;

	for (int i = 0; i < 8; ++i)
	{
		blendConfig.RenderTarget[i].BlendEnable = true;
		blendConfig.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendConfig.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendConfig.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	tEffect->m_iBlendState = RendererDX11::Get()->CreateBlendState(&blendConfig);

	RasterizerStateConfigDX11 rsConfig;
	rsConfig.CullMode = D3D11_CULL_BACK;
	int iRasterizerState = pRenderer.CreateRasterizerState(&rsConfig);

	if (iRasterizerState == -1)
	{
		Log::Get().Write(L"Failed to create light ratserizer state");
		assert(false);
	}

	tEffect->m_iRasterizerState = iRasterizerState;

	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].bRender = true;
	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].pEffect = tEffect;

	return tNewMaterial;
}

MaterialPtr MaterialGenerator::createTerrainMultiTextureMaterial(RendererDX11& pRenderer, std::wstring highlandsTextureFile, std::wstring lowlandsTextureFile)
{
	return MaterialGenerator::createMultiTextureMaterial(pRenderer, std::wstring(L"RRTTerrainMultiTextureMapping.hlsl"), highlandsTextureFile, lowlandsTextureFile);
}

MaterialPtr MaterialGenerator::createLitTexturedMaterial(RendererDX11& pRenderer, std::wstring textureFile1)
{
	MaterialPtr tMaterial = MaterialGenerator::createTextureMaterial(pRenderer, std::wstring(L"RRTPhongLitTexture.hlsl"), textureFile1);

	MaterialGenerator::setLightToMaterial(pRenderer, tMaterial);

	return  tMaterial;
}

MaterialPtr MaterialGenerator::createLitBumpTexturedMaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile, std::wstring bumpTextureFile, const std::vector<Light_Base*>& lights)
{
	//MaterialPtr tMaterial = MaterialGenerator::createTextureMaterial(pRenderer, std::wstring(L"RRTPhongMultiLitBumpTexture.hlsl"), diffuseTextureFile);
	MaterialPtr tMaterial = MaterialGenerator::createTextureMaterial(pRenderer, std::wstring(L"RRTPhongMultiLitBumpTextureV2.hlsl"), diffuseTextureFile);

	ResourcePtr tBumpTexture = RendererDX11::Get()->LoadTexture(bumpTextureFile);
	tMaterial->Parameters.SetShaderResourceParameter(L"BumpTexture", tBumpTexture);

	//MaterialGenerator::setLightToMaterial(pRenderer, tMaterial);
	//MaterialGenerator::setLightToMaterialV2(pRenderer, tMaterial);
	MaterialGenerator::setLightToMaterialV3(pRenderer, tMaterial, lights);

	return tMaterial;
}

void MaterialGenerator::setLightToMaterial(RendererDX11& pRenderer, MaterialPtr material)
{
	PointLightInfo PointLights[2];
	DirectionalLightInfo DirectionalLight;
	SpotLightInfo SpotLights[2];

	PointLights[0].PointLightColour = Vector4f(0.0f, 1.0f, 0.9f, 1.0f);
	PointLights[0].PointLightPosition = Vector4f(100.0f, 0.0f, 300.0f, 1.0f);
	PointLights[0].PointLightRange = Vector4f(100.0f, 0.0f, 0.0f, 0.0f);

	PointLights[1].PointLightColour = Vector4f(0.4f, 0.0f, 0.1f, 1.0f);
	PointLights[1].PointLightPosition = Vector4f(300.0f, 0.0f, 300.0f, 0.0f);
	PointLights[1].PointLightRange = Vector4f(200.0f, 0.0f, 0.0f, 0.0f);

	DirectionalLight.DirectionalLightColour = Vector4f(0.1f, 0.1f, 0.1f, 1.0f);
	DirectionalLight.DirectionalLightDirection = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
	DirectionalLight.DirectionalLightDirection.Normalize();


	SpotLights[0].SpotLightColour = Vector4f(0.1f, 1.0f, 0.1f, 1.0f);
	SpotLights[0].SpotLightDirection = Vector4f(0.0f, -1.0f, 0.0f, 1.0f);
	SpotLights[0].SpotLightPosition = Vector4f(200.0f, 50.0f, 100.0f, 1.0f);
	SpotLights[0].SpotLightRange = Vector4f(100.0f, 0.0f, 0.0f, 0.0f);
	SpotLights[0].SpotLightFocus = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
	SpotLights[0].SpotLightDirection.Normalize();

	SpotLights[1].SpotLightColour = Vector4f(1.0f, 0.1f, 0.1f, 1.0f);
	SpotLights[1].SpotLightDirection = Vector4f(0.0f, -1.0f, 0.0f, 1.0f);
	SpotLights[1].SpotLightPosition = Vector4f(400.0f, 50.0f, 100.0f, 1.0f);
	SpotLights[1].SpotLightRange = Vector4f(100.0f, 0.0f, 0.0f, 0.0f);
	SpotLights[1].SpotLightFocus = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
	SpotLights[1].SpotLightDirection.Normalize();

	Vector4f m_vSurfaceConstants = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	Vector4f m_vSurfaceEmissiveColour = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	material->Parameters.SetVectorParameter(L"SurfaceConstants", m_vSurfaceConstants);
	material->Parameters.SetVectorParameter(L"SurfaceEmissiveColour", m_vSurfaceEmissiveColour);

	BufferConfigDX11 tBuffConfig;
	tBuffConfig.SetDefaultConstantBuffer(2 * sizeof(PointLightInfo), false);

	BufferConfigDX11 tBuffConfig2;
	tBuffConfig2.SetDefaultConstantBuffer(sizeof(DirectionalLightInfo), false);

	BufferConfigDX11 tBuffConfig3;
	tBuffConfig3.SetDefaultConstantBuffer(2 * sizeof(SpotLightInfo), false);

	D3D11_SUBRESOURCE_DATA dataPointLights;
	D3D11_SUBRESOURCE_DATA dataDirectionalLights;
	D3D11_SUBRESOURCE_DATA dataSpotLights;
	dataPointLights.pSysMem = PointLights;
	dataDirectionalLights.pSysMem = &DirectionalLight;
	dataSpotLights.pSysMem = SpotLights;

	ResourcePtr pointLights = pRenderer.CreateConstantBuffer(&tBuffConfig, &dataPointLights);
	ResourcePtr directionalLights = pRenderer.CreateConstantBuffer(&tBuffConfig2, &dataDirectionalLights);
	ResourcePtr spotLights = pRenderer.CreateConstantBuffer(&tBuffConfig3, &dataSpotLights);

	material->Parameters.SetConstantBufferParameter(L"PointLights", pointLights);
	material->Parameters.SetConstantBufferParameter(L"DirectionalLight", directionalLights);
	material->Parameters.SetConstantBufferParameter(L"SpotLights", spotLights);
}

void MaterialGenerator::setLightToMaterialV2(RendererDX11& pRenderer, MaterialPtr material)
{
	/*Light Lights[100];

	for (int i = 0; i < tVecLights.size(); i++)
	{
		Lights[i] = tVecLights[i];
	}

	Lights[0].iLightType.x = LIGHT_TYPE_POINT;
	Lights[0].LightColour = Vector4f(0.0f, 1.0f, 0.9f, 1.0f);
	Lights[0].LightPosition = Vector4f(200.0f, 0.0f, 300.0f, 1.0f);
	Lights[0].LightRange = Vector4f(100.0f, 0.0f, 0.0f, 0.0f);

	Lights[1].iLightType.x = LIGHT_TYPE_POINT;
	Lights[1].LightColour = Vector4f(1.0f, 0.0f, 0.9f, 1.0f);
	Lights[1].LightPosition = Vector4f(300.0f, 0.0f, 300.0f, 1.0f);
	Lights[1].LightRange = Vector4f(200.0f, 0.0f, 0.0f, 0.0f);

	Lights[2].iLightType.x = LIGHT_TYPE_DIRECTIONAL;
	Lights[2].LightColour = Vector4f(0.1f, 0.1f, 0.7f, 1.0f);
	Lights[2].LightDirection = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
	Lights[2].LightDirection.Normalize();

	Vector4f m_vSurfaceConstants = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	Vector4f m_vSurfaceEmissiveColour = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	material->Parameters.SetVectorParameter(L"SurfaceConstants", m_vSurfaceConstants);
	material->Parameters.SetVectorParameter(L"SurfaceEmissiveColour", m_vSurfaceEmissiveColour);

	BufferConfigDX11 tBuffConfig;
	tBuffConfig.SetDefaultConstantBuffer(10 * sizeof(Light), false);

	D3D11_SUBRESOURCE_DATA dataLights;
	dataLights.pSysMem = Lights;


	ResourcePtr resLights = pRenderer.CreateConstantBuffer(&tBuffConfig, &dataLights);

	material->Parameters.SetConstantBufferParameter(L"cLights", resLights);*/
}

void MaterialGenerator::setLightToMaterialV3(RendererDX11& pRenderer, MaterialPtr material, const std::vector<Light_Base*>& lights)
{
	/*std::vector<Light_Base*> tVecLights;

	Light_Base* tPointLight = new Light_Point(Vector4f(0.0f, 1.0f, 0.9f, 1.0f)
		, Vector3f(200.0f, 0.0f, 300.0f)
		, Vector2f(100.0f, 0.0f));

	tVecLights.push_back(tPointLight);*/

	LightInfo tLights[100];

	for (int i = 0; i < lights.size(); i++)
	{
		tLights[i] = lights[i]->getLightInfo();// tVecLights[i]->getLightInfo();
	}

	Vector4f m_vSurfaceConstants = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	Vector4f m_vSurfaceEmissiveColour = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	material->Parameters.SetVectorParameter(L"SurfaceConstants", m_vSurfaceConstants);
	material->Parameters.SetVectorParameter(L"SurfaceEmissiveColour", m_vSurfaceEmissiveColour);

	BufferConfigDX11 tBuffConfig;
	tBuffConfig.SetDefaultConstantBuffer(10 * sizeof(LightInfo), false);

	D3D11_SUBRESOURCE_DATA dataLights;
	dataLights.pSysMem = tLights;


	ResourcePtr resLights = pRenderer.CreateConstantBuffer(&tBuffConfig, &dataLights);

	material->Parameters.SetConstantBufferParameter(L"cLights", resLights);
}

void MaterialGenerator::setLightToMaterialPrev(RendererDX11& pRenderer, MaterialPtr material)
{
	Vector4f m_vSurfaceConstants = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	Vector4f m_vSurfaceEmissiveColour = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	Vector4f m_vDirectionalLightColour = Vector4f(0.1f, 0.1f, 0.1f, 1.0f);
	Vector3f m_vDirectionalLightDirection = Vector3f(0.0f, 0.0f, 1.0f);

	Vector4f m_vSpotLightColour = Vector4f(0.1f, 1.0f, 0.1f, 1.0f);
	Vector3f m_vSpotLightDirection = Vector3f(0.0f, -1.0f, 0.0f);
	Vector4f m_vSpotLightPosition = Vector4f(200.0f, 50.0f, 100.0f, 1.0f);
	Vector4f m_vSpotLightRange = Vector4f(30000.0f, 0.0f, 0.0f, 0.0f);
	Vector4f m_vSpotLightFocus = Vector4f(10.0f, 0.0f, 0.0f, 0.0f);

	Vector4f m_vPointLightColour = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
	Vector4f m_vPointLightPosition = Vector4f(200.0f, 50.0f, 100.0f, 1.0f);
	Vector4f m_vPointLightRange = Vector4f(300.0f, 0.0f, 0.0f, 0.0f);


	material->Parameters.SetVectorParameter(L"SurfaceConstants", m_vSurfaceConstants);
	material->Parameters.SetVectorParameter(L"SurfaceEmissiveColour", m_vSurfaceEmissiveColour);

	m_vDirectionalLightDirection.Normalize();
	material->Parameters.SetVectorParameter(L"DirectionalLightColour", m_vDirectionalLightColour);
	material->Parameters.SetVectorParameter(L"DirectionalLightDirection", Vector4f(m_vDirectionalLightDirection, 1.0f));

	m_vSpotLightDirection.Normalize();
	material->Parameters.SetVectorParameter(L"SpotLightColour", m_vSpotLightColour);
	material->Parameters.SetVectorParameter(L"SpotLightDirection", Vector4f(m_vSpotLightDirection, 1.0f));
	material->Parameters.SetVectorParameter(L"SpotLightPosition",m_vSpotLightPosition);
	material->Parameters.SetVectorParameter(L"SpotLightRange", m_vSpotLightRange);
	material->Parameters.SetVectorParameter(L"SpotLightFocus", m_vSpotLightFocus);

	material->Parameters.SetVectorParameter(L"PointLightColour", m_vPointLightColour);
	material->Parameters.SetVectorParameter(L"PointLightPosition", m_vPointLightPosition);
	material->Parameters.SetVectorParameter(L"PointLightRange", m_vPointLightRange);

}
