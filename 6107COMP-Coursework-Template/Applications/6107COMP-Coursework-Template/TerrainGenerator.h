#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

//STL Includes
#include <vector>

#include "RRTVertexDX11.h"

using namespace Glyph3;

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<RRTVertexDX11::Vertex>> BasicMeshPtr;

class TerrainGenerator
{
	TerrainGenerator();
	~TerrainGenerator();

public:
	static BasicMeshPtr generateTerrainMesh(int offsetX, int offsetZ, int terrainResolution, int terrainSpacing);
	static void createTerrainIndexArray(int terrainWidth, int terrainLength,
		bool winding, std::vector<int>& indices);

	static void createSegmentedTerrainActor(std::vector<Actor*>& terrainSegments);
	static Actor* createTerrainActor(int offsetX, int offsetZ, int terrainResolution, int terrainSpacing);
};

