
#pragma once
#include "RendererDX11.h"
#include "MaterialDX11.h"

class Light_Base;

static constexpr int LIGHTS_NUM_MAX = 100;

using namespace Glyph3;

typedef  std::shared_ptr<Light_Base> LightBasePtr;

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
	/*
	* Abstract Materials (allows for specifying the shader file)
	* These materials might require a post initialisation setup (loading shader resources etc.)
	*/
	static MaterialPtr createBasicMaterial(RendererDX11& renderer);

	//// Create basic material with Vertex Shader and Pixel Shader with specified HLSL file (it has to contain both shaders)
	static MaterialPtr createBasicMaterial(RendererDX11& renderer, std::wstring shaderFile);

	// Create material with Vertex, Pixel and Geometry Shader with specified HLSL file (all three shaders should be inlcuded in the HLSL file)
	static MaterialPtr createMaterialWithGS(RendererDX11& renderer, std::wstring shaderFile);	static MaterialPtr createTextureMaterial(RendererDX11& renderer, std::wstring shaderFile, std::wstring textureFile);


	/*
	* Concrete Materials
	* These materials will be setup and any resources will be loaded upon initialisation
	*/
	static MaterialPtr createMultiTextureMaterial(RendererDX11& renderer, std::wstring shaderFile, std::wstring textureFile1, std::wstring textureFile2);
	static MaterialPtr createLitBumpTexturedMaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile, std::wstring bumpTextureFile, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo);
	static MaterialPtr createLitTexturedMaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo);

	static MaterialPtr createPlanetExplosionmaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile);

	static MaterialPtr createUVMappedTextureMaterial(RendererDX11& pRenderer, std::wstring diffuseTextureFile);
	///
	// Terrain Materials
	///
	static MaterialPtr createTerrainMultiTextureMaterial(RendererDX11& pRenderer, std::wstring highlandsTextureFile, std::wstring lowlandsTextureFile);
	static MaterialPtr createLitTerrainMultiTextureMaterial(RendererDX11& pRenderer, std::wstring highlandsTextureFile, std::wstring lowlandsTextureFile, const std::vector<LightBasePtr>& lights, const MaterialReflectanceInfo& MatReflectanceInfo);
	static MaterialPtr createLitBumpTerrainMultiTextureMaterial(RendererDX11& pRenderer, std::wstring highlandsTextureFile, std::wstring lowlandsTextureFile, std::wstring highlandsBumpTextureFile, std::wstring lowlandsBumpTextureFile, const std::vector<LightBasePtr>& lights, const MaterialReflectanceInfo& MatReflectanceInfo);



	static void setLightToMaterial(RendererDX11& renderer, MaterialPtr material, const std::vector<LightBasePtr>& lights, MaterialReflectanceInfo MatReflectanceInfo);
	static void updateMaterialLight(RendererDX11& renderer, MaterialPtr material, const std::vector<LightBasePtr>& lights);
};

