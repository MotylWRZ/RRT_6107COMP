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

#define LIGHT_NONE			 0
#define LIGHT_POINT         1
#define LIGHT_SPOT          2
#define LIGHT_DIRECTIONAL   3

struct Light
{
	Vector4f iLightType = Vector4f(LIGHT_NONE, 0.0f, 0.0f, 0.0f);
	Vector4f LightColour = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f LightPosition = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f LightDirection = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f LightRange = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f LightFocus = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
};

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

MaterialPtr MaterialGenerator::createLitBumpTexturedMaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile, std::wstring bumpTextureFile, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo)
{
	MaterialPtr tMaterial = MaterialGenerator::createTextureMaterial(pRenderer, std::wstring(L"RRTPhongMultiLitBumpTextureV2.hlsl"), diffuseTextureFile);

	ResourcePtr tBumpTexture = RendererDX11::Get()->LoadTexture(bumpTextureFile);
	tMaterial->Parameters.SetShaderResourceParameter(L"BumpTexture", tBumpTexture);

	MaterialGenerator::setLightToMaterial(pRenderer, tMaterial, lights, MatReflectanceInfo);

	return tMaterial;
}

void MaterialGenerator::setLightToMaterial(RendererDX11& pRenderer, MaterialPtr material, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo)
{
	if (!material)
	{
		return;
	}
	LightInfo tLights[LIGHTS_NUM_MAX];

	// Check if the maximum number of lights is not exceeded
	if (lights.size() > LIGHTS_NUM_MAX)
	{
		assert(!"MaterialGenerator:Number of lights exeeds the maximum number of lights");
	}

	// Load Lights info structs
	for (int i = 0; i < lights.size(); i++)
	{
		tLights[i] = lights[i]->getLightInfo();// tVecLights[i]->getLightInfo();
	}

	Vector4f tSurfaceConstants = MatReflectanceInfo.SurfaceEmissiveColour;
	Vector4f tSurfaceEmissiveColour = Vector4f(MatReflectanceInfo.Ambient, MatReflectanceInfo.Diffuse, MatReflectanceInfo.Specular, MatReflectanceInfo.Shininess);

	material->Parameters.SetVectorParameter(L"SurfaceConstants", tSurfaceConstants);
	material->Parameters.SetVectorParameter(L"SurfaceEmissiveColour", tSurfaceEmissiveColour);

	BufferConfigDX11 tBuffConfig;
	tBuffConfig.SetDefaultConstantBuffer(LIGHTS_NUM_MAX * sizeof(LightInfo), false);

	D3D11_SUBRESOURCE_DATA dataLights;
	dataLights.pSysMem = &tLights;

	ResourcePtr resLights = pRenderer.CreateConstantBuffer(&tBuffConfig, &dataLights);

	material->Parameters.SetConstantBufferParameter(L"cLights", resLights);
}

void MaterialGenerator::updateMaterialLight(RendererDX11& pRenderer, MaterialPtr material, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo)
{
	if (!material)
	{
		return;
	}
	LightInfo tLights[LIGHTS_NUM_MAX];

	// Check if the maximum number of lights is not exceeded
	if (lights.size() > LIGHTS_NUM_MAX)
	{
		assert(!"MaterialGenerator:Number of lights exeeds the maximum number of lights");
	}

	// Load Lights info structs
	for (int i = 0; i < lights.size(); i++)
	{
		tLights[i] = lights[i]->getLightInfo();// tVecLights[i]->getLightInfo();
	}

	Vector4f m_vSurfaceConstants = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	Vector4f m_vSurfaceEmissiveColour = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	material->Parameters.SetVectorParameter(L"SurfaceConstants", m_vSurfaceConstants);
	material->Parameters.SetVectorParameter(L"SurfaceEmissiveColour", m_vSurfaceEmissiveColour);

	BufferConfigDX11 tBuffConfig;
	tBuffConfig.SetDefaultConstantBuffer(LIGHTS_NUM_MAX * sizeof(LightInfo), false);

	int a = 1;

	D3D11_SUBRESOURCE_DATA dataLights;
	dataLights.pSysMem = &tLights;

	ResourcePtr resLights = pRenderer.CreateConstantBuffer(&tBuffConfig, &dataLights);


	material->Parameters.SetConstantBufferParameter(L"cLights", resLights);
}

