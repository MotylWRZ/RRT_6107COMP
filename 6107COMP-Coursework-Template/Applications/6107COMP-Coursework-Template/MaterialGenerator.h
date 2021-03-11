
#pragma once

#include "RendererDX11.h"
#include "MaterialDX11.h"

using namespace Glyph3;

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

};
