#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"


#include <vector>
#include "TerrainChunk.h"
#include "TerrainGenerator.h"

class Light_Base;

using namespace Glyph3;

typedef std::shared_ptr<Light_Base> LightBasePtr;

class Terrain : public Actor
{
public:

	Terrain(Scene* pScene, float textureMappingFactor);
	~Terrain();

	inline int getTerrainResoultion() const { return m_terrainResolution; }
	inline int getTerrainSpacing() const { return m_terrainSpacing; }
	inline int getChunksNum() const { return m_chunks.size(); }
	inline float getHeightScale() const { return m_heightScale; }
	inline const  std::vector<TerrainChunk*>& getTerrainChunks() const { return m_chunks; }

	// Set material for all terrain chunks
	void setMaterial(MaterialPtr material);
	MaterialPtr getMaterial();

	void updateLighting(RendererDX11* pRenderer, const std::vector<LightBasePtr>& lights);

	////
	// Single Mesh Terrain Generation
	////

	// Generate terrain meshes in single chunks

	void generateBasicTerrain(int resolution, int terrainSpacing, float heightScale = 12.0f, float majorHeightFrequency = 5.0f,
		float majorHeight = 1.0f,
		float minorHeightFrequency = 75.0f,
		float minorHeight = 0.25f);

	void generateTerrainFromNoise(int terrainResolution,
		float spacing,
		float heightScale,
		FastNoise::NoiseType noiseType = FastNoise::NoiseType::SimplexFractal,
		int seed = 123456,
		float frequency = 0.01f);

	void generateTerrainFromHeightmap(const char* filename,
		int actualTerrainWidth,
		int actualterrainLength,
		int terrainWidth,
		int terrainLength,
		int terrainSpacing,
		float heightScale);

	////
	// Chunked Terrain Mesh Generation
	////

	// Creates 4 equal chunks of terrain
	// if extend is false, the terrain will be divided into 4 equal chunks, each chunk with divided resolution
	// if extend is true, the terrain will be extended to 4 equal pieces, each piece with the original resolution

	void generateBasicChunkedTerrain(bool extend,
		int resolution,
		int terrainSpacing,
		float heightScale = 12.0f,
		float majorHeightFrequency = 5.0f,
		float majorHeight = 1.0f,
		float minorHeightFrequency = 75.0f,
		float minorHeight = 0.25f);

	void generateChunkedTerrainFromNoise(bool extend,
		int terrainResolution,
		float spacing,
		float heightScale,
		FastNoise::NoiseType noiseType = FastNoise::NoiseType::SimplexFractal,
		int seed = 123456,
		float frequency = 0.01f);

	void generateChunkedTerrainFromHeightmap(bool extend, const char* filename,
		int actualTerrainWidth,
		int actualterrainLength,
		int terrainWidth,
		int terrainLength,
		int terrainSpacing,
		float heightScale);

	// Delete all chunk objetcs from this terrain object and from the current scene
	void deleteTerrainMesh();

private:
	// Instantiate chunks objects and store them in the chunks vector
	void generateTerrainChunks(const std::vector<std::vector<Vector3f>>& chunksVertices, float heightScale);

private:
	int m_terrainResolution;
	int m_terrainSpacing;
	float m_heightScale;
	float m_textureMappingFactor;
	Vector4f m_textureBoundary;
	std::vector<TerrainChunk*> m_chunks;
	Scene* m_pScene;
};

