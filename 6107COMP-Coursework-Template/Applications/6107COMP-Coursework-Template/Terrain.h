#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"


#include <vector>
#include "TerrainChunk.h"

class Light_Base;

using namespace Glyph3;

typedef std::shared_ptr<Light_Base> LightBasePtr;

class Terrain
{
public:

	Terrain(int terrainResolution, int terrainSpacing, float heightScale);
	~Terrain();

	inline int getTerrainResoultion() const { return m_terrainResolution; }
	inline int getTerrainSpacing() const { return m_terrainSpacing; }
	inline int getChunksNum() const { return m_chunks.size(); }
	inline float getHeightScale() const { return m_heightScale; }
	inline const  std::vector<TerrainChunk*>& getTerrainChunks() const { return m_chunks; }

	void setMaterial(MaterialPtr material);
	void addTerrainIntoScene(Scene* pScene);

	void updateLighting(RendererDX11* pRenderer, const std::vector<LightBasePtr>& lights);

	void initializeBasic();

	// Creates 4 equal chunks of terrain
	// if extend is false, the terrain will be divided into 4 equal chunks, each chunk with divided resolution
	// if extend is true, the terrain will be extended to 4 equal pieces, each piece with the original resolution
	void initializeBasicChunkedTerrain(bool extend);

private:
	void generateTerrainChunks();

private:
	int m_terrainResolution;
	int m_terrainSpacing;
	float m_heightScale;
	std::vector<TerrainChunk*> m_chunks;
};

