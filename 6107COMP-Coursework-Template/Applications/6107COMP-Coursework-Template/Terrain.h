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

	Terrain(int terrainResolution, int terrainSpacing, int chunksNum = 1);
	~Terrain();

	inline int getTerrainResoultion() const { return m_terrainResolution; }
	inline int getTerrainSpacing() const { return m_terrainSpacing; }
	inline const  std::vector<TerrainChunk*>& getTerrainChunks() const { return m_chunks; }

	void setMaterial(MaterialPtr material);
	void addTerrainIntoScene(Scene* pScene);

	void updateLighting(RendererDX11* pRenderer, const std::vector<LightBasePtr>& lights);

private:
	void generateTerrainChunks();

private:
	int m_terrainResolution;
	int m_terrainSpacing;
	int m_chunksNum = 1;
	std::vector<TerrainChunk*> m_chunks;
};

