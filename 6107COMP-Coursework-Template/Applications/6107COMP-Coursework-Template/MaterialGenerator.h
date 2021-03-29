
#pragma once

#include "RendererDX11.h"
#include "MaterialDX11.h"

class Light_Base;

static constexpr int LIGHTS_NUM_MAX = 100;

using namespace Glyph3;

typedef std::shared_ptr<Light_Base> LightBasePtr;

struct MaterialReflectanceInfo
{
	Vector4f SurfaceEmissiveColour;
	float	 Ambient;
	float    Diffuse;
	float    Specular;
	float	 Shininess;
};

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

	static MaterialPtr createLitBumpTexturedMaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile, std::wstring bumpTextureFile, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo);

	static void setLightToMaterial(RendererDX11& pRenderer, MaterialPtr material, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo);
	static void updateMaterialLight(RendererDX11& pRenderer, MaterialPtr material, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo);

	static MaterialPtr createGSInstancingMaterial(RendererDX11& renderer, std::wstring diffuseTextureFile);
	static MaterialPtr createGSInstancingMultiTextureMaterial(RendererDX11& renderer, std::wstring diffuseTextureFile, std::wstring bumpTextureFile);


};

