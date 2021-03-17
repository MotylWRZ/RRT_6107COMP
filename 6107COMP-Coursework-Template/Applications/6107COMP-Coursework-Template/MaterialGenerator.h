
#pragma once

#include "RendererDX11.h"
#include "MaterialDX11.h"

class Light_Base;

using namespace Glyph3;

typedef std::shared_ptr<Light_Base> LightBasePtr;

class MaterialGenerator
{
public:
	MaterialGenerator();
	~MaterialGenerator();

	// Abstract Materials (allows for specifying the shader file)
	static MaterialPtr createBasicMaterial(RendererDX11& pRenderer);
	static MaterialPtr createTextureMaterial(RendererDX11& pRenderer, std::wstring shaderFile, std::wstring textureFile);
	static MaterialPtr createMultiTextureMaterial(RendererDX11& pRenderer, std::wstring shaderFile, std::wstring textureFile1, std::wstring textureFile2);

	// Concrete Materials
	static MaterialPtr createTerrainMultiTextureMaterial(RendererDX11& pRenderer, std::wstring highlandsTextureFile, std::wstring lowlandsTextureFile);

	static MaterialPtr createLitTexturedMaterial(RendererDX11& pRenderer, std::wstring textureFile1);
	static MaterialPtr createLitBumpTexturedMaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile, std::wstring bumpTextureFile, const std::vector<LightBasePtr>& lights);

	static void setLightToMaterial(RendererDX11& pRenderer, MaterialPtr material);
	static void setLightToMaterialV2(RendererDX11& pRenderer, MaterialPtr material);
	static void setLightToMaterialV3(RendererDX11& pRenderer, MaterialPtr material, const std::vector<LightBasePtr>& lights);
	static void setLightToMaterialPrev(RendererDX11& pRenderer, MaterialPtr material);
};

