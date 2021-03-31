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

#include "ConstantBufferDX11.h"

struct BufferData
{
	LightInfo Lights[LIGHTS_NUM_MAX];
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

MaterialPtr MaterialGenerator::createBasicMaterial(RendererDX11& renderer, std::wstring shaderFile)
{
	MaterialPtr tNewMaterial = MaterialPtr(new MaterialDX11());
	RenderEffectDX11* tEffect = new RenderEffectDX11();

	// Setup shader here
	tEffect->SetVertexShader(renderer.LoadShader(ShaderType::VERTEX_SHADER,
		std::wstring(shaderFile),
		std::wstring(L"VSMain"),
		std::wstring(L"vs_4_0"),
		true));

	tEffect->SetPixelShader(renderer.LoadShader(ShaderType::PIXEL_SHADER,
		std::wstring(shaderFile),
		std::wstring(L"PSMain"),
		std::wstring(L"ps_4_0"),
		true));

	DepthStencilStateConfigDX11 tDSConfig;
	int tDepthStencilState = renderer.CreateDepthStencilState(&tDSConfig);

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
	int iRasterizerState = renderer.CreateRasterizerState(&rsConfig);

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

MaterialPtr MaterialGenerator::createMaterialWithGS(RendererDX11& renderer, std::wstring shaderFile)
{
	MaterialPtr tNewMaterial = MaterialPtr(new MaterialDX11());
	RenderEffectDX11* tEffect = new RenderEffectDX11();

	tEffect->SetVertexShader(renderer.LoadShader(ShaderType::VERTEX_SHADER,
		std::wstring(shaderFile),
		std::wstring(L"VSMain"),
		std::wstring(L"vs_4_0"),
		true));

	tEffect->SetPixelShader(renderer.LoadShader(ShaderType::PIXEL_SHADER,
		std::wstring(shaderFile),
		std::wstring(L"PSMain"),
		std::wstring(L"ps_4_0"),
		true));

	tEffect->SetGeometryShader(renderer.LoadShader(ShaderType::GEOMETRY_SHADER,
		std::wstring(shaderFile),
		std::wstring(L"GSMain"),
		std::wstring(L"gs_5_0")));

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

MaterialPtr MaterialGenerator::createTerrainMultiTextureMaterial(RendererDX11& pRenderer, std::wstring highlandsTextureFile, std::wstring lowlandsTextureFile)
{
	return MaterialGenerator::createMultiTextureMaterial(pRenderer, std::wstring(L"RRTTerrainMultiTextureMapping.hlsl"), highlandsTextureFile, lowlandsTextureFile);
}

MaterialPtr MaterialGenerator::createLitBumpTexturedMaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile, std::wstring bumpTextureFile, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo)
{
	MaterialPtr tMaterial = MaterialGenerator::createTextureMaterial(pRenderer, std::wstring(L"RRTPhongMultiLitBumpTextureV2.hlsl"), diffuseTextureFile);
	//MaterialPtr tMaterial = MaterialGenerator::createTextureMaterial(pRenderer, std::wstring(L"RRTPhongLitBumpTextureIncludeTest.hlsl"), diffuseTextureFile);

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
	//tBuffConfig.SetDefaultConstantBuffer(LIGHTS_NUM_MAX * sizeof(LightInfo), false);
	tBuffConfig.SetByteWidth(LIGHTS_NUM_MAX * sizeof(LightInfo));
	tBuffConfig.SetBindFlags(D3D11_BIND_CONSTANT_BUFFER);
	tBuffConfig.SetMiscFlags(0);
	tBuffConfig.SetStructureByteStride(0);
	tBuffConfig.SetUsage(D3D11_USAGE_DYNAMIC);// D3D11_USAGE_DEFAULT);
	tBuffConfig.SetCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);

	BufferData tData;

	for (int i = 0; i < LIGHTS_NUM_MAX; i++)
	{
		tData.Lights[i] = tLights[i];
	}

	D3D11_SUBRESOURCE_DATA dataLights;
	dataLights.pSysMem = &tData;
	dataLights.SysMemPitch = 0;
	dataLights.SysMemSlicePitch = 0;

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
		tLights[i] = lights[i]->getLightInfo();
	}

	material->Parameters.SetVectorParameter(L"SurfaceConstants", MatReflectanceInfo.SurfaceEmissiveColour);
	material->Parameters.SetVectorParameter(L"SurfaceEmissiveColour", Vector4f(MatReflectanceInfo.Ambient, MatReflectanceInfo.Diffuse, MatReflectanceInfo.Specular, MatReflectanceInfo.Shininess));

	D3D11_SUBRESOURCE_DATA dataLights;
	dataLights.pSysMem = tLights;
	dataLights.SysMemPitch = 0;
	dataLights.SysMemSlicePitch = 0;

	// Get CBuffer Paramameter Writer from the material
	ConstantBufferParameterWriterDX11* tWriter =  material->Parameters.GetConstantBufferParameterWriter(L"cLights");

	if (!tWriter)
	{
		return;
	}

	// Get actual constant buffer from the renderer
	ConstantBufferDX11* tBuffer = pRenderer.GetConstantBufferByIndex(tWriter->GetValue()->m_iResource);

	// Create a pointer to the DeviceContext
	ID3D11DeviceContext* tContext;
	pRenderer.GetDevice()->GetImmediateContext(&tContext);

	// Create a pointer to the source resource
	ID3D11Resource* srcResource;
	srcResource = tBuffer->GetResource();

	// Create a pointer to the source constant buffer
	ID3D11Buffer* srcBuffer;
	srcResource->QueryInterface(IID_ID3D11Buffer, (void**)&srcBuffer);

	//// get buffer description
	//D3D11_BUFFER_DESC srcDesc;
	//srcBuffer->GetDesc(&srcDesc);


	// Download the data from the source buffer and save it in MappedResource
	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	tContext->Map(srcBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	// Create a pointer to the BufferData from the source buffer
	BufferData* b = &((BufferData*)(MappedResource.pData))[0];

	// Modify the data from the source buffer by changing the values in the mapped resource
	for (int i = 0; i < 100; i++)
	{
		b->Lights[i] = tLights[i];
	}

	// Finish operation by unmapping the buffer
	tContext->Unmap(srcBuffer, 0);
}
