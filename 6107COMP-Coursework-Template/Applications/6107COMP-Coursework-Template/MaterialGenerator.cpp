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

struct PointLightInfo
{
	Vector4f PointLightColour;
	Vector4f PointLightPosition;
	Vector4f PointLightRange;
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

MaterialPtr MaterialGenerator::createLitBumpTexturedMaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile, std::wstring bumpTextureFile)
{
	MaterialPtr tMaterial = MaterialGenerator::createTextureMaterial(pRenderer, std::wstring(L"RRTPhongMultiLitBumpTexture.hlsl"), diffuseTextureFile);

	ResourcePtr tBumpTexture = RendererDX11::Get()->LoadTexture(bumpTextureFile);
	tMaterial->Parameters.SetShaderResourceParameter(L"BumpTexture", tBumpTexture);

	MaterialGenerator::setLightToMaterial(pRenderer, tMaterial);






	return tMaterial;
}



void MaterialGenerator::setLightToMaterial(RendererDX11& pRenderer, MaterialPtr material)
{
	PointLightInfo PointLights[2];

	PointLights[0].PointLightColour = Vector4f(0.0f, 1.0f, 0.9f, 1.0f);
	PointLights[0].PointLightPosition = Vector4f(100.0f, 0.0f, 100.0f, 1.0f);
	PointLights[0].PointLightRange = Vector4f(120.0f, 0.0f, 0.0f, 0.0f);

	PointLights[1].PointLightColour = Vector4f(0.4f, 0.0f, 0.1f, 1.0f);
	PointLights[1].PointLightPosition = Vector4f(300.0f, 0.0f, 100.0f, 0.0f);
	PointLights[1].PointLightRange = Vector4f(120.0f, 0.0f, 0.0f, 0.0f);

	Vector4f m_vSurfaceConstants = Vector4f(0.0f, 1.0f, 1.0f, 20.0f);
	Vector4f m_vSurfaceEmissiveColour = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	Vector4f m_vDirectionalLightColour = Vector4f(0.1f, 0.1f, 0.1f, 1.0f);
	Vector3f m_vDirectionalLightDirection = Vector3f(0.0f, 0.0f, 1.0f);

	Vector4f m_vSpotLightColour = Vector4f(0.1f, 0.1f, 0.1f, 1.0f);
	Vector3f m_vSpotLightDirection = Vector3f(-1.0f, 0.0f, 0.0f);
	Vector4f m_vSpotLightPosition = Vector4f(100.0f, 0.0f, 0.0f, 1.0f);
	Vector4f m_vSpotLightRange = Vector4f(100.0f, 0.0f, 0.0f, 0.0f);
	Vector4f m_vSpotLightFocus = Vector4f(100.0f, 0.0f, 0.0f, 0.0f);

	Vector4f m_vPointLightColour = Vector4f(1.0f, 1.0f, 0.9f, 1.0f);
	Vector4f m_vPointLightPosition = Vector4f(100.0f, 0.0f, 100.0f, 1.0f);
	Vector4f m_vPointLightRange = Vector4f(120.0f, 0.0f, 0.0f, 0.0f);

	material->Parameters.SetVectorParameter(L"SurfaceConstants", m_vSurfaceConstants);
	material->Parameters.SetVectorParameter(L"SurfaceEmissiveColour", m_vSurfaceEmissiveColour);

	m_vDirectionalLightDirection.Normalize();
	material->Parameters.SetVectorParameter(L"DirectionalLightColour", m_vDirectionalLightColour);
	material->Parameters.SetVectorParameter(L"DirectionalLightDirection", Vector4f(m_vDirectionalLightDirection, 1.0f));

	m_vSpotLightDirection.Normalize();
	material->Parameters.SetVectorParameter(L"SpotLightColour", m_vSpotLightColour);
	material->Parameters.SetVectorParameter(L"SpotLightDirection", Vector4f(m_vSpotLightDirection, 1.0f));
	material->Parameters.SetVectorParameter(L"SpotLightPosition", m_vSpotLightPosition);
	material->Parameters.SetVectorParameter(L"SpotLightRange", m_vSpotLightRange);
	material->Parameters.SetVectorParameter(L"SpotLightFocus", m_vSpotLightFocus);

	material->Parameters.SetVectorParameter(L"PointLightColour", m_vPointLightColour);
	material->Parameters.SetVectorParameter(L"PointLightPosition", m_vPointLightPosition);
	material->Parameters.SetVectorParameter(L"PointLightRange", m_vPointLightRange);

	//Vector4f* array = new Vector4f[10];
	//array[0] = Vector4f(1.0f, 1.0f, 0.9f, 0.0f);

	BufferConfigDX11 tBuffConfig;
	tBuffConfig.SetDefaultConstantBuffer(10 * sizeof(Vector4f), false);


	Vector4f a = Vector4f(0.0f, 1.0f, 0.9f, 1.0f);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &PointLights;
	//data.pSysMem = &a;

	ResourcePtr lights = pRenderer.CreateConstantBuffer(&tBuffConfig, &data);

	material->Parameters.SetConstantBufferParameter(L"BufferPointLights", lights);
	//material->Parameters.SetVectorParameter(L"testVector", a);

}
