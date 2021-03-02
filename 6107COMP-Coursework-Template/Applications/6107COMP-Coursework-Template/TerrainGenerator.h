#pragma once

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

//STL Includes
#include <vector>

using namespace Glyph3;

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<BasicVertexDX11::Vertex>> BasicMeshPtr;

class TerrainGenerator
{
	TerrainGenerator();
	~TerrainGenerator();

public:
	static BasicMeshPtr generateTerrainMesh(int OffsetX, int OffsetZ, int TerrainResolution, int TerrainSpacing);
	static void createTerrainIndexArray(int TerrainWidth, int TerrainLength,
		bool Winding, std::vector<int>& Indices);

	static void createSegmentedTerrainActor(std::vector<Actor*>& TerrainSegments);
	static Actor* createTerrainActor();
};