MaterialPtr MaterialGenerator::createGSInstancingMaterial(RendererDX11& renderer, std::wstring diffuseTextureFile)
{
	MaterialPtr tNewMaterial = MaterialPtr(new MaterialDX11());
	RenderEffectDX11* tEffect = new RenderEffectDX11();

	tEffect->SetVertexShader(renderer.LoadShader(ShaderType::VERTEX_SHADER,
		std::wstring(L"RTR_Instancing.hlsl"),
		std::wstring(L"VSMain"),
		std::wstring(L"vs_4_0"),
		true));

	tEffect->SetPixelShader(renderer.LoadShader(ShaderType::PIXEL_SHADER,
		std::wstring(L"RTR_Instancing.hlsl"),
		std::wstring(L"PSMain"),
		std::wstring(L"ps_4_0"),
		true));

	tEffect->SetGeometryShader(renderer.LoadShader(ShaderType::GEOMETRY_SHADER,
		std::wstring(L"RTR_Instancing.hlsl"),
		std::wstring(L"GSMain"),
		std::wstring(L"gs_5_0")));

	ResourcePtr tTexture = RendererDX11::Get()->LoadTexture(std::wstring(L"mars.tif"));
	tNewMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture", tTexture);

	Vector4f tInstancePos1(200.0f, 50.0f, 100.0f, 1.0f);
	Vector4f tInstancePos2(300.0f, 50.0f, 100.0f, 1.0f);
	Vector4f tInstancePos3(400.0f, 50.0f, 100.0f, 1.0f);
	Vector4f tInstancePos4(500.0f, 50.0f, 100.0f, 1.0f);

	tNewMaterial->Parameters.SetVectorParameter(L"instancePosition1", tInstancePos1);
	tNewMaterial->Parameters.SetVectorParameter(L"instancePosition2", tInstancePos2);
	tNewMaterial->Parameters.SetVectorParameter(L"instancePosition3", tInstancePos3);
	tNewMaterial->Parameters.SetVectorParameter(L"instancePosition4", tInstancePos4);


	//D3D11_SAMPLER_DESC tSamplerConfig;
	SamplerStateConfigDX11 tSamplerConfig;
	tSamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	tSamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	tSamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	tSamplerConfig.MaxAnisotropy = 0;

	int tTextureSampler = RendererDX11::Get()->CreateSamplerState(&tSamplerConfig);
	tNewMaterial->Parameters.SetSamplerParameter(L"TextureSampler", tTextureSampler);

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
		blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
		blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	tEffect->m_iBlendState = RendererDX11::Get()->CreateBlendState(&blendConfig);

	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].bRender = true;
	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].pEffect = tEffect;

	return tNewMaterial;
}

MaterialPtr MaterialGenerator::createGSInstancingMultiTextureMaterial(RendererDX11& renderer, std::wstring diffuseTextureFile, std::wstring bumpTextureFile)
{
	MaterialPtr tNewMaterial = MaterialPtr(new MaterialDX11());
	RenderEffectDX11* tEffect = new RenderEffectDX11();

	tEffect->SetVertexShader(renderer.LoadShader(ShaderType::VERTEX_SHADER,
		std::wstring(L"RTR_MultiTextureInstancing.hlsl"),
		std::wstring(L"VSMain"),
		std::wstring(L"vs_4_0"),
		true));

	tEffect->SetPixelShader(renderer.LoadShader(ShaderType::PIXEL_SHADER,
		std::wstring(L"RTR_MultiTextureInstancing.hlsl"),
		std::wstring(L"PSMain"),
		std::wstring(L"ps_4_0"),
		true));

	tEffect->SetGeometryShader(renderer.LoadShader(ShaderType::GEOMETRY_SHADER,
		std::wstring(L"RTR_MultiTextureInstancing.hlsl"),
		std::wstring(L"GSMain"),
		std::wstring(L"gs_5_0")));

	ResourcePtr tTexture1 = RendererDX11::Get()->LoadTexture(std::wstring(L"earth.tif"));
	ResourcePtr tTexture2 = RendererDX11::Get()->LoadTexture(std::wstring(L"mars.tif"));
	ResourcePtr tTexture3 = RendererDX11::Get()->LoadTexture(std::wstring(L"TerrainGrass.tif"));
	ResourcePtr tTexture4 = RendererDX11::Get()->LoadTexture(std::wstring(L"SnowScuffedGround.tif"));

	tNewMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture1", tTexture1);
	tNewMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture2", tTexture2);
	tNewMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture3", tTexture3);
	tNewMaterial->Parameters.SetShaderResourceParameter(L"DiffuseTexture4", tTexture4);

	Vector4f tInstancePos1(30.0f, 30.0f, 30.0f, 1.0f);
	Vector4f tInstancePos2(30.0f, -30.0f, 30.0f, 1.0f);
	Vector4f tInstancePos3(30.0f, 30.0f, -30.0f, 1.0f);
	Vector4f tInstancePos4(30.0f, -30.0f, -30.0f, 1.0f);

	tNewMaterial->Parameters.SetVectorParameter(L"instancePosition1", tInstancePos1);
	tNewMaterial->Parameters.SetVectorParameter(L"instancePosition2", tInstancePos2);
	tNewMaterial->Parameters.SetVectorParameter(L"instancePosition3", tInstancePos3);
	tNewMaterial->Parameters.SetVectorParameter(L"instancePosition4", tInstancePos4);


	//D3D11_SAMPLER_DESC tSamplerConfig;
	SamplerStateConfigDX11 tSamplerConfig;
	tSamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	tSamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	tSamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	tSamplerConfig.MaxAnisotropy = 0;

	int tTextureSampler = RendererDX11::Get()->CreateSamplerState(&tSamplerConfig);
	tNewMaterial->Parameters.SetSamplerParameter(L"TextureSampler", tTextureSampler);

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
		blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
		blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	tEffect->m_iBlendState = RendererDX11::Get()->CreateBlendState(&blendConfig);

	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].bRender = true;
	tNewMaterial->Params[VIEWTYPE::VT_PERSPECTIVE].pEffect = tEffect;

	return tNewMaterial;
}
